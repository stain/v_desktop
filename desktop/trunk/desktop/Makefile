#
# Makefile for the Voyager Desktop.
# Note: that the desktop is compiled as a DLL
# due to some problems with libc<->EMX

include ./make.inc


OBJDIR		=	./../../../o
BINDIR		=	./../../../bin
CDIR		=	./src

CLASSCDIR	=	./class_c
CLASSTEMPLATEDIR	=	./../../../class_templates
CLASSINC	=	./class_h
INCDIR		=	./include
IDLDIR		=	./idl

TKIDLDIR	=	$(NOMPATH)/idl  # The object TK lives here

#INC		= 	./include -I $(NOMPATH)/include -I $(NOMPATH)/class_h
INC		= 	./include -I ./class_h -I $(NOMPATH)/class_h -I $(NOMPATH)/include

ADDLIBS		=	 -l$(OBJDIR)/nobjtk -l$(OBJDIR)/voywp
CLASSDLLLIBS	=	-l$(OBJDIR)/nobjtk

# Define objects to build

DLLOBJECTS	=	$(OBJDIR)/vdesktop.o \
			$(OBJDIR)/debug_window.o \
			$(OBJDIR)/quitwindow.o

CLASSOBJECTS	=	$(OBJDIR)/wpobject.o \
			$(OBJDIR)/wpfilesystem.o \
			$(OBJDIR)/wpdatafile.o \
			$(OBJDIR)/wpfolder.o \
			$(OBJDIR)/helper.o


.PRECIOUS:	$(CLASSINC)/%.ih $(CLASSCDIR)/%.c $(CDIR)/%.c

all:	$(BINDIR)/voywp.dll $(BINDIR)/vdesktop.dll

# Leave $(ADDLIBS) at the end or linking will fail. OR use additional flag -Zomf for linking.
$(BINDIR)/vdesktop.dll:	$(OBJDIR)/o.dep $(DLLOBJECTS) ./src/exports.def
	@echo "[33;1;mLinking "$@"...[0;m"
	cmd.exe /C .\src\create_vdesktop_def $(OBJDIR)/vdesktop.def
	$(CC) $(GCCLDFLAGSDLL)  -o $@ $(DLLOBJECTS) $(LIBS) $(ADDLIBS) $(OBJDIR)/vdesktop.def
	@echo "[32;1;mDone linking "$@"...[0;m"


$(BINDIR)/voywp.dll:	 $(OBJDIR)/o.dep $(CLASSOBJECTS) exports.def
	@echo "[33;1;mLinking "$@"...[0;m"
	@cmd.exe /C create_voywp_def $(OBJDIR)/voywp.def
	$(CC) $(GCCLDFLAGS) -o $@ $(CLASSOBJECTS) $(LIBS) $(CLASSDLLLIBS) $(OBJDIR)/voywp.def
	emximp -o $(OBJDIR)/$(basename $(notdir $@)).a $(OBJDIR)/voywp.def
	@echo "[32;1;mDone linking "$@"...[0;m"


###################################
#
#    Rules for compiling
#
###################################

BASE_NOMCOMPILEH	=	$(IDLCOMP) -include=$(TKIDLDIR) --showcpperrors --header --output-dir=$(CLASSINC) $<
BASE_NOMCOMPILEIH	=	$(IDLCOMP) -include=$(TKIDLDIR) --showcpperrors --ihfile --output-dir=$(CLASSINC) $<
BASE_NOMCOMPILEC	=	$(IDLCOMP) -include=$(TKIDLDIR) --showcpperrors --c-template --output-dir=$(CLASSTEMPLATEDIR) $< 


# Rules for desktop classes
$(OBJDIR)/%.o:	$(CLASSCDIR)/%.c $(CLASSINC)/%.ih
	$(CC) -I $(INC) $(GCCFLAGS)  -o$@ $<

$(OBJDIR)/%.o:	$(CLASSCDIR)/%.c
	$(CC) -I $(INC) $(GCCFLAGS)  -o$@ $<

$(CLASSINC)/%.ih:	$(IDLDIR)/%.idl
	$(BASE_NOMCOMPILEIH)
	$(BASE_NOMCOMPILEH)
	$(BASE_NOMCOMPILEC)

$(OBJDIR)/%.o:	$(CDIR)/%.c
	$(CC) -I $(INC) $(GCCFLAGSDLL)  -o$@ $<

# Create the directories for temp files
$(OBJDIR)/o.dep:
	mkdir -p $(OBJDIR)
	echo > $(OBJDIR)/o.dep
	mkdir -p $(BINDIR)
	mkdir -p $(CLASSTEMPLATEDIR)

clean:
	-cd $(OBJDIR) && rm -f *.o
	-cd $(BINDIR) && rm -f *.dll
