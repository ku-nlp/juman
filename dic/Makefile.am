
MAKEINT = ../makeint/makeint
MAKEMAT = ../makemat/makemat
MAKEPAT = ../makepat/makepat
DICSORT = ../makepat/dicsort
MKDARTS = ../darts/mkdarts

GRM_FILES = JUMAN.grammar JUMAN.kankei JUMAN.katuyou
TBL_FILES = jumandic.mat jumandic.tab
DIC_FILES = $(sort $(wildcard *.dic) Rendaku.dic)
INT_FILES = $(patsubst %.dic,%.int,$(DIC_FILES))

dicdir = $(pkgdatadir)/dic
nodist_dic_DATA = JUMAN.connect $(TBL_FILES) jumandic.dat jumandic.da
dist_dic_DATA = $(GRM_FILES)
EXTRA_DIST = JUMAN.connect.c *.dic

Rendaku.dic: ContentW.dic
	tac $< | perl rendaku.perl | tac > $@

.dic.int: $(MAKEINT)
	$(MAKEINT) $<

clean:
	rm -f jumandic.* *.int JUMAN.connect Rendaku.dic

JUMAN.connect: JUMAN.connect.c
	@CPP@ -P $< | LANG=C sed "s/\(\#pragma\)/\;\1/" > $@

jumandic.tab: $(GRM_FILES) JUMAN.connect $(MAKEMAT)
	$(MAKEMAT)

jumandic.mat: jumandic.tab

$(INT_FILES): $(GRM_FILES) $(TBL_FILES) $(MAKEINT)

jumandic.txt: $(INT_FILES)
	cat $(INT_FILES) > $@

jumandic.dat: jumandic.txt $(DICSORT)
	$(DICSORT) jumandic.txt > $@

jumandic.da: jumandic.dat $(MKDARTS)
	$(MKDARTS) jumandic.dat $@
