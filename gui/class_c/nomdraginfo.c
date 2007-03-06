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

#ifndef NOM_NOMDragInfo_IMPLEMENTATION_FILE
#define NOM_NOMDragInfo_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#include <os2.h>

#include "nom.h"
#include <nomtk.h>

#include <gtk/gtk.h>

#include "nomguitk.h"
#include "nomwindow.h"
#include "desktoptypes.h"
#include "wpnotebook.h"
#include "wpfolderwindow.h"
#include "wpfolder.h"
#include "wpobject.h"

#include "nomdraginfo.ih"


NOM_Scope gulong NOMLINK impl_NOMDragInfo_queryNumDragItems(NOMDragInfo* nomSelf,
                                                            CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);

  if(!_arrayDragItems)
    return 0;

  return _arrayDragItems->len;
}

NOM_Scope gulong NOMLINK impl_NOMDragInfo_addWPObjectToItemList(NOMDragInfo* nomSelf,
                                                                const PWPObject wpObject,
                                                                const PWPFolder wpFolder,
                                                                const PGtkWidget wgtSource,
                                                                CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);
  DRAGITEM diTemp;

  diTemp.wgtSource=wgtSource;
  diTemp.wpObjectSource=wpFolder;
  diTemp.chrSourceName="";
  diTemp.wpObject=wpObject;
  diTemp.chrObjectName="";

  _arrayDragItems=g_array_append_val(_arrayDragItems, diTemp);
  return _arrayDragItems->len-1;
}

NOM_Scope void NOMLINK impl_NOMDragInfo_deleteAllDragItems(NOMDragInfo* nomSelf,
                                                           CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);

  g_array_set_size(_arrayDragItems, 0);
}


NOM_Scope CORBA_boolean NOMLINK impl_NOMDragInfo_queryIsItemWPObject(NOMDragInfo* nomSelf,
                                                                     const gulong ulIdx,
                                                                     CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);
  DRAGITEM *pdi;

  /* Index out of range? */
  if(ulIdx>=_arrayDragItems->len)
    return FALSE;

  pdi=&g_array_index(_arrayDragItems, DRAGITEM, ulIdx);
  
  if(!nomIsObj(pdi->wpObject))
    return FALSE;

  return TRUE;
}


NOM_Scope PWPObject NOMLINK impl_NOMDragInfo_queryWPObjectFromItemList(NOMDragInfo* nomSelf,
                                                                       const gulong ulIdx,
                                                                       CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);
  DRAGITEM *pdi;

  /* Index out of range? */
  if(ulIdx>=_arrayDragItems->len)
    return NULL;

  pdi=&g_array_index(_arrayDragItems, DRAGITEM, ulIdx);
  
  if(!nomIsObj(pdi->wpObject))
    return NULL;

  return pdi->wpObject;
}

NOM_Scope void NOMLINK impl_NOMDragInfo_setGdkDragContextPointer(NOMDragInfo* nomSelf,
                                                                 const PGdkDragContext gdkDragContext,
                                                                 CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);

  _dragContext=gdkDragContext;
}


NOM_Scope PGtkWidget NOMLINK impl_NOMDragInfo_querySourceWidgetHandle(NOMDragInfo* nomSelf,
                                                                      const gulong ulIdx,
                                                                      CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);
  DRAGITEM *pdi;

  /* Check index */
  if(ulIdx>=_arrayDragItems->len)
    return NULLHANDLE;

  pdi=&g_array_index(_arrayDragItems, DRAGITEM, ulIdx);

  return pdi->wgtSource;
}


NOM_Scope PWPObject NOMLINK impl_NOMDragInfo_querySourceWPObject(NOMDragInfo* nomSelf,
                                                                 const gulong ulIdx,
                                                                 CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);
  DRAGITEM *pdi;

  /* Check index */
  if(ulIdx>=_arrayDragItems->len)
    return NULLHANDLE;

  pdi=&g_array_index(_arrayDragItems, DRAGITEM, ulIdx);

  return pdi->wpObjectSource;
}

NOM_Scope GdkDragAction NOMLINK impl_NOMDragInfo_queryChosenDropAction(NOMDragInfo* nomSelf,
                                                                       CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);

  return _dragContext->action;
}

NOM_Scope void NOMLINK impl_NOMDragInfo_nomInit(NOMDragInfo* nomSelf, CORBA_Environment *ev)
{
  NOMDragInfoData* nomThis=NOMDragInfoGetData(nomSelf);

  /* Let parent do initializations. */
  NOMDragInfo_nomInit_parent(nomSelf,  ev);

  /* Create an array holding items being dragged. */
  _arrayDragItems=g_array_new (FALSE, TRUE, sizeof(DRAGITEM));
}



