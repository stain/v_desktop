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
* Portions created by the Initial Developer are Copyright (C) 2007
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
#ifndef NOM_M_WPObject_IMPLEMENTATION_FILE
#define NOM_M_WPObject_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#include <os2.h>

#include <nom.h>
#include <nomtk.h>

#include <string.h>
#include <gtk/gtk.h>
#include "helper.h"
#include "desktop.h"

#include "m_wpobject.ih"

static gpointer pIconCls=NULL;

NOM_Scope gpointer NOMLINK impl_M_WPObject_wpclsQueryIcon(M_WPObject* nomSelf, CORBA_Environment *ev)
{
  /*  M_WPObjectData* nomThis=M_WPObjectGetData(nomSelf); */
  //  g_message("-----------> %s" , __FUNCTION__);
  return pIconCls;
}

NOM_Scope CORBA_string NOMLINK impl_M_WPObject_wpclsQueryTitle(M_WPObject* nomSelf, CORBA_Environment *ev)
{
  M_WPObjectData* nomThis=M_WPObjectGetData(nomSelf);

  return _nomStringTitle;
}

NOM_Scope CORBA_boolean NOMLINK impl_M_WPObject_wpclsSetIcon(M_WPObject* nomSelf, const gpointer pNewIcon, CORBA_Environment *ev)
{
  /* M_WPObjectData* nomThis=M_WPObjectGetData(nomSelf); */
  gpointer tmpPtr;

  do{
    /* Get old icon  */
    tmpPtr=g_atomic_pointer_get(&pIconCls);
  }while(!g_atomic_pointer_compare_and_exchange((gpointer*)&pIconCls, tmpPtr, pNewIcon));

  return TRUE;
}

NOM_Scope void NOMLINK impl_M_WPObject_wpclsInitData(M_WPObject* nomSelf, CORBA_Environment *ev)
{
  static boolean bInitDone=FALSE;
  M_WPObjectData* nomThis=M_WPObjectGetData(nomSelf);

  if(!bInitDone)
    {
      GError *error=NULL;
      gchar *gchrIconName=NULLHANDLE;

      _nomStringTitle="Object";
      
      /* Load default wpObject icon */
      if(!pIconCls){
        gchrIconName=g_build_filename(priv_getIconDir(), WPOBJECT_ICON_FILE, NULL);
        //  g_return_val_if_fail(g_file_test (gchrIconName, G_FILE_TEST_EXISTS), NULLHANDLE);
        pIconCls=gdk_pixbuf_new_from_file (gchrIconName, &error);
        //g_message("->  %s %x, %s", __FUNCTION__, _pIconCls, gchrIconName);
      }
      bInitDone=TRUE;
    }
}

NOM_Scope void NOMLINK impl_M_WPObject_wpclsUnInitData(M_WPObject* nomSelf, CORBA_Environment *ev)
{
/* M_WPObjectData* nomThis=M_WPObjectGetData(nomSelf); */

}

/* orbit-idl-c-stubs.c, cs_output_stub line 347 */
NOM_Scope void NOMLINK impl_M_WPObject_nomInit(M_WPObject* nomSelf, CORBA_Environment *ev)
{
/* M_WPObjectData* nomThis=M_WPObjectGetData(nomSelf); */

  M_WPObject_nomInit_parent((NOMObject*)nomSelf,  ev);

  M_WPObject_wpclsInitData(nomSelf, NULLHANDLE);
}


/* orbit-idl-c-stubs.c, cs_output_stub line 347 */
NOM_Scope void NOMLINK impl_M_WPObject_nomUnInit(M_WPObject* nomSelf, CORBA_Environment *ev)
{
/* M_WPObjectData* nomThis=M_WPObjectGetData(nomSelf); */

  M_WPObject_wpclsUnInitData(nomSelf, NULLHANDLE);

  M_WPObject_nomUnInit_parent((NOMObject*)nomSelf,  ev);
}


