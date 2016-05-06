//   rtpconnection.cpp
//
//   A container class for RTP connection information.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rtpconnection.cpp,v 1.1 2008/10/19 17:44:47 cvs Exp $
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

#include <rtpconnection.h>

RTPConnection::RTPConnection()
{
  clear();
}


uint32_t RTPConnection::peerAddress() const
{
  return rtp_peer_address;
}


void RTPConnection::setPeerAddress(uint32_t addr)
{
  rtp_peer_address=addr;
}


uint16_t RTPConnection::peerPort() const
{
  return rtp_peer_port;
}


void RTPConnection::setPeerPort(uint16_t port)
{
  rtp_peer_port=port;
}


time_t RTPConnection::timestamp() const
{
  return rtp_timestamp;
}


void RTPConnection::updateTimestamp()
{
  rtp_timestamp=time(&rtp_timestamp);
}


void RTPConnection::clear()
{
  rtp_peer_address=0;
  rtp_peer_port=0;
  rtp_timestamp=time(NULL);
}
