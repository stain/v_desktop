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

*/
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>

int main( int   argc,
          char *argv[] )
{
  APIRET rc;
  UCHAR uchrError[256];
  HMODULE hModule;
  HMODULE hModuleGTK;
  HMODULE hModuleGC;
  PFN proc;

  /*
    Initialize the garbage collector.
   */
  if((rc=DosLoadModule(uchrError, sizeof(uchrError),"nobjtk.dll", &hModuleGC))!=NO_ERROR)
    {
      printf("DosLoadmodule for nobjtk.dll failed with rc=0x%x because of module %s.\n", (int)rc, uchrError);
      return 1;
    };
  fprintf(stderr, "DLL handle for nobjtk.dll is: 0x%x\n", (int)hModuleGC);
  if((rc = DosQueryProcAddr(hModuleGC,                  /* Handle to module           */
                            0L,                         /* No ProcName specified      */
                            "nomInitGarbageCollection",    /* ProcName (not specified)   */
                            &proc))!=NO_ERROR)          /* Address returned           0 */
    {
       fprintf(stderr, "DosQueryProcAddr for nomInitGarbageCollection() failed with rc=0x%x\n",(int) rc);
      return 1;
    }
  fprintf(stderr, "   Proc address for nomInitGarbageCollection() is: 0x%x\n", (int)proc);
  proc();

  /*
    Init GTK
   */
  if((rc=DosLoadModule(uchrError, sizeof(uchrError),"gtk2.dll", &hModuleGTK))!=NO_ERROR)
    {
      fprintf(stderr, "DosLoadmodule for gtk2.dll failed with rc=0x%x because of module %s.\n", (int)rc, uchrError);
      return 1;
    };
  fprintf(stderr, "DLL handle for gtk2.dll is: 0x%x\n", (int) hModuleGTK);
  if((rc = DosQueryProcAddr(hModuleGTK,                /* Handle to module           */
                            0L,                        /* No ProcName specified      */
                            "gtk_init",                /* ProcName (not specified)   */
                            &proc))!=NO_ERROR)         /* Address returned         0 */
    {
       fprintf(stderr, "DosQueryProcAddr for gtk_init() failed with rc=0x%x\n",(int) rc);
       DosFreeModule(hModuleGC);
      return 1;
    }
  fprintf(stderr, "   Proc address for gtk_init() is: 0x%x\n", (int)proc);
  proc(&argc, &argv);

  /*
    Calling into the shell dll.
   */
  if((rc=DosLoadModule(uchrError, sizeof(uchrError),"vdesktop.dll", &hModule))!=NO_ERROR)
    {
      printf("DosLoadmodule for vdesktop.dll failed with rc=0x%x because of module %s.\n", (int)rc, uchrError);
      DosFreeModule(hModule);
      return 1;
    };
  fprintf(stderr, "DLL handle for Vdesktop.dll is: 0x%x\n", (int)hModule);
  if((rc = DosQueryProcAddr(hModule,                /* Handle to module           */
                            0L,                     /* No ProcName specified      */
                            "main_loop"    ,        /* ProcName (not specified)   */
                            &proc))!=NO_ERROR)      /* Address returned           0 */
    {
       fprintf(stderr, "DosQueryProcAddr for main_loop() failed with rc=0x%x\n",(int) rc);
       DosFreeModule(hModuleGTK);
       DosFreeModule(hModuleGC);
      return 1;
    }
  fprintf(stderr, "   Proc address for main_loop() is: 0x%x\n", (int)proc);
  proc(&argc, &argv);

  DosFreeModule(hModule);

  /*
    Cleaning up GTK
   */
  DosFreeModule(hModuleGTK);

  DosFreeModule(hModuleGC);

  return 0; 
}






