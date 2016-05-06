// amon_socket.cpp
//
// New Connection Handler for amond(8)
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amon_socket.cpp,v 1.2 2007/06/15 01:31:47 fredg Exp $
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

#include <amon_socket.h>

AmonSocket::AmonSocket(Q_UINT16 port,int id,int backlog,QObject *parent,
		       const char *name) 
  : QServerSocket(port,0,parent,name)
{
  socket_id=id;
}


AmonSocket::AmonSocket(const QHostAddress &address,Q_UINT16 port,int id,
		       int backlog,QObject *parent,const char *name) 
  : QServerSocket(address,port,0,parent,name)
{
  socket_id=id;
}


void AmonSocket::newConnection(int fd)
{
  emit connection(socket_id,fd);
}
