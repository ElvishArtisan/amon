// rtpbye.cpp
//
// (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
// A container class for RTCP bye packets.
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

#include <rtpbye.h>


RTPBye::RTPBye(unsigned version)
{
  clear();
  rtp_version=version;
}


unsigned RTPBye::version() const
{
  return rtp_version;
}


bool RTPBye::paddingBit() const
{
  return rtp_padding_bit;
}


void RTPBye::setPaddingBit(bool state)
{
  rtp_padding_bit=state;
}


unsigned RTPBye::ssrcCount() const
{
  return rtp_ssrcs.size();
}


uint32_t RTPBye::ssrc(unsigned n) const
{
  return rtp_ssrcs[n];
}


bool RTPBye::addSsrc(uint32_t ssrc)
{
  if(rtp_ssrcs.size()==RTPBYE_MAX_SSRCS) {
    return false;
  }
  rtp_ssrcs.push_back(ssrc);
  return true;
}


int RTPBye::readBlock(uint32_t *data)
{
  clear();
  if((0xff&ntohl(data[0])>>16)!=RTPBYE_PT) {
    return 0;
  }
  rtp_version=0x03&(ntohl(data[0])>>30);
  rtp_padding_bit=0x01&(ntohl(data[0])>>28);
  unsigned ssrcs=0x1f&(ntohl(data[0])>>24);
  for(unsigned i=0;i<ssrcs;i++) {
    rtp_ssrcs.push_back(ntohl(data[1+i]));
  }
  return RTPBYE_BASE_SIZE+ssrcCount();
}


int RTPBye::writeBlock(uint32_t *data,unsigned len) const
{
  if(len<(RTPBYE_BASE_SIZE+ssrcCount())) {
    return 0;
  }
  data[0]=htonl(((0x03&rtp_version)<<30)|((0x01&rtp_padding_bit)<<28)|
		((0x1f&ssrcCount())<<24)|
		((0xff&RTPBYE_PT)<<16)|
		((0xffff&(RTPBYE_BASE_SIZE+ssrcCount()-1))));
  for(unsigned i=0;i<ssrcCount();i++) {
    data[1+i]=htonl(rtp_ssrcs[i]);
  }
  return RTPBYE_BASE_SIZE+ssrcCount();
}


void RTPBye::clear()
{
  rtp_version=2;
  rtp_ssrcs.clear();
  rtp_padding_bit=false;
}
