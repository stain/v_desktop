#
# Makefile for the Voyager foundation classes.
#

include ./../make.inc

# No extension here
DLLNAME		=	voyguitk
OBJDIR		=	./../../../o
BINDIR		=	./../../../bin
DOCDIR		=	./../../../docs

CDIR		=	./src

CLASSCDIR	=	./class_c
CLASSTEMPLATEDIR	=	./../../../class_templates
CLASSINC	=	./class_h
INCDIR		=	./include
IDLDIR		=	./idl

# The object TK lives here
TKIDLDIR	=	$(NOMPATH)/idl
IDLINC		=	-include=$(TKIDLDIR) -include=$(DESKTOPPATH)/idl -include=$(FOUNDATIONPATH)/idl
 
INC		= 	./include -I ./class_h \
			-I $(NOMPATH)/class_h -I $(NOMPATH)/include \
			-I $(DESKTOPPATH)/class_h -I $(DESKTOPPATH)/include \
			-I $(FOUNDATIONPATH)/class_h -I $(FOUNDATIONPATH)/include

# NOM IDL compiler
NOMIDLINC	=	-I$(FOUNDATIONPATH)/idl -I$(TKIDLDIR) \
			-I$(DESKTOPPATH)/idl -I ./idl
NOMIDLCOMPILER	=	$(BINDIR)/nom-idl-compiler.exe 

ADDLIBS		=	 -l$(OBJDIR)/nobjtk
# -l$(OBJDIR)/voywp

# Define objects to build

CLASSOBJECTS	=	$(OBJDIR)/nomwindow.o \
			$(OBJDIR)/nommenu.o \
			$(OBJDIR)/nommenuitem.o \
			$(OBJDIR)/nomnotebook.o \
			$(OBJDIR)/nomdragwindow.o \
			$(OBJDIR)/nomdraginfo.o

#			$(OBJDIR)/nomfolderwindow.o \


.PRECIOUS:	$(CLASSINC)/%.ih $(CLASSCDIR)/%.c $(CDIR)/%.c

.PHONY:		dox

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

#BASE_NOMCOMPILEH	=	$(IDLCOMP) $(IDLINC) --showcpperrors --header --output-dir=$(CLASSINC) $<
BASE_NOMCOMPILEIH	=	$(IDLCOMP) $(IDLINC) --showcpperrors --ihfile --output-dir=$(CLASSINC) $<
BASE_NOMCOMPILEC	=	$(IDLCOMP) $(IDLINC) --showcpperrors --c-template --output-dir=$(CLASSTEMPLATEDIR) $< 

BASE_NOMCOMPILEH	=	gcc -E -x c -D__NOM_IDL_COMPILER__ $(NOMIDLINC) $< | $(NOMIDLCOMPILER) --emit-h -d $(CLASSINC) -
BASE_NOMCOMPILEIH	=	gcc -E -x c -D__NOM_IDL_COMPILER__ $(NOMIDLINC) $< | $(NOMIDLCOMPILER) --emit-ih -d $(CLASSINC) -


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
	mkdir -p $(DOCDIR)

dox:
	cd ./dox && doxygen gui.dox

clean:
	-cd $(OBJDIR) && rm -f *.o
	-cd $(BINDIR) && rm -f *.dll
