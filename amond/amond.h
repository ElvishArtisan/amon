// amond.h
//
// The AMon Audio Level Monitoring Daemon
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amond.h,v 1.8 2008/10/22 23:25:01 cvs Exp $
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

#ifndef AMOND_H
#define AMOND_H

#define AMOND_USAGE "[-d]"

#include <vector>

#include <qobject.h>
#include <qsocket.h>
#include <qserversocket.h>
#include <qsignalmapper.h>
#include <qtimer.h>
#include <qdatetime.h>

#include <portaudio.h>

#include <rtpconnection.h>
#include <rtpheader.h>
#include <rtpsourcedescription.h>
#include <rtpsenderreport.h>
#include <am.h>
#include <ammondaemonconfig.h>

void SigHandler(int signum);
void LogLine(const QString &line);

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private slots:
  void connectionData(int id,int fd);
//  void streamConnectionData(int id,int fd);
  void scanChannelsData();
  void scanConnectionsData();
  void lowLevelTimeoutData(int chan);
  void highLevelTimeoutData(int chan);
//  void rtcpSendData();
  void rtcpReadyData(int fd);
  void rtpTimeoutData();

 private:
  bool OssInit();
//  bool PaInit();
//  void PaStartCaptureDevice(int card);
  void OssStartCaptureDevice(const QString &dev,int card,int fd);
  void *OssCaptureCallback(struct oss_format *of);
  void OssProcessSdes(unsigned id,RTPSourceDescription *sd,sockaddr_in *sa);
  void OssProcessBye(unsigned id,sockaddr_in *sa);
//  void PaProcessSdes(unsigned id,RTPSourceDescription *sd,sockaddr_in *sa);
//  void PaProcessBye(unsigned id,sockaddr_in *sa);
  int GetRtpConnection(unsigned id,sockaddr_in *sa);
  uint64_t GetNtpTimestamp(const QDateTime &dt) const;
  uint32_t GetRtpTimestamp(const QDateTime &base_dt,const QDateTime &dt) const;
  //
  // Callback Parameters
  //
  int main_card;
  int main_channels;
  int main_count;
  int main_modulo;
  unsigned main_counter;

  QServerSocket *main_socket;
  std::vector<QSocket *> main_sockets;
  int main_rtp_socket[AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2)];
  RTPHeader *main_rtp_header[AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2)];
  RTPSourceDescription 
    *main_rtp_description[AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2)];
  RTPSenderReport *main_rtp_senderreport[AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2)];
  uint32_t main_rtp_timestamp;
  uint64_t main_packet_count;
  uint32_t main_octet_count;
  QDateTime main_start_datetime;
  int main_rtcp_socket[AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2)];
  std::vector<RTPConnection *> 
    main_rtp_connections[AMON_MAX_CHANNELS/(AMON_MAX_CARDS*2)];
  char main_alarms[AMON_MAX_CHANNELS/2];
  QTimer *main_low_timers[AMON_MAX_CHANNELS/2];
  QTimer *main_high_timers[AMON_MAX_CHANNELS/2];
  AMMonDaemonConfig *main_config;
  PaStream *output_pstream;
/*
  friend int AudioInputCallback(const void *input, void *output,
				unsigned long frames, 
				const PaStreamCallbackTimeInfo *timeInfo, 
				PaStreamCallbackFlags statusFlags, 
				void *userData);
*/
};


#endif  // AMOND_H
