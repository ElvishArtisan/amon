// rtpcontrolheader.cpp
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

#include <rtpcontrolheader.h>


RTPControlHeader::RTPControlHeader(unsigned version)
{
  rtp_sender_report=NULL;
  rtp_reception_report=NULL;
  rtp_source_description=NULL;
  rtp_bye=NULL;
  clear();
  rtp_version=version;
}


RTPControlHeader::~RTPControlHeader()
{
  clear();
}


unsigned RTPControlHeader::version() const
{

  return rtp_version;}


bool RTPControlHeader::paddingBit() const
{
  return rtp_padding_bit;
}


void RTPControlHeader::setPaddingBit(bool state)
{
  rtp_padding_bit=state;
}


RTPSenderReport *RTPControlHeader::senderReport() const
{
  return rtp_sender_report;
}


RTPReceptionReport *RTPControlHeader::receptionReport() const
{
  return rtp_reception_report;
}


RTPSourceDescription *RTPControlHeader::sourceDescription() const
{
  return rtp_source_description;
}


RTPBye *RTPControlHeader::bye() const
{
  return rtp_bye;
}


void RTPControlHeader::readBlock(uint32_t *data,unsigned len)
{
  RTPControlHeader::PacketType type=RTPControlHeader::PacketSenderReport;
  unsigned ptr=0;
  unsigned size=1;
  unsigned ver=rtp_version;

  ver=0x03&(ntohl(data[ptr])>>30);
  type=(RTPControlHeader::PacketType)(0xff&(ntohl(data[ptr])>>16));
  size=(0xffff&ntohl(data[ptr]))+1;
  while((ptr<(len/4))&&(size>0)&&(ver==rtp_version)) {
    // printf("LEN: %u  RAW: 0x%08X  PTR: %u  SIZE: %u  TYPE: %u\n",len,ntohl(data[ptr]),ptr,size,type);
    switch(type) {
      case RTPControlHeader::PacketSenderReport:
	if(rtp_sender_report==NULL) {
	  rtp_sender_report=new RTPSenderReport(0,rtp_version);
	  if(rtp_sender_report->readBlock(data+ptr)==0) {
	    delete rtp_sender_report;
	    rtp_sender_report=NULL;
	  }
	}
	break;

      case RTPControlHeader::PacketReceptionReport:
	if(rtp_reception_report==NULL) {
	  rtp_reception_report=new RTPReceptionReport(0,rtp_version);
	  if(rtp_reception_report->readBlock(data+ptr)==0) {
	    delete rtp_reception_report;
	    rtp_reception_report=NULL;
	  }
	}
	break;

      case RTPControlHeader::PacketSourceDescription:
	if(rtp_source_description==NULL) {
	  rtp_source_description=new RTPSourceDescription(rtp_version);
	  if(rtp_source_description->readBlock(data+ptr)==0) {
	    delete rtp_source_description;
	    rtp_source_description=NULL;
	  }
	}
	break;

      case RTPControlHeader::PacketBye:
	if(rtp_bye==NULL) {
	  rtp_bye=new RTPBye(rtp_version);
	  if(rtp_bye->readBlock(data+ptr)==0) {
	    delete rtp_bye;
	    rtp_bye=NULL;
	  }
	}
	break;

      case RTPControlHeader::PacketApp:
	break;
    }
    ptr+=size;
    ver=0x03&(ntohl(data[ptr])>>30);
    type=(RTPControlHeader::PacketType)(0xff&(ntohl(data[ptr])>>16));
    size=(0xffff&ntohl(data[ptr]))+1;
  }
}


void RTPControlHeader::clear()
{
  rtp_version=2;
  rtp_padding_bit=false;
  if(rtp_sender_report!=NULL) {
    delete rtp_sender_report;
    rtp_sender_report=NULL;
  }
  if(rtp_reception_report!=NULL) {
    delete rtp_reception_report;
    rtp_reception_report=NULL;
  }
  if(rtp_source_description!=NULL) {
    delete rtp_source_description;
    rtp_source_description=NULL;
  }
  if(rtp_bye!=NULL) {
    delete rtp_bye;
    rtp_bye=NULL;
  }
}
