// amplaymeter.cpp
//
// This implements a widget that represents a stereo audio level meter,
// complete with labels and scale.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amplaymeter.cpp,v 1.4 2007/10/06 20:32:00 fredg Exp $
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

#include <qwidget.h>
#include <qstring.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <stdio.h>
#include <qslider.h>
#include <qbuttongroup.h>
#include <qsizepolicy.h>
#include <qmessagebox.h>

#include <amplaymeter.h>

AMPlayMeter::AMPlayMeter(AMMeter::Orientation orient,int id,int chan,
			 AMStereoChannel::Channel role,
			 QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  meter_id=id;
  meter_chan=chan;
  meter_role=role;
  setBackgroundColor(black);
  orientation=orient;
  makeFont();
  meter=new AMMeter(orientation,this,"meter");
  meter->setSegmentSize(5);
  meter->setSegmentGap(1);

  switch(role) {
    case AMStereoChannel::Left:
      setLabel("L");
      break;

    case AMStereoChannel::Right:
      setLabel("R");
      break;

    case AMStereoChannel::Sum:
      setLabel("S");
      break;

    case AMStereoChannel::Difference:
      setLabel("D");
      break;

    default:
      break;
  }
}


QSize AMPlayMeter::sizeHint() const
{
  if(meter_label==QString("")) {
    return QSize(335,60);
  }
  else {
    return QSize(335,80);
  }
}


QSizePolicy AMPlayMeter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}


void AMPlayMeter::setRange(int min,int max)
{
  meter->setRange(min,max);
}


void AMPlayMeter::setDarkLowColor(const QColor &color)
{
  meter->setDarkLowColor(color);
}


void AMPlayMeter::setDarkHighColor(const QColor &color)
{
  meter->setDarkHighColor(color);
}


void AMPlayMeter::setDarkClipColor(const QColor &color)
{
  meter->setDarkClipColor(color);
}


void AMPlayMeter::setLowColor(const QColor &color)
{
  meter->setLowColor(color);
}


void AMPlayMeter::setHighColor(const QColor &color)
{
  meter->setHighColor(color);
}


void AMPlayMeter::setClipColor(const QColor &color)
{
  meter->setClipColor(color);
}


void AMPlayMeter::setHighThreshold(int level)
{
  meter->setHighThreshold(level);
}


void AMPlayMeter::setClipThreshold(int level)
{
  meter->setClipThreshold(level);
}


void AMPlayMeter::setLabel(QString label)
{
  meter_label=label;
  makeFont();
  setGeometry(geometry().left(),geometry().top(),
	      geometry().width(),geometry().height());
}


int AMPlayMeter::averagingPeriods() const
{
  return meter->averagingPeriods();
}


void AMPlayMeter::setAveragingPeriods(int periods)
{
  meter->setAveragingPeriods(periods);
}


void AMPlayMeter::setGeometry(int x,int y,int w,int h)
{
  QWidget::setGeometry(x,y,w,h);
  if(meter_label.isEmpty()) {
    meter->setGeometry(2,2,w-4,h-4);
  }
  else {
    switch(orientation) {
	case AMMeter::Left:
	  meter->setGeometry(2,2,w-4-h,h-4);
	  label_font=QFont("helvetica",height()-2,QFont::Bold);
	  label_font.setPixelSize(height()-2);
	  break;
	case AMMeter::Right:
	  meter->setGeometry(2+h,2,w-4-h,h-4);
	  label_font=QFont("helvetica",height()-2,QFont::Bold);
	  label_font.setPixelSize(height()-2);
	  break;
	case AMMeter::Up:
	  meter->setGeometry(2,2,w-4,h-4-w);
	  label_font=QFont("helvetica",width()-2,QFont::Bold);
	  label_font.setPixelSize(width()-2);
	  break;
	case AMMeter::Down:
	  meter->setGeometry(2,2+width(),w-4,h-4-w);
	  label_font=QFont("helvetica",width()-2,QFont::Bold);
	  label_font.setPixelSize(width()-2);
	  break;
    }
    makeFont();
  }
}


void AMPlayMeter::setGeometry(QRect &rect)
{
  setGeometry(rect.left(),rect.top(),rect.width(),rect.height());
}


void AMPlayMeter::setSolidBar(int level)
{
  meter->setSolidBar(level);
}


void AMPlayMeter::setPeakBar(int level)
{
  meter->setPeakBar(level);
}


void AMPlayMeter::setFloatingBar(int level)
{
  meter->setFloatingBar(level);
}


void AMPlayMeter::setSegmentSize(int size)
{
  meter->setSegmentSize(size);
}


void AMPlayMeter::setSegmentGap(int gap)
{
  meter->setSegmentGap(gap);
}


AMMeter::Mode AMPlayMeter::mode() const
{
  return meter->mode();
}


void AMPlayMeter::setMode(AMMeter::Mode mode)
{
  meter->setMode(mode);
}


void AMPlayMeter::updateLevel(int id,AMStereoChannel *schan)
{
  if((id!=meter_id)||(schan->channelNumber()!=meter_chan)) {
    return;
  }
  setPeakBar(schan->dbuLevel(meter_role));
}


void AMPlayMeter::paintEvent(QPaintEvent *paintEvent)
{
  //
  // Setup
  //
  QPainter *p=new QPainter(this);
  p->setFont(label_font);
  p->setPen(white);
  if(!meter_label.isEmpty()) {
    switch(orientation) {
	case AMMeter::Left:
	  p->drawText(width()-height()+meter_label_x,height()-2,meter_label);
	  break;
	case AMMeter::Right:
	  p->drawText(meter_label_x,height()-2,meter_label);
	  break;
	case AMMeter::Up:
	  p->drawText(meter_label_x,height()-3,meter_label);
	  break;
	case AMMeter::Down:
	  p->drawText(meter_label_x,width()-1,meter_label);
	  break;
    }
  }
  p->end();
}



void AMPlayMeter::makeFont()
{
  switch(orientation) {
      case AMMeter::Left:
	label_font=QFont("helvetica",height()-2,QFont::Bold);
	label_font.setPixelSize(height()-2);
	meter_label_x=(height()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case AMMeter::Right:
	label_font=QFont("helvetica",height()-2,QFont::Bold);
	label_font.setPixelSize(height()-2);
	meter_label_x=(height()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case AMMeter::Up:
	label_font=QFont("helvetica",width()-2,QFont::Bold);
	label_font.setPixelSize(width()-2);
	meter_label_x=(width()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case AMMeter::Down:
	label_font=QFont("helvetica",width()-2,QFont::Bold);
	label_font.setPixelSize(width()-2);
	meter_label_x=(width()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
  }
}
