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

#include <gtk/gtk.h> 

static GtkWidget * dbgWindow;

enum
{
  COLUMN_ONE,
  NUM_COLUMNS
};

static void add_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  /* column for description */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Message",
						     renderer,
						     "text",
						     COLUMN_ONE,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COLUMN_ONE);
  gtk_tree_view_append_column (treeview, column);
}

static GtkWidget* createDbgWindow(GtkWidget* dbgWindow, GtkTreeModel **model)
{
  if (!dbgWindow)
    {
      GtkWidget* window=dbgWindow;
      GtkWidget *vbox;
      GtkWidget *label;
      GtkWidget *sw;
      GtkWidget *treeview;
      /* create window, etc */
      dbgWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      window=dbgWindow;
      /*      gtk_window_set_screen (GTK_WINDOW (dbgWindow),
              gtk_widget_get_screen (do_widget)); */
      gtk_window_set_title (GTK_WINDOW (dbgWindow), "Debug output");
#if 0
      g_signal_connect (window, "destroy",
			G_CALLBACK (gtk_widget_destroyed), &window);
#endif

      gtk_container_set_border_width (GTK_CONTAINER (dbgWindow), 8);
      vbox = gtk_vbox_new (FALSE, 8);
      gtk_container_add (GTK_CONTAINER (window), vbox);
      label = gtk_label_new ("Voyager Desktop");
      gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
      sw = gtk_scrolled_window_new (NULL, NULL);
      gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
					   GTK_SHADOW_ETCHED_IN);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
				      GTK_POLICY_NEVER,
				      GTK_POLICY_AUTOMATIC);
      gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
      /* create tree view */
      treeview = gtk_tree_view_new_with_model (*model);
      gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

      gtk_container_add (GTK_CONTAINER (sw), treeview);

      /* add columns to the tree view */
      add_columns (GTK_TREE_VIEW (treeview));

      /* finish & show */
      gtk_window_set_default_size (GTK_WINDOW (window), 380, 250);

    }
  if (!GTK_WIDGET_VISIBLE (dbgWindow))
    gtk_widget_show_all (dbgWindow);
  else
    {
      gtk_widget_destroy (dbgWindow);
      dbgWindow = NULL;
    }
  return dbgWindow;
}

void dbgPrintf(const gchar* gchrFormat, ...)
{
  static GtkListStore *store=NULL;
  GtkTreeIter iter;
  gchar *gchrTemp;
  va_list arg_ptr;

  if(store==NULL)
    {
      /* Create a list store and model holding the messages which will be
       connected with a GtkTreeView */
      GtkTreeModel *model;
      store = gtk_list_store_new (NUM_COLUMNS,
                                  G_TYPE_STRING);
      /* create tree model */
      model=GTK_TREE_MODEL (store);
      dbgWindow=createDbgWindow( dbgWindow, &model);
    }
  /* add data to the list store */
  va_start (arg_ptr, gchrFormat);
  gchrTemp=g_strdup_vprintf(gchrFormat, arg_ptr);
  va_end (arg_ptr);
  gtk_list_store_append (store, &iter);
  if(gchrTemp)
    {
      gtk_list_store_set (store, &iter,
                          COLUMN_ONE, gchrTemp,
                          -1);
      g_free(gchrTemp);
    }
  else
    {
      gtk_list_store_set (store, &iter,
                          COLUMN_ONE, "Out of memory!",
                          -1);
    }
}
