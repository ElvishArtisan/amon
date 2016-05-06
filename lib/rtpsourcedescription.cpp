// rtpsourcedescription.cpp
//
// (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
// A container class for RTCP source description packets.
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

#include <rtpsourcedescription.h>

//
// FIXME: This whole class is conceptually broken, in that chunk lists need
//        to be written on a per-SSRC basis, not on a one-to-one basis
//        as is erroneously assumed by the current design.
//
//        Currently, this is hacked to work correctly with a single SSRC only.
//

RTPSourceDescription::RTPSourceDescription(unsigned version)
{
  rtp_version=version;
  clear();
}


RTPSourceDescription::~RTPSourceDescription()
{
  clear();
}


unsigned RTPSourceDescription::version() const
{
  return rtp_version;
}


bool RTPSourceDescription::paddingBit() const
{
  return rtp_padding_bit;
}


void RTPSourceDescription::setPaddingBit(bool state)
{
  rtp_padding_bit=state;
}


unsigned RTPSourceDescription::chunks() const
{
  return rtp_chunk_types.size();
}


RTPSourceDescription::ChunkType RTPSourceDescription::chunkType(unsigned n)
  const
{
  return rtp_chunk_types[n];
}


uint32_t RTPSourceDescription::chunkSsrc(unsigned n) const
{
  return rtp_chunk_ssrcs[n];
}


QString RTPSourceDescription::chunkString(unsigned n) const
{
  return QString((const char *)rtp_chunk_strings[n]);
}


unsigned RTPSourceDescription::chunkData(unsigned n,uint8_t *data,
					 unsigned maxlen)
{
  unsigned len=maxlen;
  if(rtp_chunk_lengths[n]<len) {
    len=rtp_chunk_lengths[n];
  }
  for(unsigned i=0;i<len;i++) {
    data[i]=rtp_chunk_strings[n][i];
  }
  return len;
}


bool RTPSourceDescription::addChunk(RTPSourceDescription::ChunkType type,
				    uint32_t ssrc,const QString &str)
{
  return addChunk(type,ssrc,(uint8_t *)(const char *)str,str.length());
}


bool RTPSourceDescription::addChunk(RTPSourceDescription::ChunkType type,
				    uint32_t ssrc,const uint8_t *data,
				    unsigned len)
{
  for(unsigned i=0;i<rtp_chunk_types.size();i++) {
    if(type==rtp_chunk_types[i]) {
      return false;
    }
  }
  if(len>255) {
    len=255;
  }
  rtp_chunk_types.push_back(type);
  rtp_chunk_ssrcs.push_back(ssrc);
  rtp_chunk_lengths.push_back(len);
  rtp_chunk_strings.push_back(new uint8_t[len]);
  for(unsigned i=0;i<len;i++) {
    rtp_chunk_strings.back()[i]=data[i];
  }
  return true;
}


bool RTPSourceDescription::removeChunk(unsigned n)
{
  if(n>=rtp_chunk_types.size()) {
    return false;
  }
  std::vector<RTPSourceDescription::ChunkType>::iterator it0=
    rtp_chunk_types.begin()+n;
  rtp_chunk_types.erase(it0,it0+1);
  std::vector<uint32_t>::iterator it1=rtp_chunk_ssrcs.begin()+n;
  rtp_chunk_ssrcs.erase(it1,it1+1);
  delete rtp_chunk_strings[n];
  std::vector<uint8_t *>::iterator it2=rtp_chunk_strings.begin()+n;
  rtp_chunk_strings.erase(it2,it2+1);
  std::vector<unsigned>::iterator it3=rtp_chunk_lengths.begin()+n;
  rtp_chunk_lengths.erase(it3,it3+1);
  return true;
}


int RTPSourceDescription::readBlock(uint32_t *data)
{
  uint32_t ssrc;

  clear();
  if((0xff&ntohl(data[0])>>16)!=RTPSOURCEDESCRIPTION_PT) {
    return 0;
  }
  rtp_version=0x03&(ntohl(data[0])>>30);
  rtp_padding_bit=0x01&(ntohl(data[0])>>28);
  unsigned chunks=0x1f&(ntohl(data[0])>>24);
  unsigned offset=2;
  rtp_chunk_ssrcs.push_back(ntohl(data[offset]));
  ssrc=ntohl(data[offset]);
  char *byte=(char *)(data+offset);
  for(unsigned i=0;i<chunks;i++) {
    rtp_chunk_ssrcs.push_back(ssrc);
    rtp_chunk_types.
      push_back((RTPSourceDescription::ChunkType)(0xff&byte[0]));
    unsigned len=0xff&byte[1];
    char *src=(char *)(byte+2);
    rtp_chunk_strings.push_back(new uint8_t[256]);
    for(unsigned j=0;j<len;j++) {
      rtp_chunk_strings.back()[j]=src[j];
    }
    rtp_chunk_strings.back()[len]=0;
    rtp_chunk_lengths.push_back(len);
    byte+=(2+len);
  }
  return GetTotalSize();
}


int RTPSourceDescription::writeBlock(uint32_t *data,unsigned len) const
{
  unsigned total_size=GetTotalSize();
  if(len<total_size) {
    return 0;
  }
  memset(data,0,len*sizeof(uint32_t));
  unsigned offset=2;
  data[0]=htonl(((0x03&rtp_version)<<30)|((0x01&rtp_padding_bit)<<29)|
		((0x1f&rtp_chunk_types.size())<<24)|
		((0xff&RTPSOURCEDESCRIPTION_PT)<<16)|
		((0xffff&(GetTotalSize()-1))));
  data[1]=htonl(rtp_chunk_ssrcs[0]);
  char *byte=(char *)(data+offset);
  for(unsigned i=0;i<rtp_chunk_types.size();i++) {
    byte[0]=0xff&rtp_chunk_types[i];
    byte[1]=0xff&rtp_chunk_lengths[i];
    byte+=2;
    for(unsigned j=0;j<rtp_chunk_lengths[i];j++) {
      byte[j]=rtp_chunk_strings[i][j];
    }
    byte+=rtp_chunk_lengths[i];
  }
  return total_size;
}


void RTPSourceDescription::clear()
{
  rtp_version=2;
  rtp_padding_bit=false;
  rtp_chunk_types.clear();
  rtp_chunk_ssrcs.clear();
  for(unsigned i=0;i<rtp_chunk_strings.size();i++) {
    delete rtp_chunk_strings[i];
  }
  rtp_chunk_strings.clear();
  rtp_chunk_lengths.clear();
}


unsigned RTPSourceDescription::GetTotalSize() const
{
  unsigned total=0;

  for(unsigned i=0;i<rtp_chunk_lengths.size();i++) {
    total+=2+(2+rtp_chunk_lengths[i])/4;
  }
  return total+RTPSOURCEDESCRIPTION_BASE_SIZE;
}
