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
* Portions created by the Initial Developer are Copyright (C) 2007
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

#ifndef NOM_NOMDragWindow_IMPLEMENTATION_FILE
#define NOM_NOMDragWindow_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#include <os2.h>

#include "nom.h"
#include <nomtk.h>

#include <glib.h>

#include <gtk/gtk.h>

#include "nomguitk.h"

#include "nomdragwindow.ih"


NOM_Scope void NOMLINK impl_NOMDragWindow_displayDragImage(NOMDragWindow* nomSelf,
                                                           CORBA_Environment *ev)
{
  NOMDragWindowData* nomThis=NOMDragWindowGetData(nomSelf);
  GtkWidget *wgtDrag;

  wgtDrag=NOMDragWindow_queryWindowHandle(nomSelf, NULLHANDLE);
  gdk_window_shape_combine_mask(wgtDrag->window, _ptrMaskBMPDrag, 0, 0);
  gdk_window_set_back_pixmap(wgtDrag->window, _ptrPixMapDrag, FALSE);
}


NOM_Scope void NOMLINK impl_NOMDragWindow_displayStopImage(NOMDragWindow* nomSelf,
                                                           CORBA_Environment *ev)
{
  NOMDragWindowData* nomThis=NOMDragWindowGetData(nomSelf);
  GtkWidget *wgtDrag;

  wgtDrag=NOMDragWindow_queryWindowHandle(nomSelf, NULLHANDLE);
  gdk_window_shape_combine_mask(wgtDrag->window, _ptrMaskBMPStop, 0, 0);
  gdk_window_set_back_pixmap(wgtDrag->window, _ptrPixMapStop, FALSE);
}


NOM_Scope void NOMLINK impl_NOMDragWindow_displayCustomImage(NOMDragWindow* nomSelf,
                                                             const PGdkPixmap ptrPixMap,
                                                             const PGdkBitmap ptrMaskBMP,
                                                             CORBA_Environment *ev)
{
  /* NOMDragWindowData* nomThis=NOMDragWindowGetData(nomSelf); */
  GtkWidget *wgtDrag;

  wgtDrag=NOMDragWindow_queryWindowHandle(nomSelf, NULLHANDLE);
  gdk_window_shape_combine_mask(wgtDrag->window, ptrMaskBMP, 0, 0);
  gdk_window_set_back_pixmap(wgtDrag->window, ptrPixMap, FALSE);
}


/*
  Callback which loads a pixmap into the drag widget as soon as it's realized.
 */
static void fldrWidgetRealize(GtkWidget * wgtDrag, gpointer ptrUserData)
{
  NOMDragWindow* ndw=(NOMDragWindow*)ptrUserData;

  _displayDragImage( ndw, NULLHANDLE);

  return;
}

/* orbit-idl-c-stubs.c, cs_output_stub line 347 */
NOM_Scope void NOMLINK impl_NOMDragWindow_nomInit(NOMDragWindow* nomSelf, CORBA_Environment *ev)
{
  NOMDragWindowData* nomThis=NOMDragWindowGetData(nomSelf);
  GtkWidget *wgtDrag;
  GdkPixbuf *pixBufDrag;

  NOMDragWindow_nomInit_parent(nomSelf, ev);

  wgtDrag=gtk_window_new(GTK_WINDOW_POPUP);

  /*
    FIXME:
    In GTK 2.8 use *_HINT_DND instead.
    */
  gtk_window_set_type_hint(GTK_WINDOW(wgtDrag), GDK_WINDOW_TYPE_HINT_MENU);
  gtk_widget_set_app_paintable(GTK_WIDGET(wgtDrag), TRUE);
  
  /* Get normal drag image */
  pixBufDrag=gtk_widget_render_icon(wgtDrag, GTK_STOCK_DND, GTK_ICON_SIZE_DND, NULL);
  gdk_pixbuf_render_pixmap_and_mask(pixBufDrag, &_ptrPixMapDrag, &_ptrMaskBMPDrag, 128);
  
  gtk_widget_set_size_request(wgtDrag, gdk_pixbuf_get_width(pixBufDrag),
                              gdk_pixbuf_get_height(pixBufDrag));
  g_object_unref(pixBufDrag);
  
  /* Get "stop" drag image */
  pixBufDrag=gtk_widget_render_icon(wgtDrag, GTK_STOCK_STOP, GTK_ICON_SIZE_DND, NULL);
  gdk_pixbuf_render_pixmap_and_mask(pixBufDrag, &_ptrPixMapStop, &_ptrMaskBMPStop, 128);
  g_object_unref(pixBufDrag);

  /* Make sure we have a reference to the class so the garbage collector
     doesn't unload us */
  g_object_set_data(G_OBJECT(wgtDrag), NOMOBJECT_KEY_STRING, nomSelf);

  g_signal_connect_after(GTK_WIDGET(wgtDrag), "realize", G_CALLBACK(fldrWidgetRealize),
                         (gpointer)nomSelf);

  NOMDragWindow_setWindowHandle(nomSelf, wgtDrag, NULLHANDLE);
}





