// ammonconfig.h
//
// A container class for an AMon Client configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ammonconfig.h,v 1.9 2008/11/18 19:13:21 cvs Exp $
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

#ifndef AMMONCONFIG_H
#define AMMONCONFIG_H

#include <vector>

#include <qpalette.h>
#include <qrect.h>
#include <qfont.h>
#include <qcolor.h>

#include <ammeter.h>
#include <amstereochannel.h>

class AMMonConfig
{
 public:
  AMMonConfig(QPalette p);
  AMMonConfig(QString filename);
  QString filename() const;
  void setFilename(QString filename);
  QSize globalSize() const;
  QPalette globalPalette() const;
  QString globalCaption() const;
  int globalAveragingPeriods() const;
  unsigned globalScanSources() const;
  unsigned globalScanSource(unsigned n) const;
  unsigned globalScanDwellTime() const;
  unsigned globalScanResetTime() const;
  unsigned meterQuantity() const;
  AMMeter::Orientation meterOrientation(unsigned n) const;
  QRect meterGeometry(unsigned n) const;
  QString meterMonitorHost(unsigned n) const;
  Q_UINT16 meterMonitorPort(unsigned n) const;
  int meterMonitorChannel(unsigned n) const;
  int meterMinLevel(unsigned n) const;
  int meterMaxLevel(unsigned n) const;
  int meterYellowLevel(unsigned n) const;
  int meterRedLevel(unsigned n) const;
  QColor meterLowColor(unsigned n) const;
  QColor meterMidColor(unsigned n) const;
  QColor meterHighColor(unsigned n) const;
  QColor meterLowBackground(unsigned n) const;
  QColor meterMidBackground(unsigned n) const;
  QColor meterHighBackground(unsigned n) const;
  unsigned meterDisplayElements(unsigned n) const;
  unsigned labelQuantity() const;
  QRect labelGeometry(unsigned n) const;
  QString labelText(unsigned n) const;
  QPalette labelPalette(unsigned n) const;
  QFont labelFont(unsigned n) const;
  int labelAlignment(unsigned n) const;
  unsigned alarmQuantity() const;
  QRect alarmGeometry(unsigned n) const;
  QString alarmMonitorHost(unsigned n) const;
  Q_UINT16 alarmMonitorPort(unsigned n) const;
  int alarmMonitorChannel(unsigned n) const;
  QString alarmText(unsigned n) const;
  QPalette alarmActivePalette(unsigned n) const;
  QPalette alarmInactivePalette(unsigned n) const;
  AMStereoChannel::Alarm alarmType(unsigned n) const;
  QString alarmAction(unsigned n) const;
  QFont alarmFont(unsigned n) const;
  int alarmAlignment(unsigned n) const;
  unsigned buttonQuantity() const;
  QRect buttonGeometry(unsigned n) const;
  QString buttonText(unsigned n) const;
  QString buttonMonitorHost(unsigned n) const;
  Q_UINT16 buttonMonitorPort(unsigned n) const;
  int buttonMonitorChannel(unsigned n) const;
  void load();
  void clear();

 private:
  QString conf_filename;
  QPalette conf_default_palette;
  QSize conf_global_size;
  QString conf_global_caption;
  QPalette conf_global_palette;
  int conf_global_averaging_periods;
  std::vector<AMMeter::Orientation> conf_meter_orientations;
  std::vector<QRect> conf_meter_geometries;
  std::vector<QString> conf_meter_monitor_hosts;
  std::vector<Q_UINT16> conf_meter_monitor_ports;
  std::vector<int> conf_meter_monitor_channels;
  std::vector<int> conf_meter_max_levels;
  std::vector<int> conf_meter_min_levels;
  std::vector<int> conf_meter_yellow_levels;
  std::vector<int> conf_meter_red_levels;
  std::vector<QColor> conf_meter_low_colors;
  std::vector<QColor> conf_meter_mid_colors;
  std::vector<QColor> conf_meter_high_colors;
  std::vector<QColor> conf_meter_low_backgrounds;
  std::vector<QColor> conf_meter_mid_backgrounds;
  std::vector<QColor> conf_meter_high_backgrounds;
  std::vector<unsigned> conf_meter_display_elements;
  std::vector<QRect> conf_label_geometries;
  std::vector<QString> conf_label_texts;
  std::vector<QPalette> conf_label_palettes;
  std::vector<QFont> conf_label_fonts;
  std::vector<int> conf_label_alignments;
  std::vector<QRect> conf_alarm_geometries;
  std::vector<QString> conf_alarm_monitor_hosts;
  std::vector<Q_UINT16> conf_alarm_monitor_ports;
  std::vector<int> conf_alarm_monitor_channels;
  std::vector<QString> conf_alarm_texts;
  std::vector<QPalette> conf_alarm_active_palettes;
  std::vector<QPalette> conf_alarm_inactive_palettes;
  std::vector<AMStereoChannel::Alarm> conf_alarm_alarm_types;
  std::vector<QString> conf_alarm_actions;
  std::vector<QFont> conf_alarm_fonts;
  std::vector<int> conf_alarm_alignments;
  std::vector<QRect> conf_button_geometries;
  std::vector<QString> conf_button_texts;
  std::vector<QString> conf_button_monitor_hosts;
  std::vector<Q_UINT16> conf_button_monitor_ports;
  std::vector<int> conf_button_monitor_channels;
  std::vector<unsigned> conf_global_scan_sources;
  unsigned conf_global_scan_dwell_time;
  unsigned conf_global_scan_reset_time;
};


#endif  // AMMONCONFIG_H
