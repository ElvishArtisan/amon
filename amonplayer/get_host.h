// get_host.h
//
// A dialog to get an AMon Server hostname and channel
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: get_host.h,v 1.1 2007/08/26 20:13:43 fredg Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of
//   the License, or (at your option) any later version.
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

#ifndef GET_HOST_H
#define GET_HOST_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>


class GetHost : public QDialog
{
  Q_OBJECT
 public:
  GetHost(QString *hostname,int *chan,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 private:
  QLineEdit *get_hostname_edit;
  QComboBox *get_channel_box;
  QString *get_hostname;
  int *get_channel;
};


#endif  // GET_HOST_H
