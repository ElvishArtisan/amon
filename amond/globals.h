// globals.h
//
// The AMon Audio Level Monitoring Daemon
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.8 2008/10/22 23:25:01 cvs Exp $
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
#ifndef GLOBALS_H
#define GLOBALS_H

#include <alsa/asoundlib.h>

#include <am.h>
#include <amringbuffer.h>

struct oss_format {
  int card;
  pthread_t thread;
  int fd;
  int play_fd;
  unsigned channels;
  char *card_buffer;
  //  int ptr;
  bool exiting;
  int count;
  int modulo;
};

//
// Global Variables
//
extern bool debug_mode;
extern volatile int left_level[AMON_MAX_CHANNELS/2];
extern volatile int right_level[AMON_MAX_CHANNELS/2];
extern volatile int sum_level[AMON_MAX_CHANNELS/2];
extern volatile int diff_level[AMON_MAX_CHANNELS/2];
extern volatile bool block_writable[AMON_MAX_CARDS];
extern struct alsa_format alsa_format[AMON_MAX_CARDS];
extern struct oss_format oss_format[AMON_MAX_CARDS];
extern struct AMMeterBlock *meter_block;
extern AmRingBuffer *ring_buffer[AMON_MAX_CARDS];

#endif  // GLOBALS_H
