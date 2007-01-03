#ifndef DESKTOPTYPES_H_INCLUDED
#define DESKTOPTYPES_H_INCLUDED

/* Menu handling */
typedef gpointer wpMenuId;

typedef struct _wpMenuItemInfo
{
  wpMenuId menuId;      /* A unique ID calculated by the system        */
  gchar* pszMenuItemId; /* ID string like classname.submenu.menuitem   */
  gchar* pszMenuText;   /* The actual menu text                        */
  gpointer ptrUserinfo; /* Pointer to more menu item info for the user */
}wpMenuItemInfo;

/* Inuse lists */
typedef struct _USEITEM
{
   gulong  type;              /* Item type */
   struct  _USEITEM *pNext;   /* Next item */
} USEITEM, *PUSEITEM;

/* USAGE_MEMORY */
typedef struct _MEMORYITEM
{
   gulong   cbBuffer;           /* Number of bytes in memory block */
} MEMORYITEM, *PMEMORYITEM;

#define USAGE_MEMORY          1

#define OPEN_DEFAULT          0
#define OPEN_CONTENTS         1
#define OPEN_SETTINGS         2

/* Flags for menu methods */
#define MENU_OBJECTPOPUP           0x00000001
#define MENU_OPENVIEWPOPUP         0x00000002
#define MENU_TITLEBARPULLDOWN      0x00000003

/* Menu IDs*/
#define WPMENUID_OPEN              1
#define WPMENUID_PROPERTIES        112

/* Misc types used with objects */
typedef GSList *PGSList;
typedef GMutex *PGMutex;

/* Errors */ 
#define NOMERROR_NOT_ENOUGH_MEMORY 8
  
#endif /* DESKTOPTYPES_H_INCLUDED */
