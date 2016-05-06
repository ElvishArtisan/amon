// ammeteraverage.h
//
// Average sucessive levels for a meter.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ammeteraverage.h,v 1.1 2008/10/22 23:25:01 cvs Exp $
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
#ifndef AMMETERAVERAGE_H
#define AMMETERAVERAGE_H

#include <queue>


class AmMeterAverage
{
 public:
  AmMeterAverage(int maxsize);
  double average() const;
  void addValue(double value);
  void preset(double value);

 private:
  int avg_maxsize;
  double avg_total;
  std::queue<double> avg_values;
};


#endif  // AMMETERAVERAGE_H
