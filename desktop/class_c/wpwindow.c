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
 * And remember, phase 3 is near...
 */
#ifndef NOM_WPWindow_IMPLEMENTATION_FILE
#define NOM_WPWindow_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#include <os2.h>

#include <nom.h>
#include <nomtk.h>
#include <gtk/gtk.h>
#include <nomguitk.h>
#include "nomwindow.h"
#include "desktoptypes.h"

#include <string.h>

#include "wpwindow.ih"

NOM_Scope void NOMLINK impl_WPWindow_wpSetWPObject(WPWindow* nomSelf, const PWPObject wpObject, CORBA_Environment *ev)
{
  WPWindowData* nomThis=WPWindowGetData(nomSelf);
  _wpObject=wpObject;
}

NOM_Scope PWPObject NOMLINK impl_WPWindow_wpQueryWPObject(WPWindow* nomSelf, CORBA_Environment *ev)
{
  WPWindowData* nomThis=WPWindowGetData(nomSelf);

  return _wpObject;
}

NOM_Scope void NOMLINK impl_WPWindow_wpSetWindowTitle(WPWindow* nomSelf, const PNOMString newTitle,
                                                      CORBA_Environment *ev)
{

  gtk_window_set_title (GTK_WINDOW (WPWindow_queryWindowHandle(nomSelf, NULLHANDLE)),
                        NOMString_queryCString(newTitle, NULLHANDLE));

}

