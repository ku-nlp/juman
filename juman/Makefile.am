bin_PROGRAMS = juman

INCLUDES = -I../lib

juman_SOURCES = client.c juman.c server.c icu_conv.c icu_conv.h
juman_LDADD = $(top_builddir)/lib/libjuman.la @NET_LIBS@ @ICU_LIBS@
