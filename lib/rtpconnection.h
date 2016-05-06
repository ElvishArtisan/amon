//   rtpconnection.h
//
//   A container class for RTP connection information.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rtpconnection.h,v 1.2 2008/10/22 23:25:01 cvs Exp $
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

#ifndef RTPCONNECTION_H
#define RTPCONNECTION_H

#include <time.h>
#include <stdint.h>

class RTPConnection
{
 public:
  RTPConnection();
  uint32_t peerAddress() const;
  void setPeerAddress(uint32_t addr);
  uint16_t peerPort() const;
  void setPeerPort(uint16_t port);
  time_t timestamp() const;
  void updateTimestamp();
  void clear();

 private:
  uint32_t rtp_peer_address;
  uint16_t rtp_peer_port;
  time_t rtp_timestamp;
};

#endif  // RTPCONNECTION_H
