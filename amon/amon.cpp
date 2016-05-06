// amon.cpp
//
// The AMon Audio Level Monitoring Client
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amon.cpp,v 1.19 2011/08/17 14:46:54 pcvs Exp $
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

#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <netinet/in.h>

#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qsignalmapper.h>

#include <am.h>
#include <amcmdswitch.h>
#include <amstereometer.h>
#include <amlabel.h>

#include <amon.h>

#include "../icons/amon-22x22.xpm"


void SigHandler(int signum)
{
  pid_t pid;

  switch(signum) {
      case SIGCHLD:
	pid=waitpid(-1,NULL,WNOHANG);
	while(pid>0) {
	  pid=waitpid(-1,NULL,WNOHANG);
	}
	signal(SIGCHLD,SigHandler);
	break;

      case SIGINT:
      case SIGTERM:
	exit(0);
  }
}


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  //
  // Read Command Options
  //
  AMCmdSwitch *cmd=
    new AMCmdSwitch(qApp->argc(),qApp->argv(),"amond",AMON_USAGE);
  delete cmd;

  //
  // Load Configuration
  //
  config=new AMMonConfig(palette());
  config->load();

  //
  // Scan Timers
  //
  amon_scan_timer=new QTimer(this);
  connect(amon_scan_timer,SIGNAL(timeout()),this,SLOT(scanTimerData()));

  //
  // Audio Player
  //
  amon_active_player=-1;
  amon_next_player=-1;
  amon_player=new AMPlayer(this,"amon_player");
  connect(amon_player,SIGNAL(stateChanged(AMPlayer::State)),
	  this,SLOT(stateChangedData(AMPlayer::State)));
  connect(amon_player,SIGNAL(error(AMPlayer::Error)),
	  this,SLOT(errorData(AMPlayer::Error)));

  //
  // Set Global Parameters
  //
  QPalette default_palette=palette();
  setPalette(config->globalPalette());
  setCaption(config->globalCaption());
  setGeometry(geometry().x(),geometry().y(),
	      config->globalSize().width(),config->globalSize().height());

  amon_map=new QPixmap(amon_22x22_xpm);
  setIcon(*amon_map);

  //
  // Create Label Widgets
  //
  QLabel *label;
  for(unsigned i=0;i<config->labelQuantity();i++) {
    label=new QLabel(config->labelText(i),this);
    label->setGeometry(config->labelGeometry(i));
    label->setPalette(config->labelPalette(i));
    label->setFont(config->labelFont(i));
    label->setAlignment(config->labelAlignment(i));
  }

  //
  // Create Meter Widgets
  //
  AMStereoMeter *meter;
  int parser_id=-1;
  for(unsigned i=0;i<config->meterQuantity();i++) {
    parser_id=
      GetConnection(config->meterMonitorHost(i),config->meterMonitorPort(i));
    meter=new AMStereoMeter(config->meterOrientation(i),parser_id,
			    config->meterMonitorChannel(i),this);
    meter->setMode(AMMeter::Peak);
    meter->setRange(config->meterMinLevel(i),config->meterMaxLevel(i));
    meter->setDisplayElements(config->meterDisplayElements(i));
    meter->setHighThreshold(config->meterYellowLevel(i));
    meter->setClipThreshold(config->meterRedLevel(i));
    meter->setLowColor(config->meterLowColor(i));
    meter->setHighColor(config->meterMidColor(i));
    meter->setClipColor(config->meterHighColor(i));
    meter->setDarkLowColor(config->meterLowBackground(i));
    meter->setDarkHighColor(config->meterMidBackground(i));
    meter->setDarkClipColor(config->meterHighBackground(i));
    meter->setAveragingPeriods(config->globalAveragingPeriods());
    meter->setGeometry(config->meterGeometry(i));
    connect(amon_parsers[parser_id],
	    SIGNAL(levelUpdateReceived(int,AMStereoChannel *)),
	    meter,SLOT(updateLevel(int,AMStereoChannel *)));
  }

  //
  // Create Alarm Widgets
  //
  AMLabel *alarm;
  for(unsigned i=0;i<config->alarmQuantity();i++) {
    parser_id=
      GetConnection(config->alarmMonitorHost(i),config->alarmMonitorPort(i));
    alarm=new AMLabel(config->alarmMonitorChannel(i),
		      config->alarmMonitorChannel(i),this);
    alarm->setText(config->alarmText(i));
    alarm->setGeometry(config->alarmGeometry(i));
    alarm->setPalette(config->alarmInactivePalette(i));
    alarm->setActivePalette(config->alarmActivePalette(i));
    alarm->setAlarmType(config->alarmType(i));
    alarm->setAlarmAction(config->alarmAction(i));
    alarm->setFont(config->alarmFont(i));
    alarm->setAlignment(config->alarmAlignment(i));
    connect(amon_parsers[parser_id],
	    SIGNAL(levelUpdateReceived(int,AMStereoChannel *)),
	    alarm,SLOT(updateStatus(int,AMStereoChannel *)));
  }

  //
  // Create Monitor Buttons
  //
  QSignalMapper *mapper=new QSignalMapper(this,"monitor_button_mapper");
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(monitorButtonClickedData(int)));
  for(unsigned i=0;i<config->buttonQuantity();i++) {
    amon_monitor_buttons.
      push_back(new AMPushButton(config->buttonText(i),this));
    amon_monitor_buttons.back()->setGeometry(config->buttonGeometry(i));
    amon_monitor_buttons.back()->setPalette(default_palette);
    mapper->setMapping(amon_monitor_buttons.back(),i);
    connect(amon_monitor_buttons.back(),SIGNAL(clicked()),mapper,SLOT(map()));
  }

  //
  // Build Scan List
  //
  amon_scan_ptr=0;
  for(unsigned i=0;i<config->globalScanSources();i++) {
    if((config->globalScanSource(i)<=amon_monitor_buttons.size())&&
       (config->globalScanSource(i)>0)) {
      amon_scan_buttons.push_back(config->globalScanSource(i)-1);
    }
  }
  if(amon_scan_buttons.size()>0) {
    scanTimerData();
  }

  //
  // Map Signals
  //
  ::signal(SIGINT,SigHandler);
  ::signal(SIGCHLD,SigHandler);
}


void MainWidget::monitorButtonClickedData(int n)
{
  amon_scan_timer->stop();
  ToggleMonitor(n);
}


void MainWidget::stateChangedData(AMPlayer::State state)
{
  if(amon_active_player<0) {
    return;
  }
  switch(state) {
    case AMPlayer::StatePlaying:
      amon_monitor_buttons[amon_active_player]->setFlashingEnabled(true);
      break;

    case AMPlayer::StateStopped:
      amon_monitor_buttons[amon_active_player]->setFlashingEnabled(false);
      amon_active_player=-1;
      if(amon_next_player>=0) {
	monitorButtonClickedData(amon_next_player);
      }
      break;
  }
}


void MainWidget::errorData(AMPlayer::Error err)
{
}


void MainWidget::scanTimerData()
{
  if(amon_scan_ptr<0) {
    return;
  }
  monitorButtonClickedData(amon_scan_buttons[amon_scan_ptr++]);
  if(amon_scan_ptr>=amon_scan_buttons.size()) {
    amon_scan_ptr=0;
  }
  amon_scan_timer->start(1000*config->globalScanDwellTime(),true);
}


void MainWidget::ToggleMonitor(int n)
{
  if(amon_active_player<0) {
    amon_active_player=n;
    amon_next_player=-1;
    amon_player->
      play(config->buttonMonitorHost(n),
	   AMON_BASE_RTP_PORT+2*(config->buttonMonitorChannel(n)-1));
  }
  else {
    if(amon_active_player!=n) {
      amon_player->stop();
      amon_player->play(config->buttonMonitorHost(n),
			AMON_BASE_RTP_PORT+2*(config->buttonMonitorChannel(n)-1));
      amon_active_player=n;
    }
    else {
      amon_player->stop();
      amon_scan_timer->start(1000*config->globalScanResetTime(),true);
    }
  }
}


unsigned MainWidget::GetConnection(const QString &hostname,Q_UINT16 port)
{
  //
  // First, see if an existing connection will work...
  //
  for(unsigned j=0;j<amon_parsers.size();j++) {
    if((amon_parsers[j]->hostName()==hostname)&&
       (amon_parsers[j]->hostPort()==port)) {
      return j;
    }
  }
  
  //
  // If not, create a new one
  //
  amon_parsers.push_back(new AMParser(amon_parsers.size(),this));
  amon_parsers.back()->connectToHost(hostname,port);
  return amon_parsers.size()-1;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  QWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->show();
  return a.exec();
}
