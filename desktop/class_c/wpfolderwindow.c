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
 */
#include <os2.h>

#include <nom.h>
#include <nomtk.h>

#include <string.h>
#include <gtk/gtk.h>
#include "nomguitk.h"
#include "nomwindow.h"
#include "desktoptypes.h"

#include "wpnotebook.h"
#include "wpfolderwindow.h"
#include "wpfolder.h"
#include "wpfolderwindow.ih"
#include "wpobject.h"

NOM_Scope PGtkWidget NOMLINK impl_WPFolderWindow_wpQueryContainerHandle(WPFolderWindow* nomSelf, CORBA_Environment *ev)
{
  WPFolderWindowData* nomThis=WPFolderWindowGetData(nomSelf);

  return (PGtkWidget) g_atomic_pointer_get(&_pgContainerHandle);
}

NOM_Scope void NOMLINK impl_WPFolderWindow_wpSetContainerHandle(WPFolderWindow* nomSelf, const PGtkWidget pgWidget,
                                                              CORBA_Environment *ev)
{
  WPFolderWindowData* nomThis=WPFolderWindowGetData(nomSelf);

  _pgContainerHandle=pgWidget;
}

/*
  Check if the right button click was within a certain time. That means
  the user released the button again within a short time period. On OS/2
  a context menu will display after the user released the button not when
  the user pressed it.
 */
#define CTXT_MENU_BUTTON_DELAY 250
static gboolean
fldr_checkContextButton(GdkEventButton *event)
{
  static guint guiTime=0;

  /* Right mouse button */
  if (event->button != 3)
    return FALSE;

  /* Ignore double-clicks and triple-clicks */
  if(event->type == GDK_BUTTON_PRESS)
    guiTime=event->time;

  if(event->type == GDK_BUTTON_RELEASE)
    {
      if(event->time-guiTime<CTXT_MENU_BUTTON_DELAY)
        return TRUE;
    }

  return FALSE;
}

static gboolean
fldr_handleButtonEvent (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  if(fldr_checkContextButton(event))
    {
      PWPFolderWindow pWindow;
      GtkTreePath* treePath;

      DosBeep(5000, 100);
      pWindow=(WPFolderWindow*)user_data;

      treePath=gtk_icon_view_get_path_at_pos(GTK_ICON_VIEW(widget), event->x, event->y );
      if(NULL==treePath)
        {
          /* Click on white space */
          WPFolder* wpFolder;
          wpFolder=(WPFolder*)WPFolderWindow_wpQueryWPObject(pWindow, NULLHANDLE);
          g_message("%s: %s", __FUNCTION__, wpFolder->mtab->nomClassName);
          WPObject_wpDisplayMenu((WPObject*)wpFolder, pWindow, NULL, 0, 0, NULL);
        }
      else
        {
          GtkTreeIter iter;
          GtkTreeModel* model;
          WPObject *wpObject;
          /* Click on an icon */

          g_message("%s: %s", __FUNCTION__, gtk_tree_path_to_string(treePath));

          model=gtk_icon_view_get_model(GTK_ICON_VIEW(widget));
          g_message("%s: model: %x", __FUNCTION__, (UINT)model);

          gtk_tree_model_get_iter(model , &iter, treePath);

          gtk_tree_model_get(model, &iter,
                             0, &wpObject,
                             -1);
          g_message("%s: %s", __FUNCTION__, wpObject->mtab->nomClassName);
          WPObject_wpDisplayMenu((WPObject*)wpObject, pWindow, NULL, 0, 0, NULL);
        }
    }
  return FALSE;
}

/* orbit-idl-c-stubs.c, cs_output_stub line 347 */
NOM_Scope void NOMLINK impl_WPFolderWindow_nomInit(WPFolderWindow* nomSelf, CORBA_Environment *ev)
{
  GtkWidget* window;
  GtkWidget *vbox;
  GtkWidget *sw;
  GtkWidget *icon_view;
  GtkWidget *tool_bar;
  GtkToolItem *up_button;
  //  GtkWidget *menuBar, *file;
  //  WPFolderWindowData* nomThis=WPFolderWindowGetData(nomSelf);

  /* Let parents initialize */
  WPFolderWindow_nomInit_parent((NOMObject*)nomSelf, NULLHANDLE);

  /* Create a default (hidden) folder window */

  /* Folder toplevel window. */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  /* Set title */
  //  gtk_window_set_title (GTK_WINDOW (window), "");

  /* FIXME: Set default size of folder frame. Will later use a stored value */
  gtk_window_set_default_size (GTK_WINDOW (window), 650, 400);

  /* Make sure we have a reference to the class so the garbage collector
     doesn't unload us */
  g_object_set_data(G_OBJECT(window), NOMOBJECT_KEY_STRING, nomSelf);

#if 0
  menuBar=gtk_menu_bar_new();
  file=gtk_menu_item_new_with_label("File");
  //gtk_signal_connect(GTK_OBJECT(file), "destroy", GTK_SIGNAL_FUNC(tst), NULL);

  gtk_container_add(GTK_CONTAINER(menuBar), file);
  gtk_widget_show(menuBar);
  gtk_widget_show(file);
#endif

  vbox = gtk_vbox_new (FALSE, 0);

  gtk_container_add (GTK_CONTAINER (window), vbox);

  /* Create and pack the toolbar */
  tool_bar = gtk_toolbar_new ();
  
  //  gtk_box_pack_start (GTK_BOX (vbox), menuBar, FALSE, FALSE, 0); /* Don't expand the toolbar vertically if sized */
  gtk_box_pack_start (GTK_BOX (vbox), tool_bar, FALSE, FALSE, 0); /* Don't expand the toolbar vertically if sized */
  
  /* Parent button */
  up_button = gtk_tool_button_new_from_stock (GTK_STOCK_GO_UP);
  gtk_tool_item_set_is_important (up_button, TRUE);
  /* Disable button */
  gtk_widget_set_sensitive (GTK_WIDGET (up_button), FALSE);
  /* Put it into the toolbar */
  gtk_toolbar_insert (GTK_TOOLBAR (tool_bar), up_button, -1);
  
  sw = gtk_scrolled_window_new (NULL, NULL);
  /* Drawing style */
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                       GTK_SHADOW_ETCHED_IN);
  /* Show scrollbars only if necessary */
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  /* Pack it into the vbox with size adjusting to the vbox */
  gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
  /* Create an icon view without model */
  icon_view = gtk_icon_view_new ();
  WPFolderWindow_wpSetContainerHandle(nomSelf, icon_view, NULLHANDLE);
                                     
  /* Allow multiple selection in icon view */
  gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (icon_view),
                                    GTK_SELECTION_MULTIPLE);
#if 0
  /* Connect to the "item_activated" signal */
  g_signal_connect (icon_view, "item-activated",
                    G_CALLBACK (itemActivated), nomSelf);
#endif
  //#endif

  /* This is for kb binding only */
#if 0
  g_signal_connect (GTK_WIDGET(icon_view), "popup-menu",
                    G_CALLBACK (fldr_cbPopupMenu), nomSelf);
#endif

  /* Handle mouse buttons */
  g_signal_connect (GTK_WIDGET(icon_view), "button-press-event",
                    G_CALLBACK (fldr_handleButtonEvent), nomSelf);
  g_signal_connect (GTK_WIDGET(icon_view), "button-release-event",
                    G_CALLBACK (fldr_handleButtonEvent), nomSelf);

#if 0
  /* Connect to the "clicked" signal of the "Up" tool button */
  g_signal_connect (up_button, "clicked",
                    G_CALLBACK (up_clicked), store);
#endif
  /* Add icon view as child to the scroll window created earlier */
  gtk_container_add (GTK_CONTAINER (sw), icon_view);

  gtk_widget_grab_focus (icon_view);

  //  g_signal_connect (GTK_WIDGET(window), "size-request",
  //                G_CALLBACK (handleEvent), nomSelf);

  WPFolderWindow_setWindowHandle(nomSelf, window, NULLHANDLE);
  /* Window is hidden here and must be shown by the caller */
}

