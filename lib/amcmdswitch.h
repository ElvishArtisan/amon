// amcmdswitch.h
//
// Process Command-Line Switches
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amcmdswitch.h,v 1.1.1.1 2007/02/25 14:54:46 fredg Exp $
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

#ifndef AMCMDSWITCH_H
#define AMCMDSWITCH_H

#include <vector>
#include <qstring.h>


class AMCmdSwitch
{
 public:
  AMCmdSwitch(int argc,char *argv[],char *modname,char *usage);
  unsigned keys() const;
  QString key(unsigned n) const;
  QString value(unsigned n) const;
  bool processed(unsigned n) const;
  void setProcessed(unsigned n,bool state);

 private:
  std::vector<QString> switch_keys;
  std::vector<QString> switch_values;
  std::vector<bool> switch_processed;
};


#endif  // AMCMDSWITCH_H
