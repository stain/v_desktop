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
#ifndef NOM_WPFolder_IMPLEMENTATION_FILE
#define NOM_WPFolder_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#include <os2.h>

#include "nom.h"
#include <nomtk.h>

#include <string.h>
#include "gtk/gtk.h"

typedef struct _PRIVFOLDERDATA
{
  GtkListStore *gstoreFldContents;
  GtkWidget    *gtkIconView;
}PRIVFOLDERDATA, *PPRIVFOLDERDATA;

typedef struct _FLDRGTREEKEY
{
  PGTree pGTree;
  gchar* chrKey;
}FLDRGTREEKEY, *PFLDRGTREEKEY;

/* Gui stuff */
#include "nomguitk.h"
#include "nomwindow.h"
#include "desktoptypes.h"

#include "wpnotebook.h"
#include "wpfolderwindow.h"
#include "m_wpfolder.h"
#include "wpfolder.ih"

#include "wpdatafile.h"
#include "desktop.h"
#include "helper.h"
#include "exception.h"

#include "nomdraginfo.h"

typedef struct _FLDRGTREEVALUE
{
  PGTree pGTree;
  WPObject* wpObject;
  gchar* chrKey;
}FLDRGTREEVALUE, *PFLDRGTREEVALUE;


/* Enum for the folder store */
enum
{
  COL_OBJECT_PTR,
  COL_PATH,
  COL_DISPLAY_NAME,
  COL_PIXBUF,
  NUM_COLS
};

static GtkListStore * fldr_CreateStore (void)
{
  GtkListStore *store;

  store = gtk_list_store_new (NUM_COLS,
                              G_TYPE_POINTER,
                              G_TYPE_STRING, 
                              G_TYPE_STRING, 
                              GDK_TYPE_PIXBUF);
  // g_message("%s: store: %x", __FUNCTION__, (UINT) store);
  return store;
}

/*************** Local vars ************************************/

/* This ID is used as a namespace */
static nomId WPFolderNomId;

/***************************************************************/
/*
  The folder is already populated, that means the internal folder list
  is filled with all the objects in the folder. We traverse that list
  and fill the store utilisized by the icon view with the objects.
 */
static
gboolean fillStoreTraverseFunc(gpointer pKey, gpointer pTraverseValue, gpointer pData)
{
  GtkTreeIter iter;
  PFLDRGTREEVALUE pValue=pTraverseValue;
  gchar * display_name;
  GtkListStore *store=(GtkListStore *)pData;
  WPObject* wpObject;
    
  //  g_message("In %s with %s %x", __FUNCTION__, (char*)pKey, (UINT)pValue->wpObject);

#warning !!!!! Use Title here !!!!!
  display_name=g_strdup(pKey); /* New string necessary? */  

  wpObject=pValue->wpObject;
  
  if(nomIsObj(wpObject)){
#if 0
    g_message("%d %s : %s, %s",
              __LINE__, __FUNCTION__, (char*)pKey,
              NOMPath_queryCString(WPFolder_wpQueryFileName(wpFolder, TRUE, NULLHANDLE), NULLHANDLE));
#endif
    
    gtk_list_store_append (store, &iter);
    
    gtk_list_store_set (store, &iter,
                        COL_OBJECT_PTR, wpObject,
                        COL_PATH, "",
                        COL_DISPLAY_NAME, display_name,
                        COL_PIXBUF, _wpQueryIcon(wpObject, NULLHANDLE),
                        -1);
  }
  return FALSE; /* Traverse every item */
}

static gboolean
fldr_fillStore (WPFolder* nomSelf, GtkListStore *store, const gchar* gchrPath)
{
  WPFolderData* nomThis=WPFolderGetData(nomSelf); 

  /* First clear the store */
  gtk_list_store_clear (store);

  g_tree_foreach(_fldrObjects, (GTraverseFunc) fillStoreTraverseFunc, store);

  return TRUE;
}


NOM_Scope gulong NOMLINK impl_WPFolder_wpAddToContent(WPFolder* nomSelf, const PWPObject wpObject, 
                                                      const CORBA_char * chrFileName, CORBA_Environment *ev)
{
  WPFolderData* nomThis=WPFolderGetData(nomSelf);
  PFLDRGTREEVALUE pValue;

  pValue=(PFLDRGTREEVALUE)NOMMalloc(sizeof(FLDRGTREEVALUE));
  pValue->pGTree=_fldrObjects;
  pValue->wpObject=wpObject;
  pValue->chrKey=g_strdup(chrFileName);

  g_tree_insert(_fldrObjects, pValue->chrKey, pValue);

#if 0
  PFLDRGTREEKEY pKey;
  pKey=(PFLDRGTREEKEY)NOMMalloc(sizeof(FLDRGTREEKEY));
  pKey->pGTree=_fldrObjects;
  pKey->chrKey=chrFileName;
  g_tree_insert(_fldrObjects, pKey, wpObject);
#endif

  return 0;
}

NOM_Scope PWPObject NOMLINK impl_WPFolder_wpQueryContent(WPFolder* nomSelf, CORBA_Environment *ev)
{
/* WPFolderData* nomThis=WPFolderGetData(nomSelf); */

  return NULLHANDLE;
}


/* pszPath contains the fully qualified path (checked with WPS) */
NOM_Scope CORBA_boolean NOMLINK impl_WPFolder_wpPopulate(WPFolder* nomSelf, const CORBA_unsigned_long ulReserved,
                                                         const CORBA_char * pszPath, const CORBA_boolean fFoldersOnly,
                                                         CORBA_Environment *ev)
{
  /*  WPFolderData* nomThis=WPFolderGetData(nomSelf); */
  GDir *gDir;
  const gchar *gchrCurrentEntry;
  const gchar* gchrPath;
  PNOMPath fldrPath;

  //  g_log("WPFolder", G_LOG_LEVEL_DEBUG, "%s: Populating %s (0x%x)\n", __FUNCTION__, pszPath, (UINT)nomSelf);

  /* Already populated? */
  if(fFoldersOnly && 
     (_wpQueryFldrFlags(nomSelf, NULLHANDLE) & (FOI_POPULATEDWITHFOLDERS | FOI_POPULATEDWITHALL)))
    return TRUE;
  else if(_wpQueryFldrFlags(nomSelf, NULLHANDLE) & FOI_POPULATEDWITHALL)
    return TRUE;

  /* Go through the directory and extract all the file
   * information */
  fldrPath=WPFolder_wpQueryFileName(nomSelf, TRUE, NULLHANDLE);
  gchrPath=NOMPath_queryCString(fldrPath, NULLHANDLE);
  gDir = g_dir_open(gchrPath, 0, NULL);
  if (!gDir)
    return FALSE;

  while ((gchrCurrentEntry = g_dir_read_name (gDir))!= NULL)
    {
      gchar *path, *display_name;

      if (gchrCurrentEntry[0] != '\0')
        {
          WPObject* wpObject;

          path = g_build_filename (gchrPath, gchrCurrentEntry, NULL);

          display_name = g_filename_to_utf8 (gchrCurrentEntry, -1, NULL, NULL, NULL);

          if(g_file_test (path, G_FILE_TEST_IS_DIR))
            wpObject=(WPObject*)WPFolderNew(); /* It's a folder */
          else{
            if(!fFoldersOnly)
              wpObject=(WPObject*)WPDataFileNew(); /* It's a file */
            else
              wpObject=NULLHANDLE;
          }

          if(nomIsObj((PNOMObject)wpObject))
            {
              WPObject_wpLockObject(wpObject, NULLHANDLE);
              WPObject_wpSetTitleFromCString(wpObject, display_name, NULLHANDLE);
              WPFileSystem_tstSetFullPath((WPFileSystem*)wpObject, gchrCurrentEntry, NULLHANDLE);
              WPFileSystem_wpSetFolder(wpObject, nomSelf, NULLHANDLE);
#if 0
              g_message("%d %s : %s (%x) , %s",
                        __LINE__, __FUNCTION__, gchrCurrentEntry, (UINT)wpObject,
                        NOMPath_queryCString(WPFolder_wpQueryFileName(wpObject, TRUE, NULLHANDLE), NULLHANDLE));
#endif
              /* insert into contents list */
              WPFolder_wpAddToContent(nomSelf, wpObject, gchrCurrentEntry, NULLHANDLE);
            }/* if(nomIsObj(wpObject)) */
          g_free (path);
          g_free (display_name);
        }/* if (gchrCurrentEntry[0] != '\0') */
    }/* while */
  g_dir_close(gDir);

  if(fFoldersOnly)
    _wpSetFldrFlags(nomSelf, FOI_POPULATEDWITHFOLDERS, FOI_POPULATEDWITHFOLDERS, NULLHANDLE);
  else
    _wpSetFldrFlags(nomSelf, FOI_POPULATEDWITHFOLDERS| FOI_POPULATEDWITHALL,
                    FOI_POPULATEDWITHFOLDERS| FOI_POPULATEDWITHALL, NULLHANDLE);

  return TRUE;
}


/*
  This will change when NOMFolderWindow is migrated to WPFolderWindow.
 */
static
gboolean tempWPWindowDeleteHandler(GtkWidget* gtkWidget, GdkEvent* gdkEvent, gpointer pData)
{
  WPObject* wpObject;
  WPFolderWindow* wpWindow;

  PUSEITEM pUseItem=(PUSEITEM) pData;

  /* This is also in the use item */
  wpWindow=(WPFolderWindow*)g_object_get_data(G_OBJECT(gtkWidget), NOMOBJECT_KEY_STRING);

  g_return_val_if_fail(NULLHANDLE!=wpWindow, FALSE);

  wpObject=pUseItem->wpObject;
  g_return_val_if_fail(NULLHANDLE!=wpObject, FALSE);

  WPObject_wpSaveDeferred(wpObject, NULLHANDLE);
  /* Remove the open folder view from the inuse list */
  WPObject_wpDeleteFromObjUseList(wpObject, pUseItem, NULLHANDLE);

  return FALSE; /* Let other handlers run */
}


NOM_Scope gpointer NOMLINK impl_WPFolder_wpOpen(WPFolder* nomSelf, const PWPFolderWindow nomFolder,
                                                const gulong ulView, const nomId nameSpaceId,
                                                const gpointer pParam, CORBA_Environment *ev)
{
  WPFolderData* nomThis=WPFolderGetData(nomSelf);
  nomId nSpaceId=nameSpaceId;
  gulong ulV=ulView;

  //  g_message("In %s: %d %d %d", __FUNCTION__, WPFolderNomId, nameSpaceId, ulView);

  /* Special parameter representing a double click or "I just don't know" ;-) */
  if(OPEN_DEFAULT==ulView)
    ulV=WPFolder_wpQueryDefaultView(nomSelf, &nSpaceId, NULLHANDLE);

  /* We only handle items with in own name space */
  if(WPFolderNomId==nSpaceId)
    {
      switch(ulV)
        {
        case OPEN_CONTENTS:
          {
            PUSEITEM pui;
            ULONG ulError;
            WPFolderWindow * wpFldrWindow;
            gchar* pszPath;
            PPRIVFOLDERDATA priv;
            GtkListStore* gStore;
            GtkIconView*  gtkIconView;

            pszPath=NOMPath_queryCString(_wpQueryFileName(nomSelf, TRUE, NULLHANDLE), NULLHANDLE);
            g_message("%d %s, %s", __LINE__, __FUNCTION__, pszPath);
            WPFolder_wpPopulate(nomSelf, 0L, pszPath, FALSE,  NULLHANDLE);
            
            
            wpFldrWindow=WPFolder_wpCreateFolderWindow(nomSelf, NULLHANDLE);
            
            /* Insert it into inuse list */
            pui=(PUSEITEM)WPFolder_wpAllocMem(nomSelf, sizeof(USEITEM)+sizeof(VIEWITEM), &ulError, NULLHANDLE);
            /* Fill the structures */
            pui->type=(gulong)USAGE_OPENVIEW;
            pui->wpObject=(PWPObject)nomSelf;
            pui++;
            ((VIEWITEM*)pui)->ulView=OPEN_CONTENTS;
            ((VIEWITEM*)pui)->nomWindow=(NOMWindow*)wpFldrWindow;
            ((VIEWITEM*)pui)->nameSpaceId=WPFolderNomId;
            pui--;

            /* Make sure the view item is removed when the window is closed */
            g_signal_connect(G_OBJECT(NOMWindow_queryWindowHandle((NOMWindow*)wpFldrWindow, NULLHANDLE)),"delete-event", 
                             G_CALLBACK(tempWPWindowDeleteHandler), (gpointer) pui);
            WPFolder_wpAddToObjUseList(nomSelf, pui, NULLHANDLE);
            
            /* Now create a folder store and insert icons into the window */
            priv=_privFolderData;
            gStore=priv->gstoreFldContents;
            
            if(!gStore)
              {
                /* Create a store holding the folder contents */
                gStore=fldr_CreateStore();
                g_return_val_if_fail(gStore!=NULLHANDLE, FALSE);
                priv->gstoreFldContents=gStore;

                /* Fill our store */
                if(gStore)
                  fldr_fillStore(nomSelf, gStore, pszPath);
                else
                  return FALSE;
              }

            gtkIconView=WPFolderWindow_wpQueryContainerHandle(wpFldrWindow, NULLHANDLE);

            gtk_icon_view_set_model(GTK_ICON_VIEW (gtkIconView),
                                    GTK_TREE_MODEL (priv->gstoreFldContents));

            /* We now set which model columns that correspond to the text
             * and pixbuf of each item
             */
            gtk_icon_view_set_text_column (GTK_ICON_VIEW (gtkIconView), COL_DISPLAY_NAME);
            gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (gtkIconView), COL_PIXBUF);
            gtk_icon_view_set_item_width (GTK_ICON_VIEW (gtkIconView), 100);
            
            //            g_object_unref (gStore);
            
            return wpFldrWindow;
          }/* default */
        default:
          break;
        }/* switch */
    }

  return WPFolder_wpOpen_parent(nomSelf, nomFolder, ulView, nameSpaceId, pParam, ev);
}

NOM_Scope gpointer NOMLINK impl_WPFolder_wpQueryIcon(WPFolder* nomSelf, CORBA_Environment *ev)
{

  return M_WPFolder_wpclsQueryIcon( _nomGetClass((NOMObject*)nomSelf, NULLHANDLE), NULLHANDLE);

#if 0
  static const gchar *gchrIconName=NULLHANDLE;
  static gpointer ptrIcon=NULLHANDLE;
  GError *error=NULL;

/* WPFolderData* nomThis=WPFolderGetData(nomSelf); */

  /* Load default wpObject icon */
  if(!gchrIconName){
    gchrIconName=g_build_filename(priv_getIconDir(), WPFOLDER_ICON_FILE, NULL);
    
    g_return_val_if_fail(g_file_test (gchrIconName, G_FILE_TEST_EXISTS), NULLHANDLE);
    nomPrintf("IconFile: %s\n", gchrIconName);  
    //  _hPointerCls = (HPOINTER)gdk_pixbuf_new_from_file (gchrIconName, &error);
    ptrIcon=gdk_pixbuf_new_from_file (gchrIconName, &error);
  }
  return ptrIcon;

  /*  WPFolder_wpQueryIcon_parent(nomSelf,  ev); */
#endif
}

NOM_Scope gulong NOMLINK impl_WPFolder_wpQueryDefaultView(WPFolder* nomSelf, const pnomId pNameSpaceId,
                                                          CORBA_Environment *ev)
{
  *pNameSpaceId=WPFolderNomId;
  return OPEN_CONTENTS;
}


static void
itemActivated (GtkIconView *widget,
		GtkTreePath *treePath,
		gpointer     user_data)
{
  PWPFolderWindow pWindow;

  pWindow=(WPFolderWindow*)user_data;

  if(NULL!=treePath)
    {
      GtkTreeIter iter;
      GtkTreeModel* model;
      WPObject *wpObject;

      /* Click on an icon */            
      model=gtk_icon_view_get_model(GTK_ICON_VIEW(widget));
      gtk_tree_model_get_iter(model , &iter, treePath);
      
      gtk_tree_model_get(model, &iter,
                         0, &wpObject,
                         -1);
      g_message("%s: %s", __FUNCTION__, wpObject->mtab->nomClassName);
      /*      WPObject_wpOpen(wpObject, pWindow, OPEN_CONTENTS, NULL, NULL); */
      TRY(FLDR_ITEMACTIVATED) {
        WPObject_wpViewObject(wpObject, pWindow, OPEN_DEFAULT, 0, NULLHANDLE, NULL);
      }
      CATCH{
        LOUD;
        g_message("Line %d: Trap in %s", __LINE__, __FUNCTION__);  
      }END_CATCH(FLDR_ITEMACTIVATED);
    }
}


#if 0
/**
   Get the WPFolder object from a container widget (icon view). If the object can't be queried
   NULL is returned.
   Note that this function checks if the pointer points to a NOMObject so a call to
   nomIsObj() is not necessary.
 */
static PWPFolder fldrQueryWPFolderFromContainer(GtkWidget * wgtThis)
{
  GtkWidget* wgtToplevel;
  PWPFolder wpFolder;

  if((wgtToplevel=gtk_widget_get_toplevel(wgtThis))==NULLHANDLE)
    return NULLHANDLE;

  if(!GTK_WIDGET_TOPLEVEL(wgtToplevel))
    return NULLHANDLE;

  wpFolder=(PWPFolder) g_object_get_data(G_OBJECT(wgtToplevel), NOMOBJECT_KEY_STRING);
  if(nomIsObj(wpFolder))
    return wpFolder;

  return NULLHANDLE;
}
#endif


/**
  This method creates the folder window it doesn't query any files or creates
  models and stuff.
*/
NOM_Scope PWPFolderWindow NOMLINK impl_WPFolder_wpCreateFolderWindow(WPFolder* nomSelf, CORBA_Environment *ev)
{
  WPFolderWindow * wpFldrWindow;
  PPRIVFOLDERDATA priv;
  WPFolderData *nomThis = WPFolderGetData(nomSelf);
  
  priv=(PPRIVFOLDERDATA)_privFolderData;

  wpFldrWindow=WPFolderWindowNew();

  /* Save a pointer to the desktop folder object */
  WPFolderWindow_wpSetWPObject(wpFldrWindow, (WPObject*)nomSelf, NULLHANDLE);

  /* Connect to the "item_activated" signal */
  g_signal_connect (WPFolderWindow_wpQueryContainerHandle(wpFldrWindow, NULLHANDLE), "item-activated",
                    G_CALLBACK (itemActivated), nomSelf);

  WPFolderWindow_wpSetWindowTitle(wpFldrWindow, WPFolder_wpQueryTitle(nomSelf, NULLHANDLE), NULLHANDLE);

  /* Show the new window */
  WPFolderWindow_show(wpFldrWindow, ev);

  return wpFldrWindow;
}


NOM_Scope gulong NOMLINK impl_WPFolder_wpQueryFldrFlags(WPFolder* nomSelf, CORBA_Environment *ev)
{
  WPFolderData* nomThis=WPFolderGetData(nomSelf);

  return g_atomic_int_get(&_ulFldrFlags);
}


NOM_Scope CORBA_boolean NOMLINK impl_WPFolder_wpSetFldrFlags(WPFolder* nomSelf, const gulong ulFlags,
                                                             const gulong ulMask, CORBA_Environment *ev)
{
  WPFolderData* nomThis=WPFolderGetData(nomSelf);

  WPFolder_wpRequestObjectMutexSem(nomSelf, 0,ev);

  _ulFldrFlags=(_ulFldrFlags & ~ulMask)| ulFlags;

  WPFolder_wpReleaseObjectMutexSem(nomSelf,ev);

  return TRUE;
}


/* FIXME: type and name of containerHandle is wrong */
NOM_Scope gulong NOMLINK impl_WPFolder_wpDragOver(WPFolder* nomSelf, const gpointer containerHandle,
                                                  const gpointer pDragInfo, CORBA_Environment *ev)
{
  /* WPFolderData* nomThis=WPFolderGetData(nomSelf); */
  gulong ulNumItems, ulLoop;
  NOMDragInfo* nomDragInfo=(NOMDragInfo*)pDragInfo;

  if(!nomIsObj(nomDragInfo))
    return DOR_NODROP;

  ulNumItems=NOMDragInfo_queryNumDragItems(nomDragInfo, NULLHANDLE);

  /* No items? */
  if(0==ulNumItems)
    return DOR_NODROP;

  for(ulLoop=0; ulLoop<ulNumItems; ulLoop++)
    {
      /* We only accept desktop objects for now */
      if(!NOMDragInfo_queryIsItemWPObject(nomDragInfo, ulLoop, NULLHANDLE))
        return DOR_NODROP;
      /* No dropping on the source folder for now */
      if(nomSelf==(WPFolder*)
         NOMDragInfo_querySourceWPObject(nomDragInfo, ulLoop, NULLHANDLE))
        return DOR_NODROP;
      /* No dropping on itself */
      if(nomSelf==(WPFolder*)
         NOMDragInfo_queryWPObjectFromItemList(nomDragInfo, ulLoop, NULLHANDLE))
        return DOR_NODROP;
    }

  return GDK_ACTION_MOVE;
  
#if 0
  WPFolder_wpDragOver_parent(nomSelf,  ev);
#endif
}


#if 0
static gint fldrCompareObjects(gconstpointer a, gconstpointer b, gpointer pUserData)
{
  return strcmp(((PFLDRGTREEKEY)a)->chrKey, ((PFLDRGTREEKEY)b)->chrKey);
}
#endif

static void fldrCatchDuplicates(gpointer pOrgValue)
{
  PFLDRGTREEVALUE pValue;
  PFLDRGTREEVALUE pValueRemove;

  pValue=(PFLDRGTREEVALUE)pOrgValue;

  g_message("%d %s: value (object): %x %s", __LINE__, __FUNCTION__, (UINT)pValue->wpObject, pValue->chrKey);

#warning !!!!! Duplicate handling is still kind of broken !!!!!
  /*
    When we end here another object with the same key was inserted. This may be due to populating
    all when we first just populated with folders. ATM we just remove the accidently inserted
    second data and reinsert the original data (which may be already used by now). The wrong data
    must be freed here later. 
    We should check the lock counter if we really dealing with wrong insertion here before freeing.
    This way we might use this func also for freeing when normally removing objects.  
   */
  /* Remove the wrong data */
  pValueRemove=(PFLDRGTREEVALUE)g_tree_lookup(pValue->pGTree, pValue->chrKey);
  g_tree_steal(pValue->pGTree, pValue->chrKey); /* This does not free the stuff in this func */
  /* Reinsert the old one */
  g_tree_insert(pValue->pGTree, pValue->chrKey, pValue);
}


NOM_Scope void NOMLINK impl_WPFolder_wpInitData(WPFolder* nomSelf, CORBA_Environment *ev)
{
  gulong ulErr;
  WPFolderData* nomThis=WPFolderGetData(nomSelf); 

  WPFolder_wpInitData_parent((WPObject*)nomSelf,  ev);

  /* Get our unique class ID. We need it for example when inserting menu items to
     specify the namespace. We query it here because getting a GQuark from a string
     is rather time consuming. The result is saved in a var for later use. */
  WPFolderNomId=nomIdFromString("WPFolder");//g_quark_from_string("WPFolder");

  nomPrintf("    Entering %s with nomSelf: 0x%x. nomSelf is: %s.\n",
            __FUNCTION__, nomSelf , nomSelf->mtab->nomClassName);
  _privFolderData=_wpAllocMem((WPObject*)nomSelf, sizeof(PRIVFOLDERDATA),&ulErr, NULLHANDLE);

  /* This balanced binary tree holds the objects in this folder. We create a tree
     which may be searched using the name of the file/directory */
  //_fldrObjects=g_tree_new((GCompareFunc)strcmp);
  _fldrObjects=g_tree_new_full((GCompareFunc)strcmp, nomSelf, NULL,
                               (GDestroyNotify) fldrCatchDuplicates);
}

NOM_Scope gulong NOMLINK impl_WPFolder_wpDrop(WPFolder* nomSelf,
                                              const gpointer containerHandle,
                                              const gpointer pDragInfo,
                                              CORBA_Environment *ev)
{
/* WPFolderData* nomThis=WPFolderGetData(nomSelf); */
  gulong ulNumItems, ulLoop;
  GdkDragAction gda;

  NOMDragInfo* nomDragInfo=(NOMDragInfo*)pDragInfo;

  g_message("%s", __FUNCTION__);

  if(!nomIsObj(nomDragInfo))
    return 0;

  ulNumItems=NOMDragInfo_queryNumDragItems(nomDragInfo, NULLHANDLE);

  /* No items? */
  if(0==ulNumItems)
    return 0;

  gda=NOMDragInfo_queryChosenDropAction(nomDragInfo, NULLHANDLE);

  for(ulLoop=0; ulLoop<ulNumItems; ulLoop++)
    {
      WPObject* wpObject;

      wpObject=NOMDragInfo_querySourceWPObject(nomDragInfo, ulLoop, NULLHANDLE);
      g_message("Source folder is: %s",
                NOMPath_queryCString(WPFolder_wpQueryFileName(wpObject, TRUE, NULLHANDLE),
                                     NULLHANDLE));
      wpObject=NOMDragInfo_queryWPObjectFromItemList(nomDragInfo, ulLoop, NULLHANDLE);
      g_message("Dropped object is: %s",
                NOMPath_queryCString(WPFolder_wpQueryFileName(wpObject, TRUE, NULLHANDLE),
                                     NULLHANDLE));

      g_message("%s: drop action: %d", __FUNCTION__, gda);

      wpObject=NOMDragInfo_queryWPObjectFromItemList(nomDragInfo, ulLoop, NULLHANDLE);
      switch(gda)
        {
        case GDK_ACTION_MOVE:
          {
            WPObject_wpMoveObject(wpObject, nomSelf, NULLHANDLE);
            break;
          }
        default:
          {
            WPObject_wpCopyObject(wpObject, nomSelf, FALSE, NULLHANDLE);
            break;
          }
        }
    }



#if 0
  WPFolder_wpDrop_parent(nomSelf,  ev);
#endif
  return 0;
}







