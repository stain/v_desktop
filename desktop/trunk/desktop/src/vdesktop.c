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
#define INCL_DOSPROCESS
#define INCL_DOS
#define INCL_DOSPROFILE
#define INCL_DOSERRORS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h> 
#include "debug_window.h"
#include <gc.h>

#include "nom.h"
#include "nomtk.h"
#include "nomgc.h"
#include "nomguitk.h"
#include "nomwindow.h"
#include "desktoptypes.h"

#include "wpfolderwindow.h"
#include "wpnotebook.h"
#include "m_wpfolder.h"
#include "wpfolder.h"
#include "m_wprootfolder.h"
#include "wprootfolder.h"
#include "wpobject.h"

#include "nomfilepath.h"


int createQuitWindow(void);

static gboolean
handleEvent (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{

  DosBeep(5000, 100);
  //g_malloc(1250000);
  //g_message("%s: allocated...", __FUNCTION__);
  //WPObject_wpDisplayMenu(wpFolder, pWindow, NULL, 0, 0, NULL);
  
  return FALSE;
}

WPFolder* wpRootFolder;

/*
  Main entry point. This function is called from the EMX wrapper. Be aware that gtk_init()
  is already called in the wrapper.
 */
int _System  main_loop()
{
  char desktopDir[CCHMAXPATH]={0};
  PNOM_ENV pEnv;
  NOMClassMgr *NOMClassMgrObject;
  HREGDLL hReg=NULLHANDLE;
  
  /* Desktop folder */
  WPFolder *wpDesktop;
  WPFolder *wpTempFolder;
  PNOMPath nomPath;  
  PNOMPath np;
  gchar *chrDisplayName;

  /* Register DLLs with the garbage collector */
  hReg=nomBeginRegisterDLLWithGC();
  if(NULLHANDLE==hReg)
    return 1;
  
  g_assert(nomRegisterDLLByName(hReg, "GLIB2.DLL" ));
  g_assert(nomRegisterDLLByName(hReg, "GOBJECT2.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "GMODULE2.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "GDK2.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "GDKPIX2.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "GTK2.DLL" ));
  g_assert(nomRegisterDLLByName(hReg, "ATK.DLL" ));
  g_assert(nomRegisterDLLByName(hReg, "NOBJTK.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "VDESKTOP.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "VOYFCLS.DLL"));
  //g_assert(nomRegisterDLLByName(hReg, "VOYWP.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "VOYGUITK.DLL"));
  //  g_assert(nomRegisterDLLByName(hReg, "PBL-PNG.DLL"));
  //  g_assert(nomRegisterDLLByName(hReg, "BASIC-FC.DLL"));
  /* Add Pango */
  g_assert(nomRegisterDLLByName(hReg, "PANGO.DLL"));
  nomEndRegisterDLLWithGC(hReg);

  g_message("We started...\n");

#if 0
  /* Initialize thread subsystem. This needs a multithreaded glib */
  if(!g_thread_supported())
    g_thread_init(NULL);
#endif
  
  /* Create a window with a 'quit' button to terminate us */
  createQuitWindow();
    
  /* Query current dir */
  g_strlcpy(desktopDir, g_get_current_dir(), sizeof(desktopDir));
  g_message("Desktop: %s", desktopDir);

  /*
    Bootstrap our objects...
  */
#if 0
  pEnv=nomTkInit();

  if(!pEnv) {
    nomPrintf("Can't initialize NOM environment. Exit...\n");
    return(1); 
  }
#endif

  /* Init NOM */
  NOMClassMgrObject=nomEnvironmentNew();
  //dbgPrintf( "NOMClassMgrObject: %x", NOMClassMgrObject);

  /* Desktop directory oath */
  nomPath=NOMPathNew();
  NOMPath_assignCString(nomPath, desktopDir, NULLHANDLE);
  /* Make sure there's no '/' at the end */
  nomPath=NOMPath_stripSeparator(nomPath, NULLHANDLE);

  /* Create root folder */
  np=NOMPath_queryPathBegin(nomPath, NULLHANDLE);
  wpRootFolder=WPRootFolderNew();
  WPRootFolder_wpLockObject(wpRootFolder, NULLHANDLE);
  WPRootFolder_tstSetFullPath(wpRootFolder, NOMPath_queryCString(NOMPath_queryRoot(np, NULLHANDLE),NULLHANDLE),
                          NULLHANDLE);
  chrDisplayName = g_filename_to_utf8 (NOMPath_queryCString(np,NULLHANDLE), -1, NULL, NULL, NULL);
  WPRootFolder_wpSetTitleFromCString((WPObject*)wpRootFolder, chrDisplayName, NULLHANDLE);

  wpTempFolder=wpRootFolder;
  nomPath=NOMPath_erasePathBegin(nomPath, NULLHANDLE);

  /* Now create all folders up the chain */
  while(NOMPath_length(nomPath, NULLHANDLE)>0)
    {
      WPFolder* wpFolder;

      np=NOMPath_queryPathBegin(nomPath, NULLHANDLE);

      wpFolder=WPFolderNew();
      WPFolder_wpLockObject(wpFolder, NULLHANDLE);
      WPFolder_tstSetFullPath(wpFolder, NOMPath_queryCString(np,NULLHANDLE),
                              NULLHANDLE);
      chrDisplayName = g_filename_to_utf8 (NOMPath_queryCString(np,NULLHANDLE), -1, NULL, NULL, NULL);
      WPFolder_wpSetTitleFromCString((WPObject*)wpFolder, chrDisplayName, NULLHANDLE);
      WPFolder_wpSetFolder(wpFolder, wpTempFolder, NULLHANDLE);
      
      /* insert into contents list */
      WPFolder_wpAddToContent(wpTempFolder, (WPObject*) wpFolder, 
                              NULLHANDLE);
      
      wpTempFolder=wpFolder;
      /* Move to next path part */
      nomPath=NOMPath_erasePathBegin(nomPath, NULLHANDLE);
      //WPFolder_wpQueryFileName(wpFolder, TRUE, NULLHANDLE);
      //g_message("   path: %s", NOMPath_queryCString(WPFolder_wpQueryFileName(wpFolder, TRUE, NULLHANDLE),
      //                                            NULLHANDLE));
    };

  g_message("   --> Desktop dir: %s", NOMPath_queryCString(WPFolder_wpQueryFileName(wpTempFolder, TRUE, NULLHANDLE),
                                                            NULLHANDLE));

  /* Create desktop folder */
  wpDesktop=wpTempFolder;//WPFolderNew();

  //WPFolder_tstSetFullPath(wpDesktop, desktopDir, NULLHANDLE);
  //WPFolder_tstSetFullPath(wpDesktop, "r:", NULLHANDLE);
  g_message("WPFolder ID: %d", nomIdFromString("WPFolder"));
  WPFolder_wpOpen(wpDesktop, NULL, OPEN_DEFAULT, 0, NULL, NULL);

  /* All GTK applications must have a gtk_main(). Control ends here
   * and waits for an event to occur (like a key press or
   * mouse event). */
  gtk_main ();

    
#warning !!!!! A call to NOMTerminate() or alike is missing 
    printf("And now we quit...\n");

  return 0;
}
