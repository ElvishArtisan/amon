// amplayer.cpp
//
//   A virtual audio player for AMMon.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amplayer.cpp,v 1.5 2008/10/22 23:25:01 cvs Exp $
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

#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/socket.h>

#include <rtpheader.h>
#include <rtpsourcedescription.h>
#include <rtpbye.h>
#include <amplayer.h>


void SendRtcpPacket(AMPlayer *player)
{
  int n;
  char buffer[3000];

  //
  // Send RTCP packet
  //
  if(player->player_rtcp_trans_counter++>20) {
    RTPSourceDescription *rtp=new RTPSourceDescription();
    rtp->addChunk(RTPSourceDescription::ChunkCname,1234,"fredg@asterisk");
    n=rtp->writeBlock((uint32_t *)buffer,325);
    sendto(player->player_rtcp_socket,buffer,n*4,0,
	   (sockaddr *)&player->player_rtcp_sa,sizeof(player->player_rtcp_sa));
    delete rtp;
    player->player_rtcp_trans_counter=0;
  }
}

int AudioOutputCallback(const void *input, void *output, 
			unsigned long frameCount, 
			const PaStreamCallbackTimeInfo *timeInfo, 
			PaStreamCallbackFlags statusFlags, void *userData)
{
  char buffer[3000];
  int16_t *abuf=(int16_t *)output;
  int n=0;
  int l=0;
  RTPHeader hdr(RTPHeader::PayloadL16_2);
  AMPlayer *player=(AMPlayer *)userData;
  SRC_DATA src_data;
  float src_in[AMON_SRC_PCM_BUFFER_SIZE];
  float src_out[AMON_SRC_PCM_BUFFER_SIZE];

  //
  // Feed Ringbuffer
  //
  if((n=recv(player->player_rtp_socket,buffer,1500,0))>0) {
    l=hdr.readBlock((uint32_t *)buffer,n/4);
    for(int i=0;i<((n-4*l)/2);i++) {
      abuf[i]=ntohs(((int16_t *)(buffer+4*l))[i]);
    }
    player->player_pll_offset=player->player_pll_average->average();
    src_short_to_float_array(abuf,src_in,(n-4*l)/2);
    memset(&src_data,0,sizeof(src_data));
    src_data.data_in=src_in;
    src_data.data_out=src_out;
    src_data.input_frames=(n-4*l)/4;
    src_data.output_frames=AMON_SRC_PCM_BUFFER_SIZE/2;
    src_data.src_ratio=
      (AMON_SAMPLERATE+player->player_pll_offset)/AMON_SAMPLERATE;
    src_set_ratio(player->player_src_state,src_data.src_ratio);
    src_process(player->player_src_state,&src_data);
    src_float_to_short_array(src_out,(int16_t *)buffer,
			     src_data.output_frames_gen*2);
    player->player_ring->write((char *)buffer,4*src_data.output_frames_gen);
  }

  //
  // Prefill the ring buffer before unmuting
  //
//  printf("readSpace: %ld  setpt: %lu\n",codec->codec_ring->readSpace(),      
//	 (2*frameCount*codec->codec_channels*CODEC_BUFFER_SETPOINT));
  if((player->player_ring->readSpace()<
     (4*frameCount*AMON_PLL_BUFFER_SETPOINT))&&(!player->player_pll_locked)) {
    memset(abuf,0,2*frameCount*sizeof(int16_t));
    SendRtcpPacket(player);
    return paContinue;
  }

  //
  // Phase Detector
  //
  player->player_pll_locked=true;
  if(player->player_ring->readSpace()<
     (4*frameCount*AMON_PLL_BUFFER_SETPOINT)) {
    player->player_pll_average->addValue(AMON_PLL_INTEGRATOR_STEP);
  }
  else {
    player->player_pll_average->addValue(-AMON_PLL_INTEGRATOR_STEP);
  }

  //
  // Process Output
  //
  if(player->player_ring->readSpace()>=(4*frameCount)) {
    player->player_ring->read((char *)abuf,4*frameCount);
  }
  else {
    for(unsigned long i=0;i<(2*frameCount);i++) {
      abuf[i]=0;
    }
  }
/*
  printf("PLL Offset: %6.1lf  Ratio: %6.4lf  Buffered: %d\n",
	 player->player_pll_offset,
	 src_data.src_ratio,
	 player->player_ring->readSpace());
*/
  SendRtcpPacket(player);
  
  return paContinue;
}


AMPlayer::AMPlayer(QObject *parent,const char *name)
  : QObject(parent,name)
{
  player_state=AMPlayer::StateStopped;
  player_rtcp_trans_counter=0;
  player_src_state=NULL;

  //
  // DNS Resolver
  //
  player_dns=NULL;

  //
  // DNS Reset Timer
  //
  player_dns_timer=new QTimer(this,"player_dns_timer");
  connect(player_dns_timer,SIGNAL(timeout()),
	  this,SLOT(dnsTimerData()));

  //
  // Phase Locked Loop
  //
  player_ring=new AmRingBuffer(AMON_PLL_RING_SIZE);
  player_pll_offset=0.0;
  player_pll_average=new AmMeterAverage(AMON_PLL_INTEGRATOR_PERIODS);

  Pa_Initialize();
}


void AMPlayer::play(const QString &hostname,Q_UINT16 port)
{
  if(player_state!=AMPlayer::StateStopped) {
    return;
  }
  player_port=port;
  player_dns=new QDns(hostname,QDns::A);
  connect(player_dns,SIGNAL(resultsReady()),this,SLOT(dnsResultsReadyData()));
}


void AMPlayer::stop()
{
  AudioFree();
}


QString AMPlayer::errorText(AMPlayer::Error err)
{
  switch(err) {
    case AMPlayer::ErrOk:
      return "OK";

    case AMPlayer::ErrUnknown:
      return "Unknown Error";

    case AMPlayer::ErrInvalidFormat:
      return "Unsupported audio format";

    case AMPlayer::ErrInvalidChannels:
      return "Unsupported channel count";

    case AMPlayer::ErrInvalidSampleRate:
      return "Unsupported sample rate";

    case AMPlayer::ErrConnectionRefused:
      return "Connection refused";

    case AMPlayer::ErrHostNotFound:
      return "Host not found";

    case AMPlayer::ErrAlreadyBound:
      return "Socket already bound";

    case AMPlayer::ErrInaccessible:
      return "Socket not accessible";

    case AMPlayer::ErrNoResources:
      return "Insufficient resources";

    case AMPlayer::ErrInternal:
      return "Internal error";

    case AMPlayer::ErrImpossible:
      return "Impossible error";   // !?!

    case AMPlayer::ErrNoFiles:
      return "No files";           // ???

    case AMPlayer::ErrNetworkFailure:
      return "Network failure";
  }
  return "Unknown Error";
}


void AMPlayer::dnsResultsReadyData()
{
  QValueList<QHostAddress> addrlist=player_dns->addresses();
  QValueList<QHostAddress>::Iterator it=addrlist.begin();
  while(it!=addrlist.end()) {
    if(!(*it).isNull()) {
      if(!AudioInit(*it,player_port,Pa_GetDefaultOutputDevice())) {
	return;
      }
      player_dns_timer->start(10,true);
      return;
    }
    it++;
  }
  emit error(AMPlayer::ErrHostNotFound);
}


void AMPlayer::dnsTimerData()
{
  if((player_dns!=NULL)&&(!player_dns->isWorking())) {
    delete player_dns;
    player_dns=NULL;
  }
/*
  if(player_fmt.state) {
    return;
  }
  player_state_timer->stop();
  pthread_join(player_fmt.thread,NULL);
  ::close(player_fmt.fd);
  switch(player_fmt.error) {
    case QSocketDevice::NoError:
      break;
      
    case QSocketDevice::AlreadyBound:
      emit error(AMPlayer::ErrAlreadyBound);
      break;
      
    case QSocketDevice::Inaccessible:
      emit error(AMPlayer::ErrInaccessible);
      break;
      
    case QSocketDevice::NoResources:
      emit error(AMPlayer::ErrNoResources);
      break;
      
    case QSocketDevice::InternalError:
      emit error(AMPlayer::ErrInternal);
      break;
      
    case QSocketDevice::Impossible:
      emit error(AMPlayer::ErrImpossible);
      break;
      
    case QSocketDevice::NoFiles:
      emit error(AMPlayer::ErrNoFiles);
      break;
      
    case QSocketDevice::ConnectionRefused:
      emit error(AMPlayer::ErrConnectionRefused);
      break;
      
    case QSocketDevice::NetworkFailure:
      emit error(AMPlayer::ErrNetworkFailure);
      break;
      
    case QSocketDevice::UnknownError:
      emit error(AMPlayer::ErrUnknown);
      break;
  }
  player_state=AMPlayer::StateStopped;
  emit stateChanged(AMPlayer::StateStopped);
*/
}


bool AMPlayer::AudioInit(const QHostAddress &addr,Q_UINT16 port,
			 PaDeviceIndex dev)
{
  PaError perr;
  int src_err;
  struct PaStreamParameters out_params;
  sockaddr_in sa;
  int n;
  char buffer[1500];

  //
  // Open RTP Socket
  //
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  sa.sin_port=htons(port+32);
  player_rtp_socket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if(player_rtp_socket<0) {
    printf("QSocketDevice::NoResources\n");
    Pa_CloseStream(player_pstream);
    return false;
  }
  uint8_t tos=IPTOS_LOWDELAY;
  if(setsockopt(player_rtp_socket,IPPROTO_IP,IP_TOS,&tos,sizeof(tos))<0) {
    printf("Unable to set IPTOS_LOWDELAY\n");
    ::close(player_rtp_socket);
    Pa_CloseStream(player_pstream);
    return false;
  }
  if(bind(player_rtp_socket,(sockaddr *)&sa,sizeof(sa))<0) {
    printf("QSocketDevice::AlreadyBound\n");
    ::close(player_rtp_socket);
    Pa_CloseStream(player_pstream);
    return false;
  }
  fcntl(player_rtp_socket,F_SETFL,O_NONBLOCK);

  //
  // Open RTCP Socket
  //
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  sa.sin_port=htons(port+33);
  player_rtcp_socket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if(player_rtcp_socket<0) {
    printf("QSocketDevice::NoResources\n");
    ::close(player_rtp_socket);
    Pa_CloseStream(player_pstream);
    return false;
  }
  if(bind(player_rtcp_socket,(sockaddr *)&sa,sizeof(sa))<0) {
    printf("QSocketDevice::AlreadyBound\n");
    ::close(player_rtp_socket);
    ::close(player_rtcp_socket);
    Pa_CloseStream(player_pstream);
    return false;
  }
  fcntl(player_rtcp_socket,F_SETFL,O_NONBLOCK);
  memset(&player_rtcp_sa,0,sizeof(player_rtcp_sa));
  player_rtcp_sa.sin_family=AF_INET;
  player_rtcp_sa.sin_addr.s_addr=htonl(addr.toIPv4Address());
  player_rtcp_sa.sin_port=htons(port+1);

  // printf("Addr: %s:%d\n",(const char *)addr.toString(),(int)(0xffff&port));

  //
  // Send Initial RTCP packet
  //
  RTPSourceDescription *rtp=new RTPSourceDescription();
  rtp->addChunk(RTPSourceDescription::ChunkCname,1234,"fredg@asterisk");
  n=rtp->writeBlock((uint32_t *)buffer,325);
  sendto(player_rtcp_socket,buffer,n*4,0,(sockaddr *)&player_rtcp_sa,
	 sizeof(player_rtcp_sa));
  delete rtp;

  //
  // Phase Locked Loop
  //
/*
  codec_recv_total_packets_lost=0;
  codec_recv_interval_packets_lost=0;
  codec_recv_sequence_lo_number=0;
  codec_recv_sequence_hi_number=0;
  codec_recv_last_sequence_number=0;
  codec_recv_ssrc=0;
  codec_recv_ntp_timestamp=0;
  codec_recv_start_sequence_number=0;
  codec_recv_start_packet_count=0;
*/
  player_ring->reset();
  player_pll_offset=0.0;
  if((player_src_state=src_new(SRC_SINC_MEDIUM_QUALITY,2,&src_err))==NULL) {
    fprintf(stderr,"src error: %s\n",src_strerror(src_err));
    return false;
  }
  player_pll_average->preset(0.0);
  player_pll_locked=false;

  //
  // Output Interface
  //
  memset(&out_params,0,sizeof(out_params));
  out_params.device=dev;
  out_params.channelCount=2;
  out_params.sampleFormat=paInt16;
  if((perr=Pa_OpenStream(&player_pstream,NULL,&out_params,
      AMON_SAMPLERATE,AMON_FRAMES_PER_PACKET,paNoFlag,
			 AudioOutputCallback,this))!=paNoError) {
    printf("Pa Error: %s\n",Pa_GetErrorText(perr));
    Pa_CloseStream(player_pstream);
    return false;
  }
  if((perr=Pa_StartStream(player_pstream))!=paNoError) {
    printf("Pa Error: %s\n",Pa_GetErrorText(perr));
    Pa_CloseStream(player_pstream);
    return false;
  }
  player_state=StatePlaying;
  emit stateChanged(AMPlayer::StatePlaying);

  return true;
}


void AMPlayer::AudioFree()
{
  uint32_t n;
  unsigned char buffer[1500];

  //
  // Shutdown Audio Interface
  //
  Pa_StopStream(player_pstream);
  Pa_CloseStream(player_pstream);

  //
  // Shutdown Phase Locked Loop
  //
  player_src_state=src_delete(player_src_state);

  //
  // Send BYE packet
  //
  RTPSourceDescription *rtp=new RTPSourceDescription();
  rtp->addChunk(RTPSourceDescription::ChunkCname,1234,"fredg@asterisk");
  n=rtp->writeBlock((uint32_t *)buffer,325);
  RTPBye *bye=new RTPBye();
  bye->addSsrc(1234);
  n+=bye->writeBlock((uint32_t *)buffer+n,325-n);
  sendto(player_rtcp_socket,buffer,n*4,0,
	 (sockaddr *)&player_rtcp_sa,sizeof(player_rtcp_sa));
  delete rtp;

  //
  // Shutdown Sockets
  //
  ::close(player_rtcp_socket);
  ::close(player_rtp_socket);

  player_state=StateStopped;
  emit stateChanged(AMPlayer::StateStopped);
}
