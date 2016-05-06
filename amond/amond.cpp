// amond.cpp
//
// The AMon Audio Level Monitoring Daemon
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amond.cpp,v 1.12 2008/11/22 17:35:46 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>
#include <sys/mman.h>

#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qsignalmapper.h>

#include <amcmdswitch.h>

#include <globals.h>
#include <amon_socket.h>
#include <amond.h>

#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <fcntl.h>

//
// Global Variables
//
bool debug_mode=false;
int meter_block_id;
struct oss_format oss_format[AMON_MAX_CARDS];
struct AMMeterBlock *meter_block;


void SigHandler(int signum)
{
  pid_t pid;

  switch(signum) {
    case SIGINT:
    case SIGTERM:
      shmctl(meter_block_id,IPC_RMID,NULL);
      unlink(AMON_AMOND_PID_FILE);
      LogLine("amond shutting down normally");
      exit(0);

    case SIGCHLD:
      pid=waitpid(-1,NULL,WNOHANG);
      while(pid>0) {
	pid=waitpid(-1,NULL,WNOHANG);
      }
      ::signal(SIGCHLD,SigHandler);
      break;
  }
}


void LogLine(const QString &line)
{
  FILE *file;
  QDateTime current=QDateTime::currentDateTime();

  file=fopen(AMON_LOG_FILE,"a");
  if(file==NULL) {
    return;
  }
  chmod(AMON_LOG_FILE,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  fprintf(file,"%02d/%02d/%4d - %02d:%02d:%02d.%03d : %s\n",
	  current.date().month(),
	  current.date().day(),
	  current.date().year(),
	  current.time().hour(),
	  current.time().minute(),
	  current.time().second(),
	  current.time().msec(),
	  (const char *)line);
  fclose(file);
}


MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  struct shmid_ds shmid_ds;

  //
  // Read Command Options
  //
  AMCmdSwitch *cmd=
    new AMCmdSwitch(qApp->argc(),qApp->argv(),"amond",AMOND_USAGE);
  delete cmd;

  //
  // Load Configuration
  //
  main_config=new AMMonDaemonConfig(AMON_DAEMON_CONF_FILE);
  main_config->load();

  //
  // Initialize Data Structures
  //
  if(qApp->argc()>1) {
    for(int i=1;i<qApp->argc();i++) {
      if(!strcmp(qApp->argv()[i],"-d")) {
	debug_mode=true;
      }
    }
  }
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    main_alarms[i]=0;
  }

  //
  // Initialize Random Number Generator
  //
  srandom(time(NULL));

  //
  // Create the Meter Block
  //
  // First try to create a new shared memory segment.
  if((meter_block_id=
      shmget(AMON_METER_SHM_KEY,sizeof(struct AMMeterBlock),
	     IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
    if(errno!=EEXIST) {
      LogLine(QString().
	      sprintf("can't allocate shared memory segment, error:%d",
		      errno));
      exit(1);
    }
    // The shmget() error was due to an existing segment, so try to get it, release it, and re-get it.
    meter_block_id = shmget(AMON_METER_SHM_KEY,sizeof(struct AMMeterBlock),0);
    shmctl(meter_block_id,IPC_RMID,NULL);
    if((meter_block_id=shmget(AMON_METER_SHM_KEY,sizeof(struct AMMeterBlock),
			      IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
      LogLine(QString().sprintf("can't allocate shared memory segment after trying to destroy, error:%d",errno));
      exit(1);
    }
  }
  shmctl(meter_block_id,IPC_STAT,&shmid_ds);
  shmid_ds.shm_perm.uid=0;
  shmctl(meter_block_id,IPC_SET,&shmid_ds);
  meter_block=(AMMeterBlock *)shmat(meter_block_id,NULL,0);
  for(int i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    meter_block->left_level[i]=0;
    meter_block->right_level[i]=0;
    meter_block->sum_level[i]=0;
    meter_block->diff_level[i]=0;
  }
  for(int i=0;i<AMON_MAX_CARDS;i++) {
    meter_block->block_writeable[i]=true;
  }

  //
  // Level Alarm Timers
  //
  QSignalMapper *mapper=new QSignalMapper(this,"low_mapper");
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(lowLevelTimeoutData(int)));
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    main_low_timers[i]=new QTimer(this);
    mapper->setMapping(main_low_timers[i],i);
    connect(main_low_timers[i],SIGNAL(timeout()),mapper,SLOT(map()));
  }
  mapper=new QSignalMapper(this,"high_mapper");
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(highLevelTimeoutData(int)));
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    main_high_timers[i]=new QTimer(this);
    mapper->setMapping(main_high_timers[i],i);
    connect(main_high_timers[i],SIGNAL(timeout()),mapper,SLOT(map()));
  }

  //
  // Detach
  //
  if(!debug_mode) {
    daemon(0,0);
  }

  //
  // Map Signals
  //
  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGCHLD,SigHandler);

  //
  // Attempt to get realtime permissions
  //
  if(main_config->globalUseRealtime()) {
    struct sched_param sp;
    int ret=0;
    memset(&sp,0,sizeof(sp));
    sp.sched_priority=50;
    ret|=sched_setscheduler(0,SCHED_FIFO,&sp);
    ret|=mlockall(MCL_CURRENT|MCL_FUTURE);
    if(ret==0) {
      LogLine("got realtime permissions, priority=50");
    }
    else {
      LogLine("failed to get realtime permissions");
    }
  }

  //
  // Start the Sound Driver
  //
  if(OssInit()) {
    return;
  }

  //
  // Open the Meter Server Socket
  //
  main_socket=new AmonSocket(AMON_DEFAULT_SOCKET,0,1,this,"main_socket");
  connect(main_socket,SIGNAL(connection(int,int)),
	  this,SLOT(connectionData(int,int)));

  //
  // Start Scanning Connections
  //
  QTimer *timer=new QTimer(this,"main_channel_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(scanConnectionsData()));
  timer->start(AMON_CONNECTION_SCAN_INTERVAL);

  //
  // Start Scanning Channels
  //
  timer=new QTimer(this,"main_scanner_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(scanChannelsData()));
  timer->start(AMON_CHANNEL_SCAN_INTERVAL);

  //
  // Start Alarm Timers
  //
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    if(main_config->lowTimeout(i)>0) {
      main_low_timers[i]->start(main_config->lowTimeout(i),true);
    }
    if(main_config->highTimeout(i)>0) {
      main_high_timers[i]->start(main_config->highTimeout(i),true);
    }
  }

  //
  // Log PID
  //
  FILE *pidfile;
  if((pidfile=fopen(AMON_AMOND_PID_FILE,"w"))==NULL) {
    perror("amond");
  }
  else {
    fprintf(pidfile,"%d",getpid());
    fclose(pidfile);
  }
}


void MainObject::connectionData(int id,int fd)
{
  main_sockets.push_back(new QSocket(this));
  main_sockets.back()->setSocket(fd);
}
 
 
void MainObject::scanChannelsData()
{
  /*
  for(unsigned i=0;i<AMON_MAX_CARDS;i++) {
    if(!meter_block->block_writeable[i]) {
      for(unsigned j=0;j<(AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2));j++) {
	printf("%dL: %11d | %6.2lf dBu\n",i*AMON_MAX_CARDS+j+1,
	       meter_block->left_level[i*AMON_MAX_CARDS+j],
	       AMON_LEVEL_OFFSET/100+
	       20.0*log10(((double)(meter_block->
				    left_level[i*AMON_MAX_CARDS+j]))/
			  2147483648.0));
	printf("%dR: %11d | %6.2lf dBu\n",i*AMON_MAX_CARDS+j+1,
	       meter_block->right_level[i*AMON_MAX_CARDS+j],
	       AMON_LEVEL_OFFSET/100+
	       20.0*log10(((double)(meter_block->
				    right_level[i*AMON_MAX_CARDS+j]))/
			  2147483648.0));
	printf("%dS: %11d | %6.2lf dBu\n",i*AMON_MAX_CARDS+j+1,
	       meter_block->sum_level[i*AMON_MAX_CARDS+j],
	       AMON_LEVEL_OFFSET/100+
	       20.0*log10(((double)(meter_block->
				    sum_level[i*AMON_MAX_CARDS+j]))/
			  2147483648.0));
	printf("%dD: %11d | %6.2lf dBu\n",i*AMON_MAX_CARDS+j+1,
	       meter_block->diff_level[i*AMON_MAX_CARDS+j],
	       AMON_LEVEL_OFFSET/100+
	       20.0*log10(fabs((double)(meter_block->
					diff_level[i*AMON_MAX_CARDS+j]))/
			  2147483648.0));
	printf("\n");
	printf("\n");
      }
      meter_block->block_writeable[i]=true;
    }
  }
  return;
  */

  char buf[23]={0xaa,18,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint32_t *ibuf=(uint32_t *)(buf+4);
  int chan=0;
  for(unsigned i=0;i<AMON_MAX_CARDS;i++) {
    //
    // Process Meter Data
    //
    if(!meter_block->block_writeable[i]) {
      for(unsigned j=0;j<(AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2));j++) {
	//
	// Send Channel Updates
	//
	chan=i*AMON_MAX_CARDS+j;
	buf[3]=0xff&chan;   // Channel number
	ibuf[0]=htonl(meter_block->left_level[chan]);
	ibuf[1]=htonl(meter_block->right_level[chan]);
	ibuf[2]=htonl(meter_block->sum_level[chan]);
	ibuf[3]=htonl(meter_block->diff_level[chan]);
	buf[19]=main_alarms[chan];  // Alarm Status
	buf[20]=0;       // Checksum
	for(unsigned k=2;k<20;k++) {
	  buf[20]=0xFF&(buf[20]+buf[k]);
	}
	for(int k=main_sockets.size()-1;k>=0;k--) {
	  main_sockets[k]->writeBlock(buf,22);
	}
	
	//
	// Update Alarm Timers
	//
	if((meter_block->left_level[chan]>main_config->lowLevel(chan))&&
	   (meter_block->right_level[chan]>main_config->lowLevel(chan))) {
	  main_alarms[chan]&=(~AMON_ALARM_LOW_LEVEL);
	  if(main_config->lowTimeout(chan)>0) {
	    main_low_timers[chan]->stop();
	    main_low_timers[chan]->start(main_config->lowTimeout(chan),true);
	  }
	}
	if((meter_block->left_level[chan]<main_config->highLevel(chan))&&
	   (meter_block->right_level[chan]<main_config->highLevel(chan))) {
	  main_alarms[chan]&=(~AMON_ALARM_HIGH_LEVEL);
	  if(main_config->highTimeout(chan)>0) {
	    main_high_timers[chan]->stop();
	    main_high_timers[chan]->
	      start(main_config->highTimeout(chan),true);
	  }
	}
	
	//
	// Clear Level Data
	//
	meter_block->left_level[chan]=0;
	meter_block->right_level[chan]=0;
	meter_block->sum_level[chan]=0;
	meter_block->diff_level[chan]=0;
      }
      meter_block->block_writeable[i]=true;
    }
  }
}


void MainObject::scanConnectionsData()
{
  for(int i=main_sockets.size()-1;i>=0;i--) {
    if(main_sockets[i]->state()!=QSocket::Connected) {
      delete main_sockets[i];
      std::vector<QSocket *>::iterator it=main_sockets.begin()+i;
      main_sockets.erase(it,it+1);
    }
  }
}


void MainObject::lowLevelTimeoutData(int chan)
{
  main_alarms[chan]|=AMON_ALARM_LOW_LEVEL;
  LogLine(QString().sprintf("channel %d: LOW LEVEL",chan+1));
}


void MainObject::highLevelTimeoutData(int chan)
{
  main_alarms[chan]|=AMON_ALARM_HIGH_LEVEL;
  LogLine(QString().sprintf("channel %d: HIGH LEVEL",chan+1));
}


int main(int argc,char *argv[])
{
  int rc;
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  rc=a.exec();
  shmctl(meter_block_id,IPC_RMID,NULL);
  return rc;
}
