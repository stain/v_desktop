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
* Portions created by the Initial Developer are Copyright (C) 2006
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

#define INCL_DOS
#include <os2.h>

#include <nom.h>
#include <nomtk.h>

#include <string.h>
#include <glib.h>

#include <gtk/gtk.h>

#include "nomguitk.h"
#include "nommenuitem.ih"


NOM_Scope PGtkWidget NOMLINK impl_NOMMenuItem_queryMenuItemHandle(NOMMenuItem* nomSelf, CORBA_Environment *ev)
{
  NOMMenuItemData* nomThis=NOMMenuItemGetData(nomSelf);

  return _pgMenuItemHandle;
}

NOM_Scope void NOMLINK impl_NOMMenuItem_setNameSpaceId(NOMMenuItem* nomSelf, const nomId nameSpaceId, CORBA_Environment *ev)
{
  NOMMenuItemData* nomThis=NOMMenuItemGetData(nomSelf);
  _nomIdNameSpace=nameSpaceId;
}

NOM_Scope nomId NOMLINK impl_NOMMenuItem_queryNameSpaceId(NOMMenuItem* nomSelf, CORBA_Environment *ev)
{
  NOMMenuItemData* nomThis=NOMMenuItemGetData(nomSelf);

  return _nomIdNameSpace;
}

NOM_Scope void NOMLINK impl_NOMMenuItem_setMenuItemId(NOMMenuItem* nomSelf, const CORBA_unsigned_long ulMenuItemId, CORBA_Environment *ev)
{
  NOMMenuItemData* nomThis=NOMMenuItemGetData(nomSelf); 
  _ulMenuItemId=ulMenuItemId;
}

NOM_Scope CORBA_unsigned_long NOMLINK impl_NOMMenuItem_queryMenuItemId(NOMMenuItem* nomSelf, CORBA_Environment *ev)
{
  NOMMenuItemData* nomThis=NOMMenuItemGetData(nomSelf);

  return _ulMenuItemId;
}


NOM_Scope PNOMMenuItem NOMLINK impl_NOMMenuItem_setupMenuItem(NOMMenuItem* nomSelf, const CORBA_char * chrLabel, 
                                                              const nomId nameSpaceId, const CORBA_unsigned_long ulMenuItemId,
                                                              CORBA_Environment *ev)
{
  GtkWidget *accWidget;

  NOMMenuItemData* nomThis=NOMMenuItemGetData(nomSelf); 

  NOMMenuItem_setNameSpaceId(nomSelf, nameSpaceId, ev);
  NOMMenuItem_setMenuItemId(nomSelf, ulMenuItemId, ev);

  /* Create the label with mnemonic if any */
  accWidget=g_object_new(GTK_TYPE_ACCEL_LABEL, NULL);
  gtk_label_set_text_with_mnemonic(GTK_LABEL(accWidget), chrLabel);
  gtk_misc_set_alignment(GTK_MISC(accWidget), 0.0, 0.5);
  gtk_container_add(GTK_CONTAINER(_pgMenuItemHandle), accWidget);
  gtk_accel_label_set_accel_widget(GTK_ACCEL_LABEL(accWidget), _pgMenuItemHandle);
  gtk_widget_show(accWidget);

  gtk_widget_show(_pgMenuItemHandle);
  return nomSelf;
}


/* orbit-idl-c-stubs.c, cs_output_stub line 347 */
NOM_Scope void NOMLINK impl_NOMMenuItem_nomInit(NOMMenuItem* nomSelf, CORBA_Environment *ev)
{
  NOMMenuItemData* nomThis=NOMMenuItemGetData(nomSelf);
  NOMMenuItem_nomInit_parent((NOMObject*)nomSelf,  ev);

  _pgMenuItemHandle=gtk_menu_item_new();

  /* Make sure we have a reference to the class so the garbage collector
     doesn't unload us */
  g_object_set_data(G_OBJECT(_pgMenuItemHandle), NOMOBJECT_KEY_STRING, nomSelf);
}






