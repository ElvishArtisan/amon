// amprofileline.cpp
//
// A container class for profile lines.
//
// (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amprofileline.cpp,v 1.1 2007/02/25 22:51:07 fredg Exp $
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


#include <amprofileline.h>

AMProfileLine::AMProfileLine()
{
  clear();
}


QString AMProfileLine::tag() const
{
  return line_tag;
}


void AMProfileLine::setTag(QString tag)
{
  line_tag=tag;
}


QString AMProfileLine::value() const
{
  return line_value;
}


void AMProfileLine::setValue(QString value)
{
  line_value=value;
}


void AMProfileLine::clear()
{
  line_tag="";
  line_value="";
}
