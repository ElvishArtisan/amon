// amplayer.h
//
//   A virtual audio player for AMMon.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amplayer.h,v 1.4 2008/10/22 23:25:01 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef AMPLAYER_H
#define AMPLAYER_H

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
//#include <pthread.h>

#include <qobject.h>
#include <qhostaddress.h>
#include <qsocketdevice.h>
#include <qsocket.h>
#include <qtimer.h>
#include <qdns.h>

#include <samplerate.h>
#include <portaudio.h>

#include <am.h>
#include <amringbuffer.h>
#include <ammeteraverage.h>

#define AMPLAYER_POLL_INTERVAL 100
#define AMPLAYER_OSS_DEVICE "/dev/dsp"
#define AMPLAYER_BUFFER_FRAGMENTS 2

struct AMPlayerFormat {
  pthread_t thread;
  int fd;
  QHostAddress addr;
  Q_UINT16 port;
  bool state;
  QSocketDevice::Error error;
};

class AMPlayer : public QObject
{
 Q_OBJECT
 public:
  enum Error {ErrOk=0,ErrUnknown=1,ErrInvalidFormat=2,ErrInvalidChannels=3,
	      ErrInvalidSampleRate=4,ErrConnectionRefused=5,
	      ErrHostNotFound=6,ErrAlreadyBound=7,ErrInaccessible=8,
	      ErrNoResources=9,ErrInternal=10,ErrImpossible=11,
	      ErrNoFiles=12,ErrNetworkFailure=13};
  enum State {StateStopped=0,StatePlaying=1};
  AMPlayer(QObject *parent=0,const char *name=0);
  static QString errorText(AMPlayer::Error err);

 public slots:
  void play(const QString &hostname,Q_UINT16 port);
  void stop();

 signals:
  void stateChanged(AMPlayer::State state);
  void error(AMPlayer::Error err);

 private slots:
  void dnsResultsReadyData();
  void dnsTimerData();

 private:
  bool AudioInit(const QHostAddress &addr,Q_UINT16 port,PaDeviceIndex dev);
  void AudioFree();
  //bool OssInit(const QString &dev);
  char player_oss_buffer[AMON_PLAY_BUFFER_SIZE];
  int player_rtp_socket;
  int player_rtcp_socket;
  unsigned player_rtcp_trans_counter;
  sockaddr_in player_rtcp_sa;
  AmRingBuffer *player_ring;
  SRC_STATE *player_src_state;
  AmMeterAverage *player_pll_average;
  double player_pll_offset;
  bool player_pll_locked;
  Q_UINT16 player_port;
  AMPlayer::State player_state;
  QTimer *player_dns_timer;
  QDns *player_dns;
  PaStream *player_pstream;
  friend int AudioOutputCallback(const void *input, void *output,
				 unsigned long frameCount, 
				 const PaStreamCallbackTimeInfo *timeInfo, 
				 PaStreamCallbackFlags statusFlags,
				 void *userData);
  friend void SendRtcpPacket(AMPlayer *player);
};


#endif  // AMPLAYER_H
