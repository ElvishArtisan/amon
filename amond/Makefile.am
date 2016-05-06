## automake.am
##
## AMon Metering Daemon
##
## (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.6 2008/10/22 23:25:01 cvs Exp $
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@ -g
INCLUDES = -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


sbin_PROGRAMS = amond

dist_amond_SOURCES = amond.cpp amond.h\
                     amon_socket.cpp amon_socket.h\
                     amon_oss.cpp\
                     globals.h

nodist_amond_SOURCES = moc_amond.cpp moc_amon_socket.cpp

amond_LDADD = @QT_LIBS@ -lamon -lportaudio -lsamplerate

EXTRA_DIST = 

CLEANFILES = *~ moc_* *.obj *.idb *.pdb *ilk
MAINTAINERCLEANFILES = *~ Makefile.in
