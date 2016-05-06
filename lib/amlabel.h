//   amlabel.h
//
//   An alarm light widget for AMon.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amlabel.h,v 1.2 2007/10/06 21:54:34 fredg Exp $
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

#ifndef AMLABEL_H
#define AMLABEL_H


#include <qlabel.h>

#include <amstereochannel.h>

class AMLabel : public QLabel
{
  Q_OBJECT
 public:
  AMLabel(unsigned id,int chan,
	  QWidget *parent=0,const char *name=0,WFlags f=0);
  AMLabel(unsigned id,int chan,const QString &text,QWidget *parent=0,
	  const char *name=0,WFlags f=0);
  AMLabel(unsigned id,int chan,QWidget *buddy,
	  const QString &text,QWidget *parent=0,
	  const char *name=0,WFlags f=0);
  unsigned id() const;
  QPalette activePalette() const;
  void setActivePalette(const QPalette &p);
  void setPalette(const QPalette &p);
  AMStereoChannel::Alarm alarmType() const;
  void setAlarmType(AMStereoChannel::Alarm alarm);
  QString alarmAction() const;
  void setAlarmAction(const QString &cmd);

 public slots:
  void updateStatus(int id,AMStereoChannel *schan);

 private:
  int label_id;
  int label_chan;
  QPalette label_active_palette;
  QPalette label_default_palette;
  AMStereoChannel::Alarm label_alarm_type;
  bool label_alarm_active;
  QString label_alarm_action;
};

#endif  // AMLABEL_H
