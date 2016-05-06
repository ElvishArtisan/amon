// ammondaemonconfig.cpp
//
// A container class for an AMon Daemon configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ammondaemonconfig.cpp,v 1.3 2008/11/22 17:35:46 cvs Exp $
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

#include <stdio.h>
#include <math.h>

#include <amprofile.h>
#include <ammondaemonconfig.h>


AMMonDaemonConfig::AMMonDaemonConfig(QString filename)
{
  clear();
  conf_filename=filename;
}


QString AMMonDaemonConfig::filename() const
{
  return conf_filename;
}


void AMMonDaemonConfig::setFilename(QString filename)
{
  conf_filename=filename;
}


bool AMMonDaemonConfig::globalUseRealtime() const
{
  return conf_use_realtime;
}


int AMMonDaemonConfig::lowLevel(unsigned n) const
{
  return conf_low_levels[n];
}


void AMMonDaemonConfig::setLowLevel(unsigned n,int val)
{
  conf_low_levels[n]=val;
}


int AMMonDaemonConfig::lowTimeout(unsigned n) const
{
  return conf_low_timeouts[n];
}


void AMMonDaemonConfig::setLowTimeout(unsigned n,int val)
{
  conf_low_timeouts[n]=val;
}


int AMMonDaemonConfig::highLevel(unsigned n) const
{
  return conf_high_levels[n];
}


void AMMonDaemonConfig::setHighLevel(unsigned n,int val)
{
  conf_high_levels[n]=val;
}


int AMMonDaemonConfig::highTimeout(unsigned n)
{
  return conf_high_timeouts[n];
}


void AMMonDaemonConfig::setHighTimeout(unsigned n,int val)
{
  conf_high_timeouts[n]=val;
}

void AMMonDaemonConfig::load()
{
  QString section;
  double lvl;

  AMProfile *profile=new AMProfile();
  profile->setSource(conf_filename);

  conf_use_realtime=profile->boolValue("Global","UseRealtime",true);
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    section=QString().sprintf("Channel%d",i+1);
    if((lvl=profile->doubleValue(section,"LowLevel",-100.0))<18.0) {
      conf_low_levels[i]=
	(int)(2147483648.0*exp10((lvl-AMON_LEVEL_OFFSET/100.0)/20.0));
    }
    else {
      conf_low_levels[i]=2147483647;
    }
    conf_low_timeouts[i]=profile->intValue(section,"LowTimeout",0);
    if((lvl=profile->doubleValue(section,"HighLevel",18.0))<18.0) {
      conf_high_levels[i]=
	(int)(2147483648.0*exp10((lvl-AMON_LEVEL_OFFSET/100.0)/20.0));
    }
    else {
      conf_high_levels[i]=2147483647;
    }
    conf_high_timeouts[i]=profile->intValue(section,"HighTimeout",0);
  }

  delete profile;
}


void AMMonDaemonConfig::save() const
{
  FILE *f=fopen(conf_filename,"w");
  if(f==NULL) {
    return;
  }
  fprintf(f,"[Global]\n");
  if(conf_use_realtime) {
    fprintf(f,"UseRealtime=Yes");
  }
  else {
    fprintf(f,"UseRealtime=No");
  }
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    fprintf(f,"[Channel%d]\n",i+1);
    fprintf(f,"LowLevel=%d\n",conf_low_levels[i]);
    fprintf(f,"LowTimeout=%d\n",conf_low_timeouts[i]);
    fprintf(f,"HighLevel=%d\n",conf_high_levels[i]);
    fprintf(f,"HighTimeout=%d\n",conf_high_timeouts[i]);
    fprintf(f,"\n");
  }
  fclose(f);
}


void AMMonDaemonConfig::clear()
{
  conf_use_realtime=true;
  for(unsigned i=0;i<(AMON_MAX_CHANNELS/2);i++) {
    conf_low_levels[i]=0;
    conf_low_timeouts[i]=0;
    conf_high_levels[i]=2147483647;
    conf_high_timeouts[i]=0;
  }
}
