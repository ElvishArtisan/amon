//   amparser.h
//
//   The AMon protocol parser.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amparser.h,v 1.5 2007/08/26 20:13:43 fredg Exp $
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

#ifndef AMPARSER_H
#define AMPARSER_H


#include <qobject.h>
#include <qsocket.h>

#include <am.h>
#include <amstereochannel.h>

class AMParser : public QObject
{
  Q_OBJECT
 public:
  AMParser(unsigned id,QObject *parent=0,const char *name=0);
  unsigned id() const;
  QString hostName() const;
  Q_UINT16 hostPort() const;
  void connectToHost(const QString &host,Q_UINT16 port);
  void disconnect();

 signals:
  void connected(int id);
  void connectionClosed(int id);
  void error(int id,int err);
  void levelUpdateReceived(int id,AMStereoChannel *lvls);

 private slots:
  void connectedData();
  void connectionClosedData();
  void errorData(int err);
  void readyReadData();

 private:
  void DispatchCommand(char verb,char *data,int datalen);
  int par_id;
  QString par_host;
  Q_UINT16 par_port;
  QSocket *par_socket;
  unsigned par_istate;
  char par_buffer[AMON_MAX_COMMAND_LENGTH];
  unsigned par_length;
  unsigned par_length_read;
  unsigned par_checksum;
  char par_verb;
  char par_data[AMON_MAX_COMMAND_LENGTH];
};

#endif  // AMPARSER_H
