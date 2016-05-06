// rtpsourcedescription.h
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


#ifndef RTPSOURCEDESCRIPTION_H
#define RTPSOURCEDESCRIPTION_H

#include <sys/types.h>
#include <vector>
#ifdef WIN32
#include <win32_types.h>
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <qstring.h>

#define RTPSOURCEDESCRIPTION_BASE_SIZE 1

//
// Purely theoretical -- we'd be having problems with MTU sizes *way* before
// we get near this!
//
#define RTPSOURCEDESCRIPTION_MAX_SIZE 7906
#define RTPSOURCEDESCRIPTION_PT 202

class RTPSourceDescription
{
 public:
  enum ChunkType {ChunkCname=1,ChunkName=2,ChunkEmail=3,ChunkPhone=4,
		  ChunkLoc=5,ChunkTool=6,ChunkNote=7,ChunkPriv=8};
  RTPSourceDescription(unsigned version=2);
  ~RTPSourceDescription();
  unsigned version() const;
  bool paddingBit() const;
  void setPaddingBit(bool state);
  unsigned chunks() const;
  RTPSourceDescription::ChunkType chunkType(unsigned n) const;
  uint32_t chunkSsrc(unsigned n) const;
  QString chunkString(unsigned n) const;
  unsigned chunkData(unsigned n,uint8_t *data,unsigned maxlen);
  bool addChunk(RTPSourceDescription::ChunkType type,uint32_t ssrc,
		const QString &str);
  bool addChunk(RTPSourceDescription::ChunkType type,uint32_t ssrc,
		const uint8_t *data,unsigned len);
  bool removeChunk(unsigned n);
  int readBlock(uint32_t *data);
  int writeBlock(uint32_t *data,unsigned len) const;
  void clear();

 private:
  unsigned GetTotalSize() const;
  unsigned rtp_version;
  bool rtp_padding_bit;
  std::vector<RTPSourceDescription::ChunkType> rtp_chunk_types;
  std::vector<uint32_t> rtp_chunk_ssrcs;
  std::vector<uint8_t *> rtp_chunk_strings;
  std::vector<unsigned> rtp_chunk_lengths;
//  std::vector<QString> rtp_chunk_strings;
};


#endif  // RTPSOURCEDESCRIPTION_H
