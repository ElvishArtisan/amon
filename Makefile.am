## automake.am
##
## Top level automake.am for AMon
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.11 2011/08/03 19:22:02 pcvs Exp $
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

SUBDIRS = icons xdg lib amond amon amonplayer

install-exec-local:	
	cp rc.amond /etc/init.d/amond

uninstall-local:	
	rm /etc/init.d/amond

rpm:	dist
	cp amon-$(VERSION).tar.gz $(RPM_ROOT)/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux amon.spec
	mv $(RPM_ROOT)/RPMS/$(ARCH)/amon-$(VERSION)-@RPM_RELEASE@.$(ARCH).rpm $(top_srcdir)/
	mv $(RPM_ROOT)/SRPMS/amon-$(VERSION)-@RPM_RELEASE@.src.rpm $(top_srcdir)/
	rm $(RPM_ROOT)/SOURCES/amon-$(VERSION).tar.gz
	rm -rf $(RPM_ROOT)/BUILD/amon-$(VERSION)

zip:	dist
	tar -zxf amon-@VERSION@.tar.gz
	zip -r amon-@VERSION@.zip amon-@VERSION@
	rm -rf amon-@VERSION@

slack:	all
	./make_slack

EXTRA_DIST = autogen.sh amon.spec.in protocol.txt amon.conf-sample\
             amond.conf-sample rc.amond.in COPYING make_slack.in\
             slack-desc.in slack_doinst.sh rc.amond.slackware\
             get_target.sh get_distro.sh

CLEANFILES = *~ *.tar.gz *.tgz *.mo *.rpm *.zip
DISTCLEANFILES = *.spec rc.amond make_slack slack-desc
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz\
                       config.guess config.status config.sub install-sh\
                       libtool ltmain.sh missing mkinstalldirs depcomp\
                       *.spec
