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
#include <os2.h>

#include "nom.h"
#include <nomtk.h>

#include <string.h>
#include "gtk/gtk.h"

#warning !!!!! nomIsObj() must be globaly defined !!!!!
#define nomIsObj(a) ((a)!= 0)

typedef struct _PRIVFOLDERDATA
{
  GtkListStore *gstoreFldContents;
  GtkWidget    *gtkIconView;
}PRIVFOLDERDATA, *PPRIVFOLDERDATA;

/* Gui stuff */
#include "nomguitk.h"
#include "nomfolderwindow.h"

#include "wpfolder.ih"
#include "wpdatafile.h"
#include "desktop.h"
#include "helper.h"
#include "desktoptypes.h"


/* Enum for the folder store */
enum
{
  COL_OBJECT_PTR,
  COL_PATH,
  COL_DISPLAY_NAME,
  COL_PIXBUF,
  COL_IS_DIRECTORY,
  NUM_COLS
};

static GtkListStore * fldr_CreateStore (void)
{
  GtkListStore *store;

  store = gtk_list_store_new (NUM_COLS,
                              G_TYPE_POINTER,
                              G_TYPE_STRING, 
                              G_TYPE_STRING, 
                              GDK_TYPE_PIXBUF,
                              G_TYPE_BOOLEAN);
  g_message("%s: store: %x", __FUNCTION__, store);
  return store;
}

static BOOL
fldr_fillStore (GtkListStore *store, const gchar* gchrPath)
{

  GDir *gDir;
  const gchar *gchrCurrentEntry;
  GtkTreeIter iter;

  nomPrintf("In %s:  %s\n", __FUNCTION__, gchrPath);          

  /* First clear the store */
  gtk_list_store_clear (store);

  /* Now go through the directory and extract all the file
   * information */
  gDir = g_dir_open (gchrPath, 0, NULL);
  if (!gDir)
    return FALSE;

  while ((gchrCurrentEntry = g_dir_read_name (gDir))!= NULL)
    {
      gchar *path, *display_name;
      gboolean is_dir;

      if (gchrCurrentEntry[0] != '\0')
        {
          path = g_build_filename (gchrPath, gchrCurrentEntry, NULL);
          nomPrintf("  %s\n", path);          

          display_name = g_filename_to_utf8 (gchrCurrentEntry, -1, NULL, NULL, NULL);
          if(g_file_test (path, G_FILE_TEST_IS_DIR))
            {
              /* It's a folder */
              WPFolder* wpFolder;

              wpFolder=WPFolderNew();
              if(nomIsObj(wpFolder))
                {
                  gtk_list_store_append (store, &iter);

                  gtk_list_store_set (store, &iter,
                                      COL_OBJECT_PTR, wpFolder,
                                      COL_PATH, path,
                                      COL_DISPLAY_NAME, display_name,
                                      COL_IS_DIRECTORY, is_dir,
                                      COL_PIXBUF, _wpQueryIcon(wpFolder, NULLHANDLE), //folder_pixbuf ,
                                      -1);

                }/* if(nomIsObj(wpFolder)) */
            }/* if(g_file_test (path, G_FILE_TEST_IS_DIR)) */
          else
            {
              /* It's a file */
              WPDataFile* wpDataFile;

              wpDataFile=WPDataFileNew();

              if(nomIsObj(wpDataFile))
                {
                  gtk_list_store_append (store, &iter);

#warning !!!! some problems with icon handling here !!!!
                  nomPrintf("Icon ptr: %x\n", _wpQueryIcon((WPObject*)wpDataFile, NULLHANDLE));
                  gtk_list_store_set (store, &iter,
                                      COL_OBJECT_PTR, wpDataFile,
                                      COL_PATH, path,
                                      COL_DISPLAY_NAME, display_name,
                                      COL_IS_DIRECTORY, is_dir,
                                      COL_PIXBUF, _wpQueryIcon((WPObject*)wpDataFile, NULLHANDLE), //file_pixbuf,
                                      -1);
                }
            }
          g_free (path);
          g_free (display_name);
        }/* if (gchrCurrentEntry[0] != '\0') */
    }/* while */

  g_dir_close(gDir);

  return TRUE;
}

/* pszPath contains the fully qualified path (checked with WPS) */
NOM_Scope CORBA_boolean NOMLINK impl_WPFolder_wpPopulate(WPFolder* nomSelf, const CORBA_unsigned_long ulReserved,
                                                         const CORBA_char * pszPath, const CORBA_boolean fFoldersOnly,
                                                         CORBA_Environment *ev)
{
 WPFolderData* nomThis=WPFolderGetData(nomSelf); 
  GtkListStore* gStore;
  PPRIVFOLDERDATA priv;

  nomPrintf("    Entering %s with nomSelf: 0x%x. nomSelf is: %s. Path is %s\n",
            __FUNCTION__, nomSelf , nomSelf->mtab->nomClassName, pszPath);

  g_return_val_if_fail(_privFolderData!=NULLHANDLE, FALSE);      /* Huh! What happened in wpInitData()? */
  g_log("WPFolder", G_LOG_LEVEL_DEBUG, "%s: Populating %s\n", __FUNCTION__, pszPath);


#if 0
  /* Already populated? */
  if(fFoldersOnly && 
     (_wpQueryFldrFlags(somSelf) & (FOI_POPULATEDWITHFOLDERS | FOI_POPULATEDWITHALL)))
    return TRUE;
  else if(_wpQueryFldrFlags(somSelf) & FOI_POPULATEDWITHALL)
    return TRUE;
#endif

  priv=_privFolderData;

  if(!priv->gstoreFldContents)
    {
      /* Create a store holding the folder contents */
      gStore=fldr_CreateStore();
      g_return_val_if_fail(gStore!=NULLHANDLE, FALSE);
      priv->gstoreFldContents=gStore;
    }

  /* Fill our store */
  if(gStore)
    fldr_fillStore(gStore, pszPath);
  else
    return FALSE;

  gtk_icon_view_set_model(GTK_ICON_VIEW (priv->gtkIconView), GTK_TREE_MODEL (priv->gstoreFldContents));

  /* We now set which model columns that correspond to the text
   * and pixbuf of each item
   */
  gtk_icon_view_set_text_column (GTK_ICON_VIEW (priv->gtkIconView), COL_DISPLAY_NAME);
  gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (priv->gtkIconView), COL_PIXBUF);
  gtk_icon_view_set_item_width (GTK_ICON_VIEW (priv->gtkIconView),
                                100);

  g_object_unref (gStore);

  return FALSE;
}

NOM_Scope gpointer NOMLINK impl_WPFolder_wpOpen(WPFolder* nomSelf, const PNOMFolderWindow nomFolder,
                                                const gulong ulView, const gpointer pParam,
                                                CORBA_Environment *ev)
{
/* WPFolderData* nomThis=WPFolderGetData(nomSelf); */

  switch(ulView)
    {
    case OPEN_CONTENTS:
    case OPEN_DEFAULT:
      {
        static NOMFolderWindow * nomFldrWindow;

#warning !!!!! Folder window must be inserted into inuse list !!!!!
        nomFldrWindow=WPFolder_wpCreateFolderWindow(nomSelf, ev);
        NOMFolderWindow_setWPFolderObject(nomFldrWindow, nomSelf, ev);

#warning !!!!! Path taken from a test location !!!!!
        WPFolder_wpPopulate(nomSelf, 0L, NOMPath_getCString(WPFolder_wpQueryRealName(nomSelf, TRUE, ev) , ev), FALSE,  ev);
        break;

#if 0
        char path[CCHMAXPATH];
        ULONG ulSize;
        
        /* Get full path of folder */
        ulSize=sizeof(path);
        
        if(!_wpQueryRealName(somSelf, path, &ulSize, TRUE))
          return NULLHANDLE; /* Error */
        
        nomPrintf("%s: opening %s\n", __FUNCTION__, path);
        
        /* Create folder window */
        hwndFolder=fldr_createFolderWindow(somSelf, path);
        
        if(!hwndFolder)
          return NULLHANDLE;

        /* populate the folder */
        _wpPopulate(somSelf, 0, path, FALSE); /* Contents or details. Tree isn't supported yet */

        break;
#endif
      }/* default */
    default:
      break;
    }/* switch */

  return WPFolder_wpOpen_parent(nomSelf, nomFolder, ulView, pParam, ev);
}

NOM_Scope gpointer NOMLINK impl_WPFolder_wpQueryIcon(WPFolder* nomSelf, CORBA_Environment *ev)
{
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
}

/*
  This method creates the folder window it doesn't query any files or creates
  models and stuff.
*/
NOM_Scope PNOMFolderWindow NOMLINK impl_WPFolder_wpCreateFolderWindow(WPFolder* nomSelf, CORBA_Environment *ev)
{
  NOMFolderWindow * nomFldrWindow;
  PPRIVFOLDERDATA priv;
  WPFolderData *nomThis = WPFolderGetData(nomSelf);

  priv=(PPRIVFOLDERDATA)_privFolderData;

  nomFldrWindow=NOMFolderWindowNew();

#warning !!!!! This is only for testing !!!!!
  priv->gtkIconView=NOMFolderWindow_queryContainerHandle(nomFldrWindow, ev);

  /* Show the new window */
  NOMFolderWindow_show(nomFldrWindow, ev);

  return nomFldrWindow;
}

NOM_Scope void NOMLINK impl_WPFolder_wpInitData(WPFolder* nomSelf, CORBA_Environment *ev)
{
  gulong ulErr;
  WPFolderData* nomThis=WPFolderGetData(nomSelf); 

  WPFolder_wpInitData_parent((WPObject*)nomSelf,  ev);

  nomPrintf("    Entering %s with nomSelf: 0x%x. nomSelf is: %s.\n",
            __FUNCTION__, nomSelf , nomSelf->mtab->nomClassName);
  _privFolderData=_wpAllocMem((WPObject*)nomSelf, sizeof(PRIVFOLDERDATA), (CORBA_unsigned_long*)&ulErr, NULLHANDLE);
}



