//   amstereochannel.h
//
//   Abstract a stereo audio channel.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amstereochannel.h,v 1.3 2007/03/24 19:34:15 fredg Exp $
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

#ifndef AMSTEREOCHANNEL_H
#define AMSTEREOCHANNEL_H

#include <sys/types.h>

#include <am.h>

class AMStereoChannel
{
 public:
  enum Channel {Left=0,Right=1,Sum=2,Difference=3,ChannelSize=4};
  enum Alarm {NoAlarm=0,LowLevelAlarm=AMON_ALARM_LOW_LEVEL,
	      HighLevelAlarm=AMON_ALARM_HIGH_LEVEL,
	      HighDifferenceAlarm=AMON_ALARM_HIGH_DIFFERENCE,
	      SummaryAlarm=0x100};
  AMStereoChannel();
  AMStereoChannel(int chan,int32_t left_lvl,int32_t right_lvl,
		  int32_t sum_lvl,int32_t diff_lvl,char alarm_flags);
  int channelNumber() const;
  void setChannelNumber(int chan);
  int32_t sampleLevel(AMStereoChannel::Channel chan) const;
  void setSampleLevel(AMStereoChannel::Channel chan,int32_t lvl);
  int dbfsLevel(AMStereoChannel::Channel chan) const;
  void setDbfsLevel(AMStereoChannel::Channel chan,int lvl);
  int dbuLevel(AMStereoChannel::Channel chan) const;
  void setDbuLevel(AMStereoChannel::Channel chan,int lvl);
  bool alarmActive(AMStereoChannel::Alarm alarm);
  void clear();

 private:
  int chan_channel_number;
  int32_t chan_channel_levels[AMStereoChannel::ChannelSize];
  char chan_alarm_flags;
};


#endif  // AMSTEREOCHANNEL_H
