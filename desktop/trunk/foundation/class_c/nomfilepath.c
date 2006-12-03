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

  The method returns the NOMPath instance after appending.
 */
NOM_Scope PNOMPath NOMLINK impl_NOMPath_appendPath(NOMPath* nomSelf, const PNOMPath nomPath, CORBA_Environment *ev)
{
  gchar*  chrTemp;

  if(!nomPath)
    return nomSelf;

  if(0==NOMPath_length((NOMString*)nomPath, ev))
    return NOMPath_appendSeparator(nomSelf, ev);

  if(G_DIR_SEPARATOR==chrTemp[0])
    NOMPath_stripSeparator(nomSelf, ev);
  else
    NOMPath_appendSeparator(nomSelf, ev); /* Make sure current path has a separator */

  return (NOMPath*) NOMPath_appendString((NOMString*) nomSelf, (NOMString*)nomPath, NULLHANDLE);
}

/*
  Append a separator to the path. If the path already has a separator at the end this method does
  nothing other than returning the path.
*/
NOM_Scope PNOMPath NOMLINK impl_NOMPath_appendSeparator(NOMPath* nomSelf, CORBA_Environment *ev)
{
  gchar*  chrTemp;
  gulong len;
  
  if((len=NOMPath_length((NOMString*)nomSelf, ev))==0)
    return (NOMPath*)NOMPath_appendCString((NOMString*)nomSelf, G_DIR_SEPARATOR_S, ev);

  if(G_DIR_SEPARATOR!=chrTemp[len-1])
    return (NOMPath*)NOMPath_appendCString( (NOMString*)nomSelf, G_DIR_SEPARATOR_S, ev);

  return nomSelf;
}

/*
  Strips the path separator from the end of a path if there's one.
 */
NOM_Scope PNOMPath NOMLINK impl_NOMPath_stripSeparator(NOMPath* nomSelf, CORBA_Environment *ev)
{
  gchar*  chrTemp;
  gulong len;

  if((len=NOMPath_length((NOMString*)nomSelf, ev))==0)
    return nomSelf;

  chrTemp=NOMPath_getCString((NOMString*)nomSelf, ev);
  if(chrTemp[len-1]==G_DIR_SEPARATOR)
    return (NOMPath*)NOMPath_truncateString( (NOMString*)nomSelf, len-1, ev);

  return nomSelf;
}
