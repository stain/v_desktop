/* */
call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

deffile=ARG(1)
if ARG(1)="" then deffile="tst.def"

theVersion=VALUE("DESKTOPVERSION", ,"OS2ENVIRONMENT")
if theVersion="" THEN theVersion="0.0.0"

PARSE SOURCE os fun thePath
newdir=FILESPEC('d', thePath)||filespec('p', thePath)
newdir=SUBSTR(newDir, 1, LENGTH(newDir)-1)

curdir=DIRECTORY()
rc=DIRECTORY(newdir)

/* File defining the DLL exports */
exportsfile=DIRECTORY()||"\EXPORTS.DEF"
rc=DIRECTORY(curdir)

def="; VDESKTOP.DEF: DEF-file for vdesktop.dll"||'0d'x||'0a'x
def=def||";"||'0d'x||'0a'x
def=def||";                    (C) Chris Wohlgemuth 2003-"||WORD(DATE(), 3)  ||'0d'x||'0a'x
def=def||";/*"||'0d'x||'0a'x
def=def||"; */"||'0d'x||'0a'x
def=def||";/*"||'0d'x||'0a'x
def=def||"; * http://www.os2world.com/cdwriting"||'0d'x||'0a'x
def=def||"; * http://www.geocities.com/SiliconValley/Sector/5785/"||'0d'x||'0a'x
def=def||"; */"||'0d'x||'0a'x

def=def||'0d'x||'0a'x

def=def||"LIBRARY VDESKTOP INITGLOBAL"||'0d'x||'0a'x
def=def||"DESCRIPTION '$@#(C) C. Wohlgemuth 1996-"
def=def||WORD(DATE(),3)||":"||LEFT(theVersion,3)||"        #@##1## "||DATE("E")||" "||TIME("N")||"        My ;-)  ::::"||WORD(TRANSLATE(theVersion," ", "."),3)||"::@@Voyager Desktop V"||theVersion||"'"||'0d'x||'0a'x



'@type NUL > 'deffile
/* Write header */
call lineout deffile, def
/* Write the exports */
DO WHILE LINES(exportsfile)
	call lineout deffile, LINEIN(exportsfile)
END
call stream exportsfile, "C", "close"
call stream deffile, "C", "close"

SAY "New VDESKTOP.DEF created for V"||theVersion||"."
exit
