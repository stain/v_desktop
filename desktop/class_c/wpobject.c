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
* Portions created by the Initial Developer are Copyright (C) 2005-2007
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
#ifndef NOM_WPObject_IMPLEMENTATION_FILE
#define NOM_WPObject_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include <os2.h>

#include <nom.h>
#include <nomtk.h>

#include <string.h>
#include <gtk/gtk.h>
#include <nomguitk.h>

#include "nomwindow.h"
#include "desktoptypes.h"

#include "wpnotebook.h"
#include "wpfolderwindow.h"
#include "m_wpfolder.h"
#include "wpfolder.h"
#include "wpobject.ih"


/*************** Local vars ************************************/

static nomId WPObjectNomId;

/***************************************************************/

/**
   \brief Function which implements the wpAllocMem() method of WPObject.
 */
NOM_Scope gpointer NOMLINK impl_WPObject_wpAllocMem(WPObject* nomSelf, const gulong cbBytes,
                                                    gulong* prc, CORBA_Environment *ev)
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
  pui->wpObject=nomSelf;
  pui++;
  ((MEMORYITEM*)pui)->cbBuffer=cbBytes;
  
  /* Add memory to in use list */
  WPObject_wpAddToObjUseList(nomSelf, (PUSEITEM)ptrMem, ev);
  
  return (gpointer)((PBYTE)ptrMem+sizeof(USEITEM)+sizeof(MEMORYITEM));    
}

/**
   \brief Function which implements the wpFreeMem() method of WPObject.
 */
NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpFreeMem(WPObject* nomSelf, const gpointer pByte, CORBA_Environment *ev)
{
  gpointer ptrMem=NULLHANDLE;

  if(!pByte)
    return FALSE;

  ptrMem=(gpointer)((PBYTE)pByte-sizeof(USEITEM)-sizeof(MEMORYITEM));

  /* remove from inuse list */
  WPObject_wpDeleteFromObjUseList(nomSelf, (PUSEITEM)ptrMem, ev );

  NOMFree(ptrMem);
  return TRUE; /* free can't fail */;
}

/**
   \brief Override of nomInit() from NOMObject.
 */
NOM_Scope void NOMLINK impl_WPObject_nomInit(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  /* orbit-idl-c-stubs.c, VoyagerWriteProtoForParentCall line 84 */
  WPObject_nomInit_parent((NOMObject*) nomSelf,  ev);

  //nomPrintf("    Entering %s with nomSelf: 0x%x. nomSelf is: %s.\n",
  //        __FUNCTION__, nomSelf , nomSelf->mtab->nomClassName);

  /* Initialize important data before letting subclasses do their stuff */
  //_gObjectMutex=g_mutex_new();
  if(NO_ERROR!=DosCreateMutexSem(NULL, &_gObjectMutex, 0, FALSE))
    {
      g_error("Can't create a mutex for WPObject!");
    }

  _wpInitData(nomSelf, ev);
}

NOM_Scope void NOMLINK impl_WPObject_nomUnInit(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  _wpUnInitData(nomSelf, ev);

  //g_mutex_free(_gObjectMutex);
  DosCloseMutexSem(_gObjectMutex);
  WPObject_nomUnInit_parent((NOMObject*)nomSelf, ev);
}

/**
   \brief Function which implements the wpInitData() method of WPObject.
 */
NOM_Scope void NOMLINK impl_WPObject_wpInitData(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  /* Get our unique class ID. We need it for example when inserting menu items to
     specify the namespace. We query it here because getting a GQuark from a string
     is rather time consuming. The result is saved in a var for later use. */
  WPObjectNomId=nomIdFromString("WPObject"); //g_quark_from_string("WPObject");

  /* Make sure a title exists (even if it's an empty string */
  _pnomStringTitle=NOMStringNew();
}

/**
   \brief Function which implements the wpUnInitData() method of WPObject.
 */
NOM_Scope void NOMLINK impl_WPObject_wpUnInitData(WPObject* nomSelf, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

}

static
gboolean defaultWPWindowDeleteHandler(GtkWidget* gtkWidget, GdkEvent* gdkEvent, gpointer pData)
{
  WPObject* wpObject;
  WPWindow* wpWindow;

  PUSEITEM pUseItem=(PUSEITEM) pData;

  wpWindow=(WPWindow*)g_object_get_data(G_OBJECT(gtkWidget), NOMOBJECT_KEY_STRING);

  g_return_val_if_fail(NULLHANDLE!=wpWindow, FALSE);

  /* This is also in the use item */
  wpObject=WPNoteBook_wpQueryObject(wpWindow, NULLHANDLE);

  g_return_val_if_fail(NULLHANDLE!=wpObject, FALSE);

  WPObject_wpSaveDeferred(wpObject, NULLHANDLE);

  WPObject_wpDeleteFromObjUseList(wpObject, pUseItem, NULLHANDLE);

  return FALSE; /* Let other handlers run */
}

/**
   \brief Function which implements the wpOpen() method of WPObject.
 */
NOM_Scope gpointer NOMLINK impl_WPObject_wpOpen(WPObject* nomSelf, const PWPFolderWindow nomFolder,
                                                const gulong ulView, const nomId nameSpaceId,
                                                const gpointer pParam, CORBA_Environment *ev)
{
  /* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  nomId nSpaceId=nameSpaceId;
  gulong ulV=ulView;

  /* Special parameter representing a double click or "I just don't know" ;-) */
  if(OPEN_DEFAULT==ulView)
    ulV=WPObject_wpQueryDefaultView(nomSelf, &nSpaceId, NULLHANDLE);

  /* We only handle items with in own name space */
  if(WPObjectNomId==nSpaceId)
    {
      switch(ulV)
        {
        case OPEN_SETTINGS:
          {
            PUSEITEM pui;
            gulong ulError;
            WPNoteBook* wpNoteBook;

            if(CCVIEW_ON==WPObject_wpQueryConcurrentView(nomSelf, NULLHANDLE)){
              /* Concurrent view is ignored for the settings. */
              if((wpNoteBook=WPObject_wpSwitchTo(nomSelf, ulV, nSpaceId, NULLHANDLE))!=NULLHANDLE)
                return wpNoteBook;
            }
            wpNoteBook=WPNoteBookNew();
            WPNoteBook_wpSetObject(wpNoteBook, nomSelf, NULLHANDLE);
            
            _wpAddSettingsPages(nomSelf, wpNoteBook, ev);
            
            /* Add a view item to inuse list */
            pui=(PUSEITEM)WPObject_wpAllocMem(nomSelf, sizeof(USEITEM)+sizeof(VIEWITEM), &ulError, ev);
            /* Fill the structures */
            
            pui->type=(gulong)USAGE_OPENVIEW;
            pui->wpObject=nomSelf;
            pui++;
            ((VIEWITEM*)pui)->ulView=OPEN_SETTINGS;
            ((VIEWITEM*)pui)->nomWindow=(NOMWindow*)wpNoteBook;
            ((VIEWITEM*)pui)->nameSpaceId=WPObjectNomId;
            pui--;
            //g_message("   in %s wpNoteBook: %lx pui %lx", __FUNCTION__, wpNoteBook, pui);
            /* Make sure the view item is removed when the window is closed */
            g_signal_connect(G_OBJECT(NOMWindow_queryWindowHandle((NOMWindow*)wpNoteBook, NULLHANDLE)),
                             "delete-event", 
                             G_CALLBACK(defaultWPWindowDeleteHandler), (gpointer) pui);
            WPObject_wpAddToObjUseList(nomSelf, pui, ev);
            WPNoteBook_show(wpNoteBook, ev);
            return (gpointer) wpNoteBook;
          }
        default:
          /* We are the very last in the chain of classes being called */
          g_return_val_if_reached(NULLHANDLE);
          break;
        }
    }
  return NULLHANDLE;
}

/**
   \brief Function which implements the wpViewObject() method of WPObject.
 */
NOM_Scope gpointer NOMLINK impl_WPObject_wpViewObject(WPObject* nomSelf, const PWPFolderWindow nomFolder,
                                                      const gulong ulView, const nomId nameSpaceId,
                                                      const gpointer pParam, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  gpointer nomRetval=NULLHANDLE;
  nomId nSpaceId=nameSpaceId;
  gulong ulV=ulView;

  /* Special parameter representing a double click or "I just don't know" ;-) */
  if(OPEN_DEFAULT==ulView)
    ulV=WPObject_wpQueryDefaultView(nomSelf, &nSpaceId, NULLHANDLE);

  if(CCVIEW_ON==WPObject_wpQueryConcurrentView(nomSelf, NULLHANDLE))
    return WPObject_wpOpen(nomSelf, nomFolder, ulV, nSpaceId, pParam, NULLHANDLE);

  if((nomRetval=WPObject_wpSwitchTo(nomSelf, ulV, nSpaceId, NULLHANDLE))!=NULLHANDLE)
    return nomRetval;

  return WPObject_wpOpen(nomSelf, nomFolder, ulV, nSpaceId, pParam, NULLHANDLE);
}

/**
   \brief Function which implements the wpSwitchTo() method of WPObject.
 */
NOM_Scope gpointer NOMLINK impl_WPObject_wpSwitchTo(WPObject* nomSelf, const gulong ulView, const nomId nameSpaceId,
                                                    CORBA_Environment *ev)
{
  /* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  PVIEWITEM pViewItem;

  pViewItem=WPObject_wpFindViewItem(nomSelf, ulView, nameSpaceId, NULLHANDLE, NULLHANDLE);

  if(pViewItem){
    /* Bring the window to the foreground */
    NOMWindow_present(pViewItem->nomWindow, ev);
    return (gpointer)pViewItem->nomWindow;
  }/* if() */

  return NULLHANDLE;
}

/**
   \brief Function which implements the wpRegisterView() method of WPObject.
 */
NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpRegisterView(WPObject* nomSelf, const PNOMWindow pWindow,
                                                             const PNOMString nomStrViewTitle, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  g_message("Method %s not implemented.", __FUNCTION__);
  return FALSE;
}

/**
   \brief Function which implements the wpAddToObjUseList() method of WPObject.
 */
NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpAddToObjUseList(WPObject* nomSelf, const PUSEITEM pUseItem,
                                                                CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  if(!pUseItem)
    return FALSE;

  //g_message("   in %s : %x", __FUNCTION__, pUseItem);

  WPObject_wpRequestObjectMutexSem(nomSelf, 0,ev);

  _glstObjectInUse=g_slist_append( _glstObjectInUse, (gpointer)pUseItem);

  WPObject_wpReleaseObjectMutexSem(nomSelf,ev);

  return TRUE;
}

/**
   \brief Function which implements the wpDeleteFromObjUseList() method of WPObject.
 */
NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpDeleteFromObjUseList(WPObject* nomSelf, const PUSEITEM pUseItem,
                                                                     CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  if(!pUseItem)
    return FALSE;

  WPObject_wpRequestObjectMutexSem(nomSelf, 0,ev);
  //g_message("   in %s %lx", __FUNCTION__, pUseItem);
  _glstObjectInUse=g_slist_remove( _glstObjectInUse, (gpointer)pUseItem);

  WPObject_wpReleaseObjectMutexSem(nomSelf,ev);

  return TRUE;
}


NOM_Scope PUSEITEM NOMLINK impl_WPObject_wpFindUseItem(WPObject* nomSelf, const gulong ulType,
                                                       const PUSEITEM pCurrentUseItem, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);
  PUSEITEM pUseItem=NULLHANDLE;
  GSList* tmpList;

  if(NULLHANDLE==_glstObjectInUse)
    return NULLHANDLE;

  WPObject_wpRequestObjectMutexSem(nomSelf, 0,ev);

  if(NULLHANDLE==pCurrentUseItem)
    tmpList=_glstObjectInUse;
  else{
    tmpList=g_slist_find(_glstObjectInUse, pCurrentUseItem);
    tmpList=g_slist_next(tmpList);
  }

  while(tmpList)
    {
      pUseItem=(PUSEITEM)tmpList->data;

      //g_message("     a in %s  type: %d", __FUNCTION__, pUseItem->type);
      if(pUseItem && ulType==pUseItem->type)
        break;

      tmpList=g_slist_next(tmpList);
      pUseItem=NULLHANDLE;
    };

  WPObject_wpReleaseObjectMutexSem(nomSelf,ev);

  return pUseItem;
}


NOM_Scope PVIEWITEM NOMLINK impl_WPObject_wpFindViewItem(WPObject* nomSelf, const gulong ulView,
                                                         const nomId nameSpaceId,
                                                         const PVIEWITEM pCurrentItem, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  PUSEITEM pUseItem;
  PVIEWITEM pViewItem=NULLHANDLE;


  if(!ulView)
    return NULLHANDLE;

  if(NULLHANDLE==pCurrentItem)
    pUseItem=WPObject_wpFindUseItem(nomSelf, USAGE_OPENVIEW,  NULLHANDLE, ev);
  else{
    pUseItem=(PUSEITEM)pCurrentItem;
    pUseItem--; /* Note that VIEWITEM comes after the USEITEM structure */
    pUseItem=WPObject_wpFindUseItem(nomSelf, USAGE_OPENVIEW,  pUseItem, ev);
  }

  while(pUseItem)
    {
      ++pUseItem;
      pViewItem=(PVIEWITEM)pUseItem;
      pUseItem--; /* Note that VIEWITEM comes after the USEITEM structure */
      //g_message("        a in %s %d", __FUNCTION__, flViews);
      if((pViewItem->ulView == ulView) && (pViewItem->nameSpaceId==nameSpaceId))
        break;

      pUseItem=WPObject_wpFindUseItem(nomSelf, USAGE_OPENVIEW,  pUseItem, ev);
      pViewItem=NULLHANDLE;
    }

  return pViewItem;
}

NOM_Scope gulong NOMLINK impl_WPObject_wpQueryDefaultView(WPObject* nomSelf, const pnomId pNameSpaceId,
                                                          CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  *pNameSpaceId=WPObjectNomId;

  return OPEN_SETTINGS;
}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpSetDefaultView(WPObject* nomSelf, const gulong ulView,
                                                               const nomId nameSpaceId, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  CORBA_boolean nomRetval=FALSE;

  g_message("%d %s not implemented", __LINE__, __FUNCTION__);
  return nomRetval;
}

NOM_Scope void NOMLINK impl_WPObject_wpSetConcurrentView(WPObject* nomSelf, const gulong ulCCView, 
                                                         CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  g_message("%d %s not implemented", __LINE__, __FUNCTION__);

}

NOM_Scope gulong NOMLINK impl_WPObject_wpQueryConcurrentView(WPObject* nomSelf, CORBA_Environment *ev)
{
  /* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  return CCVIEW_ON;
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

  //g_message("%d %s !!! Achtung explicit metaclass nicht in mtab ---> %x %x", __LINE__, __FUNCTION__,
  //        _nomGetClass((NOMObject*)nomSelf, NULLHANDLE), _WPObject);
  return M_WPObject_wpclsQueryIcon(_nomGetClass((NOMObject*)nomSelf, NULLHANDLE), NULLHANDLE);
  // return M_WPObject_wpclsQueryIcon(_WPObject, NULLHANDLE);
}

NOM_Scope CORBA_unsigned_long NOMLINK impl_WPObject_wpRequestObjectMutexSem(WPObject* nomSelf,
                                                                            const CORBA_unsigned_long ulReserved,
                                                                            CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  return DosRequestMutexSem(_gObjectMutex, -1L);
}

NOM_Scope CORBA_unsigned_long NOMLINK impl_WPObject_wpReleaseObjectMutexSem(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);
  return DosReleaseMutexSem(_gObjectMutex);
}

NOM_Scope void NOMLINK impl_WPObject_wpSetTitle(WPObject* nomSelf, const PNOMString pnomStrNewTitle,
                                                      CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);
  PNOMString tmpString=(PNOMString)NOMString_new(pnomStrNewTitle, NULLHANDLE);
  gpointer tmpPtr;

  /* Create a new title */
  NOMString_assign(tmpString, pnomStrNewTitle, NULLHANDLE);

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
}

NOM_Scope void NOMLINK impl_WPObject_wpSetTitleFromCString(WPObject* nomSelf, const CORBA_char * chrNewTitle,
                                                           CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);
  PNOMString orgTitle;
  PNOMString tmpString;

  orgTitle=(PNOMString)g_atomic_pointer_get(&_pnomStringTitle);
  tmpString=(PNOMString)NOMString_new(orgTitle, NULLHANDLE);

  NOMString_assignCString(tmpString, chrNewTitle, NULLHANDLE);
  do{
    /* Get old NOMString containing old title */
    orgTitle=(PNOMString)g_atomic_pointer_get(&_pnomStringTitle);
  }while(!g_atomic_pointer_compare_and_exchange((gpointer*)&_pnomStringTitle,
                                                (gpointer)orgTitle, (gpointer) tmpString));
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

  return NOMString_copy(tmpPtr, ev);
}

NOM_Scope CORBA_unsigned_long NOMLINK impl_WPObject_wpAddObjectGeneralPage(WPObject* nomSelf,
                                                                           const PWPNoteBook wpNoteBook,
                                                                           CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  GtkWidget *frame;
  GtkWidget *label;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *button;
  GtkWidget *entry;

  /* A vbox to layout the settings page */
  vbox=gtk_vbox_new(FALSE, 0);

  hbox=gtk_hbutton_box_new ();
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbox), GTK_BUTTONBOX_SPREAD);
  gtk_box_set_spacing (GTK_BOX (hbox), 2);

  /* Create buttons */
  button = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_container_add (GTK_CONTAINER (hbox), button);
  button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_container_add (GTK_CONTAINER (hbox), button);
  /* Put the buttons at the very bottom */
  gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 1);  

  /* The icon part */
  frame = gtk_frame_new ("Current Icon");
  gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
  gtk_widget_set_size_request(frame, 200, 100);
  gtk_box_pack_end (GTK_BOX (vbox), frame, FALSE, FALSE, 1);

  /* The Title part */
  frame = gtk_frame_new ("Title");
  gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
  entry=gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(entry), CCHMAXPATH);
  gtk_container_add (GTK_CONTAINER (frame), entry);
  gtk_box_pack_end (GTK_BOX (vbox), frame, FALSE, FALSE, 1);

  /* The label for the tab */
  label = gtk_label_new ("Icon");

  gtk_widget_show_all (vbox);

  NOMNoteBook_prependPage(WPNoteBook_wpQueryNoteBook(wpNoteBook, ev), vbox, label, ev);

  return 1234;
}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpAddSettingsPages(WPObject* nomSelf, const PWPNoteBook wpNoteBook,
                                                                 CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  _wpAddObjectGeneralPage(nomSelf, wpNoteBook, ev);
  return TRUE;
}

NOM_Scope CORBA_unsigned_long NOMLINK impl_WPObject_wpInsertSettingsPage(WPObject* nomSelf, 
                                                                         const PWPNoteBook wpNoteBook,
                                                                         const gpointer ppageinfo, 
                                                                         CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  CORBA_unsigned_long nomRetval;

  return nomRetval;
}


NOM_Scope PNOMMenu NOMLINK impl_WPObject_wpDisplayMenu(WPObject* nomSelf, const PWPFolderWindow nomFolder,
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
  gtk_menu_popup(GTK_MENU(NOMMenu_queryMenuHandle(nomMenu,ev)), NULL, NULL, NULL, NULL, 0,
                 gtk_get_current_event_time());

  return nomMenu;
}

NOM_Scope void NOMLINK impl_WPObject_wpModifyMenu(WPObject* nomSelf, const PWPFolderWindow nomFolder, 
                                                  const PNOMMenu nomMenu, const CORBA_unsigned_long ulMenuType,
                                                  CORBA_Environment *ev)
{
  NOMMenuItem* mItem;
  /*  WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  mItem=NOMMenuItemNew();

  NOMMenuItem_setup(mItem, __FUNCTION__, WPObjectNomId, 0, ev);
  WPObject_wpInsertMenuItem(nomSelf, nomMenu, mItem, 0, ev);

  mItem=NOMMenuItemNew();
  NOMMenuItem_setup(mItem, "Settings...", WPObjectNomId, WPMENUID_PROPERTIES, ev);
  WPObject_wpInsertMenuItem(nomSelf, nomMenu, mItem, 0, ev);
}

NOM_Scope void NOMLINK impl_WPObject_wpFilterMenu(WPObject* nomSelf, const PWPFolderWindow nomFolder,
                                                  const PNOMMenu nomMenu, const CORBA_unsigned_long ulMenuType,
                                                  const CORBA_unsigned_long ulFlags, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpMenuItemSelected(WPObject* nomSelf, const PWPFolderWindow nomFolder,
                                                                 const PNOMMenuItem nomMenuItem,
                                                                 CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  /* We only handle items within own name space */
  if(WPObjectNomId==NOMMenuItem_queryNameSpaceId(nomMenuItem, ev))
    {
      switch(NOMMenuItem_queryId(nomMenuItem, ev))
        {
        case WPMENUID_PROPERTIES:
          {
            WPObject_wpViewObject(nomSelf, nomFolder,
                            OPEN_SETTINGS, WPObjectNomId, NULLHANDLE, NULLHANDLE);
            return TRUE; /* We always return TRUE to show, we handled the menu item. It doesn't
                          matter if we actually succeeded with creating the settings notebook. */
          }
        default:
          break;
        }
    }
  return FALSE;
}

/*
  Callback for desktop menu items. This callback takes the object pointer and call wpMenuItemSelected()
  of the object.
 */
static void menuItemActivateCallBack(GtkMenuItem* gtkMenuItem, gpointer wpObject)
{
  PNOMMenuItem pItem;

#if 0
  if(!nomIsObj((WPObject*)wpObject))
    return;
#endif

  pItem=(PNOMMenuItem) g_object_get_data(G_OBJECT(gtkMenuItem), NOMOBJECT_KEY_STRING);

  WPObject_wpMenuItemSelected((WPObject*) wpObject, NULLHANDLE,
                              pItem, NULLHANDLE);
}

/*
  This method adds the correct callback function to the menuitem and inserts it into the given menu.

  FIXME: position information for the menu item isn't used yet. The Item is always appended.
  
 */
NOM_Scope void NOMLINK impl_WPObject_wpInsertMenuItem(WPObject* nomSelf, const PNOMMenu nomMenu,
                                                      const PNOMMenuItem nomMenuItem, 
                                                      const CORBA_unsigned_long ulPosition, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  g_signal_connect(NOMMenuItem_queryMenuItemHandle(nomMenuItem, ev), "activate",
                   (GCallback) menuItemActivateCallBack, (gpointer) nomSelf);

  gtk_menu_shell_append(GTK_MENU_SHELL(NOMMenu_queryMenuHandle(nomMenu,ev)), 
                        NOMMenuItem_queryMenuItemHandle(nomMenuItem, ev));

}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpSaveDeferred(WPObject* nomSelf, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  g_message("%s not implemented", __FUNCTION__);

  return TRUE;
}

NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpSaveImmediate(WPObject* nomSelf, CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  g_message("%s not implemented", __FUNCTION__);

  return TRUE;
}

NOM_Scope void NOMLINK impl_WPObject_wpSetFolder(WPObject* nomSelf, const PWPFolder wpParentFolder,
                                                 CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  _wpParentFldr=wpParentFolder;
}

NOM_Scope PWPFolder NOMLINK impl_WPObject_wpQueryFolder(WPObject* nomSelf, CORBA_Environment *ev)
{
  WPObjectData* nomThis=WPObjectGetData(nomSelf);

  return _wpParentFldr;
}


NOM_Scope gulong NOMLINK impl_WPObject_wpDragOver(WPObject* nomSelf, const gpointer containerHandle,
                                                  const gpointer pDragInfo, CORBA_Environment *ev)
{
  /* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  return 0; /* Don't allow drop */
}

NOM_Scope gulong NOMLINK impl_WPObject_wpDrop(WPObject* nomSelf, const gpointer containerHandle,
                                              const gpointer pDragInfo, CORBA_Environment *ev)
{
  /* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  g_message("Calling %s", __FUNCTION__);
  return 0;
}


NOM_Scope CORBA_boolean NOMLINK impl_WPObject_wpMoveObject(WPObject* nomSelf,
                                                           const PWPFolder wpTargetFolder,
                                                           CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */
  WPFolder * wpFolder;
  PUSEITEM pui;

  if(!nomIsObj(wpTargetFolder))
    return FALSE;

  /* Get folder holding the object */
  wpFolder=WPObject_wpQueryFolder(nomSelf, NULLHANDLE);

  g_message("Parent folder is %s", 
            NOMPath_queryCString(WPFolder_wpQueryFileName(wpFolder, TRUE, NULLHANDLE),
                                 NULLHANDLE));

  /* Remove it from the parent folders content list. */
  WPFolder_wpDeleteFromContent(wpFolder, nomSelf, NULLHANDLE);

  /* Remove it from the folders model thus any folder window. */
  pui=_wpDeleteFromStore(wpFolder, nomSelf, NULLHANDLE);
  _wpFreeMem(nomSelf, pui, NULLHANDLE);

  g_message("Target folder is %s", 
            NOMPath_queryCString(WPFolder_wpQueryFileName(wpTargetFolder, TRUE, NULLHANDLE),
                                 NULLHANDLE));

  /* Insert into the new folder */
  _wpAddToContent(wpTargetFolder, nomSelf, NULLHANDLE);
  _wpAddToStore(wpTargetFolder, nomSelf, NULLHANDLE);

  return FALSE;
}

NOM_Scope PWPObject NOMLINK impl_WPObject_wpCopyObject(WPObject* nomSelf,
                                                       const PWPFolder wpTargetFolder,
                                                       const CORBA_boolean fLock,
                                                       CORBA_Environment *ev)
{
/* WPObjectData* nomThis=WPObjectGetData(nomSelf); */

  g_message("Calling %s, not implmented yet", __FUNCTION__);

  return FALSE;
}

