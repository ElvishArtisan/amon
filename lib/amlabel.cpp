//   amlabel.cpp
//
//   An alarm light widget for AMon.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amlabel.cpp,v 1.3 2008/11/04 23:22:57 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <stdlib.h>

#include <amlabel.h>

AMLabel::AMLabel(unsigned id,int chan,
		 QWidget *parent,const char *name,WFlags f)
  : QLabel(parent,name,f)
{
  label_id=id;
  label_chan=chan;
  label_active_palette=palette();
  label_alarm_type=AMStereoChannel::NoAlarm;
  label_alarm_active=false;
}


AMLabel::AMLabel(unsigned id,int chan,const QString &text,QWidget *parent,
		 const char *name,WFlags f)
  : QLabel(text,parent,name,f)
{
  label_id=id;
  label_chan=chan;
}


AMLabel::AMLabel(unsigned id,int chan,QWidget *buddy,const QString &text,
		 QWidget *parent,const char *name,WFlags f)
  : QLabel(buddy,text,parent,name,f)
{
  label_id=id;
  label_chan=chan;
}


unsigned AMLabel::id() const
{
  return label_id;
}


QPalette AMLabel::activePalette() const
{
  return label_active_palette;
}


void AMLabel::setActivePalette(const QPalette &p)
{
  label_active_palette=p;
}


void AMLabel::setPalette(const QPalette &p)
{
  label_default_palette=p;
  QLabel::setPalette(p);
}


AMStereoChannel::Alarm AMLabel::alarmType() const
{
  return label_alarm_type;
}


void AMLabel::setAlarmType(AMStereoChannel::Alarm alarm)
{
  label_alarm_type=alarm;
}


QString AMLabel::alarmAction() const
{
  return label_alarm_action;
}


void AMLabel::setAlarmAction(const QString &cmd)
{
  label_alarm_action=cmd;
}


void AMLabel::updateStatus(int id,AMStereoChannel *schan)
{
  if(label_chan!=schan->channelNumber()) {
    return;
  }
  if(schan->alarmActive(label_alarm_type)&&(!label_alarm_active)) {
    QLabel::setPalette(label_active_palette);
    if(!label_alarm_action.isEmpty()) {
      if(fork()==0) {
	system(label_alarm_action);
	exit(0);
      }
    }
    label_alarm_active=true;
  }
  if((!schan->alarmActive(label_alarm_type))&&label_alarm_active) {
    QLabel::setPalette(label_default_palette);
    label_alarm_active=false;
  }
}
