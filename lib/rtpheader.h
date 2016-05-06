// rtpheader.h
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


#ifndef RTPHEADER_H
#define RTPHEADER_H

#include <sys/types.h>
#include <vector>
#include <stdint.h>
#ifdef WIN32
#include <win32_types.h>
#include <Winsock2.h>
#endif

#define RTPHEADER_MIN_SIZE 8
#define RTPHEADER_MAX_CSRCS 15
#define RTPHEADER_MAX_SIZE 72

class RTPHeader
{
 public:
  //
  // FIXME: The payload values for Speex (20, 21, 22) and Vorbis (23) are 
  //        totally bogus.  RFC3551 specifies that these should be negotiated 
  //        dynamically.
  //
  enum PayloadType {PayloadPcmu=0,PayloadGsm=3,PayloadG723=4,PayloadDvi4_8000=5,
		    PayloadDvi4_16000=6,PayloadLpc=7,PayloadPcma=8,
		    PayloadG722=9,PayloadL16_2=10,PayloadL16_1=11,
		    PayloadQcelp=12,PayloadCn=13,PayloadMpa=14,PayloadG728=15,
		    PayloadDvi4_11025=16,PayloadDvi4_22050=17,PayloadG729=18,
		    PayloadSpeexNarrow=20,PayloadSpeexWide=21,
		    PayloadSpeexSuperWide=22,PayloadVorbis=23,
		    PayloadCelB=25,PayloadJpeg=26,PayloadNv=28,PayloadH261=31,
		    PayloadMpv=32,PayloadMp2t=33,PayloadH263=34,
		    PayloadDynamicFirst=96,PayloadDynamicLast=127};
  RTPHeader(RTPHeader::PayloadType type,unsigned version=2);
  unsigned version() const;
  bool paddingBit() const;
  void setPaddingBit(bool state);
  bool extensionBit() const;
  void setExtensionBit(bool state);
  bool markerBit() const;
  void setMarkerBit(bool state);
  RTPHeader::PayloadType payloadType() const;
  void setPayloadType(RTPHeader::PayloadType type);
  uint16_t sequenceNumber() const;
  void setSequenceNumber(uint16_t num);
  uint32_t timestamp() const;
  void setTimestamp(uint32_t stamp);
  uint32_t timestampOrigin() const;
  void setTimestampOrigin(uint32_t stamp);
  unsigned timestampInterval() const;
  void setTimestampInterval(unsigned interval);
  uint32_t ssrc() const;
  void setSsrc(uint32_t ssrc);
  unsigned csrcCount() const;
  uint32_t csrc(unsigned n) const;
  bool addCsrc(uint32_t csrc);
  void removeCsrc(unsigned n);
  RTPHeader &operator++();
  RTPHeader operator++(int);
  int readBlock(uint32_t *data,unsigned len);
  int writeBlock(uint32_t *data,unsigned len) const;
  void clear();

 private:
  unsigned rtp_version;
  bool rtp_padding_bit;
  bool rtp_extension_bit;
  bool rtp_marker_bit;
  RTPHeader::PayloadType rtp_payload_type;
  uint16_t rtp_sequence_number;
  uint32_t rtp_timestamp;
  uint32_t rtp_timestamp_origin;
  unsigned rtp_timestamp_interval;
  uint32_t rtp_ssrc;
  unsigned rtp_random_seedp;
  std::vector<uint32_t> rtp_csrcs;
};


#endif  // RTPHEADER_H
