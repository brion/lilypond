
depth = ..

NAME = lilypond

MODULE_LIBS=$(depth)/flower
MODULE_INCLUDES= $(depth)/flower/include


HELP2MAN_EXECS = lilypond
STEPMAKE_TEMPLATES=c c++ executable po help2man

OUT_DIST_FILES=$(addprefix $(outdir)/,parser.hh parser.cc)

include $(depth)/make/stepmake.make

ifeq ($(LINK_GXX_STATICALLY),yes)
MODULE_LDFLAGS += -L$(outdir) -static-libgcc
endif

CXXFLAGS += -Woverloaded-virtual


# for profiling, link guile statically:
#
# CONFIG_LDFLAGS += -static -lltdl -ldl
#

ifeq ($(PLATFORM_WINDOWS),yes)
WINDRES_FLAGS += -DLilyPond=0 -DLY=1
O_FILES += $(outdir)/lilypond.rc.o
$(outdir)/lilypond: $(outdir)/lilypond.rc.o

$(outdir)/lilypond.rc.o: $(outdir)/lilypond.ico $(outdir)/ly.ico
$(top-build-dir)/Documentation/logo/$(outbase)/lilypond.ico:
	$(MAKE) -C $(top-build-dir)/Documentation/logo

$(outdir)/lilypond.ico: $(top-build-dir)/Documentation/logo/$(outbase)/lilypond.ico
	cp $< $@
$(top-build-dir)/Documentation/logo/$(outbase)/ly.ico:
	$(MAKE) -C $(top-build-dir)/Documentation/logo

$(outdir)/ly.ico: $(top-build-dir)/Documentation/logo/$(outbase)/ly.ico
	cp $< $@
endif

ETAGS_FLAGS += --regex='{c++}/^LY_DEFINE *(\([^,]+\)/\1/'
ETAGS_FLAGS += --regex='{c++}/^LY_DEFINE *([^"]*"\([^"]+\)"/\1/'

default:


$(outdir)/libstdc++.a:
	rm -f $@
	ln -s `$(CXX) -print-file-name=libstdc++.a` $(outdir)

ifeq ($(LINK_GXX_STATICALLY),yes)
$(outdir)/lilypond: $(outdir)/libstdc++.a
endif

# force these: Make can't know these have to be generated in advance
$(outdir)/lily-parser.o $(outdir)/parser.o: $(outdir)/parser.hh
$(outdir)/lily-lexer.o: $(outdir)/parser.hh $(outdir)/FlexLexer.h
$(outdir)/lexer.o: $(outdir)/parser.hh $(outdir)/version.hh
$(outdir)/lily-lexer-scheme.o \
  $(outdir)/lily-parser-scheme.o \
  $(outdir)/parse-scm.o: $(outdir)/parser.hh

ifneq ($(FLEXLEXER_FILE),)
$(outdir)/FlexLexer.h: $(FLEXLEXER_FILE) $(config_h)
	cp $< $@
endif

$(outdir)/lexer.cc: $(config_h)
$(outdir)/parser.cc: $(config_h)

ifneq ($(LILYPOND_EXTERNAL_BINARY),)
$(outdir)/lilypond.1: $(LILYPOND_EXTERNAL_BINARY) $(buildscript-dir)/help2man
	$(buildscript-dir)/help2man $(LILYPOND_EXTERNAL_BINARY) > $@
endif

$(outdir)/general-scheme.o: $(outdir)/version.hh
$(outdir)/lily-guile.o: $(outdir)/version.hh
$(outdir)/lily-version.o: $(outdir)/version.hh
$(outdir)/main.o: $(outdir)/version.hh
$(outdir)/relocate.o: $(outdir)/version.hh
$(outdir)/warn-scheme.o: $(outdir)/version.hh

local-po: $(outdir)/parser.cc
