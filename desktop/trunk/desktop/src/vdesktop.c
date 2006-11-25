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

#define INCL_DOS
#define INCL_PM
#include <os2.h>
#include <stdio.h>
#include <gtk/gtk.h> 
#include "debug_window.h"

#include "nom.h"
#include "nomtk.h"
#include "wpobject.h"
#include "wpfolder.h"

int createQuitWindow(void);

PNOM_ENV pEnv;
NOMClassMgr *NOMClassMgrObject;


/* Desktop folder */
WPFolder *wpDesktop;


/*
  Main entry point. This function is called from the EMX wrapper. Be aware that gtk_init()
  is already called in the wrapper.
 */
int _System  main_loop()
{
 char desktopDir[CCHMAXPATH]={0};
 
 g_message("We started...\n");    


  /* Create a window with a 'quit' button to terminate us */
 createQuitWindow();


  /* Query current dir */
  g_strlcpy(desktopDir, g_get_current_dir(), sizeof(desktopDir));
  dbgPrintf("Desktop: %s", desktopDir);

    /*
      Bootstrap our objects...
     */
    pEnv=nomTkInit();
    //dbgPrintf( "nomTKinit returned: %x", pEnv);

    if(!pEnv) {
      nomPrintf("Can't initialize NOM environment. Exit...\n");
      return(1); 
    }

    /* Init SOM */
    NOMClassMgrObject=nomEnvironmentNew();
    //dbgPrintf( "NOMClassMgrObject: %x", NOMClassMgrObject);



    WPObject* wpObject;
    wpObject=WPObjectNew();
    //dbgPrintf( "wpObject: %x", wpObject);

    /* Create desktop folder */
    wpDesktop=WPFolderNew();
    dbgPrintf( "Created desktop object: %x", wpDesktop);

    WPFolder_wpPopulate(wpDesktop, 0, desktopDir, 0,  NULL);
    /*    WPFolder_wpPopulate(wpObject, 0,"blabla 2", 0,  NULL);  */

    
#if 0
    /* Base classes */
    wpObject=WPDataFileNew();

    somPrintf("\nNew wpObject: %x\n", wpObject);
    somPrintf("  -> Classname is: %s\n", _somGetClassName(wpObject));


    /* Test only!!! */
    _tstSetFullPath(wpDesktop, desktopDir);

    _wpSetFolder(wpDesktop, NULLHANDLE); /* FIXME: Instance var is zero anyway but this way I don't forget it ;-) */
    _wpOpen(wpDesktop, NULLHANDLE, OPEN_DEFAULT, 0); /* remove parameter later!!! */
    /*  _dbgPrintRegisteredClasses(SOMClassMgrObject);*/

#endif
    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main ();

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
