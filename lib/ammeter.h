//   ammeter.h
//
//   An audio meter display widget.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: ammeter.h,v 1.5 2008/11/18 17:31:37 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
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

#ifndef AMMETER_H

#define AMMETER_H

#include <vector>
#include <queue>

#include <qwidget.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qtimer.h>

/*
 * Default Colors
 */
#define DEFAULT_LOW_COLOR Qt::green
#define DEFAULT_DARK_LOW_COLOR QColor(0,80,0)
#define DEFAULT_HIGH_COLOR Qt::yellow
#define DEFAULT_DARK_HIGH_COLOR QColor(75,75,0)
#define DEFAULT_CLIP_COLOR Qt::red
#define DEFAULT_DARK_CLIP_COLOR QColor(85,0,0)

/*
 * Global Settings
 */
#define PEAK_HOLD_TIME 750

class AMMeter : public QWidget
{
 Q_OBJECT
 public:
  enum Mode {Independent=0,Peak=1};
  enum Orientation {Left=0,Right=1,Up=2,Down=3};
  AMMeter(AMMeter::Orientation o,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setRange(int min,int max);
  void setDarkLowColor(const QColor &color);
  void setDarkHighColor(const QColor &color);
  void setDarkClipColor(const QColor &color);
  void setLowColor(const QColor &color);
  void setHighColor(const QColor &color);
  void setClipColor(const QColor &color);
  void setHighThreshold(int level);
  void setClipThreshold(int level);
  void setSegmentSize(int size);
  void setSegmentGap(int gap);
  AMMeter::Mode mode() const;
  void setMode(AMMeter::Mode mode);
  int averagingPeriods() const;
  void setAveragingPeriods(int periods);

 public slots:
  void setSolidBar(int level);
  void setFloatingBar(int level);
  void setPeakBar(int level);
  void setGeometry(int x,int y,int h,int w);
  void setGeometry(const QRect &r);

 protected:
  void paintEvent(QPaintEvent *);

 private slots:
  void peakData();

 private:
  void GenerateMeterMaps();
  QPixmap *MakeMeterMap(int seg);
  AMMeter::Orientation orient;
  AMMeter::Mode seg_mode;
  QTimer *peak_timer;
  int range_min,range_max;
  QColor dark_low_color;
  QColor dark_high_color;
  QColor dark_clip_color;
  QColor low_color;
  QColor high_color;
  QColor clip_color;
  int high_threshold,clip_threshold;
  int solid_bar,floating_bar;
  int seg_size,seg_gap;
  int seg_quantity;
  std::queue<int> peak_levels;
  long long peak_sum;
  int averaging_periods;
  std::vector<QPixmap *> meter_maps;
};


#endif
