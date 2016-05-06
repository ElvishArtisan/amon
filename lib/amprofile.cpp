// amprofile.cpp
//
// A class to read an ini formatted configuration file.
//
// (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amprofile.cpp,v 1.1 2007/02/25 22:51:07 fredg Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <qfile.h>
#include <qtextstream.h>

#include <amprofile.h>


AMProfile::AMProfile()
{
}


QString AMProfile::source() const
{
  return profile_source;
}


bool AMProfile::setSource(QString filename)
{
  QString section;
  int offset;

  profile_source=filename;
  profile_section.resize(0);
  profile_section.push_back(AMProfileSection());
  profile_section.back().setName("");
  QFile *file=new QFile(filename);
  if(!file->open(IO_ReadOnly)) {
    delete file;
    return false;
  }
  QTextStream *text=new QTextStream(file);
  QString line=text->readLine().stripWhiteSpace();
  while(!line.isNull()) {
    if((line.left(1)!=";")&&(line.left(1)!="#")) {
      if((line.left(1)=="[")&&(line.right(1)=="]")) {
	section=line.mid(1,line.length()-2);
	profile_section.push_back(AMProfileSection());
	profile_section.back().setName(section);
      }
      else if(((offset=line.find('='))!=-1)) {
//      else if(((offset=line.find('='))!=-1)&&(!section.isEmpty())) {
	profile_section.back().
	  addValue(line.left(offset),
		   line.right(line.length()-offset-1).stripWhiteSpace());
      }
    }
    line=text->readLine().stripWhiteSpace();
  }
  delete text;
  delete file;
  return true;
}


QString AMProfile::stringValue(QString section,QString tag,
			      QString default_str,bool *ok) const
{
  QString result;

  for(unsigned i=0;i<profile_section.size();i++) {
    if(profile_section[i].name()==section) {
      if(profile_section[i].getValue(tag,&result)) {
	if(ok!=NULL) {
	  *ok=true;
	}
	return result;
      }
      if(ok!=NULL) {
	*ok=false;
      }
      return default_str;
    }
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_str;
}


int AMProfile::intValue(QString section,QString tag,
		       int default_value,bool *ok) const
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,10);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


int AMProfile::hexValue(QString section,QString tag,
		       int default_value,bool *ok) const
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,16);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


float AMProfile::floatValue(QString section,QString tag,
			   float default_value,bool *ok) const
{
  bool valid;

  float result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


double AMProfile::doubleValue(QString section,QString tag,
			    double default_value,bool *ok) const
{
  bool valid;

  double result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


bool AMProfile::boolValue(QString section,QString tag,
			 bool default_value,bool *ok) const
{
  bool valid;

  QString str=stringValue(section,tag,"",&valid).lower();
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if((str=="yes")||(str=="true")||(str=="on")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return true;
  }
  if((str=="no")||(str=="false")||(str=="off")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return false;
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_value;
}


void AMProfile::clear()
{
  profile_source="";
  profile_section.resize(0);
}
