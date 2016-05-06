// amprofilesection.cpp
//
// A container class for profile sections.
//
// (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amprofilesection.cpp,v 1.1 2007/02/25 22:51:07 fredg Exp $
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

#include <amprofilesection.h>

AMProfileSection::AMProfileSection()
{
  clear();
}


QString AMProfileSection::name() const
{
  return section_name;
}


void AMProfileSection::setName(QString name)
{
  section_name=name;
}


bool AMProfileSection::getValue(QString tag,QString *value) const
{
  for(unsigned i=0;i<section_line.size();i++) {
    if(section_line[i].tag()==tag) {
      *value=section_line[i].value();
      return true;
    }
  }
  return false;
}


void AMProfileSection::addValue(QString tag,QString value)
{
  section_line.push_back(AMProfileLine());
  section_line.back().setTag(tag);
  section_line.back().setValue(value);
}


void AMProfileSection::clear()
{
  section_name="";
  section_line.resize(0);
}
