// rtpbye.h
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


#ifndef RTPBYE_H
#define RTPBYE_H

#include <sys/types.h>
#include <vector>
#ifdef WIN32
#include <win32_types.h>
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <qstring.h>
#include <sys/types.h>

#define RTPBYE_BASE_SIZE 1
#define RTPBYE_MAX_SSRCS 31
#define RTPBYE_PT 203

class RTPBye
{
 public:
  RTPBye(unsigned version=2);
  unsigned version() const;
  bool paddingBit() const;
  void setPaddingBit(bool state);
  unsigned ssrcCount() const;
  uint32_t ssrc(unsigned n) const;
  bool addSsrc(uint32_t ssrc);
  int readBlock(uint32_t *data);
  int writeBlock(uint32_t *data,unsigned len) const;
  void clear();

 private:
  unsigned rtp_version;
  std::vector<uint32_t> rtp_ssrcs;
  bool rtp_padding_bit;
};


#endif  // RTPBYE_H
