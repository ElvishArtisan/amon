// rtpreceptionblock.cpp
//
// (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
// A container class for RTCP reception report data blocks.
//
// For information on these data structures, see RFC 3550.
//
//  $id:$
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

#ifndef WIN32
#include <arpa/inet.h>
#endif  // WIN32

#include <rtpreceptionblock.h>

RTPReceptionBlock::RTPReceptionBlock(uint32_t ssrc)
{
  clear();
  rtp_ssrc=ssrc;
}


uint32_t RTPReceptionBlock::ssrc() const
{
  return rtp_ssrc;
}


void RTPReceptionBlock::setSsrc(uint32_t ssrc)
{
  rtp_ssrc=ssrc;
}


double RTPReceptionBlock::fractionPacketsLost() const
{
  return rtp_fraction_packets_lost;
}


void RTPReceptionBlock::setFractionPacketsLost(double fraction)
{
  rtp_fraction_packets_lost=fraction;
}


uint32_t RTPReceptionBlock::cumulativePacketsLost() const
{
  return rtp_cumulative_packets_lost;
}


void RTPReceptionBlock::setCumulativePacketsLost(uint32_t count)
{
  rtp_cumulative_packets_lost=count;
}


uint32_t RTPReceptionBlock::highestSequenceNumber() const
{
  return rtp_highest_sequence_number;
}


void RTPReceptionBlock::setHighestSequenceNumber(uint32_t num)
{
  rtp_highest_sequence_number=num;
}


uint32_t RTPReceptionBlock::interarrivalJitter() const
{
  return rtp_interarrival_jitter;
}


void RTPReceptionBlock::setInterarrivalJitter(uint32_t jitter)
{
  rtp_interarrival_jitter=jitter;
}


uint32_t RTPReceptionBlock::lastSendReportTimestamp() const
{
  return rtp_last_send_report_timestamp;
}


void RTPReceptionBlock::setLastSendReportTimestamp(uint32_t stamp)
{
  rtp_last_send_report_timestamp=stamp;
}


uint32_t RTPReceptionBlock::sendReportDelay() const
{
  return rtp_send_report_delay;
}


void RTPReceptionBlock::setSendReportDelay(uint32_t delay)
{
  rtp_send_report_delay=delay;
}


int RTPReceptionBlock::readBlock(uint32_t *data)
{
  rtp_ssrc=ntohl(data[0]);
  rtp_fraction_packets_lost=((double)(0xff&ntohl(data[1])>>24))/256.0;
  rtp_cumulative_packets_lost=0x7ff&ntohl(data[1]);
  rtp_highest_sequence_number=ntohl(data[2]);
  rtp_interarrival_jitter=ntohl(data[3]);
  rtp_last_send_report_timestamp=ntohl(data[4]);
  rtp_send_report_delay=ntohl(data[5]);
  return RTPRECEPTIONBLOCK_MAX_SIZE;
}


int RTPReceptionBlock::writeBlock(uint32_t *data,unsigned len) const
{
  if(len<RTPRECEPTIONBLOCK_MAX_SIZE) {
    return 0;
  }
  data[0]=htonl(rtp_ssrc);
  data[1]=htonl(((0x0f&(uint32_t)(256.0*rtp_fraction_packets_lost))<<24)|
		(0x07ff&rtp_cumulative_packets_lost));
  data[2]=htonl(rtp_highest_sequence_number);
  data[3]=htonl(rtp_interarrival_jitter);
  data[4]=htonl(rtp_last_send_report_timestamp);
  data[5]=htonl(rtp_send_report_delay);
  return RTPRECEPTIONBLOCK_MAX_SIZE;
}


void RTPReceptionBlock::clear()
{
  rtp_ssrc=0;
  rtp_fraction_packets_lost=0.0;
  rtp_cumulative_packets_lost=0;
  rtp_highest_sequence_number=0;
  rtp_interarrival_jitter=0;
  rtp_last_send_report_timestamp=0;
  rtp_send_report_delay=0;
}
