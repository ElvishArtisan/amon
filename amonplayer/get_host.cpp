// get_host.cpp
//
// A dialog to get an AMon Server hostname and channel
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: get_host.cpp,v 1.1 2007/08/26 20:13:43 fredg Exp $
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

#include <qpushbutton.h>
#include <qlabel.h>

#include <am.h>

#include <get_host.h>


GetHost::GetHost(QString *hostname,int *chan,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  get_hostname=hostname;
  get_channel=chan;
  setCaption(tr("Get Host"));

  //
  // Create Fonts
  //
  QFont button_font=QFont("helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont font=QFont("helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Hostname
  //
  get_hostname_edit=new QLineEdit(this,"get_hostname_edit");
  get_hostname_edit->setGeometry(100,10,sizeHint().width()-115,20);
  get_hostname_edit->setText(*hostname);
  QLabel *label=new QLabel(get_hostname_edit,"Host Name:",
			   this,"get_hostname_label");
  label->setGeometry(10,10,85,20);
  label->setFont(button_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Channel
  //
  get_channel_box=new QComboBox(this,"get_channel_box");
  get_channel_box->setGeometry(100,32,45,20);
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    get_channel_box->insertItem(QString().sprintf("%d",i+1));
  }
  get_channel_box->setCurrentItem(*chan);
  label=new QLabel(get_channel_box,"Channel:",this,"get_channel_label");
  label->setGeometry(10,32,85,20);
  label->setFont(button_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // OK Button
  //
  QPushButton *button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-120,sizeHint().height()-40,50,30);
  button->setFont(button_font);
  button->setText("&OK");
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-60,sizeHint().height()-40,50,30);
  button->setFont(button_font);
  button->setText("&Cancel");
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize GetHost::sizeHint() const
{
  return QSize(250,100);
}


QSizePolicy GetHost::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void GetHost::okData()
{
  *get_hostname=get_hostname_edit->text();
  *get_channel=get_channel_box->currentItem();
  done(0);
}


void GetHost::cancelData()
{
  done(-1);
}
