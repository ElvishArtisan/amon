//   amparser.cpp
//
//   The AMon protocol parser.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amparser.cpp,v 1.7 2008/10/22 23:25:01 cvs Exp $
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
//

#include <netinet/in.h>

#include <amparser.h>


AMParser::AMParser(unsigned id,QObject *parent,const char *name)
  : QObject(parent,name)
{
  par_id=id;
  par_istate=0;
  par_length=0;
  par_length_read=0;
  par_checksum=0;

  par_socket=new QSocket(this,"par_socket");
  connect(par_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(par_socket,SIGNAL(connectionClosed()),
	  this,SLOT(connectionClosedData()));
  connect(par_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));
  connect(par_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}


unsigned AMParser::id() const
{
  return par_id;
}


QString AMParser::hostName() const
{
  return par_host;
}


Q_UINT16 AMParser::hostPort() const
{
  return par_port;
}


void AMParser::connectToHost(const QString &host,Q_UINT16 port)
{
  par_host=host;
  par_port=port;
  par_socket->connectToHost(host,port);
}


void AMParser::disconnect()
{
  par_socket->close();
}


void AMParser::connectedData()
{
  emit connected(par_id);
}


void AMParser::connectionClosedData()
{
  emit connectionClosed(par_id);
}


void AMParser::errorData(int err)
{
  emit error(par_id,err);
}


void AMParser::readyReadData()
{
  char buffer[256];
  Q_LONG n;

  n=par_socket->readBlock(buffer,255);
  while(n>0) {
    for(int i=0;i<n;i++) {
      // printf("Processing: 0x%X  istate: %d  i: %d\n",0xff&buffer[i],par_istate,i);
      switch(par_istate) {
	case 0:  // Start of message
	  if((unsigned char)buffer[i]==0xAA) {
	    par_istate=1;
	  }
	  break;

	case 1:  // Message Length
	  par_length=buffer[i];
	  par_istate=2;
	  break;

	case 2:  // Message Verb
	  switch(buffer[i]) {
	    case 1:  // Stereo Channel Data
	      par_verb=buffer[i];
	      par_checksum=par_verb&0xFF;
	      if(++par_length_read==par_length) {
		par_istate=4;
	      }
	      else {
		par_istate=3;
	      }
	      break;
	      
	    default:  // Unrecognized Verb
	      par_length=0;
	      par_istate=0;
	      break;
	  }
	  break;

	case 3:  // Message Data
	  par_data[par_length_read-1]=buffer[i];
	  par_checksum+=(0xFF&buffer[i]);
	  if(++par_length_read==par_length) {
	    par_istate=4;
	  }
	  break;

	case 4:  // Checksum
	  if((par_checksum&0xFF)==(0xFF&buffer[i])) {
	    DispatchCommand(par_verb,par_data,par_length-1);
	  }
	  par_length=0;
	  par_length_read=0;
	  par_istate=0;
	  break;

	default:
	    par_length=0;
	    par_length_read=0;
	    par_istate=0;
	    break;
      }
    }
    n=par_socket->readBlock(buffer,255);
  }
}


void AMParser::DispatchCommand(char verb,char *data,int datalen)
{
  AMStereoChannel *schan;
  uint32_t *ibuf=(uint32_t *)(data+1);

  switch(verb) {
    case 01:  // Stereo Levels
      schan=new AMStereoChannel(0xFF&data[0],ntohl(ibuf[0]),ntohl(ibuf[1]),
				ntohl(ibuf[2]),ntohl(ibuf[3]),0xFF&data[16]);
      emit levelUpdateReceived(par_id,schan);
      delete schan;
//      printf("chan: %d  left: %d  right: %d\n",
//	     0xff&data[0],ntohl(ibuf[0]),ntohl(ibuf[1]));
      break;
  }
}
