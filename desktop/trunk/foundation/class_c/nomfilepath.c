/* ***** BEGIN LICENSE BLOCK *****
* Version: CDDL 1.0/LGPL 2.1
*
* The contents of this file are subject to the COMMON DEVELOPMENT AND
* DISTRIBUTION LICENSE (CDDL) Version 1.0 (the "License"); you may not use
* this file except in compliance with the License. You may obtain a copy of
* the License at http://www.sun.com/cddl/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is "NOM" Netlabs Object Model
*
* The Initial Developer of the Original Code is
* netlabs.org: Chris Wohlgemuth <cinc-ml@netlabs.org>.
* Portions created by the Initial Developer are Copyright (C) 2006
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*
* Alternatively, the contents of this file may be used under the terms of
* the GNU Lesser General Public License Version 2.1 (the "LGPL"), in which
* case the provisions of the LGPL are applicable instead of those above. If
* you wish to allow use of your version of this file only under the terms of
* the LGPL, and not to allow others to use your version of this file under
* the terms of the CDDL, indicate your decision by deleting the provisions
* above and replace them with the notice and other provisions required by the
* LGPL. If you do not delete the provisions above, a recipient may use your
* version of this file under the terms of any one of the CDDL or the LGPL.
*
* ***** END LICENSE BLOCK ***** */
/*
 * And remember, phase 3 is near...
 */
#ifndef NOM_NOMPath_IMPLEMENTATION_FILE
#define NOM_NOMPath_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#include <os2.h>

#include <string.h>
#include <glib.h>

#include "nomfilepath.ih"
/*
  Append a path to the string. A path separator will be added to the current path if necessary and the
  given string appended. If the given string starts with a separator no additional separator will be 
  added to the path prior to appending. If the given string starts with a separator and the current
  path ends with a separator the ending separator will be removed before appending.

  Note that there's no check if the input string is an absolute path. So if an absolute path is given as
  input the resulting path may not be valid.

  The method returns a new NOMPath after appending.
 */
NOM_Scope PNOMPath NOMLINK impl_NOMPath_appendPath(NOMPath* nomSelf, const PNOMPath nomPath, CORBA_Environment *ev)
{
  gchar*  chrTemp;
  PNOMPath np;

  if(!nomPath)
    return (PNOMPath)NOMPath_appendSeparator(nomSelf, NULLHANDLE);

  if(0==NOMPath_length((NOMString*)nomPath, ev))
    return NOMPath_appendSeparator(nomSelf, ev);

  chrTemp=NOMPath_queryCString(nomPath, NULLHANDLE);
  if(G_DIR_SEPARATOR==chrTemp[0])
    np=NOMPath_stripSeparator(nomSelf, ev);
  else{
    np=NOMPath_appendSeparator(nomSelf, ev); /* Make sure current path has a separator */
  }

  return (NOMPath*) NOMPath_append((NOMString*) np, (NOMString*)nomPath, NULLHANDLE);
}

/*
  Append a separator to the path. If the path already has a separator at the end this method does
  nothing other than returning a new path object. If the given path has zero length a path object
  only holding a separator is returned.
 
  This method always returns a new instance of a NOMPath owned by the caller.
*/
NOM_Scope PNOMPath NOMLINK impl_NOMPath_appendSeparator(NOMPath* nomSelf, CORBA_Environment *ev)
{
  gchar*  chrTemp;
  gulong len;

  if((len=NOMPath_length((NOMString*)nomSelf, ev))==0)
    return (NOMPath*)NOMPath_appendCString((NOMString*)nomSelf, G_DIR_SEPARATOR_S, ev);

  if(G_DIR_SEPARATOR!=chrTemp[len-1])
    return (NOMPath*)NOMPath_appendCString( (NOMString*)nomSelf, G_DIR_SEPARATOR_S, ev);

  return (PNOMPath)NOMPath_copy(nomSelf, NULLHANDLE);
}

/*
  Strips the path separator from the end of a path if there's one.

  This method always returns a new instance of a NOMPath owned by the caller.
 */
NOM_Scope PNOMPath NOMLINK impl_NOMPath_stripSeparator(NOMPath* nomSelf, CORBA_Environment *ev)
{
  gchar*  chrTemp;
  gulong len;

  if((len=NOMPath_length((NOMString*)nomSelf, NULLHANDLE))==0)
    return (PNOMPath)NOMPath_copy(nomSelf, NULLHANDLE);

  chrTemp=NOMPath_queryCString((NOMString*)nomSelf, NULLHANDLE);

  if(chrTemp[len-1]==G_DIR_SEPARATOR)
    return (NOMPath*)NOMPath_truncate( (NOMString*)nomSelf, len-1, NULLHANDLE);

  return (PNOMPath)NOMPath_copy(nomSelf, NULLHANDLE);
}

/*
  Returns TRUE if the given path is absolute.
  On OS/2 this means it starts with a letter followed by a colon.
*/
NOM_Scope CORBA_boolean NOMLINK impl_NOMPath_pathIsAbsolute(NOMPath* nomSelf, CORBA_Environment *ev)
{
  /* NOMPathData* nomThis=NOMPathGetData(nomSelf); */
  gchar* chrString;

#ifndef __OS2__
#error !!!!! Only implemented for OS/2 !!!!!
#endif

  if(NOMPath_length(nomSelf, NULLHANDLE)<2)
    return FALSE;

  chrString=NOMPath_queryCString(nomSelf, NULLHANDLE);

  if(!g_ascii_isalpha(chrString[0]) || chrString[1]!=':')
    return FALSE;

  return TRUE;
}

NOM_Scope PNOMPath NOMLINK impl_NOMPath_queryRoot(NOMPath* nomSelf, CORBA_Environment *ev)
{
/* NOMPathData* nomThis=NOMPathGetData(nomSelf); */
  PNOMPath nomRetval=(PNOMPath)NOMPath_new(nomSelf, NULLHANDLE);
  gchar os2Root[4]="a:"; /* includes padding */
  gchar *chrTemp;

#ifndef __OS2__
#error !!!!! Only implemented for OS/2 !!!!!
#endif

  if(!NOMPath_pathIsAbsolute(nomSelf, NULLHANDLE))
    return nomRetval; /* Return a zero length string */

  chrTemp=NOMPath_queryCString(nomSelf, NULLHANDLE);

  os2Root[0]=chrTemp[0];
  return (PNOMPath) NOMPath_assignCString(nomRetval, os2Root, NULLHANDLE);
}

/*
  This method strips all characters from the beginning of a path till the first
  directory separator and also this first separator. If there's no separator in
  the path a zero length path is returned.

  This method always returns a new instance of a NOMPath owned by the caller.
 */
NOM_Scope PNOMPath NOMLINK impl_NOMPath_erasePathBegin(NOMPath* nomSelf, CORBA_Environment *ev)
{
  /* NOMPathData* nomThis=NOMPathGetData(nomSelf); */
  PNOMPath nomRetval=(PNOMPath) NOMPath_new(nomSelf, NULLHANDLE);
  gchar *chrTemp;

  chrTemp=NOMPath_queryCString(nomSelf, NULLHANDLE); /* Not a copy */

  while(*chrTemp!='\0' && *chrTemp!=G_DIR_SEPARATOR)
    chrTemp++;

  if(*chrTemp==G_DIR_SEPARATOR)
    chrTemp++;

  return (PNOMPath)NOMPath_assignCString(nomRetval, chrTemp, NULLHANDLE);
}

/*
  Returns the part of the path up to the first directory separator ('\' on OS/2).
  If there's no directory separator the whole path is returned. This method does
  not remove the part from the given path. Use erasePathBegin() to do that.
*/
NOM_Scope PNOMPath NOMLINK impl_NOMPath_queryPathBegin(NOMPath* nomSelf, CORBA_Environment *ev)
{
/* NOMPathData* nomThis=NOMPathGetData(nomSelf); */
  PNOMPath nomRetval=NOMPathNew();
  gchar *chrTemp;
  gchar *chrTemp2;

  chrTemp2 = chrTemp = NOMPath_copyCString(nomSelf, NULLHANDLE); /* This is a copy */

  while(*chrTemp!='\0' && *chrTemp!=G_DIR_SEPARATOR)
    chrTemp++;
  *chrTemp='\0';

  nomRetval=(PNOMPath)NOMPath_assignCString(nomRetval, chrTemp2, NULLHANDLE);

  g_free(chrTemp);
  return (PNOMPath) nomRetval;
}

