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

#include "nomstring.ih"


NOM_Scope PNOMString NOMLINK impl_NOMString_assign(NOMString* nomSelf, const PNOMString nomString,
                                                         CORBA_Environment *ev)
{
  /* NOMStringData* nomThis=NOMStringGetData(nomSelf); */

  NOMString_assignCString(nomSelf, NOMString_queryCString(nomString, NULLHANDLE), NULLHANDLE);
  return nomSelf;
}

/* Assign a C string to this NOMString. An initially created NOMString object is empty. */
NOM_Scope PNOMString NOMLINK impl_NOMString_assignCString(NOMString* nomSelf, const CORBA_char * chrString,
                                                          CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);

  g_string_assign(_gString, chrString); /* This copies the input string */
  return nomSelf;
}

/* Returns the C string held by this NOMString. */
NOM_Scope CORBA_string NOMLINK impl_NOMString_queryCString(NOMString* nomSelf, CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);

  return _gString->str;
}

/* Returns a copy of the C string held by this NOMString. */
NOM_Scope CORBA_string NOMLINK impl_NOMString_copyCString(NOMString* nomSelf, CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);

  return g_strdup(_gString->str);
}

NOM_Scope PNOMString NOMLINK impl_NOMString_appendCString(NOMString* nomSelf, const CORBA_char * chrString,
                                                          CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);
  PNOMString nomRetval=(PNOMString) NOMString_new(nomSelf, NULLHANDLE);
  GString* gStrTmp;

  gStrTmp=g_string_new(_gString->str);
  g_string_append(gStrTmp, chrString);
  NOMString_assignCString(nomRetval, gStrTmp->str, NULLHANDLE);
  g_string_free(gStrTmp, TRUE);

  return nomRetval;
}

NOM_Scope PNOMString NOMLINK impl_NOMString_append(NOMString* nomSelf, const PNOMString nomString,
                                                         CORBA_Environment *ev)
{
  /*  NOMStringData* nomThis=NOMStringGetData(nomSelf); */

  return NOMString_appendCString(nomSelf, NOMString_queryCString(nomString, NULLHANDLE), NULLHANDLE);
}

NOM_Scope PNOMString NOMLINK impl_NOMString_prepend(NOMString* nomSelf, const PNOMString nomString,
                                                          CORBA_Environment *ev)
{
/* NOMStringData* nomThis=NOMStringGetData(nomSelf); */

  return NOMString_prependCString(nomSelf, NOMString_queryCString(nomString, NULLHANDLE), NULLHANDLE);
}

NOM_Scope PNOMString NOMLINK impl_NOMString_prependCString(NOMString* nomSelf, const CORBA_char * chrString, CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);
  PNOMString nomRetval=(PNOMString)NOMString_new(nomSelf, NULLHANDLE);
  GString* gStrTmp;

  gStrTmp=g_string_new(_gString->str);
  g_string_prepend(gStrTmp, chrString);
  NOMString_assignCString(nomRetval, gStrTmp->str, NULLHANDLE);
  g_string_free(gStrTmp, TRUE);

  return nomRetval;
}

NOM_Scope CORBA_unsigned_long NOMLINK impl_NOMString_length(NOMString* nomSelf, CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);

  return _gString->len;
}

NOM_Scope PNOMString NOMLINK impl_NOMString_truncate(NOMString* nomSelf, const CORBA_unsigned_long ulNewLen,
                                                           CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);
  PNOMString nomRetval=(PNOMString)NOMString_new(nomSelf, NULLHANDLE);
  GString* gStrTmp;

  gStrTmp=g_string_new(_gString->str);
  g_string_truncate(gStrTmp, ulNewLen);
  NOMString_assignCString(nomRetval, gStrTmp->str, NULLHANDLE);
  g_string_free(gStrTmp, TRUE);

  return nomRetval;
}

NOM_Scope PNOMString NOMLINK impl_NOMString_copy(NOMString* nomSelf, CORBA_Environment *ev)
{
  /*  NOMStringData* nomThis=NOMStringGetData(nomSelf); */
  PNOMString nomRetval;
  NOMClass* nomCls;

  /* We don't know which class we're actually. So we can't just create a new NOMString here.
     It is possible that we are called by a subclass. So get the class object and let the
     class object create the correct class. */
  nomCls=NOMObject_nomGetClass((PNOMObject) nomSelf, NULLHANDLE);
  nomRetval=(PNOMString)NOMString_new(nomSelf, NULLHANDLE);

  NOMString_assign(nomRetval, nomSelf, ev);

  return nomRetval;
}


NOM_Scope void NOMLINK impl_NOMString_nomInit(NOMString* nomSelf, CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);

  NOMString_nomInit_parent((NOMObject*)nomSelf,  ev);

  /* Alloc a zero length GString */
  _gString=g_string_new("");
}



NOM_Scope void NOMLINK impl_NOMString_nomUninit(NOMString* nomSelf, CORBA_Environment *ev)
{
  NOMStringData* nomThis=NOMStringGetData(nomSelf);

  /* Free GString including the C string data */
  g_string_free(_gString, TRUE);

  NOMString_nomUninit_parent((NOMObject*)nomSelf,  ev);

}

