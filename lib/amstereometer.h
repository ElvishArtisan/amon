//   amstereometer.h
//
//   A high-level stereo meter widget for AMon.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amstereometer.h,v 1.4 2007/10/06 20:32:00 fredg Exp $
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

#ifndef AMSTEREOMETER_H
#define AMSTEREOMETER_H

#include <amplaymeter.h>


class AMStereoMeter : public QWidget
{
 Q_OBJECT
 public:
  enum Elements {LeftMeter=0x0001,RightMeter=0x0002,SumMeter=0x0004,
		 DiffMeter=0x0008,SummaryAlarm=0x0010,LowAlarm=0x0020,
		 HighAlarm=0x0040,PhaseAlarm=0x0080};
  enum DisplayMode {NormalDisplay=0,PhaseDisplay=1};
  AMStereoMeter(AMMeter::Orientation o,int id,int chan,
		QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setChannel(int chan);
  void setRange(int min,int max);
  void setHighThreshold(int level);
  void setClipThreshold(int level);
  void setDarkLowColor(const QColor &color);
  void setDarkHighColor(const QColor &color);
  void setDarkClipColor(const QColor &color);
  void setLowColor(const QColor &color);
  void setHighColor(const QColor &color);
  void setClipColor(const QColor &color);
  unsigned displayElements() const;
  void setDisplayElements(unsigned elems);
  AMMeter::Mode mode() const;
  void setMode(AMMeter::Mode mode);
  AMStereoMeter::DisplayMode displayMode() const;
  void setDisplayMode(AMStereoMeter::DisplayMode mode);
  int averagingPeriods() const;
  void setAveragingPeriods(int periods);
  void clear();

 public slots:
  void updateLevel(int id,AMStereoChannel *schan);
  void setGeometry(int x,int y,int w,int h);
  void setGeometry(const QRect &rect);

 private:
  unsigned meter_display_elements;
  AMPlayMeter *meter_meters[AMStereoChannel::ChannelSize];
  AMMeter::Orientation meter_orientation;
  int meter_active_meters;
  int meter_id;
  int meter_chan;
  AMStereoMeter::DisplayMode meter_display_mode;
};


#endif  // AMSTEREOMETER_H
