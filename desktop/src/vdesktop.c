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

#define INCL_DOSPROCESS
#define INCL_DOS
#define INCL_DOSPROFILE
#define INCL_DOSERRORS
#define INCL_PM

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

#include "nomfolderwindow.h"
#include "wpobject.h"
#include "wpfolder.h"
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
  GtkWidget* window;
  int a;
  ULONG pMem;
  HREGDLL hReg=NULLHANDLE;
  
  /* Desktop folder */
  WPFolder *wpDesktop;
  WPFolder *wpTempFolder;
  PNOMPath nomPath;  
  PNOMPath np;

  hReg=nomBeginRegisterDLLWithGC();
  if(NULLHANDLE==hReg)
    return 1;
  
  /* Register DLLs with the garbage collector */
  g_assert(nomRegisterDLLByName(hReg, "GLIB2.DLL" ));
  g_assert(nomRegisterDLLByName(hReg, "GOBJECT2.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "GMODULE2.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "GDK2.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "GDKPIX2.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "GTK2.DLL" ));
  g_assert(nomRegisterDLLByName(hReg, "ATK.DLL" ));
  g_assert(nomRegisterDLLByName(hReg, "NOBJTK.DLL"));
  g_assert(nomRegisterDLLByName(hReg, "VDESKTOP.DLL"));
  /* Add Pango here? */

  nomEndRegisterDLLWithGC(hReg);

  g_message("We started...\n");

#if 0
  /* Initialize thread subsystem */
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
  pEnv=nomTkInit();

  if(!pEnv) {
    nomPrintf("Can't initialize NOM environment. Exit...\n");
    return(1); 
  }


  /* Init SOM */
  NOMClassMgrObject=nomEnvironmentNew();
  //dbgPrintf( "NOMClassMgrObject: %x", NOMClassMgrObject);

  nomPath=NOMPathNew();
  NOMPath_assignCString(nomPath, desktopDir, NULLHANDLE);
  /* Make sure there's no '/' at the end */
  nomPath=NOMPath_stripSeparator(nomPath, NULLHANDLE);

  /* Create root folder */
  wpRootFolder=WPFolderNew();
  WPFolder_tstSetFullPath(wpRootFolder, NOMPath_queryCString(NOMPath_queryRoot(nomPath, NULLHANDLE),NULLHANDLE),
                          NULLHANDLE);
  wpTempFolder=wpRootFolder;

  nomPath=NOMPath_erasePathBegin(nomPath, NULLHANDLE);

  /* Now create all folders up the chain */
  while(NOMPath_length(nomPath, NULLHANDLE)>0)
    {
      NOMPath* np;
      WPFolder* wpFolder;
      np=NOMPath_queryPathBegin(nomPath, NULLHANDLE);

      wpFolder=WPFolderNew();
      WPFolder_tstSetFullPath(wpFolder, NOMPath_queryCString(np,NULLHANDLE),
                              NULLHANDLE);
      WPFolder_wpSetFolder(wpFolder, wpTempFolder, NULLHANDLE);
      wpTempFolder=wpFolder;
      nomPath=NOMPath_erasePathBegin(nomPath, NULLHANDLE);
      WPFolder_wpQueryFileName(wpFolder, TRUE, NULLHANDLE);
      //g_message("   path: %s", NOMPath_queryCString(WPFolder_wpQueryFileName(wpFolder, TRUE, NULLHANDLE),
      //                                            NULLHANDLE));
    };

  g_message("   --> Desktop dir: %s", NOMPath_queryCString(WPFolder_wpQueryFileName(wpTempFolder, TRUE, NULLHANDLE),
                                                            NULLHANDLE));

  /* Create desktop folder */
  wpDesktop=wpTempFolder;//WPFolderNew();

  //WPFolder_tstSetFullPath(wpDesktop, desktopDir, NULLHANDLE);
  //WPFolder_tstSetFullPath(wpDesktop, "r:", NULLHANDLE);
  WPFolder_wpOpen(wpDesktop, NULL, OPEN_DEFAULT,  NULL, NULL);
  /*    WPFolder_wpPopulate(wpObject, 0,"blabla 2", 0,  NULL);  */
    
#if 0
  /* Folder toplevel window. */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  /* FIXME: Set default size of folder frame. Will later use a stored value */
  gtk_window_set_default_size (GTK_WINDOW (window), 650, 400);

  g_signal_connect (GTK_WIDGET(window), "size-request",
                    G_CALLBACK (handleEvent), NULL/*nomSelf*/);

  gtk_widget_show(window);
  g_message("Window handle: %x", window);
#endif

  /* All GTK applications must have a gtk_main(). Control ends here
   * and waits for an event to occur (like a key press or
   * mouse event). */
  gtk_main ();

    
#if 0
    mem=g_malloc(1250000);
    memset(mem, 0xaa, 10000);
    //  *pGlobalMemInExe=mem;
    for(a=0;a<50;a++){
      g_malloc(1250000);
      printf("%x %x %x %x %d\n", *mem, *(mem+1), *(mem+2), *(mem+3), 0);
      //printf("%x\n", *((ULONG*)pGlobalMemInExe));
    }
#endif

    printf("And now we quit...\n");

  return 0;
}


/*
  Just a test function for making noise...
 */
void _System theBeep(void)
{
  DosBeep(2500,400);
  DosBeep(500,200);
  DosBeep(1500,200);
  DosBeep(2500,400);
  DosBeep(1500,200);
  DosBeep(500,200);
  DosBeep(2500,400);
}
