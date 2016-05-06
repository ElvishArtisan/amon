// ammondaemonconfig.h
//
// A container class for an AMon Daemon configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ammondaemonconfig.h,v 1.2 2008/11/22 17:35:46 cvs Exp $
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

#ifndef AMMONDAEMONCONFIG_H
#define AMMONDAEMONCONFIG_H

#include <qstring.h>

#include <am.h>

class AMMonDaemonConfig
{
 public:
  AMMonDaemonConfig(QString filename);
  QString filename() const;
  void setFilename(QString filename);
  bool globalUseRealtime() const;
  int lowLevel(unsigned n) const;
  void setLowLevel(unsigned n,int val);
  int lowTimeout(unsigned n) const;
  void setLowTimeout(unsigned n,int val);
  int highLevel(unsigned n) const;
  void setHighLevel(unsigned n,int val);
  int highTimeout(unsigned n);
  void setHighTimeout(unsigned n,int val);
  void load();
  void save() const;
  void clear();

 private:
  QString conf_filename;
  bool conf_use_realtime;
  int conf_low_levels[AMON_MAX_CHANNELS/2];
  int conf_low_timeouts[AMON_MAX_CHANNELS/2];
  int conf_high_levels[AMON_MAX_CHANNELS/2];
  int conf_high_timeouts[AMON_MAX_CHANNELS/2];
};


#endif  // AMMONDAEMONCONFIG_H
