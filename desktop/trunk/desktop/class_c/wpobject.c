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
* Portions created by the Initial Developer are Copyright (C) 2005-2006
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
 * This file was generated by orbit-idl-2 for Voyager
 *
 *
 * And remember, phase 3 is near...
 */
#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include <os2.h>

#include <nom.h>
#include <nomtk.h>

#include <string.h>
#include <gtk/gtk.h>
#include <nomguitk.h>
#include "desktoptypes.h"

/* We have to declare this here to make PNOMFolderWindow known
   to wpDisplayMenu(). */
#ifndef NOMFolderWindow
typedef struct NOMFolderWindow_struct {
  struct nomMethodTabStruct  *mtab;
  gulong body[1];
} NOMFolderWindowObj;
#define NOMFolderWindow NOMFolderWindowObj
typedef NOMFolderWindow *PNOMFolderWindow;
#endif

#include "wpobject.ih"

#include "nomfolderwindow.h"

NOM_Scope gpointer NOMLINK impl_WPObject_wpAllocMem(WPObject* nomSelf, const CORBA_unsigned_long cbBytes,
                                                    CORBA_unsigned_long* prc, CORBA_Environment *ev)
{

/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  gpointer ptrMem=NULLHANDLE;
  PUSEITEM pui;

  /* Add mem for the inuse list structures */
  ptrMem=NOMCalloc(1, cbBytes+sizeof(USEITEM)+sizeof(MEMORYITEM));

  if(!ptrMem) {
    if(prc)
      *prc=(gulong)NOMERROR_NOT_ENOUGH_MEMORY;
    return NULLHANDLE;
  }

  /* Fill the structures */
  pui=(PUSEITEM)ptrMem;
  pui->type=(ULONG)USAGE_MEMORY;
  pui++;
  ((MEMORYITEM*)pui)->cbBuffer=cbBytes;
  
#warning !!!!! Memory is not in inuse list !!!!!
  /* Add memory to in use list */
  //_wpAddToObjUseList(somSelf, (PUSEITEM)pMem);
  
  return ptrMem+sizeof(USEITEM)+sizeof(MEMORYITEM);    
}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpFreeMem(WPObject* nomSelf, const gpointer pByte, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  if(!pByte)
    return FALSE;

  /* remove from inuse list */
#warning !!!!! Memory not removed from inuse list !!!!!
  //_wpDeleteFromObjUseList(somSelf, (PUSEITEM)(pByte-sizeof(USEITEM)-sizeof(MEMORYITEM)) );

  NOMFree(pByte-sizeof(USEITEM)-sizeof(MEMORYITEM));
  return TRUE; /* free can't fail */;
}

NOM_Scope void NOMLINK impl_WPObject_nomInit(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  /* orbit-idl-c-stubs.c, VoyagerWriteProtoForParentCall line 84 */
  WPObject_nomInit_parent((NOMObject*) nomSelf,  ev);

  nomPrintf("    Entering %s with nomSelf: 0x%x. nomSelf is: %s.\n",
            __FUNCTION__, nomSelf , nomSelf->mtab->nomClassName);

  /* Initialize important data before letting subclasses do their stuff */
  //_gObjectMutex=g_mutex_new();
  if(NO_ERROR!=DosCreateMutexSem(NULL, &_gObjectMutex, 0, FALSE))
    {
      g_error("Can't create a mutex for WPObject!");
    }

  _wpInitData(nomSelf, ev);
}

NOM_Scope void NOMLINK impl_WPObject_nomUninit(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  _wpUnInitData(nomSelf, ev);

  //g_mutex_free(_gObjectMutex);
  DosCloseMutexSem(_gObjectMutex);
  WPObject_nomUninit_parent((NOMObject*)nomSelf, ev);
}

NOM_Scope void NOMLINK impl_WPObject_wpInitData(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  /* Make sure a title exists (even if it's an empty string */
  _pnomStringTitle=NOMStringNew();
}

NOM_Scope void NOMLINK impl_WPObject_wpUnInitData(WPObject* nomSelf, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

}

NOM_Scope gpointer NOMLINK impl_WPObject_wpOpen(WPObject* nomSelf, const gpointer ptrReserved, const CORBA_unsigned_long ulView, const gpointer ptrParams, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  gpointer nomRetval=NULLHANDLE;

  return nomRetval;
}


NOM_Scope void NOMLINK impl_WPObject_wpLockObject(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  g_atomic_int_inc(&_iLockCounter);
}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpUnlockObject(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  /* We return TRUE if the counter is 0 */
  if(g_atomic_int_dec_and_test(&_iLockCounter))
    {
      /* Counter is 0 so the object should go dormant */
      return TRUE;
    }
  return FALSE;
}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpObjectIsLocked(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  return g_atomic_int_get(&_iLockCounter) && TRUE;
}

NOM_Scope gpointer NOMLINK impl_WPObject_wpQueryIcon(WPObject* nomSelf, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  return NULLHANDLE;
}

NOM_Scope CORBA_unsigned_long NOMLINK impl_WPObject_wpRequestObjectMutexSem(WPObject* nomSelf,
                                                                            const CORBA_unsigned_long ulReserved, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  return DosRequestMutexSem(_gObjectMutex, -1L);
}

NOM_Scope CORBA_unsigned_long NOMLINK impl_WPObject_wpReleaseObjectMutexSem(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);
  return DosReleaseMutexSem(_gObjectMutex);
}

NOM_Scope PNOMString NOMLINK impl_WPObject_wpSetTitle(WPObject* nomSelf, const PNOMString pnomStrNewTitle, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);
  PNOMString tmpString=NOMStringNew();
  gpointer tmpPtr;

  /* Create a new title */
  NOMString_assignString(tmpString, pnomStrNewTitle, ev);

  /* It may happen that someone changed the title from another thread. We may either
     bail out then or just use our string as the mother of all strings. Since we are the last one
     trying to change the title our string is presumably the correct one. At least we act
     so :P. The memory holding the old string (which may not be the correct one anymore) is
     still valid because we hold a pointer to it in tmpPtr. Garbage collection won't free it
     under our feet.  */
  do{
    /* Get old NOMString containing old title */
    tmpPtr=g_atomic_pointer_get(&_pnomStringTitle);
  }while(!g_atomic_pointer_compare_and_exchange((gpointer*)&_pnomStringTitle, tmpPtr, (gpointer) tmpString));

  return pnomStrNewTitle;
}

NOM_Scope PNOMString NOMLINK impl_WPObject_wpQueryTitle(WPObject* nomSelf, CORBA_Environment *ev)
{
  gpointer tmpPtr;
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  /*
    This string will remain valid as long as we use it. Even if someone changes the title
    while we are in this method the garbage collector won't free it. The other thread
    changing the title is working on a copy so no problem here. */
  tmpPtr=g_atomic_pointer_get(&_pnomStringTitle);

  return NOMString_copyString(tmpPtr, ev);
}

NOM_Scope PNOMMenu NOMLINK impl_WPObject_wpDisplayMenu(WPObject* nomSelf, const PNOMFolderWindow nomFolder,
                                                       const gpointer gReserved, const CORBA_unsigned_long ulMenuType,
                                                       const CORBA_unsigned_long ulReserved, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  PNOMMenu nomMenu=NOMMenuNew();

  /* Let classes insert menu items now */
  WPObject_wpModifyMenu(nomSelf, nomFolder, nomMenu, ulMenuType, ev);

  /* Let classes filter menu items now */
  WPObject_wpFilterMenu(nomSelf, nomFolder, nomMenu, ulMenuType, 0, ev);

  /* And finally show it */
  gtk_menu_popup(GTK_MENU(NOMMenu_getMenuHandle(nomMenu,ev)), NULL, NULL, NULL, NULL, 0,
                 gtk_get_current_event_time());
  return nomMenu;
}

NOM_Scope void NOMLINK impl_WPObject_wpModifyMenu(WPObject* nomSelf, const PNOMFolderWindow nomFolder, const PNOMMenu nomMenu, const CORBA_unsigned_long ulMenuType, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

}

NOM_Scope void NOMLINK impl_WPObject_wpFilterMenu(WPObject* nomSelf, const PNOMFolderWindow nomFolder, const PNOMMenu nomMenu, const CORBA_unsigned_long ulMenuType, const CORBA_unsigned_long ulFlags, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpMenuItemSelected(WPObject* nomSelf, const PNOMFolderWindow nomFolder,
                                                                 const PNOMMenu nomMenu,
                                                                 const CORBA_unsigned_long ulMenuType,
                                                                 CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  CORBA_boolean nomRetval;

  return nomRetval;
}

NOM_Scope void NOMLINK impl_WPObject_wpInsertMenuItem(WPObject* nomSelf, const PNOMMenu nomMenu,
                                                      const CORBA_unsigned_long ulId, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

}
