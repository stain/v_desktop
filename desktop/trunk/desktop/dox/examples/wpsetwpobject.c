/**
  This method creates the folder window it doesn't query any files or creates
  models and stuff.
*/
NOM_Scope PWPFolderWindow NOMLINK impl_WPFolder_wpCreateFolderWindow(WPFolder* nomSelf, CORBA_Environment *ev)
{
  WPFolderWindow * wpFldrWindow;
  PPRIVFOLDERDATA priv;
  WPFolderData *nomThis = WPFolderGetData(nomSelf);
  
  priv=(PPRIVFOLDERDATA)_privFolderData;

  /* Create a new folder window of class WPFolderWindow */
  wpFldrWindow=WPFolderWindowNew();

  /* Save a pointer to the desktop folder object */
  WPFolderWindow_wpSetWPObject(wpFldrWindow, (WPObject*)nomSelf, NULLHANDLE);

  /* Connect to the "item_activated" signal */
  g_signal_connect (WPFolderWindow_wpQueryContainerHandle(wpFldrWindow, NULLHANDLE), "item-activated",
                    G_CALLBACK (itemActivated), nomSelf);

  WPFolderWindow_wpSetWindowTitle(wpFldrWindow, WPFolder_wpQueryTitle(nomSelf, NULLHANDLE), NULLHANDLE);

  /* Prepare drag and drop */
  gtk_drag_source_set(WPFolderWindow_wpQueryContainerHandle(wpFldrWindow, NULLHANDLE), GDK_BUTTON3_MASK, targetEntries, 1 ,
                      GDK_ACTION_LINK|GDK_ACTION_COPY|GDK_ACTION_MOVE);
  gtk_drag_dest_set(WPFolderWindow_wpQueryContainerHandle(wpFldrWindow, NULLHANDLE), GTK_DEST_DEFAULT_ALL, targetEntries, 1 ,
                      GDK_ACTION_LINK|GDK_ACTION_COPY|GDK_ACTION_MOVE);
  g_signal_connect(WPFolderWindow_wpQueryContainerHandle(wpFldrWindow, NULLHANDLE),"drag_data_received",
                   G_CALLBACK(fldrDragDataReceived), NULL);
  g_signal_connect(WPFolderWindow_wpQueryContainerHandle(wpFldrWindow, NULLHANDLE),"drag_begin",
                   G_CALLBACK(fldrDragBegin), NULL);
  g_signal_connect(WPFolderWindow_wpQueryContainerHandle(wpFldrWindow, NULLHANDLE),"drag_motion",
                   G_CALLBACK(fldrDragMotion), NULL);

  /* Show the new window */
  WPFolderWindow_show(wpFldrWindow, ev);

  return wpFldrWindow;
}
