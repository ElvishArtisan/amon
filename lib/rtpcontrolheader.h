// rtpcontrolheader.h
//
// (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
// A container class for RTCP packet header data.
//
// For information on these data structures, see RFCs 3550 and 3551.
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


#ifndef RTPCONTROLHEADER_H
#define RTPCONTROLHEADER_H

#include <sys/types.h>
#ifdef WIN32
#include <win32_types.h>
#include <Winsock2.h>
#endif

#include <rtpsenderreport.h>
#include <rtpreceptionreport.h>
#include <rtpsourcedescription.h>
#include <rtpbye.h>

class RTPControlHeader
{
 public:
  enum PacketType {PacketSenderReport=200,PacketReceptionReport=201,
		   PacketSourceDescription=202,PacketBye=203,PacketApp=204};
  RTPControlHeader(unsigned version=2);
  ~RTPControlHeader();
  unsigned version() const;
  bool paddingBit() const;
  void setPaddingBit(bool state);
  RTPSenderReport *senderReport() const;
  RTPReceptionReport *receptionReport() const;
  RTPSourceDescription *sourceDescription() const;
  RTPBye *bye() const;
  void readBlock(uint32_t *data,unsigned len);
  void clear();

 private:
  unsigned rtp_version;
  bool rtp_padding_bit;
  RTPSenderReport *rtp_sender_report;
  RTPReceptionReport *rtp_reception_report;
  RTPSourceDescription *rtp_source_description;
  RTPBye *rtp_bye;
};


#endif  // RTPCONTROLHEADER_H
