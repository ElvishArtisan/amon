// amon_oss.cpp
//
// The AMon Audio Level Monitoring Daemon
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amon_oss.cpp,v 1.9 2008/11/22 17:35:46 cvs Exp $
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

#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include <am.h>

#include <qapplication.h>
#include <qsocketnotifier.h>

#include <globals.h>
#include <amond.h>
#include <amon_socket.h>
#include <rtpcontrolheader.h>
#include <rtpheader.h>

#define MODULO 4


void *MainObject::OssCaptureCallback(struct oss_format *of)
{
  /*
  struct oss_format *of=(struct oss_format *)ptr;
  */
  int left=0;
  int right=0;
  int sum=0;
  int diff=0;
  //  char abuf[AMON_PERIOD_SIZE*64];
  // int16_t *audio=(int16_t *)abuf;
  int16_t *audio=(int16_t *)of->card_buffer;
  uint32_t rtp_buf[325];
  int n;
  sockaddr_in rtp_sa;
  char hostname[255];
  unsigned counter=0;

  //
  // Initialize Data Structures
  //
  main_packet_count=0;
  main_octet_count=0;
  main_start_datetime=QDateTime(QDate::currentDate(),QTime::currentTime());

  //
  // Normalize Channel Count
  //
  int chans=of->channels;
  if(chans>(AMON_MAX_CHANNELS/AMON_MAX_CARDS)) {
    chans=AMON_MAX_CHANNELS/AMON_MAX_CARDS;
  }

  //
  // Get Hostname
  //
  gethostname(hostname,255);

  //
  // Open the RTP Streaming Sockets
  //
  QSocketNotifier *notifier;
  sockaddr_in sa;
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2));i++) {
    int chan=of->card*(AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2));

    //
    // RTP Stream
    //
    main_rtp_socket[i]=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    sa.sin_port=htons(AMON_BASE_RTP_PORT+2*(chan+i));
    bind(main_rtp_socket[i],(sockaddr *)&sa,sizeof(sa));
    fcntl(main_rtp_socket[i],F_SETFL,O_NONBLOCK);
    main_rtp_header[i]=new RTPHeader(RTPHeader::PayloadL16_2);
    main_rtp_header[i]->setSsrc((uint32_t)random());
    main_rtp_header[i]->setTimestampInterval(256);
    main_rtp_senderreport[i]=new RTPSenderReport(main_rtp_header[i]->ssrc());
    main_rtp_description[i]=new RTPSourceDescription();
    main_rtp_description[i]->
      addChunk(RTPSourceDescription::ChunkCname,main_rtp_header[i]->ssrc(),
	       QString().sprintf("amon%d@%s",chan+1,hostname));

    //
    // RTCP Stream
    //
    main_rtcp_socket[i]=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    sa.sin_port=htons(AMON_BASE_RTP_PORT+2*(chan+i)+1);
    bind(main_rtcp_socket[i],(sockaddr *)&sa,sizeof(sa));
    fcntl(main_rtcp_socket[i],F_SETFL,O_NONBLOCK);
    notifier=
      new QSocketNotifier(main_rtcp_socket[i],QSocketNotifier::Read,this);
    connect(notifier,SIGNAL(activated(int)),this,SLOT(rtcpReadyData(int)));
  }
  memset(&rtp_sa,0,sizeof(rtp_sa));
  rtp_sa.sin_family=AF_INET;

  //
  // Timeout Timer
  //
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(rtpTimeoutData()));
  timer->start(AMON_RTP_TIMEOUT_INTERVAL);

  signal(SIGTERM,SigHandler);
  signal(SIGINT,SigHandler);

  while(!of->exiting) {
    int s=read(of->fd,of->card_buffer,2*AMON_FRAMES_PER_PACKET*of->channels);
    int frames=s/16;
    if(meter_block->block_writeable[of->card]) {
      for(int i=0;i<chans;i+=2) {
	left=meter_block->left_level[AMON_MAX_CARDS*of->card+i/2]>>16;
	right=meter_block->right_level[AMON_MAX_CARDS*of->card+i/2]>>16;
	sum=meter_block->sum_level[AMON_MAX_CARDS*of->card+i/2]>>16;
	diff=meter_block->diff_level[AMON_MAX_CARDS*of->card+i/2]>>16;
	for(int k=0;k<frames;k++) {
	  if(abs(((int16_t *)of->card_buffer)[of->channels*k+i])>left) {
	    left=abs(((int16_t *)of->card_buffer)[of->channels*k+i]);
	  }
	  if(abs(((int16_t *)of->card_buffer)[of->channels*k+i+1])>right) {
	    right=abs(((int16_t *)of->card_buffer)[of->channels*k+i+1]);
	  }
	  if((abs(((int16_t *)of->card_buffer)[of->channels*k+i]+
		 ((int16_t *)of->card_buffer)[of->channels*k+i+1])/2)>sum) {
	    sum=abs(((int16_t *)of->card_buffer)[of->channels*k+i]+
		    ((int16_t *)of->card_buffer)[of->channels*k+i+1])/2;
	  }
	  if((abs(((int16_t *)of->card_buffer)[of->channels*k+i]-
		 ((int16_t *)of->card_buffer)[of->channels*k+i+1])/2)>abs(diff)) {
	    diff=abs(((int16_t *)of->card_buffer)[of->channels*k+i]-
	      ((int16_t *)of->card_buffer)[of->channels*k+i+1])/2;
	  }
	}
	meter_block->left_level[AMON_MAX_CARDS*of->card+i/2]=left<<16;
	meter_block->right_level[AMON_MAX_CARDS*of->card+i/2]=right<<16;
	meter_block->sum_level[AMON_MAX_CARDS*of->card+i/2]=sum<<16;
	meter_block->diff_level[AMON_MAX_CARDS*of->card+i/2]=diff<<16;
      }

      if(++of->count==of->modulo) {
	of->count=0;
	meter_block->block_writeable[of->card]=false;
      }
    }

    //
    // Process Streaming Data
    //
    if(frames>0) {
      int16_t cbuf[frames*2];
      
      for(unsigned i=0;i<(AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2));i++) {
	//
	// De-Interleave to stereo and set network byte order
	//
	for(int j=0;j<frames;j++) {
	  cbuf[2*j]=htons(audio[8*j+2*i]);
	  cbuf[2*j+1]=htons(audio[8*j+2*i+1]);
	}
	
	//
	// Generate RTP Packet
	//
	n=main_rtp_header[i]->writeBlock(rtp_buf,325);
	for(int j=0;j<frames;j++) {
	  rtp_buf[n+j]=((uint32_t *)cbuf)[j];
	}
	//
	// Stream It
	//
	for(unsigned j=0;j<main_rtp_connections[i].size();j++) {
	  rtp_sa.sin_addr.s_addr=
	    htonl(main_rtp_connections[i][j]->peerAddress());
	  rtp_sa.sin_port=htons(main_rtp_connections[i][j]->peerPort()-1);
	  sendto(main_rtp_socket[i],(void *)rtp_buf,4*(n+frames),0,
		 (sockaddr *)&rtp_sa,sizeof(rtp_sa));
	}
	(*(main_rtp_header[i]))++;
	main_packet_count++;
	main_octet_count+=(4*(n+frames));
      }
      if(counter++>20) {
	//
	// Generate RTCP Packet
	//
	QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());
	uint64_t ntp_stamp=GetNtpTimestamp(dt);		
	uint32_t rtp_stamp=GetRtpTimestamp(main_start_datetime,dt);
	for(unsigned i=0;i<(AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2));i++) {
	  main_rtp_senderreport[i]->setNtpTimestamp(ntp_stamp);
	  main_rtp_senderreport[i]->incrementRtpTimestamp(rtp_stamp);
	  main_rtp_senderreport[i]->setSenderPacketCount(main_packet_count);
	  main_rtp_senderreport[i]->setSenderOctetCount(main_octet_count);
	  n=main_rtp_senderreport[i]->writeBlock(rtp_buf,325);
	  n+=main_rtp_description[i]->writeBlock(rtp_buf+n,325-n);

	  //
	  // Stream It
	  //
	  for(unsigned j=0;j<main_rtp_connections[i].size();j++) {
	    rtp_sa.sin_addr.s_addr=
	      htonl(main_rtp_connections[i][j]->peerAddress());
	    rtp_sa.sin_port=htons(main_rtp_connections[i][j]->peerPort());
	    sendto(main_rtp_socket[i],(void *)rtp_buf,4*n,0,
		   (sockaddr *)&rtp_sa,sizeof(rtp_sa));
	  }
	}
	counter=0;
      }
    }
    qApp->processEvents();
  }

  return NULL;
}


void MainObject::rtcpReadyData(int fd)
{
  unsigned char buf[1500];
  sockaddr_in sa;
  socklen_t sa_len=sizeof(sa);
  int n;
  RTPControlHeader hdr;
  unsigned id=0;

  //
  // Get ID
  //
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2));i++) {
    if(main_rtcp_socket[i]==fd) {
      id=i;
    }
  }

  //
  // Process packets
  //
  while((n=recvfrom(fd,buf,1500,0,(sockaddr *)&sa,&sa_len))>0) {
    buf[n]=0;
    hdr.readBlock((uint32_t *)buf,n);
    if(hdr.sourceDescription()!=NULL) {
      OssProcessSdes(id,hdr.sourceDescription(),&sa);
    }
    if(hdr.bye()!=NULL) {
      OssProcessBye(id,&sa);
    }
  }
}


void MainObject::rtpTimeoutData()
{
  time_t ct=time(NULL);
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2));i++) {
    for(unsigned j=0;j<main_rtp_connections[i].size();j++) {
      if(main_rtp_connections[i][j]->timestamp()<
	 (ct-AMON_RTP_TIMEOUT_INTERVAL/1000)) {
	delete main_rtp_connections[i][j];
	main_rtp_connections[i].erase(main_rtp_connections[i].begin()+j);
	// printf("timed out %u:%u\n",i,j);
      }
    }
  }
}


bool MainObject::OssInit()
{
  int fd;
  QString dev;

  //
  // Start Up Interfaces
  //
  if(fork()==0) {
    if((fd=open("/dev/dsp",O_RDONLY))<0) {
      exit(0);
    }
    OssStartCaptureDevice("/dev/dsp",0,fd);
    exit(0);
  }
  for(int i=0;i<AMON_MAX_CARDS;i++) {
    dev=QString().sprintf("/dev/dsp%d",i);
    if(fork()==0) {
      if((fd=open(dev,O_RDONLY))<0) {
	exit(0);
      }
      OssStartCaptureDevice(dev,i,fd);
      exit(0);
    }
  }
  return false;
}


void MainObject::OssStartCaptureDevice(const QString &dev,int card,int fd)
{
  unsigned fraghint=0x0004000A;  // 1024 bytes/fragment
  int format=AFMT_S16_LE;
  int channels=AMON_MAX_CHANNELS/AMON_MAX_CARDS;
  int speed=AMON_SAMPLERATE;
  int cook=0;

  LogLine(QString().sprintf("Starting OSS Capture Device %s:",
			    (const char *)dev));

  //
  // Set parameters
  //
  if(ioctl(fd,SNDCTL_DSP_SETFMT,&format)<0) {
    LogLine("  unable to set format for device");
    return;
  }
  LogLine("  format = 16 bit little-endian");

  if(ioctl(fd,SNDCTL_DSP_CHANNELS,&channels)<0) {
    LogLine("  unable to set channel count for device");
    return;
  }
  LogLine(QString().sprintf("  aggreggate channels = %d",channels));

  if(ioctl(fd,SNDCTL_DSP_SPEED,&speed)<0) {
    LogLine("  unable to set sample rate for device");
    return;
  }
  LogLine(QString().sprintf("  sample rate = %d",speed));
  LogLine("  Device started successfully");

  //
  // Allocate Buffer
  //
  oss_format[card].card=card;
  oss_format[card].card_buffer=new char[AMON_PERIOD_SIZE*256];
  oss_format[card].fd=fd;
  oss_format[card].channels=channels;
  oss_format[card].exiting=false;
  oss_format[card].count=0;
  oss_format[card].modulo=1;

  //
  // Start the Callback
  //
  OssCaptureCallback(&oss_format[card]);

  return;
}


void MainObject::OssProcessBye(unsigned id,sockaddr_in *sa)
{
  int conn=GetRtpConnection(id,sa);
  if(conn<0) {
    return;
  }
  delete main_rtp_connections[id][conn];
  main_rtp_connections[id].erase(main_rtp_connections[id].begin()+conn);
  // printf("disconnected %u:%u\n",id,conn);
}


void MainObject::OssProcessSdes(unsigned id,RTPSourceDescription *sd,
				sockaddr_in *sa)
{
  int conn=GetRtpConnection(id,sa);
  if(conn<0) {
    main_rtp_connections[id].push_back(new RTPConnection());
    main_rtp_connections[id].back()->
      setPeerAddress(ntohl(sa->sin_addr.s_addr));
    main_rtp_connections[id].back()->setPeerPort(ntohs(sa->sin_port));
    // printf("created %u:%u\n",id,main_rtp_connections[id].size()-1);
    return;
  }
  main_rtp_connections[id][conn]->updateTimestamp();
}


int MainObject::GetRtpConnection(unsigned id,sockaddr_in *sa)
{
  for(unsigned i=0;i<main_rtp_connections[id].size();i++) {
    if((sa->sin_addr.s_addr==
	htonl(main_rtp_connections[id][i]->peerAddress()))&&
       (sa->sin_port==htons(main_rtp_connections[id][i]->peerPort()))) {
      return i;
    }
  }
  return -1;
}


uint64_t MainObject::GetNtpTimestamp(const QDateTime &dt) const
{
  double stamp=QDateTime(QDate(1900,1,1),QTime(0,0,0,1)).
    secsTo(QDateTime(dt.date(),QTime(0,0,0,1)));
  stamp+=(double)(QTime(0,0,0,1).msecsTo(dt.time())/1000.0);
  return (uint64_t)(4294967296.0*stamp);
}


uint32_t MainObject::GetRtpTimestamp(const QDateTime &base_dt,
				     const QDateTime &dt) const
{
  uint64_t diff=86400000*base_dt.daysTo(dt);
  if(base_dt.time()<dt.time()) {
    diff+=base_dt.time().msecsTo(dt.time());
  }
  else {
    diff+=dt.time().msecsTo(base_dt.time());
  }
  diff*=AMON_SAMPLERATE;
  diff/=1000;

  return (uint32_t)diff;
}
