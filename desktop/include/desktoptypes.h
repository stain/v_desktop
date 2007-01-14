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

#ifndef WPObject
typedef struct WPObject_struct {
  struct nomMethodTabStruct  *mtab;
  gulong body[1];
} WPObjectObj;
#define WPObject WPObjectObj
typedef WPObject *PWPObject;
#endif

/* Inuse lists */
typedef struct _USEITEM
{
   gulong    type;       /* Item type */
   PWPObject wpObject;   /* Remark: this is different to OS/2  */
} USEITEM, *PUSEITEM;

/* USAGE_MEMORY */
typedef struct _MEMORYITEM
{
   gulong   cbBuffer;           /* Number of bytes in memory block */
} MEMORYITEM, *PMEMORYITEM;

/* USAGE_OPENVIEW */
typedef struct _VIEWITEM {
  gulong          ulView;
  NOMWindow*      nomWindow;
  gulong          ulViewState;  /*   */
  nomId           nameSpaceId;
  gpointer        pReserved;
 } VIEWITEM, *PVIEWITEM;


#define USAGE_MEMORY          1
#define USAGE_OPENVIEW        5

#define OPEN_DEFAULT          0
#define OPEN_CONTENTS         1
#define OPEN_SETTINGS         2

#if 0
/* Views for wpFindViewItem() */
#define VIEW_CONTENTS         0x00000001
#define VIEW_SETTINGS         0x00000002
#define VIEW_HELP             0x00000004
#define VIEW_RUNNING          0x00000008
#define VIEW_DETAILS          0x00000010
#define VIEW_TREE             0x00000020
#define VIEW_ANY              0xFFFFFFFF
#endif

/* Concurrent view flags */
#define CCVIEW_DEFAULT        0UL
#define CCVIEW_ON             1UL
#define CCVIEW_OFF            2UL

/* Flags for menu methods */
#define MENU_OBJECTPOPUP           0x00000001
#define MENU_OPENVIEWPOPUP         0x00000002
#define MENU_TITLEBARPULLDOWN      0x00000003

/* Menu IDs*/
#define WPMENUID_OPEN              1
#define WPMENUID_PROPERTIES        112

/* Folder flags */
#define FOI_POPULATEDWITHALL       0x0001
#define FOI_POPULATEDWITHFOLDERS   0x0002

/* Misc types used with objects */
typedef GSList *PGSList;
typedef GMutex *PGMutex;

/* Errors */ 
#define NOMERROR_NOT_ENOUGH_MEMORY 8
  
#endif /* DESKTOPTYPES_H_INCLUDED */

