// ammonconfig.cpp
//
// A container class for an AMon Client configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ammonconfig.cpp,v 1.11 2008/11/18 19:13:21 cvs Exp $
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

#include <qnamespace.h>

#include <am.h>
#include <ammeter.h>
#include <ammonconfig.h>
#include <amprofile.h>
#include <amstereometer.h>


AMMonConfig::AMMonConfig(QPalette p)
{
  clear();
  conf_default_palette=p;
}


AMMonConfig::AMMonConfig(QString filename)
{
  clear();
  conf_filename=filename;
}


QString AMMonConfig::filename() const
{
  return conf_filename;
}


void AMMonConfig::setFilename(QString filename)
{
  conf_filename=filename;
}


QSize AMMonConfig::globalSize() const
{
  return conf_global_size;
}


QPalette AMMonConfig::globalPalette() const
{
  return conf_global_palette;
}


QString AMMonConfig::globalCaption() const
{
  return conf_global_caption;
}


int AMMonConfig::globalAveragingPeriods() const
{
  return conf_global_averaging_periods;
}


unsigned AMMonConfig::globalScanSources() const
{
  return conf_global_scan_sources.size();
}


unsigned AMMonConfig::globalScanSource(unsigned n) const
{
  return conf_global_scan_sources[n];
}


unsigned AMMonConfig::globalScanDwellTime() const
{
  return conf_global_scan_dwell_time;
}


unsigned AMMonConfig::globalScanResetTime() const
{
  return conf_global_scan_reset_time;
}


unsigned AMMonConfig::meterQuantity() const
{
  return conf_meter_orientations.size();
}


AMMeter::Orientation AMMonConfig::meterOrientation(unsigned n) const
{
  return conf_meter_orientations[n];
}


QRect AMMonConfig::meterGeometry(unsigned n) const
{
  return conf_meter_geometries[n];
}


QString AMMonConfig::meterMonitorHost(unsigned n) const
{
  return conf_meter_monitor_hosts[n];
}


Q_UINT16 AMMonConfig::meterMonitorPort(unsigned n) const
{
  return conf_meter_monitor_ports[n];
}


int AMMonConfig::meterMonitorChannel(unsigned n) const
{
  return conf_meter_monitor_channels[n];
}


int AMMonConfig::meterMinLevel(unsigned n) const
{
  return conf_meter_min_levels[n];
}


int AMMonConfig::meterMaxLevel(unsigned n) const
{
  return conf_meter_max_levels[n];
}


int AMMonConfig::meterYellowLevel(unsigned n) const
{
  return conf_meter_yellow_levels[n];
}


int AMMonConfig::meterRedLevel(unsigned n) const
{
  return conf_meter_red_levels[n];
}


QColor AMMonConfig::meterLowColor(unsigned n) const
{
  return conf_meter_low_colors[n];
}


QColor AMMonConfig::meterMidColor(unsigned n) const
{
  return conf_meter_mid_colors[n];
}


QColor AMMonConfig::meterHighColor(unsigned n) const
{
  return conf_meter_high_colors[n];
}


QColor AMMonConfig::meterLowBackground(unsigned n) const
{
  return conf_meter_low_backgrounds[n];
}



QColor AMMonConfig::meterMidBackground(unsigned n) const
{
  return conf_meter_mid_backgrounds[n];
}


QColor AMMonConfig::meterHighBackground(unsigned n) const
{
  return conf_meter_high_backgrounds[n];
}


unsigned AMMonConfig::meterDisplayElements(unsigned n) const
{
  return conf_meter_display_elements[n];
}


unsigned AMMonConfig::labelQuantity() const
{
  return conf_label_geometries.size();
}


QRect AMMonConfig::labelGeometry(unsigned n) const
{
  return conf_label_geometries[n];
}


QString AMMonConfig::labelText(unsigned n) const
{
  return conf_label_texts[n];
}


QPalette AMMonConfig::labelPalette(unsigned n) const
{
  return conf_label_palettes[n];
}


QFont AMMonConfig::labelFont(unsigned n) const
{
  return conf_label_fonts[n];
}


int AMMonConfig::labelAlignment(unsigned n) const
{
  return conf_label_alignments[n];
}


unsigned AMMonConfig::alarmQuantity() const
{
  return conf_alarm_geometries.size();
}


QRect AMMonConfig::alarmGeometry(unsigned n) const
{
  return conf_alarm_geometries[n];
}


QString AMMonConfig::alarmMonitorHost(unsigned n) const
{
  return conf_alarm_monitor_hosts[n];
}


Q_UINT16 AMMonConfig::alarmMonitorPort(unsigned n) const
{
  return conf_alarm_monitor_ports[n];
}


int AMMonConfig::alarmMonitorChannel(unsigned n) const
{
  return conf_alarm_monitor_channels[n];
}


QString AMMonConfig::alarmText(unsigned n) const
{
  return conf_alarm_texts[n];
}


QPalette AMMonConfig::alarmActivePalette(unsigned n) const
{
  return conf_alarm_active_palettes[n];
}


QPalette AMMonConfig::alarmInactivePalette(unsigned n) const
{
  return conf_alarm_inactive_palettes[n];
}


AMStereoChannel::Alarm AMMonConfig::alarmType(unsigned n) const
{
  return conf_alarm_alarm_types[n];
}


QString AMMonConfig::alarmAction(unsigned n) const
{
  return conf_alarm_actions[n];
}


QFont AMMonConfig::alarmFont(unsigned n) const
{
  return conf_alarm_fonts[n];
}


int AMMonConfig::alarmAlignment(unsigned n) const
{
  return conf_alarm_alignments[n];
}


unsigned AMMonConfig::buttonQuantity() const
{
  return conf_button_geometries.size();
}


QRect AMMonConfig::buttonGeometry(unsigned n) const
{
  return conf_button_geometries[n];
}


QString AMMonConfig::buttonText(unsigned n) const
{
  return conf_button_texts[n];
}


QString AMMonConfig::buttonMonitorHost(unsigned n) const
{
  return conf_button_monitor_hosts[n];
}


Q_UINT16 AMMonConfig::buttonMonitorPort(unsigned n) const
{
  return conf_button_monitor_ports[n];
}


int AMMonConfig::buttonMonitorChannel(unsigned n) const
{
  return conf_button_monitor_channels[n];
}


void AMMonConfig::load()
{
  bool ok=false;
  QString orientation;
  int num=1;
  QString font_weight_name;
  QFont::Weight font_weight=QFont::Normal;
  QString alignment_name;
  int alignment=0;
  QString section;
  unsigned source;

  AMProfile *profile=new AMProfile();
  profile->setSource(conf_filename);

  //
  // Load Globals
  //
  conf_global_size=QSize(profile->intValue("Global","XSize",400),
			 profile->intValue("Global","YSize",300));
  conf_global_palette.setColor(QPalette::Active,QColorGroup::Background,
	profile->stringValue("Global","BackgroundColor",
			     conf_default_palette.color(QPalette::Active,
					 QColorGroup::Background).name()));
  conf_global_palette.setColor(QPalette::Inactive,QColorGroup::Background,
	profile->stringValue("Global","BackgroundColor",
			     conf_default_palette.color(QPalette::Active,
					 QColorGroup::Background).name()));
  conf_global_caption=
    profile->stringValue("Global","Caption",conf_global_caption);
  conf_global_averaging_periods=profile->intValue("Global","AveragingPeriods",
						conf_global_averaging_periods);
  source=
    profile->intValue("Global",QString().sprintf("ScanSource%d",num++),0,&ok);
  while(ok) {
    conf_global_scan_sources.push_back(source);
    source=profile->
      intValue("Global",QString().sprintf("ScanSource%d",num++),0,&ok);
  }
  conf_global_scan_dwell_time=
    profile->intValue("Global","ScanDwellTime",AMON_DEFAULT_SCAN_DWELL_TIME);
  conf_global_scan_reset_time=
    profile->intValue("Global","ScanResetTime",AMON_DEFAULT_SCAN_RESET_TIME);

  //
  // Load Meters
  //
  num=1;
  section=QString().sprintf("Meter%d",num);
  orientation=profile->stringValue(section,"Orientation","",&ok);
  while(ok) {
    if(orientation.lower()=="horizontal") {
      conf_meter_orientations.push_back(AMMeter::Right);
    }
    else {
      conf_meter_orientations.push_back(AMMeter::Up);
    }
    conf_meter_geometries.
      push_back(QRect(profile->intValue(section,"XPos",10),
		      profile->intValue(section,"YPos",10),
		      profile->intValue(section,"XSize",20),
		      profile->intValue(section,"YSize",200)));
    conf_meter_monitor_hosts.
      push_back(profile->stringValue(section,"MonitorHost","localhost"));
    conf_meter_monitor_ports.
      push_back(profile->intValue(section,"MonitorPort",AMON_DEFAULT_SOCKET));
    conf_meter_monitor_channels.
      push_back(profile->intValue(section,"MonitorChannel",1)-1);
    conf_meter_min_levels.
      push_back((int)(profile->doubleValue(section,"MinLevel",-30.0)*100.0));
    conf_meter_max_levels.
      push_back((int)(profile->doubleValue(section,"MaxLevel",18.0)*100.0));
    conf_meter_yellow_levels.
      push_back((int)(profile->doubleValue(section,"YellowLevel",4.0)*100.0));
    conf_meter_red_levels.
      push_back((int)(profile->doubleValue(section,"RedLevel",8.0)*100.0));
    conf_meter_low_colors.push_back(QColor(profile->
	      stringValue(section,"LowColor",QColor(DEFAULT_LOW_COLOR).name())));
    conf_meter_mid_colors.push_back(QColor(profile->
	      stringValue(section,"MidColor",DEFAULT_HIGH_COLOR.name())));
    conf_meter_high_colors.push_back(QColor(profile->
	      stringValue(section,"HighColor",DEFAULT_CLIP_COLOR.name())));
    conf_meter_low_backgrounds.push_back(QColor(profile->
	      stringValue(section,"LowBackground",DEFAULT_DARK_LOW_COLOR.name())));
    conf_meter_mid_backgrounds.push_back(QColor(profile->
	      stringValue(section,"MidBackground",DEFAULT_DARK_HIGH_COLOR.name())));
    conf_meter_high_backgrounds.push_back(QColor(profile->
	      stringValue(section,"HighBackground",DEFAULT_DARK_CLIP_COLOR.name())));
    conf_meter_display_elements.push_back(0);
    if(profile->boolValue(section,"DisplayLeft",true)) {
      conf_meter_display_elements.back()|=AMStereoMeter::LeftMeter;
    }
    if(profile->boolValue(section,"DisplayRight",true)) {
      conf_meter_display_elements.back()|=AMStereoMeter::RightMeter;
    }
    if(profile->boolValue(section,"DisplaySum",false)) {
      conf_meter_display_elements.back()|=AMStereoMeter::SumMeter;
    }
    if(profile->boolValue(section,"DisplayDiff",false)) {
      conf_meter_display_elements.back()|=AMStereoMeter::DiffMeter;
    }
    section=QString().sprintf("Meter%d",++num);
    orientation=profile->stringValue(section,"Orientation","",&ok);
  }

  //
  // Load Alarms
  //
  num=1;
  section=QString().sprintf("Alarm%d",num);
  conf_alarm_monitor_hosts.
    push_back(profile->stringValue(section,"MonitorHost","localhost",&ok));
  while(ok) {
    conf_alarm_geometries.
      push_back(QRect(profile->intValue(section,"XPos",10),
		      profile->intValue(section,"YPos",10),
		      profile->intValue(section,"XSize",20),
		      profile->intValue(section,"YSize",200)));
    conf_alarm_monitor_ports.
      push_back(profile->intValue(section,"MonitorPort",AMON_DEFAULT_SOCKET));
    conf_alarm_monitor_channels.
      push_back(profile->intValue(section,"MonitorChannel",1)-1);
    conf_alarm_texts.push_back(profile->stringValue(section,"Text",""));
    conf_alarm_active_palettes.push_back(conf_default_palette);
    conf_alarm_active_palettes.back().
      setColor(QPalette::Active,QColorGroup::Foreground,
	       profile->stringValue(section,"ActiveTextColor",
				    conf_default_palette.
				    color(QPalette::Active,QColorGroup::Text).
				    name()));
    conf_alarm_active_palettes.back().
      setColor(QPalette::Inactive,QColorGroup::Foreground,
	       profile->stringValue(section,"ActiveTextColor",
				    conf_default_palette.
				    color(QPalette::Active,
					  QColorGroup::Text).name()));
    conf_alarm_active_palettes.back().
      setColor(QPalette::Active,QColorGroup::Background,
	       profile->stringValue(section,"ActiveBackgroundColor",
				    conf_default_palette.
				    color(QPalette::Active,
					  QColorGroup::Background).name()));
    conf_alarm_active_palettes.back().
      setColor(QPalette::Inactive,QColorGroup::Background,
	       profile->stringValue(section,"ActiveBackgroundColor",
				    conf_default_palette.
				    color(QPalette::Active,
					  QColorGroup::Background).name()));
    conf_alarm_inactive_palettes.push_back(conf_default_palette);
    conf_alarm_inactive_palettes.back().
      setColor(QPalette::Active,QColorGroup::Foreground,
	       profile->stringValue(section,"InactiveTextColor",
				    conf_default_palette.
				    color(QPalette::Active,QColorGroup::Text).
				    name()));
    conf_alarm_inactive_palettes.back().
      setColor(QPalette::Inactive,QColorGroup::Foreground,
	       profile->stringValue(section,"InactiveTextColor",
				    conf_default_palette.
				    color(QPalette::Active,
					  QColorGroup::Text).name()));
    conf_alarm_inactive_palettes.back().
      setColor(QPalette::Active,QColorGroup::Background,
	       profile->stringValue(section,"InactiveBackgroundColor",
				    conf_default_palette.
				    color(QPalette::Active,
					  QColorGroup::Background).name()));
    conf_alarm_inactive_palettes.back().
      setColor(QPalette::Inactive,QColorGroup::Background,
	       profile->stringValue(section,"InactiveBackgroundColor",
				    conf_default_palette.
				    color(QPalette::Active,
					  QColorGroup::Background).name()));
    conf_alarm_alarm_types.push_back(AMStereoChannel::NoAlarm);
    if(profile->stringValue(section,"AlarmType","").lower()=="highlevel") {
      conf_alarm_alarm_types.back()=AMStereoChannel::HighLevelAlarm;
    }
    if(profile->stringValue(section,"AlarmType","").lower()=="lowlevel") {
      conf_alarm_alarm_types.back()=AMStereoChannel::LowLevelAlarm;
    }
    if(profile->stringValue(section,"AlarmType","").lower()=="highdiff") {
      conf_alarm_alarm_types.back()=AMStereoChannel::HighDifferenceAlarm;
    }
    if(profile->stringValue(section,"AlarmType","").lower()=="summary") {
      conf_alarm_alarm_types.back()=AMStereoChannel::SummaryAlarm;
    }
    conf_alarm_actions.push_back(profile->
				 stringValue(section,"AlarmAction",""));
    font_weight_name=profile->stringValue(section,"FontWeight","normal");
    if(font_weight_name.lower()=="light") {
      font_weight=QFont::Light;
    }
    if(font_weight_name.lower()=="normal") {
      font_weight=QFont::Normal;
    }
    if(font_weight_name.lower()=="demibold") {
      font_weight=QFont::DemiBold;
    }
    if(font_weight_name.lower()=="bold") {
      font_weight=QFont::Bold;
    }
    if(font_weight_name.lower()=="black") {
      font_weight=QFont::Black;
    }
    conf_alarm_fonts.
      push_back(QFont(profile->stringValue(section,"FontName","helvetica"),
		      profile->intValue(section,"FontSize",12),
		      font_weight));
    conf_alarm_fonts.back().
      setPixelSize(profile->intValue(section,"FontSize",12));
    alignment_name=profile->
      stringValue(section,"HorizontalAlignment","center");
    if(alignment_name.lower()=="left") {
      alignment=Qt::AlignLeft;
    }
    if(alignment_name.lower()=="center") {
      alignment=Qt::AlignCenter;
    }
    if(alignment_name.lower()=="right") {
      alignment=Qt::AlignRight;
    }
    alignment_name=profile->
      stringValue(section,"VerticalAlignment","center");
    if(alignment_name.lower()=="top") {
      alignment|=Qt::AlignTop;
    }
    if(alignment_name.lower()=="center") {
      alignment|=Qt::AlignVCenter;
    }
    if(alignment_name.lower()=="bottom") {
      alignment|=Qt::AlignBottom;
    }
    conf_alarm_alignments.push_back(alignment);

    section=QString().sprintf("Alarm%d",++num);
    conf_alarm_monitor_hosts.
      push_back(profile->stringValue(section,"MonitorHost","localhost",&ok));
  }

  //
  // Load Labels
  //
  num=1;
  section=QString().sprintf("Label%d",num);
  profile->intValue(section,"XPos",10,&ok);
  while(ok) {
    conf_label_geometries.
      push_back(QRect(profile->intValue(section,"XPos",10),
		      profile->intValue(section,"YPos",10),
		      profile->intValue(section,"XSize",20),
		      profile->intValue(section,"YSize",200)));
    conf_label_texts.push_back(profile->stringValue(section,"Text",section));
    conf_label_palettes.push_back(conf_default_palette);
    conf_label_palettes.back().setColor(QPalette::Active,QColorGroup::Text,
	 profile->stringValue(section,"TextColor",conf_default_palette.
		     color(QPalette::Active,QColorGroup::Text).name()));
    conf_label_palettes.back().setColor(QPalette::Inactive,QColorGroup::Text,
	 profile->stringValue(section,"TextColor",conf_default_palette.
		     color(QPalette::Active,QColorGroup::Text).name()));
    conf_label_palettes.back().
      setColor(QPalette::Active,QColorGroup::Foreground,
	 profile->stringValue(section,"TextColor",conf_default_palette.
		     color(QPalette::Active,QColorGroup::Text).name()));
    conf_label_palettes.back().
      setColor(QPalette::Inactive,QColorGroup::Foreground,
	 profile->stringValue(section,"TextColor",conf_default_palette.
		     color(QPalette::Active,QColorGroup::Text).name()));
    conf_label_palettes.back().
      setColor(QPalette::Active,QColorGroup::Background,
	 profile->stringValue(section,"BackgroundColor",conf_default_palette.
		     color(QPalette::Active,QColorGroup::Background).name()));
    conf_label_palettes.back().
      setColor(QPalette::Inactive,QColorGroup::Background,
	 profile->stringValue(section,"BackgroundColor",conf_default_palette.
		     color(QPalette::Active,QColorGroup::Background).name()));
    profile->intValue(section,"XPos",10,&ok);
    font_weight_name=profile->stringValue(section,"FontWeight","normal");
    if(font_weight_name.lower()=="light") {
      font_weight=QFont::Light;
    }
    if(font_weight_name.lower()=="normal") {
      font_weight=QFont::Normal;
    }
    if(font_weight_name.lower()=="demibold") {
      font_weight=QFont::DemiBold;
    }
    if(font_weight_name.lower()=="bold") {
      font_weight=QFont::Bold;
    }
    if(font_weight_name.lower()=="black") {
      font_weight=QFont::Black;
    }
    conf_label_fonts.
      push_back(QFont(profile->stringValue(section,"FontName","helvetica"),
		      profile->intValue(section,"FontSize",12),
		      font_weight));
    conf_label_fonts.back().
      setPixelSize(profile->intValue(section,"FontSize",12));
    alignment_name=profile->
      stringValue(section,"HorizontalAlignment","center");
    if(alignment_name.lower()=="left") {
      alignment=Qt::AlignLeft;
    }
    if(alignment_name.lower()=="center") {
      alignment=Qt::AlignCenter;
    }
    if(alignment_name.lower()=="right") {
      alignment=Qt::AlignRight;
    }
    alignment_name=profile->
      stringValue(section,"VerticalAlignment","center");
    if(alignment_name.lower()=="top") {
      alignment|=Qt::AlignTop;
    }
    if(alignment_name.lower()=="center") {
      alignment|=Qt::AlignVCenter;
    }
    if(alignment_name.lower()=="bottom") {
      alignment|=Qt::AlignBottom;
    }
    conf_label_alignments.push_back(alignment);

    section=QString().sprintf("Label%d",++num);
    profile->intValue(section,"XPos",10,&ok);
  }

  //
  // Load Monitor Buttons
  //
  num=1;
  section=QString().sprintf("MonitorButton%d",num);
  profile->intValue(section,"XPos",10,&ok);
  while(ok) {
    conf_button_geometries.
      push_back(QRect(profile->intValue(section,"XPos",10),
		      profile->intValue(section,"YPos",10),
		      profile->intValue(section,"XSize",20),
		      profile->intValue(section,"YSize",200)));
    conf_button_texts.push_back(profile->stringValue(section,"Text",section));
    conf_button_monitor_hosts.
      push_back(profile->stringValue(section,"MonitorHost","localhost"));
    conf_button_monitor_ports.
      push_back(profile->intValue(section,"MonitorPort",AMON_DEFAULT_SOCKET));
    conf_button_monitor_channels.
      push_back(profile->intValue(section,"MonitorChannel",1));
    section=QString().sprintf("MonitorButton%d",++num);
    profile->intValue(section,"XPos",10,&ok);
  }
  delete profile;
}


void AMMonConfig::clear()
{
  conf_filename=AMON_DEFAULT_CONF_FILE;
  conf_global_palette=conf_default_palette;
  conf_global_size=QSize(400,300);
  conf_global_caption="Amon Monitor";
  conf_global_averaging_periods=AMON_DEFAULT_AVERAGING_PERIODS;
  conf_meter_orientations.clear();
  conf_meter_geometries.clear();
  conf_meter_monitor_hosts.clear();
  conf_meter_monitor_ports.clear();
  conf_meter_monitor_channels.clear();
  conf_meter_min_levels.clear();
  conf_meter_max_levels.clear();
  conf_meter_yellow_levels.clear();
  conf_meter_red_levels.clear();
  conf_meter_display_elements.clear();
  conf_label_geometries.clear();
  conf_label_texts.clear();
  conf_label_palettes.clear();
  conf_label_fonts.clear();
  conf_label_alignments.clear();
  conf_alarm_geometries.clear();
  conf_alarm_monitor_hosts.clear();
  conf_alarm_monitor_ports.clear();
  conf_alarm_monitor_channels.clear();
  conf_alarm_texts.clear();
  conf_alarm_active_palettes.clear();
  conf_alarm_inactive_palettes.clear();
  conf_alarm_alarm_types.clear();
  conf_alarm_actions.clear();
  conf_button_geometries.clear();
  conf_button_texts.clear();
  conf_button_monitor_hosts.clear();
  conf_button_monitor_ports.clear();
  conf_global_scan_sources.clear();
  conf_global_scan_dwell_time=AMON_DEFAULT_SCAN_DWELL_TIME;
  conf_global_scan_reset_time=AMON_DEFAULT_SCAN_RESET_TIME;
}
