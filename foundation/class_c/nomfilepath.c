/*
 * This file was generated by orbit-idl-2 for Voyager - DO NOT EDIT!
 *
 *
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
