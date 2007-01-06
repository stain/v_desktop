#
# Makefile for the Voyager Desktop.
# Note: that the desktop is compiled as a DLL
# due to some problems with libc<->EMX

include ./../make.inc


OBJDIR		=	./../../../o
BINDIR		=	./../../../bin
CDIR		=	./src

CLASSCDIR	=	./class_c
CLASSTEMPLATEDIR	=	./../../../class_templates
CLASSINC	=	./class_h
INCDIR		=	./include
IDLDIR		=	./idl

TKIDLDIR	=	$(NOMPATH)/idl  # The object TK lives here
IDLINC		=	-include=$(FOUNDATIONPATH)/idl -include=$(TKIDLDIR) \
			-include=$(GUITKPATH)/idl

INC		= 	./include -I ./class_h -I $(NOMPATH)/class_h -I $(NOMPATH)/include\
			-I $(GUITKPATH)/class_h -I $(GUITKPATH)/include -I $(FOUNDATIONPATH)/class_h \
			-I $(FOUNDATIONPATH)/include

ADDLIBS		=	 -l$(OBJDIR)/nobjtk -l$(OBJDIR)/voywp
CLASSDLLLIBS	=	-l$(OBJDIR)/nobjtk -l$(OBJDIR)/voyguitk -l$(OBJDIR)/voyfcls
GCLIB		=	-l$(OBJDIR)/gc

# Define objects to build

DLLOBJECTS	=	$(OBJDIR)/vdesktop.o \
			$(OBJDIR)/debug_window.o \
			$(OBJDIR)/quitwindow.o

CLASSOBJECTS	=	$(OBJDIR)/wpobject.o \
			$(OBJDIR)/wpfilesystem.o \
			$(OBJDIR)/wpdatafile.o \
			$(OBJDIR)/wpfolder.o \
			$(OBJDIR)/wpwindow.o \
			$(OBJDIR)/wpnotebook.o \
			$(OBJDIR)/wpclassmanager.o \
			$(OBJDIR)/helper.o


.PRECIOUS:	$(CLASSINC)/%.ih $(CLASSCDIR)/%.c $(CDIR)/%.c

all:	$(BINDIR)/voywp.dll $(BINDIR)/vdesktop.dll

# Leave $(ADDLIBS) at the end or linking will fail. OR use additional flag -Zomf for linking.
$(BINDIR)/vdesktop.dll:	$(OBJDIR)/o.dep $(DLLOBJECTS) ./src/exports.def
	@echo "[33;1;mLinking "$@"...[0;m"
	cmd.exe /C .\src\create_vdesktop_def $(OBJDIR)/vdesktop.def
	$(CC) $(GCCLDFLAGSDLL)  -o $@ $(DLLOBJECTS) $(LIBS) $(ADDLIBS) $(GCLIB) $(OBJDIR)/vdesktop.def
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

BASE_NOMCOMPILEH	=	$(IDLCOMP) $(IDLINC) --showcpperrors --header --output-dir=$(CLASSINC) $<
BASE_NOMCOMPILEIH	=	$(IDLCOMP) $(IDLINC) --showcpperrors --ihfile --output-dir=$(CLASSINC) $<
BASE_NOMCOMPILEC	=	$(IDLCOMP) $(IDLINC) --showcpperrors --c-template --output-dir=$(CLASSTEMPLATEDIR) $< 


# Rules for desktop classes
$(OBJDIR)/%.o:	$(CLASSCDIR)/%.c
	$(CC) -I $(INC) $(GCCFLAGS)  -o$@ $<

$(OBJDIR)/helper.o:	$(CLASSCDIR)/helper.c
	$(CC) -I $(INC) $(GCCFLAGS)  -o$@ $<

$(CLASSINC)/%.ih:	$(IDLDIR)/%.idl
	$(BASE_NOMCOMPILEIH)
	$(BASE_NOMCOMPILEH)
	$(BASE_NOMCOMPILEC)

$(CLASSINC)/%.h:	$(IDLDIR)/%.idl
	$(BASE_NOMCOMPILEH)

$(OBJDIR)/%.o:	$(CDIR)/%.c
	$(CC) -I $(INC) $(GCCFLAGSDLL)  -o$@ $<

$(CLASSCDIR)/%.c:	$(IDLDIR)/%.idl
	$(BASE_NOMCOMPILEIH)
	$(BASE_NOMCOMPILEH)
	$(BASE_NOMCOMPILEC)

# Create the directories for temp files
$(OBJDIR)/o.dep:
	mkdir -p $(OBJDIR)
	echo > $(OBJDIR)/o.dep
	mkdir -p $(BINDIR)
	mkdir -p $(CLASSTEMPLATEDIR)

clean:
	-cd $(OBJDIR) && rm -f *.o
	-cd $(BINDIR) && rm -f *.dll
