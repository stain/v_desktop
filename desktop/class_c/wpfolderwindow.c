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
 * This file was generated by orbit-idl-2 for Voyager
 *
 */
#ifndef NOM_WPFolderWindow_IMPLEMENTATION_FILE
#define NOM_WPFolderWindow_IMPLEMENTATION_FILE
#endif

#define INCL_DOS

#define METHOD_DEBUG

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
#include "exception.h"

#include "nomdragwindow.h"
#include "nomdraginfo.h"

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
  

  TRY(FLDR_BUTTONEVENT) {
    if(fldr_checkContextButton(event))
      {
        PWPFolderWindow pWindow;
        GtkTreePath* treePath;

        pWindow=(WPFolderWindow*)user_data;
        //TST_OBJECT(pWindow);
        treePath=gtk_icon_view_get_path_at_pos(GTK_ICON_VIEW(widget), event->x, event->y );
        if(NULL==treePath)
          {
            /* Click on white space */
            WPFolder* wpFolder;
            wpFolder=(WPFolder*)WPFolderWindow_wpQueryWPObject(pWindow, NULLHANDLE);
            TST_OBJECT(wpFolder);
            
            if(nomIsObj(wpFolder))
              WPObject_wpDisplayMenu((WPObject*)wpFolder, pWindow, NULL, MENU_OPENVIEWPOPUP, 0, NULL);
            else
              g_message("%d %s: WPFolder object not valid!", __LINE__, __FUNCTION__);
          }
        else
          {
            GtkTreeIter iter;
            GtkTreeModel* model;
            WPObject *wpObject;
            /* Click on an icon */
            
            //g_message("%s: %s", __FUNCTION__, gtk_tree_path_to_string(treePath));
            
            model=gtk_icon_view_get_model(GTK_ICON_VIEW(widget));
            //g_message("%s: model: %x", __FUNCTION__, (UINT)model);
            
            gtk_tree_model_get_iter(model , &iter, treePath);     
            //g_message("%s: iter.stamp: %x %x %x %x", __FUNCTION__, (UINT)iter.stamp, (UINT) iter.user_data
            //        , (UINT)iter.user_data2, (UINT)iter.user_data3);       
            gtk_tree_model_get(model, &iter, 0, &wpObject, -1);

            TST_OBJECT(wpObject);

            if(nomIsObj(wpObject)){
              g_message("%s: %s calling wpDisplayMenu()", __FUNCTION__, wpObject->mtab->nomClassName);
              WPObject_wpDisplayMenu((WPObject*)wpObject, pWindow, NULL, MENU_OBJECTPOPUP, 0, NULL);
            }
            else
              g_message("%d %s: WPDataFile object not valid!", __LINE__, __FUNCTION__);

          }
      }/* if() */
  }CATCH{    
    //LOUD;
    g_message("Line %d: Trap in %s", __LINE__, __FUNCTION__);  
  }  END_CATCH(FLDR_BUTTONEVENT);

  return FALSE;
}

/* FIXME: move to an include file */
enum{
  WPOBJECT_TARGET_OBJECT,
  WPOBJECT_TARGET_CPATH,
  WPOBJECT_TARGET_PATH,
  WPOBJECT_TARGET_STRING
};

static GtkTargetEntry targetEntries[]=
{
  {"WPObject", 0, WPOBJECT_TARGET_OBJECT},
  {"WPObject-C-path", 0, WPOBJECT_TARGET_CPATH},
  {"WPObject-path", 0, WPOBJECT_TARGET_PATH},
  {"STRING", 0, WPOBJECT_TARGET_STRING},
};

static NOMDragWindow *nomDragWindow=NULL;
static NOMDragInfo   *nomDragInfo=NULL;

/*
  This callback handles the drop of an item on an object.
 */
static gboolean fldrWindowHandleDragDrop(GtkWidget * wgtThis, GdkDragContext* dragContext,
                                         gint x, gint y,
                         guint t, gpointer ptrUserData)
{
  GtkTreePath* treePath;
  PWPFolderWindow pWindow;
  gulong rc=0;
  WPObject *wpObject;

  //g_message("   %s", __FUNCTION__);
  //g_message("   action: %d", dragContext->action );

  /* Find the object beneath the pointer */
  treePath=gtk_icon_view_get_path_at_pos(GTK_ICON_VIEW(wgtThis), x, y );
  if(NULL==treePath)
    {      

      /* White space, so ask the folder */
      
      pWindow=(WPFolderWindow*)ptrUserData;
      if(!nomIsObj(pWindow))
        return FALSE;

      wpObject=WPFolderWindow_wpQueryWPObject(pWindow, NULLHANDLE);     

      if(!nomIsObj(wpObject))
        return FALSE;
    }
  else
    {
      GtkTreeIter iter;
      GtkTreeModel* model;

      /* Over an icon */
      
      model=gtk_icon_view_get_model(GTK_ICON_VIEW(wgtThis));            
      gtk_tree_model_get_iter(model , &iter, treePath);            
      gtk_tree_model_get(model, &iter, 0, &wpObject, -1);

      //TST_OBJECT(wpObject);

      if(!nomIsObj(wpObject))
        return FALSE;
    }
  rc=WPObject_wpDragOver(wpObject, wgtThis, nomDragInfo, NULL);

  if(dragContext->targets)
    {
      GdkAtom targetType;
      targetType=GDK_POINTER_TO_ATOM(g_list_nth_data(dragContext->targets, WPOBJECT_TARGET_OBJECT));
      //    g_message("  calling gtk_drag_get_data()..." );
      gtk_drag_get_data(wgtThis, dragContext, targetType, t);

      /* FIXME:
         We don't use the returned data for now but use our own draginfo stuff. This is
         broken.
         The following may be moved to dragDataReceived later. */
      if(rc) {
        //        g_message("Now call wpDrop() actions: %d action: %d sugg %d", dragContext->action,
        //        dragContext->action, dragContext->suggested_action);

        WPObject_wpDrop( wpObject, wgtThis, nomDragInfo, NULLHANDLE);
        
        gtk_drag_finish(dragContext, TRUE, FALSE, t);
        return TRUE;
      }

    }
  return FALSE;
}

#if 0
static void fldrDragLeave(GtkWidget * widget, GdkDragContext* dragContext,
                          guint t, gpointer ptrUserData)
{
  GdkPixmap *pixMapDrag;
  gdk_pixbuf_render_pixmap_and_mask(pixBufStop, &pixMapDrag, NULL, 128);
  gdk_window_set_back_pixmap(wgtDrag->window, pixMapDrag, FALSE);
  g_object_unref(pixMapDrag);
  gtk_widget_queue_draw(wgtDrag);
  g_message("%s", __FUNCTION__);
  return;
}
#endif

/**
   Callback for the drag-begin signal.
 */
static void fldrWindowHandleDragBegin(GtkWidget * widget, GdkDragContext* dragContext,
                                      gpointer ptrUserData)
{
  GtkTreePath* treePath;
  gint x, y;

  /* Create the drag window displayed to the user */
  if(NULLHANDLE==nomDragWindow)
    {
      nomDragWindow=NOMDragWindowNew();
    }

  if(NULLHANDLE==nomDragInfo)
    {
      nomDragInfo=NOMDragInfoNew();
    }

  if(!dragContext)
    return;

  /* Make sure we have no leftovers... */
  NOMDragInfo_deleteAllDragItems(nomDragInfo, NULLHANDLE);
  NOMDragInfo_setGdkDragContextPointer(nomDragInfo, dragContext, NULLHANDLE);

  /* Get the object beneath the pointer if any */
  gtk_widget_get_pointer(widget, &x, &y);
  treePath=gtk_icon_view_get_path_at_pos(GTK_ICON_VIEW(widget), x, y );

  if(NULL==treePath)
    {
      /* Click on white space */
      //    NOMDragWindow_hide(nomDragWindow, NULLHANDLE);
      g_message("%d %s: White space!", __LINE__, __FUNCTION__);
    }
  else
    {
      GtkTreeIter iter;
      GtkTreeModel* model;
      WPObject *wpObject;
      WPFolderWindow *wpFolderWindow=(WPFolderWindow*)ptrUserData;

      /* Click on an icon occurred */

      if(!nomIsObj(wpFolderWindow))
        return; //Oops...
      
      g_message("%s: %s", __FUNCTION__, gtk_tree_path_to_string(treePath));
      
      /* Get the object belonging to the tree path */
      model=gtk_icon_view_get_model(GTK_ICON_VIEW(widget));
      gtk_tree_model_get_iter(model , &iter, treePath);            
      gtk_tree_model_get(model, &iter, 0, &wpObject, -1);
      
      //  TST_OBJECT(wpObject);
      
      if(nomIsObj(wpObject)){
        g_message("%s: %s begin DnD", __FUNCTION__, wpObject->mtab->nomClassName);

        /* Add the object to the list of items for this drag */
        NOMDragInfo_addWPObjectToItemList(nomDragInfo, wpObject,
                                       (PWPFolder)WPFolderWindow_wpQueryWPObject(wpFolderWindow, NULLHANDLE),
                                       widget, NULLHANDLE);
      }
      else
        g_message("%d %s: WPDataFile object not valid!", __LINE__, __FUNCTION__);
  
     NOMDragWindow_show(nomDragWindow, NULLHANDLE);
    }


  /* Tell GTK to use our drag window as an 'icon' while dragging. */
  gtk_drag_set_icon_widget(dragContext,
                           NOMDragWindow_queryWindowHandle(nomDragWindow, NULLHANDLE), 0, 0);   

  // gdk_drag_status(dragContext, GDK_ACTION_MOVE, t);
  //NOMDragWindow_hide(nomDragWindow, NULLHANDLE);
  return;
}

static gboolean fldrWindowHandleDragMotion(GtkWidget * wgtThis, GdkDragContext* dragContext, int x, int y, guint t,
                    gpointer ptrUserData)
{
  GtkWidget *wgtSource;
  GtkTreePath* treePath;
  PWPFolderWindow pWindow;
  WPObject *wpObject;
  gulong rc=0;
  static gulong oldRc=0xff;/* Make sure we at last one time have an rc!=oldRc so the icon is
                              set properly below */

  if(dragContext==NULL)
    return FALSE;

  if((wgtSource=gtk_drag_get_source_widget(dragContext))==NULLHANDLE)
    return FALSE;

  /* This does not work for some reason */
  //  gtk_drag_highlight(wgtThis);
  //gtk_widget_queue_draw(wgtThis);


  if(dragContext)
    {
      //        g_message("  Actions: %x %x" , dragContext->actions, dragContext->suggested_action);
    }

#if 0
  if(wgtSource==wgtThis){
    gdk_drag_status(dragContext, GDK_ACTION_MOVE, t);
    //    gdk_drag_status(dragContext, 0, t);
    gtk_drag_highlight(wgtThis);
  }
  else{
    gdk_drag_status(dragContext, GDK_ACTION_COPY, t);
    gtk_drag_unhighlight(wgtSource);
  }
#endif

  /* Find the object beneath the pointer */
  treePath=gtk_icon_view_get_path_at_pos(GTK_ICON_VIEW(wgtThis), x, y );
  if(NULL==treePath)
    {      
      /* White space, so ask the folder */

      pWindow=(WPFolderWindow*)ptrUserData;
      if(!nomIsObj(pWindow))
        return FALSE;

      wpObject=WPFolderWindow_wpQueryWPObject(pWindow, NULLHANDLE);     

      if(!nomIsObj(wpObject))
        return FALSE;
    }
  else
    {
      GtkTreeIter iter;
      GtkTreeModel* model;

      /* Over an icon */
      
      model=gtk_icon_view_get_model(GTK_ICON_VIEW(wgtThis));
            
      gtk_tree_model_get_iter(model , &iter, treePath);            
      gtk_tree_model_get(model, &iter, 0, &wpObject, -1);

      //TST_OBJECT(wpObject);

      if(!nomIsObj(wpObject))
        return FALSE;
      //g_message("Over an icon");
    }
  rc=WPObject_wpDragOver(wpObject, wgtThis, nomDragInfo, NULL);
  //g_message("rc: %d oldRc %d", rc, oldRc);

  /* Change the drag icon if necesary. Note that this will not change the
     cursor. */
  if(oldRc!=rc)
    {
      /* drag icon */
      switch(rc)
        {
        case DOR_NODROP:
          {
            /* Don't drop */    
            NOMDragWindow_displayStopImage(nomDragWindow, NULLHANDLE);
            //     gdk_drag_status(dragContext, 0, t);
            break;
          }
        default:
          {
            NOMDragWindow_displayDragImage(nomDragWindow, NULLHANDLE);
            //  gdk_drag_status(dragContext, GDK_ACTION_MOVE, t);
           break;
          }
        } 
      /* Tell GTK to redraw the drag window */
      gtk_widget_queue_draw(NOMDragWindow_queryWindowHandle(nomDragWindow, NULLHANDLE));
    }
  oldRc=rc;

  switch(dragContext->actions)
    {  
    case GDK_ACTION_COPY:
      {
        gdk_drag_status(dragContext, GDK_ACTION_COPY, t);
        break;
      }
    case GDK_ACTION_MOVE:
      {
        gdk_drag_status(dragContext, GDK_ACTION_MOVE, t);
        break;
      }
    case GDK_ACTION_LINK:
      {
        gdk_drag_status(dragContext, GDK_ACTION_LINK, t);
        break;
      }
    default:
      {
        if(dragContext->actions & GDK_ACTION_MOVE) //FIXME
          gdk_drag_status(dragContext, GDK_ACTION_MOVE, t);        
        break;
      }
    }
#if 0
  /* Now change the cursor */
  switch(rc & DOR_ALLFLAGS)
    {
    case DOR_NODROP:
      {
        /* Don't drop */    
        gdk_drag_status(dragContext, 0, t);
        break;
      }
    default:
      {
        switch(rc & DO_ALLFLAGS)
          {
          case DO_MOVE:
            {
              gdk_drag_status(dragContext, GDK_ACTION_MOVE, t);
              break;
            }
          case DO_COPY:
            {
              gdk_drag_status(dragContext, GDK_ACTION_COPY, t);
              break;
            }
          case DO_LINK:
            {
              gdk_drag_status(dragContext, GDK_ACTION_LINK, t);
              break;
            }

          default:
            if(dragContext->actions & GDK_ACTION_MOVE) //FIXME
              gdk_drag_status(dragContext, GDK_ACTION_MOVE, t);
            break;
          }
        break;
      }
    }/* switch() */
#endif
 
  return FALSE;
}


static void fldrWindowHandleDragDataReceived(GtkWidget * widget, GdkDragContext* dragContext, int x, int y, GtkSelectionData *selData,
                          guint info, guint uiTime, gpointer ptrUserData)
{
  g_message("%s", __FUNCTION__);
  //  gdk_drag_status(dragContext, GDK_ACTION_COPY, uiTime);
  return;
}


/*

  \param uiInfo the entry in the GtkTargetList used in gtk_drag_source_set().

 */
static void fldrWindowHandleDragDataGet(GtkWidget * widget, GdkDragContext* dragContext, GtkSelectionData *selData,
                    guint uiTargetInfo, guint t, gpointer ptrUserData)
{
  
  g_message("%s", __FUNCTION__);
  //  g_message("  info: %d, userData: %x", uiTargetInfo, ptrUserData);

  switch(uiTargetInfo)
    {
    case WPOBJECT_TARGET_OBJECT:
      {
        gpointer wpObject=1234;

        g_message("WPOBJECT_TARGET_OBJECT requested");
        gtk_selection_data_set(selData, selData->target, 8, (guchar*) &wpObject, sizeof(wpObject));
        break;
      }
    default:

      break;
    };

  return;
}

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

#if 0
  /* Handle mouse buttons */
  g_signal_connect (GTK_WIDGET(icon_view), "button-press-event",
                    G_CALLBACK (fldr_handleButtonEvent), nomSelf);
  g_signal_connect (GTK_WIDGET(icon_view), "button-release-event",
                    G_CALLBACK (fldr_handleButtonEvent), nomSelf);

  /* Handle folder DnD */
  g_signal_connect(GTK_WIDGET(icon_view), "drag-begin",
                   G_CALLBACK(fldrWindowHandleDragBegin), nomSelf);
  g_signal_connect(GTK_WIDGET(icon_view), "drag-motion",
                   G_CALLBACK(fldrWindowHandleDragMotion), nomSelf);
  g_signal_connect(GTK_WIDGET(icon_view), "drag-drop",
                   G_CALLBACK(fldrWindowHandleDragDrop), nomSelf);
  g_signal_connect(GTK_WIDGET(icon_view), "drag_data_received",
                   G_CALLBACK(fldrWindowHandleDragDataReceived), nomSelf);
  g_signal_connect(GTK_WIDGET(icon_view), "drag_data_get",
                   G_CALLBACK(fldrWindowHandleDragDataGet), nomSelf);

  /* Prepare drag and drop */
  gtk_drag_source_set(GTK_WIDGET(icon_view), GDK_BUTTON3_MASK, targetEntries,
                      G_N_ELEMENTS(targetEntries),
                      GDK_ACTION_DEFAULT|GDK_ACTION_LINK|GDK_ACTION_COPY|GDK_ACTION_MOVE);
  gtk_drag_dest_set(GTK_WIDGET(icon_view), GTK_DEST_DEFAULT_ALL, targetEntries, 1,
                      GDK_ACTION_DEFAULT|GDK_ACTION_LINK|GDK_ACTION_COPY|GDK_ACTION_MOVE);
#endif
#if 0
  /* Connect to the "clicked" signal of the "Up" tool button */
  g_signal_connect (up_button, "clicked",
                    G_CALLBACK (up_clicked), store);
#endif

  _wpConnectDefaultSignalHandlers(nomSelf, icon_view, NULLHANDLE);
  WPFolderWindow_wpSetContainerHandle(nomSelf, icon_view, NULLHANDLE);

  /* Add icon view as child to the scroll window created earlier */
  gtk_container_add (GTK_CONTAINER (sw), icon_view);

  gtk_widget_grab_focus (icon_view);

  //  g_signal_connect (GTK_WIDGET(window), "size-request",
  //                G_CALLBACK (handleEvent), nomSelf);

  WPFolderWindow_setWindowHandle(nomSelf, window, NULLHANDLE);
  /* Window is hidden here and must be shown by the caller */
}

NOM_Scope void NOMLINK impl_WPFolderWindow_wpConnectDefaultSignalHandlers(WPFolderWindow* nomSelf,
                                                                          const PGtkWidget pgWidget,
                                                                          CORBA_Environment *ev)
{
/* WPFolderWindowData* nomThis=WPFolderWindowGetData(nomSelf); */

  /* Handle mouse buttons */
  g_signal_connect (GTK_WIDGET(pgWidget), "button-press-event",
                    G_CALLBACK (fldr_handleButtonEvent), nomSelf);
  g_signal_connect (GTK_WIDGET(pgWidget), "button-release-event",
                    G_CALLBACK (fldr_handleButtonEvent), nomSelf);

  /* Handle folder DnD */
  g_signal_connect(GTK_WIDGET(pgWidget), "drag-begin",
                   G_CALLBACK(fldrWindowHandleDragBegin), nomSelf);
  g_signal_connect(GTK_WIDGET(pgWidget), "drag-motion",
                   G_CALLBACK(fldrWindowHandleDragMotion), nomSelf);
  g_signal_connect(GTK_WIDGET(pgWidget), "drag-drop",
                   G_CALLBACK(fldrWindowHandleDragDrop), nomSelf);
  g_signal_connect(GTK_WIDGET(pgWidget), "drag_data_received",
                   G_CALLBACK(fldrWindowHandleDragDataReceived), nomSelf);
  g_signal_connect(GTK_WIDGET(pgWidget), "drag_data_get",
                   G_CALLBACK(fldrWindowHandleDragDataGet), nomSelf);

  /* Prepare drag and drop */
  gtk_drag_source_set(GTK_WIDGET(pgWidget), GDK_BUTTON3_MASK, targetEntries,
                      G_N_ELEMENTS(targetEntries),
                      GDK_ACTION_DEFAULT|GDK_ACTION_LINK|GDK_ACTION_COPY|GDK_ACTION_MOVE);
  gtk_drag_dest_set(GTK_WIDGET(pgWidget), GTK_DEST_DEFAULT_ALL, targetEntries, 1,
                      GDK_ACTION_DEFAULT|GDK_ACTION_LINK|GDK_ACTION_COPY|GDK_ACTION_MOVE);

}

