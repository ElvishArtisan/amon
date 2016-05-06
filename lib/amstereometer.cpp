//   amstereometer.cpp
//
//   A high-level stereo meter widget for AMon.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amstereometer.cpp,v 1.7 2007/10/06 20:32:00 fredg Exp $
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

#include <amstereometer.h>

AMStereoMeter::AMStereoMeter(AMMeter::Orientation o,int id,int chan,
			     QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  meter_orientation=o;
  meter_id=id;
  meter_chan=chan;
  meter_display_mode=AMStereoMeter::NormalDisplay;

  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]=
      new AMPlayMeter(o,id,chan,(AMStereoChannel::Channel)i,this);
    meter_meters[i]->setMode(AMMeter::Independent);
  }
  setDisplayElements(AMStereoMeter::LeftMeter|AMStereoMeter::RightMeter);
}


QSize AMStereoMeter::sizeHint() const
{
  return QSize(400,300);
}


QSizePolicy AMStereoMeter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AMStereoMeter::setChannel(int chan)
{
  meter_chan=chan;
}


void AMStereoMeter::setRange(int min,int max)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setRange(min,max);
  }
}


void AMStereoMeter::setHighThreshold(int level)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setHighThreshold(level);
  }
}


void AMStereoMeter::setClipThreshold(int level)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setClipThreshold(level);
  }
}


void AMStereoMeter::setDarkLowColor(const QColor &color)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setDarkLowColor(color);
  }
}


void AMStereoMeter::setDarkHighColor(const QColor &color)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setDarkHighColor(color);
  }
}


void AMStereoMeter::setDarkClipColor(const QColor &color)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setDarkClipColor(color);
  }
}


void AMStereoMeter::setLowColor(const QColor &color)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setLowColor(color);
  }
}


void AMStereoMeter::setHighColor(const QColor &color)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setHighColor(color);
  }
}


void AMStereoMeter::setClipColor(const QColor &color)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setClipColor(color);
  }
}


unsigned AMStereoMeter::displayElements() const
{
  return meter_display_elements;
}


void AMStereoMeter::setDisplayElements(unsigned elems)
{
  meter_active_meters=0;
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    if((elems&(1<<i))!=0) {
      meter_meters[i]->show();
      meter_active_meters++;
    }
    else {
      meter_meters[i]->hide();
    }
  }
  meter_display_elements=elems;
  setGeometry(geometry());
}


AMMeter::Mode AMStereoMeter::mode() const
{
  return meter_meters[0]->mode();
}


void AMStereoMeter::setMode(AMMeter::Mode mode)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setMode(mode);
  }
}


AMStereoMeter::DisplayMode AMStereoMeter::displayMode() const
{
  return meter_display_mode;
}


void AMStereoMeter::setDisplayMode(AMStereoMeter::DisplayMode mode)
{
  meter_display_mode=mode;  
}


int AMStereoMeter::averagingPeriods() const
{
  return meter_meters[0]->averagingPeriods();
}


void AMStereoMeter::setAveragingPeriods(int periods)
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setAveragingPeriods(periods);
  }
}


void AMStereoMeter::clear()
{
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    meter_meters[i]->setPeakBar(-10000);
  }
}


void AMStereoMeter::updateLevel(int id,AMStereoChannel *schan)
{
  if((id!=meter_id)||(schan->channelNumber()!=meter_chan)) {
    return;
  }
  /*
  printf("CHAN: %d  H_ALARM: %d  L_ALARM: %d\n",
	 meter_chan,schan->alarmActive(AMStereoChannel::LowLevelAlarm),
	 schan->alarmActive(AMStereoChannel::HighLevelAlarm));
  */
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    if((meter_display_elements&(1<<i))!=0) {
      meter_meters[i]->
	setPeakBar(schan->dbuLevel((AMStereoChannel::Channel)i));
    }
  }
}


void AMStereoMeter::setGeometry(int x,int y,int w,int h)
{
  int slot=0;
  switch(meter_orientation) {
    case AMMeter::Left:
    case AMMeter::Right:
      for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
	if((meter_display_elements&(1<<i))!=0) {
	  meter_meters[i]->
	    setGeometry(0,slot++*(h/meter_active_meters),w,h/meter_active_meters);
	}
      }
      break;

    case AMMeter::Up:
    case AMMeter::Down:
      for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
	if((meter_display_elements&(1<<i))!=0) {
	  meter_meters[i]->setGeometry(slot++*(w/meter_active_meters),0,
				       w/meter_active_meters,h);
	}
      }
      break;
  }
  QWidget::setGeometry(x,y,w,h);
}


void AMStereoMeter::setGeometry(const QRect &rect)
{
  setGeometry(rect.x(),rect.y(),rect.width(),rect.height());
}
