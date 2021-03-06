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
#ifndef NOM_WPFileSystem_IMPLEMENTATION_FILE
#define NOM_WPFileSystem_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#include <os2.h>

#include "nom.h"
#include "nomtk.h"

#include <string.h>
#include "gtk/gtk.h"
/* Gui stuff */
#include "nomguitk.h"
#include "nomwindow.h"
#include "desktoptypes.h"

#include "wpfolderwindow.h"
#include "wpnotebook.h"
#include "wpfolder.h"
#include "m_wpfilesystem.h"
#include "wpfilesystem.ih"

NOM_Scope void NOMLINK impl_WPFileSystem_tstSetFullPath(WPFileSystem* nomSelf, const CORBA_char * fullPath,
                                                        CORBA_Environment *ev)
{
  WPFileSystemData* nomThis=WPFileSystemGetData(nomSelf);

  nomPrintf("thePath: ---> %s \n", fullPath);
  _pszFullPath=g_strdup(fullPath);
}

NOM_Scope PNOMPath NOMLINK impl_WPFileSystem_wpQueryFileName(WPFileSystem* nomSelf, const CORBA_boolean bFullPath,
                                                             CORBA_Environment *ev)
{
  WPFileSystemData* nomThis=WPFileSystemGetData(nomSelf);
  PNOMPath nomRetval;
  PNOMPath nomPath;
  PWPFolder wpParent;

  if(!bFullPath){
    nomRetval=NOMPathNew();
    return (PNOMPath) NOMPath_assignCString(nomRetval, _pszFullPath, ev);
  }

  /* we are interested in the full path */

  /* Get parent folder */
  wpParent=WPFileSystem_wpQueryFolder(nomSelf, NULLHANDLE);

  if(NULLHANDLE==wpParent)
    {
      /* We are root folder */
      nomRetval=NOMPathNew();
      return (PNOMPath) NOMPath_assignCString(nomRetval, _pszFullPath, ev);
    }

  /* We are a folder somwhere in the chain */
  nomRetval=WPFileSystem_wpQueryFileName((WPFileSystem*)wpParent, bFullPath, NULLHANDLE);

  nomPath=NOMPathNew();
  nomPath= (PNOMPath) NOMPath_assignCString(nomPath, _pszFullPath, ev);

  return (PNOMPath)NOMPath_appendPath(nomRetval, nomPath, NULLHANDLE);
}


NOM_Scope CORBA_boolean NOMLINK impl_WPFileSystem_wpMoveObject(WPFileSystem* nomSelf,
                                                               const PWPFolder wpTargetFolder,
                                                               CORBA_Environment *ev)
{
/* WPFileSystemData* nomThis=WPFileSystemGetData(nomSelf); */
  g_message("  %s", __FUNCTION__);
  return WPFileSystem_wpMoveObject_parent(nomSelf, wpTargetFolder,  ev);
  return FALSE;
}


NOM_Scope PWPObject NOMLINK impl_WPFileSystem_wpCopyObject(WPFileSystem* nomSelf,
                                                           const PWPFolder wpTargetFolder,
                                                           const CORBA_boolean fLock,
                                                           CORBA_Environment *ev)
{
/* WPFileSystemData* nomThis=WPFileSystemGetData(nomSelf); */

  return WPFileSystem_wpCopyObject_parent(nomSelf, wpTargetFolder, fLock, ev);

  return NULLHANDLE;
}

