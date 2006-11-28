#
# Makefile for the Voyager foundation classes.
#

include ./../make.inc

# No extension here
DLLNAME		=	voyfcls
OBJDIR		=	./../../../o
BINDIR		=	./../../../bin
CDIR		=	./src

CLASSCDIR	=	./class_c
CLASSTEMPLATEDIR	=	./../../../class_templates
CLASSINC	=	./class_h
INCDIR		=	./include
IDLDIR		=	./idl

TKIDLDIR	=	$(NOMPATH)/idl  # The object TK lives here

INC		= 	./include -I ./class_h -I $(NOMPATH)/class_h -I $(NOMPATH)/include

ADDLIBS		=	 -l$(OBJDIR)/nobjtk

# Define objects to build


CLASSOBJECTS	=	$(OBJDIR)/nomstring.o \
			$(OBJDIR)/nomfilepath.o


.PRECIOUS:	$(CLASSINC)/%.ih $(CLASSCDIR)/%.c $(CDIR)/%.c

all:	$(BINDIR)/$(DLLNAME).dll



$(BINDIR)/$(DLLNAME).dll:	 $(OBJDIR)/o.dep $(CLASSOBJECTS) exports.def
	@echo "[33;1;mLinking "$@"...[0;m"
	@cmd.exe /C create_$(DLLNAME)_def $(OBJDIR)/$(DLLNAME).def
	$(CC) $(GCCLDFLAGS) -o $@ $(CLASSOBJECTS) $(LIBS) $(ADDLIBS) $(OBJDIR)/$(DLLNAME).def
	emximp -o $(OBJDIR)/$(basename $(notdir $@)).a $(OBJDIR)/$(DLLNAME).def
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


$(CLASSINC)/%.ih:	$(IDLDIR)/%.idl
	$(BASE_NOMCOMPILEIH)
	$(BASE_NOMCOMPILEH)
	$(BASE_NOMCOMPILEC)

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
