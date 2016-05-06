// rtpreceptionreport.cpp
//
// (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
// A container class for RTCP reception reports
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

#include <rtpreceptionreport.h>


RTPReceptionReport::RTPReceptionReport(uint32_t ssrc,unsigned version)
{
  clear();
  rtp_ssrc=ssrc;
  rtp_version=version;
}


RTPReceptionReport::~RTPReceptionReport()
{
  for(unsigned i=0;i<rtp_reception_blocks.size();i++) {
    delete rtp_reception_blocks[i];
  }
}


unsigned RTPReceptionReport::version() const
{
  return rtp_version;
}


bool RTPReceptionReport::paddingBit() const
{
  return rtp_padding_bit;
}


void RTPReceptionReport::setPaddingBit(bool state)
{
  rtp_padding_bit=state;
}


uint32_t RTPReceptionReport::ssrc() const
{
  return rtp_ssrc;
}


void RTPReceptionReport::setSsrc(uint32_t ssrc)
{
  rtp_ssrc=ssrc;
}


unsigned RTPReceptionReport::receptionBlocks() const
{
  return rtp_reception_blocks.size();
}


RTPReceptionBlock *RTPReceptionReport::receptionBlock(unsigned n)
{
  return rtp_reception_blocks[n];
}


bool RTPReceptionReport::addReceptionBlock(RTPReceptionBlock *block)
{
  if(rtp_reception_blocks.size()==RTPRECEPTIONREPORT_MAX_BLOCKS) {
    return false;
  }
  rtp_reception_blocks.push_back(block);
  return true;
}


int RTPReceptionReport::readBlock(uint32_t *data)
{
  RTPReceptionBlock block;

  clear();
  if((0xffff&ntohl(data[0])>>16)!=RTPRECEPTIONREPORT_PT) {
    return 0;
  }
  rtp_version=0x03&(ntohl(data[0])>>30);
  rtp_padding_bit=0x01&(ntohl(data[0])>>28);
  unsigned blocks=0x1f&(ntohl(data[0])>>24);
  rtp_ssrc=ntohl(data[1]);
  for(unsigned i=0;i<blocks;i++) {
    block.
      readBlock(data+RTPRECEPTIONREPORT_BASE_SIZE+i*RTPRECEPTIONBLOCK_MAX_SIZE);
  }
  return RTPRECEPTIONREPORT_BASE_SIZE+blocks*RTPRECEPTIONBLOCK_MAX_SIZE;
}


int RTPReceptionReport::writeBlock(uint32_t *data,unsigned len) const
{
  unsigned blocks;

  if(receptionBlocks()==0) {  // RFC 3550 requires at least one reception block
    if(len<(RTPRECEPTIONREPORT_BASE_SIZE+RTPRECEPTIONBLOCK_MAX_SIZE)) {
      return 0;
    }
    blocks=1;
  }
  else {
    if(len<(RTPRECEPTIONREPORT_BASE_SIZE+
	    RTPRECEPTIONBLOCK_MAX_SIZE*receptionBlocks())) {
      return 0;
    }
    blocks=receptionBlocks();
  }
  data[0]=htonl(((0x03&rtp_version)<<30)|((0x01&rtp_padding_bit)<<28)|
		((0x1f&receptionBlocks())<<24)|
		((0xff&RTPRECEPTIONREPORT_PT)<<16)|
		((0xffff&(RTPRECEPTIONREPORT_BASE_SIZE+
			  RTPRECEPTIONBLOCK_MAX_SIZE*blocks-1))));
  data[1]=htonl(rtp_ssrc);
  if(receptionBlocks()==0) {  // Null Reception block
    for(unsigned i=0;i<RTPRECEPTIONBLOCK_MAX_SIZE;i++) {
      data[RTPRECEPTIONREPORT_BASE_SIZE+i]=0;
    }
    return RTPRECEPTIONREPORT_BASE_SIZE+RTPRECEPTIONBLOCK_MAX_SIZE;
  }
  for(unsigned i=0;i<receptionBlocks();i++) {
    rtp_reception_blocks[i]->
      writeBlock(data+i*RTPRECEPTIONBLOCK_MAX_SIZE,RTPRECEPTIONBLOCK_MAX_SIZE);
  }
  return RTPRECEPTIONREPORT_BASE_SIZE+
    RTPRECEPTIONBLOCK_MAX_SIZE*receptionBlocks();
}


void RTPReceptionReport::clear()
{
  rtp_version=2;
  rtp_padding_bit=false;
  rtp_ssrc=0;
  for(unsigned i=0;i<rtp_reception_blocks.size();i++) {
    delete rtp_reception_blocks[i];
  }
  rtp_reception_blocks.clear();
}
