//   amstereochannel.cpp
//
//   Abstract a stereo audio channel.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amstereochannel.cpp,v 1.3 2007/03/24 19:34:15 fredg Exp $
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

#include <stdio.h>
#include <math.h>

#include <am.h>
#include <amstereochannel.h>


AMStereoChannel::AMStereoChannel()
{
  clear();
}


AMStereoChannel::AMStereoChannel(int chan,int32_t left_lvl,int32_t right_lvl,
				 int32_t sum_lvl,int32_t diff_lvl,
				 char alarm_flags)
{
  chan_channel_number=chan;
  chan_channel_levels[AMStereoChannel::Left]=left_lvl;
  chan_channel_levels[AMStereoChannel::Right]=right_lvl;
  chan_channel_levels[AMStereoChannel::Sum]=sum_lvl;
  chan_channel_levels[AMStereoChannel::Difference]=diff_lvl;
  chan_alarm_flags=alarm_flags;
}


int AMStereoChannel::channelNumber() const
{
  return chan_channel_number;
}


void AMStereoChannel::setChannelNumber(int chan)
{
  chan_channel_number=chan;
}


int32_t AMStereoChannel::sampleLevel(AMStereoChannel::Channel chan) const
{
  return chan_channel_levels[chan];
}


void AMStereoChannel::setSampleLevel(AMStereoChannel::Channel chan,int32_t lvl)
{
  chan_channel_levels[chan]=lvl;
}


int AMStereoChannel::dbfsLevel(AMStereoChannel::Channel chan) const
{
  if(chan_channel_levels[chan]<42949) {
    return -10000;
  }
  return (int)
    (2000.0*log10(((double)(chan_channel_levels[chan]))/2147483648.0));
}


void AMStereoChannel::setDbfsLevel(AMStereoChannel::Channel chan,int lvl)
{
  chan_channel_levels[chan]=(int32_t)(2147483648.0*pow10((double)lvl/2000.0));
}


int AMStereoChannel::dbuLevel(AMStereoChannel::Channel chan) const
{
  if(chan_channel_levels[chan]<2703) {
    return -10000;
  }
  return (int)
    (AMON_LEVEL_OFFSET+2000.0*log10(((double)(chan_channel_levels[chan]))/
				    2147483648.0));
}


void AMStereoChannel::setDbuLevel(AMStereoChannel::Channel chan,int lvl)
{
  chan_channel_levels[chan]=
    (int32_t)(2147483648.0*pow10((double)(lvl-AMON_LEVEL_OFFSET)/2000.0));
}


bool AMStereoChannel::alarmActive(AMStereoChannel::Alarm alarm)
{
  if(alarm==AMStereoChannel::SummaryAlarm) {
    return chan_alarm_flags!=0;
  }
  return (chan_alarm_flags&alarm)==alarm;
}


void AMStereoChannel::clear()
{
  chan_channel_number=-1;
  for(unsigned i=0;i<AMStereoChannel::ChannelSize;i++) {
    chan_channel_levels[i]=0;
  }
  chan_alarm_flags=0;
}
