// amplaymeter.h
//
//   A playback audio meter widget.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amplaymeter.h,v 1.3 2007/10/06 20:32:00 fredg Exp $
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

#ifndef AMPLAYMETER_H
#define AMPLAYMETER_H

#include <qwidget.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstring.h>
#include <qrect.h>
#include <qfont.h>

#include <amstereochannel.h>
#include <ammeter.h>

class AMPlayMeter : public QWidget
{
 Q_OBJECT
 public:
 AMPlayMeter(AMMeter::Orientation orient,int id,int chan,
	     AMStereoChannel::Channel role,
	     QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setRange(int min,int max);
  void setDarkLowColor(const QColor &color);
  void setDarkHighColor(const QColor &color);
  void setDarkClipColor(const QColor &color);
  void setLowColor(const QColor &color);
  void setHighColor(const QColor &color);
  void setClipColor(const QColor &color);
  void setHighThreshold(int level);
  void setClipThreshold(int level);
  void setSegmentSize(int size);
  void setSegmentGap(int gap);
  AMMeter::Mode mode() const;
  void setMode(AMMeter::Mode mode);
  void setLabel(QString label);
  int averagingPeriods() const;
  void setAveragingPeriods(int periods);

 public slots:
  void setGeometry(int x,int y,int w,int h);
  void setGeometry(QRect &rect);
  void setSolidBar(int level);
  void setFloatingBar(int level);
  void setPeakBar(int level);
  void updateLevel(int id,AMStereoChannel *schan);

 protected:
  void paintEvent(QPaintEvent *);

 private:
  void makeFont();
  AMMeter *meter;
  QString meter_label;
  QFont label_font;
  AMMeter::Orientation orientation;
  int meter_label_x;
  int meter_id;
  int meter_chan;
  AMStereoChannel::Channel meter_role;
};


#endif
