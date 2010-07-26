SUBDIRS = lib makeint makemat makepat juman dic autodic wikipediadic doc

jumanrcdir=$(sysconfdir)
nodist_jumanrc_DATA = jumanrc
dist_noinst_DATA = jumanrc.in
CLEANFILES = jumanrc
EXTRA_DIST = perl

jumanrc: jumanrc.in
	rm -f $@ $@.tmp
	sed 's,@pkgdatadir\@,$(pkgdatadir),g' $< > $@.tmp
	mv $@.tmp $@
