//   ammeter.cpp
//
//   An audio meter display widget.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: ammeter.cpp,v 1.6 2008/11/18 17:31:37 cvs Exp $
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

#include <qwidget.h>
#include <qstring.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <stdio.h>
#include <qslider.h>
#include <qbuttongroup.h>
#include <qsizepolicy.h>
#include <qmessagebox.h>

#include <ammeter.h>
#include <am.h>

AMMeter::AMMeter(AMMeter::Orientation o,QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  orient=o;
  setBackgroundColor(black);
  dark_low_color=QColor(DEFAULT_DARK_LOW_COLOR);
  dark_high_color=QColor(DEFAULT_DARK_HIGH_COLOR);
  dark_clip_color=QColor(DEFAULT_DARK_CLIP_COLOR);
  low_color=QColor(DEFAULT_LOW_COLOR);
  high_color=QColor(DEFAULT_HIGH_COLOR);
  clip_color=QColor(DEFAULT_CLIP_COLOR);
  high_threshold=-14;
  clip_threshold=0;
  seg_size=2;
  seg_gap=1;
  range_min=-3000;
  range_max=0;
  solid_bar=-10000;
  floating_bar=-10000;
  seg_mode=AMMeter::Independent;
  peak_sum=0;
  averaging_periods=AMON_DEFAULT_AVERAGING_PERIODS;

  GenerateMeterMaps();

  peak_timer=new QTimer(this,"peak_timer");
  connect(peak_timer,SIGNAL(timeout()),this,SLOT(peakData()));
}


QSize AMMeter::sizeHint() const
{
  return QSize(0,0);
}


QSizePolicy AMMeter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AMMeter::setRange(int min,int max)
{
  range_min=min;
  range_max=max;
  GenerateMeterMaps();
  repaint(false);
}


void AMMeter::setDarkLowColor(const QColor &color)
{
  if(dark_low_color!=color) {
    dark_low_color=color;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setDarkHighColor(const QColor &color)
{
  if(dark_high_color!=color) {
    dark_high_color=color;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setDarkClipColor(const QColor &color)
{
  if(dark_clip_color!=color) {
    dark_clip_color=color;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setLowColor(const QColor &color)
{
  if(low_color!=color) {
    low_color=color;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setHighColor(const QColor &color)
{
  if(high_color!=color) {
    high_color=color;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setClipColor(const QColor &color)
{
  if(clip_color!=color) {
    clip_color=color;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setHighThreshold(int level)
{
  if(high_threshold!=level) {
    high_threshold=level;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setClipThreshold(int level)
{
  if(clip_threshold!=level) {
    clip_threshold=level;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setSegmentSize(int size)
{
  if(seg_size!=size) {
    seg_size=size;
    GenerateMeterMaps();
    repaint(false);
  }
}


void AMMeter::setSegmentGap(int gap)
{
  if(seg_gap!=gap) {
    seg_gap=gap;
    GenerateMeterMaps();
    repaint(false);
  }
}


AMMeter::Mode AMMeter::mode() const
{
  return seg_mode;
}


void AMMeter::setMode(AMMeter::Mode mode)
{
  seg_mode=mode;
  switch(seg_mode) {
      case AMMeter::Independent:
	if(peak_timer->isActive()) {
	  peak_timer->stop();
	}
	break;
      case AMMeter::Peak:
	if(!peak_timer->isActive()) {
	  peak_timer->start(PEAK_HOLD_TIME);
	}
	break;
  }
}


int AMMeter::averagingPeriods() const
{
  return averaging_periods;
}


void AMMeter::setAveragingPeriods(int periods)
{
  averaging_periods=periods;
}


void AMMeter::setSolidBar(int level)
{
  printf("setSolidBar(%d)\n",level);
  if((seg_mode==AMMeter::Independent)&&(solid_bar!=level)) {
    solid_bar=level;
    repaint(false);
  }
}


void AMMeter::setFloatingBar(int level)
{
  printf("setFloatingBar(%d)\n",level);
  if((seg_mode==AMMeter::Independent)&&(solid_bar!=level)) {
    floating_bar=level;
    repaint(false);
  }
}


void AMMeter::setPeakBar(int level)
{
  int prev_solid_bar=solid_bar;
  int prev_floating_bar=floating_bar;
  int low_bar=10000;
  int high_bar=-10000;

  if(seg_mode!=AMMeter::Peak) {
    return;
  }
  peak_sum+=level;
  peak_levels.push(level);
  int size=peak_levels.size()-averaging_periods;
  for(int i=0;i<size;i++) {
    peak_sum-=peak_levels.front();
    peak_levels.pop();
  }
  level=peak_sum/peak_levels.size();
  if(solid_bar!=level) {
    solid_bar=level;
    if(solid_bar>floating_bar) {
      floating_bar=solid_bar;
    }

    if(solid_bar<range_min) {
      floating_bar=solid_bar;
    }
    QRect r;

    if(solid_bar<low_bar) {
      low_bar=solid_bar;
    }
    if(prev_solid_bar<low_bar) {
      low_bar=prev_solid_bar;
    }
    if(floating_bar<low_bar) {
      low_bar=floating_bar;
    }
    if(prev_floating_bar<low_bar) {
      low_bar=prev_floating_bar;
    }
    if(solid_bar>high_bar) {
      high_bar=solid_bar;
    }
    if(prev_solid_bar>high_bar) {
      high_bar=prev_solid_bar;
    }
    if(floating_bar>high_bar) {
      high_bar=floating_bar;
    }
    if(prev_floating_bar>high_bar) {
      high_bar=prev_floating_bar;
    }
    if(high_bar>range_max) {
      high_bar=range_max;
    }
    if(high_bar<range_min) {
      high_bar=range_min;
    }
    if(low_bar>range_max) {
      low_bar=range_max;
    }
    if(low_bar<range_min) {
      low_bar=range_min;
    }
    int range=range_max-range_min;

    switch(orient) {
      case AMMeter::Left:
	r=QRect(0,0,geometry().width(),geometry().height());
	break;

      case AMMeter::Right:
//	printf("low_bar: %d  high_bar: %d  low_pixel: %d  high_pixel: %d\n",low_bar,high_bar,(low_bar+range_min)*geometry().width()/range,(high_bar+range_min)*geometry().width()/range);
	r=QRect((low_bar+range_min)*geometry().width()/range,
		0,
		(high_bar+range_min)*geometry().width()/range,
		geometry().height());
	break;

      case AMMeter::Up:
	r=QRect(0,
		(low_bar+range_min)*geometry().width()/range,
		geometry().width(),
		(high_bar+range_min)*geometry().width()/range);
	break;

      case AMMeter::Down:
	r=QRect(0,0,geometry().width(),geometry().height());
	break;
    }
    repaint(r,false);
  }
}


void AMMeter::setGeometry(int x,int y,int h,int w)
{
  QWidget::setGeometry(x,y,h,w);
  GenerateMeterMaps();
}


void AMMeter::setGeometry(const QRect &r)
{
  QWidget::setGeometry(r);
  GenerateMeterMaps();
}


void AMMeter::paintEvent(QPaintEvent *paintEvent)
{
  int float_region;
  QColor float_color;

  //
  // Solid Segments
  //
  int seg=0;
  if((range_max-range_min)>0) {
    if(solid_bar>range_min) {
      seg=(solid_bar-range_min)*seg_quantity/(range_max-range_min);
    }
    if(solid_bar>range_max) {
      seg=meter_maps.size()-1;
    }
  }
  QPainter *p=new QPainter();
  p->begin(this);
  p->drawPixmap(0,0,*meter_maps[seg]);

  //
  // The floating segment
  //
/*
  int seg_total=seg_size+seg_gap;
  if(floating_bar>solid_bar) {
    if(floating_bar<=high_threshold) {
      float_color=low_color;
    }
    if((floating_bar>high_threshold)&&(floating_bar<=clip_threshold)) {
      float_color=high_color;
    }
    if(floating_bar>clip_threshold) {
      float_color=clip_color;
    }
    switch(orient) {
	case AMMeter::Left:
	case AMMeter::Right:
	  float_region=(int)((double)(floating_bar-range_min)/
			     (double)(range_max-range_min)*width());
	  float_region=seg_total*(float_region/seg_total);
	  p->fillRect(float_region,0,seg_size,height(),float_color); 
	  break;

	case AMMeter::Down:
	case AMMeter::Up:
	  float_region=(int)((double)(floating_bar-range_min)/
			     (double)(range_max-range_min)*height());
	  float_region=seg_total*(float_region/seg_total);
	  p->fillRect(0,float_region,width(),seg_size,float_color); 
	  break;
    }
  } 
*/
  p->end();
  delete p;
}


void AMMeter::peakData()
{
  floating_bar=solid_bar;
  repaint(false);
}


void AMMeter::GenerateMeterMaps()
{
  //
  // Clear old map set
  //
  for(unsigned i=0;i<meter_maps.size();i++) {
    delete meter_maps[i];
  }
  meter_maps.clear();

  //
  // Generate new map set
  //
  int len=0;
  switch(orient) {
    case AMMeter::Left:
    case AMMeter::Right:
      len=geometry().width();
      break;

    case AMMeter::Up:
    case AMMeter::Down:
      len=geometry().height();
      break;
  }
  seg_quantity=len/(seg_size+seg_gap);
  for(int i=0;i<seg_quantity;i++) {
    meter_maps.push_back(MakeMeterMap(i));
  }
}


QPixmap *AMMeter::MakeMeterMap(int seg)
{
  int op_pt;
  int low_region,high_region,clip_region;
  int dark_low_region=0;
  int dark_high_region=0;
  int dark_clip_region=0;
  QColor float_color;
  int level=seg*(range_max-range_min)/seg_quantity+range_min;

  //
  // Setup
  //
  QPixmap *pix=new QPixmap(this->size());
  pix->fill(this,0,0);

  int seg_total=seg_size+seg_gap;
  QPainter *p=new QPainter(pix);
  low_region=0;
  high_region=0;
  clip_region=0;
  p->setBrush(low_color);
  p->setPen(low_color);

  //
  // Set Orientation
  //
  switch(orient) {
      case AMMeter::Left:
      case AMMeter::Up:
	p->translate(width(),height());
	p->rotate(180);
	break;

      default:
	break;
  }

  // 
  // The low range
  //
  if(level>high_threshold) {
    op_pt=high_threshold;
  }
  else {
    op_pt=level;
  }
  switch(orient) {
      case AMMeter::Left:
      case AMMeter::Right:
	low_region=(int)((double)(op_pt-range_min)/
			 (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>range_min) {
	  for(int i=0;i<low_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),low_color);
	  }
	}
	break;
      case AMMeter::Down:
      case AMMeter::Up:
	low_region=(int)((double)(op_pt-range_min)/
			 (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>range_min) {
	  for(int i=0;i<low_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,low_color);
	  }
	}
	break;
  }

  // 
  // The high range
  //
  if(level>clip_threshold) {
    op_pt=clip_threshold;
  }
  else {
    op_pt=level;
  }
  switch(orient) {
      case AMMeter::Left:
      case AMMeter::Right:
	high_region=(int)((double)(op_pt-high_threshold)/
			  (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>high_threshold) {
	  for(int i=low_region;i<low_region+high_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),high_color);
	  }
	}
	break;
      case AMMeter::Down:
      case AMMeter::Up:
	high_region=(int)((double)(op_pt-high_threshold)/
			  (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>high_threshold) {
	  for(int i=low_region;i<low_region+high_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,high_color);
	  }
	}
	break;
  }

  // 
  // The clip range
  //
  if(level>range_max) {
    op_pt=range_max;
  }
  else {
    op_pt=level;
  }
  switch(orient) {
      case AMMeter::Left:
      case AMMeter::Right:
	clip_region=(int)((double)(op_pt-clip_threshold)/
			  (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>clip_threshold) {
	  for(int i=low_region+high_region;
	      i<low_region+high_region+clip_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),clip_color);
	  }
	}
	break;
      case AMMeter::Down:
      case AMMeter::Up:
	clip_region=(int)((double)(op_pt-clip_threshold)/
			  (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>clip_threshold) {
	  for(int i=low_region+high_region;
	      i<low_region+high_region+clip_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,clip_color);
	  }
	}
	break;
  }

  // 
  // The dark low range
  //
  switch(orient) {
      case AMMeter::Left:
      case AMMeter::Right:
	dark_low_region=(int)((double)(high_threshold-range_min)/
			      (double)(range_max-range_min)*width()/seg_total);
	if(op_pt<high_threshold) {
	  for(int i=low_region;i<dark_low_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),dark_low_color);
	  }
	}
	break;
      case AMMeter::Down:
      case AMMeter::Up:
	dark_low_region=(int)((double)(high_threshold-range_min)/
		      (double)(range_max-range_min)*height()/seg_total);
	if(op_pt<high_threshold) {
	  for(int i=low_region;i<dark_low_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_low_color);
	  }
	}
	break;
  }

  // 
  // The dark high range
  //
  if(level>=high_threshold) {
    op_pt=low_region+high_region;
  }
  else {
    op_pt=dark_low_region;
  }
  switch(orient) {
      case AMMeter::Left:
	case AMMeter::Right:
	  dark_high_region=(int)((double)(clip_threshold-range_min)/
			      (double)(range_max-range_min)*width()/seg_total);
	  if(level<clip_threshold) {
	    for(int i=op_pt;
		i<dark_high_region;i++) {
	      p->fillRect(i*seg_total,0,seg_size,height(),dark_high_color);
	    }
	  }
	  break;
      case AMMeter::Down:
      case AMMeter::Up:
	dark_high_region=(int)((double)(clip_threshold-range_min)/
		       (double)(range_max-range_min)*height()/seg_total);
	if(level<clip_threshold) {
	  for(int i=op_pt;i<dark_high_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_high_color);
	  }
	}
	break;
  }

  // 
  // The dark clip range
  //
  if(level>clip_threshold) {
    op_pt=low_region+high_region+clip_region;
  }
  else {
    op_pt=dark_high_region;
  }
  switch(orient) {
      case AMMeter::Left:
      case AMMeter::Right:
	dark_clip_region=(int)((double)(range_max-range_min)/
       		       (double)(range_max-range_min)*width()/seg_total);
	if(level<range_max) {
	  for(int i=op_pt;i<dark_clip_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),dark_clip_color);
	  }
	}
	break;
      case AMMeter::Down:
      case AMMeter::Up:
	dark_clip_region=(int)((double)(range_max-range_min)/
			       (double)(range_max-range_min)*height()/seg_total);
	if(level<range_max) {
	  for(int i=op_pt;
	      i<dark_clip_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_clip_color);
	  }
	}
	break;
  }
/*
*/
  p->end();
  delete p;
  return pix;
}
