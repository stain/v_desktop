#ifndef DESKTOPTYPES_H_INCLUDED
#define DESKTOPTYPES_H_INCLUDED

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

/* Misc types used with objects */
typedef GSList *PGSList;
typedef GMutex *PGMutex;

/* Errors */ 
#define NOMERROR_NOT_ENOUGH_MEMORY 8
  
#endif /* DESKTOPTYPES_H_INCLUDED */
