// rtpsenderreport.cpp
//
// (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
// A container class for RTCP sender reports
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

#include <qdatetime.h>

#include <rtpsenderreport.h>


RTPSenderReport::RTPSenderReport(uint32_t ssrc,unsigned version)
{
  clear();
  rtp_ssrc=ssrc;
  rtp_version=version;
}


RTPSenderReport::~RTPSenderReport()
{
  for(unsigned i=0;i<rtp_reception_blocks.size();i++) {
    delete rtp_reception_blocks[i];
  }
}


unsigned RTPSenderReport::version() const
{
  return rtp_version;
}


bool RTPSenderReport::paddingBit() const
{
  return rtp_padding_bit;
}


void RTPSenderReport::setPaddingBit(bool state)
{
  rtp_padding_bit=state;
}


uint32_t RTPSenderReport::ssrc() const
{
  return rtp_ssrc;
}


void RTPSenderReport::setSsrc(uint32_t ssrc)
{
  rtp_ssrc=ssrc;
}


uint64_t RTPSenderReport::ntpTimestamp() const
{
  return rtp_ntp_timestamp;
}


void RTPSenderReport::setNtpTimestamp(uint64_t stamp)
{
  rtp_ntp_timestamp=stamp;
}


uint32_t RTPSenderReport::rtpTimestamp() const
{
  return rtp_rtp_timestamp;
}


void RTPSenderReport::incrementRtpTimestamp(uint32_t stamp)
{
  rtp_rtp_timestamp+=stamp;
}


void RTPSenderReport::setRtpTimestamp(uint32_t stamp)
{
  rtp_rtp_timestamp=stamp;
}


uint32_t RTPSenderReport::senderPacketCount() const
{
  return rtp_sender_packet_count;
}


void RTPSenderReport::setSenderPacketCount(uint32_t count)
{
  rtp_sender_packet_count=count;
}


uint32_t RTPSenderReport::senderOctetCount() const
{
  return rtp_sender_octet_count;
}


void RTPSenderReport::setSenderOctetCount(uint32_t count)
{
  rtp_sender_octet_count=count;
}


unsigned RTPSenderReport::receptionBlocks() const
{
  return rtp_reception_blocks.size();
}


RTPReceptionBlock *RTPSenderReport::receptionBlock(unsigned n)
{
  return rtp_reception_blocks[n];
}


bool RTPSenderReport::addReceptionBlock(RTPReceptionBlock *block)
{
  if(rtp_reception_blocks.size()==RTPSENDERREPORT_MAX_BLOCKS) {
    return false;
  }
  rtp_reception_blocks.push_back(block);
  return true;
}


int RTPSenderReport::readBlock(uint32_t *data)
{
  clear();
  if((0xff&ntohl(data[0])>>16)!=RTPSENDERREPORT_PT) {
    return 0;
  }
  rtp_version=0x03&(ntohl(data[0])>>30);
  rtp_padding_bit=0x01&(ntohl(data[0])>>28);
  unsigned blocks=0x1f&(ntohl(data[0])>>24);
  rtp_ssrc=ntohl(data[1]);
  rtp_ntp_timestamp=((uint64_t)ntohl(data[2]))<<32|((uint64_t)ntohl(data[3]));
  rtp_rtp_timestamp=ntohl(data[4]);
  rtp_sender_packet_count=ntohl(data[5]);
  rtp_sender_octet_count=ntohl(data[6]);
  for(unsigned i=0;i<blocks;i++) {
    rtp_reception_blocks.push_back(new RTPReceptionBlock());
    rtp_reception_blocks.back()->
      readBlock(data+RTPSENDERREPORT_BASE_SIZE+i*RTPRECEPTIONBLOCK_MAX_SIZE);
  }
  return RTPSENDERREPORT_BASE_SIZE+blocks*RTPRECEPTIONBLOCK_MAX_SIZE;
}


int RTPSenderReport::writeBlock(uint32_t *data,unsigned len) const
{
  unsigned blocks;

  if(receptionBlocks()==0) {  // RFC 3550 requires at least one reception block
    if(len<(RTPSENDERREPORT_BASE_SIZE+RTPRECEPTIONBLOCK_MAX_SIZE)) {
      return 0;
    }
    blocks=1;
  }
  else {
    if(len<(RTPSENDERREPORT_BASE_SIZE+
	    RTPRECEPTIONBLOCK_MAX_SIZE*receptionBlocks())) {
      return 0;
    }
    blocks=receptionBlocks();
  }
  data[0]=htonl(((0x03&rtp_version)<<30)|((0x01&rtp_padding_bit)<<28)|
		((0x1f&receptionBlocks())<<24)|
		((0xff&RTPSENDERREPORT_PT)<<16)|
		((0xffff&(RTPSENDERREPORT_BASE_SIZE+
			  RTPRECEPTIONBLOCK_MAX_SIZE*blocks-1))));
  data[1]=htonl(rtp_ssrc);
  data[2]=htonl(0xffffffff&(rtp_ntp_timestamp)>>32);
  data[3]=htonl(0xffffffff&rtp_ntp_timestamp);
  data[4]=htonl(rtp_rtp_timestamp);
  data[5]=htonl(rtp_sender_packet_count);
  data[6]=htonl(rtp_sender_octet_count);
  if(receptionBlocks()==0) {  // Null Reception block
    for(unsigned i=0;i<RTPRECEPTIONBLOCK_MAX_SIZE;i++) {
      data[RTPSENDERREPORT_BASE_SIZE+i]=0;
    }
    return RTPSENDERREPORT_BASE_SIZE+RTPRECEPTIONBLOCK_MAX_SIZE;
  }
  for(unsigned i=0;i<receptionBlocks();i++) {
    rtp_reception_blocks[i]->
      writeBlock(data+RTPSENDERREPORT_BASE_SIZE+i*RTPRECEPTIONBLOCK_MAX_SIZE,
		 RTPRECEPTIONBLOCK_MAX_SIZE);
  }
  return RTPSENDERREPORT_BASE_SIZE+RTPRECEPTIONBLOCK_MAX_SIZE*receptionBlocks();
}


void RTPSenderReport::clear()
{
  rtp_version=2;
  rtp_padding_bit=false;
  rtp_ssrc=0;
  rtp_ntp_timestamp=0;
  rtp_rtp_timestamp=0;
  rtp_sender_packet_count=0;
  rtp_sender_octet_count=0;
  for(unsigned i=0;i<rtp_reception_blocks.size();i++) {
    delete rtp_reception_blocks[i];
  }
  rtp_reception_blocks.clear();
}
