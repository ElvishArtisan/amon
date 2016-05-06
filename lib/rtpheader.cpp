// rtpheader.cpp
//
// (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
// A container class for RTP header data.
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

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <arpa/inet.h>
#endif  // WIN32

#include <qdatetime.h>

#include <rtpheader.h>

RTPHeader::RTPHeader(RTPHeader::PayloadType type,unsigned version)
{
  //
  // Seed the random number generator
  //
  rtp_random_seedp=QTime::currentTime().msec();

  clear();
  rtp_version=version;
  rtp_payload_type=type;
  switch(rtp_payload_type) {  // As per RFC 3551 Table 4
    case PayloadPcmu:
      rtp_timestamp_interval=8000;
      break;

    case PayloadGsm:
      rtp_timestamp_interval=8000;
      break;

    case PayloadG723:
      rtp_timestamp_interval=8000;
      break;

    case PayloadDvi4_8000:
      rtp_timestamp_interval=8000;
      break;

    case PayloadDvi4_16000:
      rtp_timestamp_interval=16000;
      break;

    case PayloadLpc:
      rtp_timestamp_interval=8000;
      break;

    case PayloadPcma:
      rtp_timestamp_interval=8000;
      break;

    case PayloadG722:
      rtp_timestamp_interval=8000;
      break;

    case PayloadL16_2:
      rtp_timestamp_interval=44100;
      break;

    case PayloadL16_1:
      rtp_timestamp_interval=44100;
      break;

    case PayloadQcelp:
      rtp_timestamp_interval=8000;
      break;

    case PayloadCn:
      rtp_timestamp_interval=8000;
      break;

    case PayloadMpa:
      rtp_timestamp_interval=90000;
      break;

    case PayloadG728:
      rtp_timestamp_interval=8000;
      break;

    case PayloadDvi4_11025:
      rtp_timestamp_interval=11025;
      break;

    case PayloadDvi4_22050:
      rtp_timestamp_interval=22050;
      break;

    case PayloadG729:
      rtp_timestamp_interval=8000;
      break;

    case PayloadSpeexNarrow:
      rtp_timestamp_interval=8000;
      break;

    case PayloadSpeexWide:
      rtp_timestamp_interval=16000;
      break;

    case PayloadSpeexSuperWide:
      rtp_timestamp_interval=32000;
      break;

    case PayloadCelB:
      rtp_timestamp_interval=90000;
      break;

    case PayloadJpeg:
      rtp_timestamp_interval=90000;
      break;

    case PayloadNv:
      rtp_timestamp_interval=90000;
      break;

    case PayloadH261:
      rtp_timestamp_interval=90000;
      break;

    case PayloadMpv:
      rtp_timestamp_interval=90000;
      break;

    case PayloadMp2t:
      rtp_timestamp_interval=90000;
      break;

    case PayloadH263:
      rtp_timestamp_interval=90000;
      break;

    default:
      rtp_timestamp_interval=8000;
      break;
  }
}


unsigned RTPHeader::version() const
{
  return rtp_version;
}


bool RTPHeader::paddingBit() const
{
  return rtp_padding_bit;
}


void RTPHeader::setPaddingBit(bool state)
{
  rtp_padding_bit=state;
}


bool RTPHeader::extensionBit() const
{
  return rtp_extension_bit;
}


void RTPHeader::setExtensionBit(bool state)
{
  rtp_extension_bit=state;
}


bool RTPHeader::markerBit() const
{
  return rtp_marker_bit;
}


void RTPHeader::setMarkerBit(bool state)
{
  rtp_marker_bit=state;
}


RTPHeader::PayloadType RTPHeader::payloadType() const
{
  return rtp_payload_type;
}


void RTPHeader::setPayloadType(RTPHeader::PayloadType type)
{
  rtp_payload_type=type;
}


uint16_t RTPHeader::sequenceNumber() const
{
  return rtp_sequence_number;
}


void RTPHeader::setSequenceNumber(uint16_t num)
{
  rtp_sequence_number=num;
}


uint32_t RTPHeader::timestamp() const
{
  return rtp_timestamp;
}


void RTPHeader::setTimestamp(uint32_t stamp)
{
  rtp_timestamp=stamp;
}


uint32_t RTPHeader::timestampOrigin() const
{
  return rtp_timestamp_origin;
}


void RTPHeader::setTimestampOrigin(uint32_t stamp)
{
  rtp_timestamp_origin=stamp;
}


unsigned RTPHeader::timestampInterval() const
{
  return rtp_timestamp_interval;
}


void RTPHeader::setTimestampInterval(unsigned interval)
{
  rtp_timestamp_interval=interval;
}


uint32_t RTPHeader::ssrc() const
{
  return rtp_ssrc;
}


void RTPHeader::setSsrc(uint32_t ssrc)
{
  rtp_ssrc=ssrc;
}


unsigned RTPHeader::csrcCount() const
{
  return rtp_csrcs.size();
}


uint32_t RTPHeader::csrc(unsigned n) const
{
  return rtp_csrcs[n];
}


bool RTPHeader::addCsrc(uint32_t csrc)
{
  if(csrcCount()==RTPHEADER_MAX_CSRCS) {
    return false;
  }
  rtp_csrcs.push_back(csrc);
  return true;
}


void RTPHeader::removeCsrc(unsigned n)
{
  std::vector<uint32_t>::iterator it=rtp_csrcs.begin();
  rtp_csrcs.erase(it,it+1);
}


RTPHeader &RTPHeader::operator++()
{
  rtp_sequence_number++;
  rtp_timestamp+=rtp_timestamp_interval;
  return *this;
}


RTPHeader RTPHeader::operator++(int)
{
  RTPHeader ret=*this;
  operator++();
  return ret;
}


int RTPHeader::readBlock(uint32_t *data,unsigned len)
{
  int ret=0;

  clear();
  uint32_t fields=ntohl(data[0]);
  rtp_version=fields>>30;
  rtp_padding_bit=(fields>>28)&0x01;
  rtp_extension_bit=(fields>>27)&0x01;
  unsigned csrc_count=(fields>>24)&0x0f;
  rtp_marker_bit=(fields>>23)&0x01;
  rtp_payload_type=(RTPHeader::PayloadType)((fields>>16)&0x7f);
  rtp_sequence_number=fields&0xffff;
  rtp_timestamp=ntohl(data[1]);
  rtp_ssrc=ntohl(data[2]);
  for(unsigned i=0;i<csrc_count;i++) {
    addCsrc(ntohl(data[3+i]));
  }
  if(extensionBit()) {
    ret=3+csrcCount()+(0xffff&ntohl(data[4+csrcCount()]));
  }
  else {
    ret=3+csrcCount();
  }
  if(ret>len) {
    fprintf(stderr,"RTPHeader: corrupt RTP header\n");
    return 0;
  }
  return ret;
}


int RTPHeader::writeBlock(uint32_t *data,unsigned len) const
{
  if(len<(3+csrcCount())) {
    return 0;
  }
  data[0]=htonl(((0x03&rtp_version)<<30)|((0x01&rtp_padding_bit)<<28)|
		((0x01&rtp_extension_bit<<27))|((0x0f&csrcCount())<<24)|
		((0x01&rtp_marker_bit)<<23)|((0x7f&rtp_payload_type)<<16)|
		(0xffff&rtp_sequence_number));
  data[1]=htonl(rtp_timestamp);
  data[2]=htonl(rtp_ssrc);
  for(unsigned i=0;i<csrcCount();i++) {
    data[3+i]=htonl(csrc(i));
  }
  return 3+csrcCount();
}


void RTPHeader::clear()
{
  rtp_version=0;
  rtp_padding_bit=false;
  rtp_extension_bit=false;
  rtp_marker_bit=false;
  rtp_payload_type=RTPHeader::PayloadL16_2;
#ifdef WIN32
  rtp_sequence_number=0xffff&rand();
#else
  rtp_sequence_number=0xffff&rand_r(&rtp_random_seedp);
#endif  // WIN32
  rtp_timestamp_origin=rand_r(&rtp_random_seedp);
  rtp_timestamp=rtp_timestamp_origin;
  rtp_timestamp_interval=44100;
  rtp_ssrc=0;
  rtp_csrcs.clear();
}
