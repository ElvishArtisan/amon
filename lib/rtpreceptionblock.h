// rtpreceptionblock.h
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


#ifndef RTPRECEPTIONBLOCK_H
#define RTPRECEPTIONBLOCK_H

#include <sys/types.h>
#include <vector>
#ifdef WIN32
#include <win32_types.h>
#include <Winsock2.h>
#endif

#define RTPRECEPTIONBLOCK_MAX_SIZE 6

class RTPReceptionBlock
{
 public:
  RTPReceptionBlock(uint32_t ssrc=0);
  uint32_t ssrc() const;
  void setSsrc(uint32_t ssrc);
  double fractionPacketsLost() const;
  void setFractionPacketsLost(double fraction);
  uint32_t cumulativePacketsLost() const;
  void setCumulativePacketsLost(uint32_t count);
  uint32_t highestSequenceNumber() const;
  void setHighestSequenceNumber(uint32_t num);
  uint32_t interarrivalJitter() const;
  void setInterarrivalJitter(uint32_t jitter);
  uint32_t lastSendReportTimestamp() const;
  void setLastSendReportTimestamp(uint32_t stamp);
  uint32_t sendReportDelay() const;
  void setSendReportDelay(uint32_t delay);
  int readBlock(uint32_t *data);
  int writeBlock(uint32_t *data,unsigned len) const;
  void clear();

 private:
  uint32_t rtp_ssrc;
  double rtp_fraction_packets_lost;
  uint32_t rtp_cumulative_packets_lost;
  uint32_t rtp_highest_sequence_number;
  uint32_t rtp_interarrival_jitter;
  uint32_t rtp_last_send_report_timestamp;
  uint32_t rtp_send_report_delay;
};


#endif  // RTPRECEPTIONBLOCK_H
