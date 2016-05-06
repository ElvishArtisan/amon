// amonplayer.h
//
// The AMon Audio Player
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amonplayer.h,v 1.4 2007/10/06 21:04:49 fredg Exp $
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

#ifndef AMONPLAYER_H
#define AMONPLAYER_H

#define AMONPLAYER_USAGE "[--use-meter-layout=<n>] [<hostname>:<channel>]\n"

#include <qwidget.h>

#include <amplayer.h>
#include <amparser.h>
#include <amtransportbutton.h>
#include <amstereometer.h>
#include <ammonconfig.h>


void SigHandler(int signum);

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void stateChangedData(AMPlayer::State state);
  void errorData(AMPlayer::Error err);
  void errorData(int id,int err);
  void ejectClickedData();
  void playClickedData();
  void stopClickedData();
  void closeClickedData();

 private:
  void SetCaption();
  AMPlayer *main_player;
  AMParser *main_parser;
  AMStereoMeter *main_meter;
  int main_meter_element_count;
  AMTransportButton *main_eject_button;
  AMTransportButton *main_play_button;
  AMTransportButton *main_stop_button;
  QPixmap *main_icon_map;
  QString main_hostname;
  int main_channel;
  AMMonConfig *main_config;
};


#endif  // AMONPLAYER_H
