// amon.h
//
// The AMon Audio Level Monitoring Client
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amon.h,v 1.8 2008/11/18 19:13:21 cvs Exp $
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

#ifndef AMON_H
#define AMON_H

#define AMON_USAGE ""

#include <vector>

#include <qwidget.h>
#include <qpixmap.h>
#include <qtimer.h>

#include <ampushbutton.h>
#include <amparser.h>
#include <amplayer.h>
#include <am.h>
#include <ammonconfig.h>

void SigHandler(int signum);

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);

 private slots:
  void monitorButtonClickedData(int n);
  void stateChangedData(AMPlayer::State state);
  void errorData(AMPlayer::Error err);
  void scanTimerData();

 private:
  void ToggleMonitor(int n);
  unsigned GetConnection(const QString &hostname,Q_UINT16 port);
  std::vector <AMParser *> amon_parsers;
  std::vector <AMPushButton *> amon_monitor_buttons;
  QPixmap *amon_map;
  AMPlayer *amon_player;
  int amon_active_player;
  int amon_next_player;
  AMMonConfig *config;
  QTimer *amon_scan_timer;
  std::vector<int> amon_scan_buttons;
  unsigned amon_scan_ptr;
};


#endif  // AMON_H
