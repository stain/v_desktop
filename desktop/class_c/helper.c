
#define INCL_DOS
#include <os2.h>

#include <gtk/gtk.h>

const gchar* priv_getIconDir(void)
{
  static gchar gchrIconDir[CCHMAXPATH]={0};
  gchar* pgchrCurDir;

  if(!gchrIconDir[0])
    { 
      pgchrCurDir=g_get_current_dir();
      g_strlcpy(gchrIconDir, pgchrCurDir, sizeof(gchrIconDir));
      g_free(pgchrCurDir);
    }
  return gchrIconDir;
}
