// am.h
//
// The AMon Audio Level Monitoring Daemon
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: am.h,v 1.13 2008/11/18 19:13:21 cvs Exp $
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

#ifndef AM_H
#define AM_H

#include <sys/types.h>

//
// Various System-wide defines
//
#define AMON_DEFAULT_CONF_FILE "/etc/amon.conf"
#define AMON_DAEMON_CONF_FILE "/etc/amond.conf"
#define AMON_DEFAULT_SOCKET 7007
#define AMON_BASE_RTP_PORT 24024
#define AMON_RTP_TIMEOUT_INTERVAL 10000
#define AMON_CHANNEL_SCAN_INTERVAL 10
#define AMON_CONNECTION_SCAN_INTERVAL 5000
#define AMON_MAX_CHANNELS 32
#define AMON_MAX_CARDS 4
#define AMON_DEVICE_PREFIX "am"
#define AMON_LOG_FILE "/var/log/amond"
#define AMON_SAMPLERATE 44100
#define AMON_FRAMES_PER_PACKET 256
#define AMON_SAMPLERATE_TOLERANCE 100
#define AMON_PERIOD_QUANTITY 2
#define AMON_PERIOD_SIZE 2048
#define AMON_MAX_COMMAND_LENGTH 256
#define AMON_AMOND_PID_FILE "/var/run/amond.pid"
#define AMON_LEVEL_OFFSET 1800.0
#define AMON_PLAY_BUFFER_SIZE 512
#define AMON_OSS_BUFFER_SIZE 4096
#define AMON_DEFAULT_AVERAGING_PERIODS 16
#define AMON_DEFAULT_SCAN_DWELL_TIME 10
#define AMON_DEFAULT_SCAN_RESET_TIME 10
#define AMON_RTCP_SEND_INTERVAL 20

//
// Phase Locked Loop Parameters
//
#define AMON_PLL_RING_SIZE 1048576
#define AMON_PLL_INTEGRATOR_STEP 200.0
#define AMON_PLL_INTEGRATOR_PERIODS 500
#define AMON_PLL_BUFFER_SETPOINT 2
#define AMON_SRC_PCM_BUFFER_SIZE 4096

//
// Alarm Bitmasks
//
#define AMON_ALARM_LOW_LEVEL 0x01
#define AMON_ALARM_HIGH_LEVEL 0x02
#define AMON_ALARM_HIGH_DIFFERENCE 0x04

//
// Meter Block Layout
//
struct AMMeterBlock {
  int32_t left_level[AMON_MAX_CHANNELS/2];
  int32_t right_level[AMON_MAX_CHANNELS/2];
  int32_t sum_level[AMON_MAX_CHANNELS/2];
  int32_t diff_level[AMON_MAX_CHANNELS/2];
  bool block_writeable[AMON_MAX_CARDS];
};

//
// The Meter Block Shared Memory Key
//
#define AMON_METER_SHM_KEY 0x616d6f6e

#endif  // AM_H
