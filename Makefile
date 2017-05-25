first: all

TOPDIR	=	.
include config.mk

# Sub directories to build
SRCDIRS		=	src
PLUGINSDIR	=	plugins
MODULESDIR      =       submodules

SUBDIRS		=	$(SRCDIRS) $(MODULESDIR)

#LIBS		=	$(addsuffix /out.a,$(SUBDIRS))
MODULES         =       $(wildcard $(addsuffix /*.a,$(MODULESDIR)))

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@echo " [ LD ] $(BINFILE)"
	@$(CC) -o $(BINFILE) $(CFLAGS) -Wl,--whole-archive $(addsuffix /out.a,$(SRCDIRS)) -Wl,--no-whole-archive $(MODULES) $(LDFLAGS)
	
	@echo "Build complete."
	@echo 

clean: $(SUBDIRS)
	@echo " [ RM ] $(BINFILE)"
	@$(RM) $(BINFILE)
	
	@echo
	@echo "Source tree cleaned."
	@echo

$(SUBDIRS):
	@echo " [ CD ] $(CURRENTPATH)$@/"
	@+make -C "$@" "CURRENTPATH=$(CURRENTPATH)$@/" $(MAKECMDGOALS)
