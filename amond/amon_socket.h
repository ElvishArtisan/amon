// amon_socket.h
//
// New Connection Handler for amond(8)
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amon_socket.h,v 1.2 2007/06/15 01:31:47 fredg Exp $
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

#ifndef AMON_SOCKET_H
#define AMON_SOCKET_H

#include <qobject.h>
#include <qstring.h>
#include <qserversocket.h>
#include <qhostaddress.h>


class AmonSocket : public QServerSocket
{
  Q_OBJECT
 public:
  AmonSocket(Q_UINT16 port,int id=0,int backlog=0,QObject *parent=0,
	     const char *name=0);
  AmonSocket(const QHostAddress &address,Q_UINT16 port,int id,int backlog=0,
             QObject *parent=0,const char *name=0);
  void newConnection(int socket);

 signals:
  void connection(int id,int fd);

 private:
  int socket_id;
  QServerSocket *socket;
};


#endif  // AMON_SOCKET_H
