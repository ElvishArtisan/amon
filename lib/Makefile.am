## automake.am
##
## Automake.am for amon/lib
##
## (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.10 2008/11/04 23:27:34 cvs Exp $
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall @QT_CXXFLAGS@
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


lib_LTLIBRARIES = libamon.la
dist_libamon_la_SOURCES = ammeter.cpp ammeter.h\
                          amplaymeter.cpp amplaymeter.h\
                          amcmdswitch.cpp amcmdswitch.h\
                          amparser.cpp amparser.h\
                          amstereochannel.cpp amstereochannel.h\
                          amstereometer.cpp amstereometer.h\
                          ammonconfig.cpp ammonconfig.h\
                          ammondaemonconfig.cpp ammondaemonconfig.h\
                          amprofile.cpp amprofile.h\
                          amprofilesection.cpp amprofilesection.h\
                          amprofileline.cpp amprofileline.h\
                          amlabel.cpp amlabel.h\
                          amringbuffer.cpp amringbuffer.h\
                          amplayer.cpp amplayer.h\
                          amtransportbutton.cpp amtransportbutton.h\
                          ampushbutton.cpp ampushbutton.h\
                          ammeteraverage.cpp ammeteraverage.h\
                          am.h\
                          rtpbye.cpp rtpbye.h\
                          rtpconnection.cpp rtpconnection.h\
                          rtpcontrolheader.cpp rtpcontrolheader.h\
                          rtpheader.cpp rtpheader.h\
                          rtpreceptionblock.cpp rtpreceptionblock.h\
                          rtpreceptionreport.cpp rtpreceptionreport.h\
                          rtpsenderreport.cpp rtpsenderreport.h\
                          rtpsourcedescription.cpp rtpsourcedescription.h

nodist_libamon_la_SOURCES = moc_ammeter.cpp moc_amplaymeter.cpp\
                            moc_amstereometer.cpp moc_amparser.cpp\
                            moc_amlabel.cpp moc_amplayer.cpp\
                            moc_amtransportbutton.cpp moc_ampushbutton.cpp

libamon_la_LDFLAGS = -release $(VERSION)

EXTRA_DIST = 

CLEANFILES = *~ moc_*
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz moc_*
