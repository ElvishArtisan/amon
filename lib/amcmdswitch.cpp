// amcmdswitch.cpp
//
// Process Command-Line Switches
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amcmdswitch.cpp,v 1.2 2008/11/04 23:22:57 cvs Exp $
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

#include <stdlib.h>

#include <amcmdswitch.h>
#include <qmessagebox.h>

AMCmdSwitch::AMCmdSwitch(int argc,char *argv[],char *modname,char *usage)
{
  unsigned l=0;
  bool handled=false;

  for(int i=1;i<argc;i++) {
#ifndef WIN32
    if(!strcmp(argv[i],"--version")) {
      printf("Rivendell v%s [%s]\n",VERSION,modname);
      exit(0);
    }
#endif  // WIN32
    if(!strcmp(argv[i],"--help")) {
      printf("\n%s %s\n",modname,usage);
      exit(0);
    }
    l=strlen(argv[i]);
    handled=false;
    for(unsigned j=0;j<l;j++) {
      if(argv[i][j]=='=') {
	switch_keys.push_back(QString(argv[i]).left(j));
	switch_values.push_back(QString(argv[i]).right(l-(j+1)));
	switch_processed.push_back(false);
	j=l;
	handled=true;
      }
    }
    if(!handled) {
      switch_keys.push_back(QString(argv[i]));
      switch_values.push_back(QString(""));
      switch_processed.push_back(false);
    }
  }
}


unsigned AMCmdSwitch::keys() const
{
  return switch_keys.size();
}


QString AMCmdSwitch::key(unsigned n) const
{
  return switch_keys[n];
}


QString AMCmdSwitch::value(unsigned n) const
{
  return switch_values[n];
}


bool AMCmdSwitch::processed(unsigned n) const
{
  return switch_processed[n];
}


void AMCmdSwitch::setProcessed(unsigned n,bool state)
{
  switch_processed[n]=state;
}
