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
#include <os2.h>

#include <nom.h>
#include <nomtk.h>

#include <string.h>
#include <gtk/gtk.h>
#include <nomguitk.h>
#include "nomwindow.h"
#include "desktoptypes.h"

#include "nomfolderwindow.h"
#include "wpnotebook.ih"

NOM_Scope PNOMNoteBook NOMLINK impl_WPNoteBook_wpQueryNoteBookObject(WPNoteBook* nomSelf, CORBA_Environment *ev)
{
  WPNoteBookData* nomThis=WPNoteBookGetData(nomSelf);

  return _nomNoteBook;
}

NOM_Scope void NOMLINK impl_WPNoteBook_wpSetNoteBookObject(WPNoteBook* nomSelf, const PNOMNoteBook nomNBook,
                                                           CORBA_Environment *ev)
{
  WPNoteBookData* nomThis=WPNoteBookGetData(nomSelf);

  _nomNoteBook=nomNBook;
}


NOM_Scope void NOMLINK impl_WPNoteBook_nomInit(WPNoteBook* nomSelf, CORBA_Environment *ev)
{
  /* WPNoteBookData* nomThis=WPNoteBookGetData(nomSelf);  */
  GtkWidget* gtkWindow;
  NOMNoteBook* nomNoteBook;

  WPNoteBook_nomInit_parent((NOMObject*)nomSelf,  ev);

  /* Create the toplevel window */
  gtkWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(gtkWindow), 1);
  /* Make sure we have a reference to the class so the garbage collector
     doesn't unload us */
  g_object_set_data(G_OBJECT(gtkWindow), NOMOBJECT_KEY_STRING, nomSelf);
  WPNoteBook_setWindowHandle(nomSelf, gtkWindow, ev);

  /* Create the notebook */
  nomNoteBook=NOMNoteBookNew();
  _wpSetNoteBookObject(nomSelf, nomNoteBook, ev);

  gtk_container_add(GTK_CONTAINER(gtkWindow), NOMNoteBook_queryWindowHandle(nomNoteBook, ev));

}

