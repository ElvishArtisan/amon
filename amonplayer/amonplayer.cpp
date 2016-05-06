// amonplayer.cpp
//
// The AMon Audio Player
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amonplayer.cpp,v 1.9 2011/08/17 14:46:54 pcvs Exp $
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
#include <math.h>
#include <netinet/in.h>
#include <sched.h>
#include <sys/mman.h>

#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include <am.h>
#include <amcmdswitch.h>
#include <amstereometer.h>
#include <amlabel.h>
#include <amonplayer.h>

#include <get_host.h>

#include "../icons/amon-22x22.xpm"


void SigHandler(int signum)
{
  switch(signum) {
      case SIGINT:
      case SIGTERM:
	exit(0);
  }
}


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  main_hostname="";
  main_channel=0;
  main_meter_element_count=2;
  int meter_layout=-1;
  bool ok=false;

  //
  // Read Command Options
  //
  AMCmdSwitch *cmd=
    new AMCmdSwitch(qApp->argc(),qApp->argv(),"amonplayer",AMONPLAYER_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--use-meter-layout") {
      meter_layout=cmd->value(i).toInt(&ok);
      cmd->setProcessed(i,true);
      if(!ok) {
	QMessageBox::warning(this,"AMonPlayer - Error",
			     "Invalid command option!");
	exit(256);
      }
    }
  }
  if((cmd->keys()>0)&&(!cmd->processed(cmd->keys()-1))) {
    bool ok=false;
    QString str=QString(qApp->argv()[qApp->argc()-1]);
    int n=str.find(":");
    if(n<0) {
      QMessageBox::warning(this,"AMonPlayer - Error",
			   "Invalid command option!");
      exit(256);
    }
    main_hostname=str.left(n);
    main_channel=str.right(str.length()-n-1).toInt(&ok)-1;
    if((!ok)||(main_channel<0)||(main_channel>=(AMON_MAX_CHANNELS/2))) {
      QMessageBox::warning(this,"AMonPlayer - Error",
			   "Invalid command option!");
      exit(256);
    }
  }
  delete cmd;
  main_config=new AMMonConfig(palette());

  //
  // Create Fonts
  //
  QFont button_font=QFont("helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Create And Set Icon
  //
  main_icon_map=new QPixmap(amon_22x22_xpm);
  setIcon(*main_icon_map);
  SetCaption();

  //
  // Audio Meter
  //
  main_meter=new AMStereoMeter(AMMeter::Right,0,0,this,"main_meter");
  main_meter->setMode(AMMeter::Peak);
  main_meter->setRange(-3000,1800);
  main_meter->setHighThreshold(-1000);
  main_meter->setClipThreshold(800);
  if(meter_layout>=0) {  // Customize the meter layout
    main_config->load();
    if(main_config->meterQuantity()>=meter_layout) {
      main_meter->setRange(main_config->meterMinLevel(meter_layout-1),
		      main_config->meterMaxLevel(meter_layout-1));
      main_meter->
	setDisplayElements(main_config->meterDisplayElements(meter_layout-1));
      main_meter->
	setHighThreshold(main_config->meterYellowLevel(meter_layout-1));
      main_meter->setClipThreshold(main_config->meterRedLevel(meter_layout-1));
      main_meter->setLowColor(main_config->meterLowColor(meter_layout-1));
      main_meter->setHighColor(main_config->meterMidColor(meter_layout-1));
      main_meter->setClipColor(main_config->meterHighColor(meter_layout-1));
      main_meter->
	setDarkLowColor(main_config->meterLowBackground(meter_layout-1));
      main_meter->
	setDarkHighColor(main_config->meterMidBackground(meter_layout-1));
      main_meter->
	setDarkClipColor(main_config->meterHighBackground(meter_layout-1));
      main_meter->setAveragingPeriods(main_config->globalAveragingPeriods());
      main_meter_element_count=0;
      unsigned elems=main_config->meterDisplayElements(meter_layout-1);
      if((elems&AMStereoMeter::LeftMeter)!=0) {
	main_meter_element_count++;
      }
      if((elems&AMStereoMeter::RightMeter)!=0) {
	main_meter_element_count++;
      }
      if((elems&AMStereoMeter::SumMeter)!=0) {
	main_meter_element_count++;
      }
      if((elems&AMStereoMeter::DiffMeter)!=0) {
	main_meter_element_count++;
      }
    }
  }
  main_meter->
    setGeometry(10,10,sizeHint().width()-20,10*main_meter_element_count);

  //
  // Eject Button
  //
  main_eject_button=
    new AMTransportButton(AMTransportButton::Eject,this,"main_eject_button");
  main_eject_button->setGeometry(10,sizeHint().height()-40,50,30);
  connect(main_eject_button,SIGNAL(clicked()),this,SLOT(ejectClickedData()));

  //
  // Play Button
  //
  main_play_button=
    new AMTransportButton(AMTransportButton::Play,this,"main_play_button");
  main_play_button->setGeometry(70,sizeHint().height()-40,50,30);
  main_play_button->setDisabled(true);
  connect(main_play_button,SIGNAL(clicked()),this,SLOT(playClickedData()));

  //
  // Stop Button
  //
  main_stop_button=
    new AMTransportButton(AMTransportButton::Stop,this,"main_stop_button");
  main_stop_button->setGeometry(130,sizeHint().height()-40,50,30);
  main_stop_button->on();
  main_stop_button->setDisabled(true);
  connect(main_stop_button,SIGNAL(clicked()),this,SLOT(stopClickedData()));

  //
  // Close Button
  //
  QPushButton *button=
    new QPushButton("Close",this,"main_close_button");
  button->setGeometry(sizeHint().width()-60,sizeHint().height()-40,50,30);
  button->setFont(button_font);
  connect(button,SIGNAL(clicked()),this,SLOT(closeClickedData()));

  //
  // Player
  //
  main_player=new AMPlayer(this,"main_player");
  connect(main_player,SIGNAL(stateChanged(AMPlayer::State)),
	  this,SLOT(stateChangedData(AMPlayer::State)));
  connect(main_player,SIGNAL(error(AMPlayer::Error)),
	  this,SLOT(errorData(AMPlayer::Error)));

  //
  // Level Parser
  //
  main_parser=new AMParser(0,this,"main_parser");
  connect(main_parser,SIGNAL(levelUpdateReceived(int,AMStereoChannel *)),
	  main_meter,SLOT(updateLevel(int,AMStereoChannel *)));
  connect(main_parser,SIGNAL(error(int,int)),this,SLOT(errorData(int,int)));

  //
  // Map Signals
  //
  ::signal(SIGINT,SigHandler);

  //
  // Try to get realtime permissions
  //
  struct sched_param sp;
  int ret=0;
  memset(&sp,0,sizeof(sp));
  sp.sched_priority=50;
  ret|=sched_setscheduler(0,SCHED_FIFO,&sp);
  ret|=mlockall(MCL_CURRENT|MCL_FUTURE);
  if(ret!=0) {
    fprintf(stderr,"amonplayer: unable to get realtime permissions\n");
  }

  //
  // Start Playing If Hostname Was Specified
  //
  if(!main_hostname.isEmpty()) {
    main_play_button->setEnabled(true);
    main_stop_button->setEnabled(true);
    playClickedData();
  }
}


QSize MainWidget::sizeHint() const
{
  return QSize(300,60+10*main_meter_element_count);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::stateChangedData(AMPlayer::State state)
{
  switch(state) {
    case AMPlayer::StatePlaying:
      main_meter->setChannel(main_channel);
      main_parser->connectToHost(main_hostname,AMON_DEFAULT_SOCKET);
      main_play_button->on();
      main_stop_button->off();
      main_eject_button->setDisabled(true);
      break;

    case AMPlayer::StateStopped:
      main_parser->disconnect();
      main_meter->clear();
      main_play_button->off();
      main_stop_button->on();
      main_eject_button->setEnabled(true);
      break;
  }
}


void MainWidget::errorData(AMPlayer::Error err)
{
  QMessageBox::warning(this,"AMPlayer Error",AMPlayer::errorText(err));
}


void MainWidget::errorData(int id,int err)
{
}


void MainWidget::ejectClickedData()
{
  
  GetHost *dialog=new GetHost(&main_hostname,&main_channel,this);
  if(dialog->exec()==0) {
    SetCaption();
  }  
  main_play_button->setDisabled(main_hostname.isEmpty());
  main_stop_button->setDisabled(main_hostname.isEmpty());
  delete dialog;
}


void MainWidget::playClickedData()
{
  main_meter->setAveragingPeriods(main_config->globalAveragingPeriods());
  main_player->play(main_hostname,2*main_channel+AMON_BASE_RTP_PORT);
}


void MainWidget::stopClickedData()
{
  main_meter->setAveragingPeriods(1);
  main_player->stop();
}


void MainWidget::closeClickedData()
{
  qApp->quit();
}


void MainWidget::SetCaption()
{
  if(main_hostname.isEmpty()) {
    setCaption("AMonPlayer: [idle]");
  }
  else {
    setCaption(QString().sprintf("AMonPlayer: %s:%d",
				 (const char *)main_hostname,main_channel+1));
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  QWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->show();
  return a.exec();
}
