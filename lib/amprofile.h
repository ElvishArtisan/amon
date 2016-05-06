// amprofile.h
//
// A class to read an ini formatted configuration file.
//
// (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amprofile.h,v 1.1 2007/02/25 22:51:07 fredg Exp $
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


#ifndef AMPROFILE_H
#define AMPROFILE_H


#include <vector>

#include <qstring.h>

#include <amprofilesection.h>

class AMProfile
{
 public:
 /**
  * Instantiates the class.
  **/
  AMProfile();
  QString source() const;
  bool setSource(QString filename);
  QString stringValue(QString section,QString tag,
		      QString default_value="",bool *ok=0) const;
  int intValue(QString section,QString tag,
	       int default_value=0,bool *ok=0) const;
  int hexValue(QString section,QString tag,
	       int default_value=0,bool *ok=0) const;
  float floatValue(QString section,QString tag,
		   float default_value=0.0,bool *ok=0) const;
  double doubleValue(QString section,QString tag,
		    double default_value=0.0,bool *ok=0) const;
  bool boolValue(QString section,QString tag,
		 bool default_value=false,bool *ok=0) const;
  void clear();

 private:
  QString profile_source;
  std::vector<AMProfileSection> profile_section;
};


#endif  // AMPROFILE_H
