/*
 * This file was generated by orbit-idl-2 for Voyager - DO NOT EDIT!
 *
 *
 * And remember, phase 3 is near...
 */

#ifndef NOM_M_WPRootFolder_IMPLEMENTATION_FILE
#define NOM_M_WPRootFolder_IMPLEMENTATION_FILE
#endif

#define INCL_DOS
#include <os2.h>

#include "nom.h"
#include <nomtk.h>

#include "m_wprootfolder.ih"


/* orbit-idl-c-stubs.c, cs_output_stub line 347 */
NOM_Scope CORBA_string NOMLINK impl_M_WPRootFolder_wpclsQueryTitle(M_WPRootFolder* nomSelf, CORBA_Environment *ev)
{
  M_WPRootFolderData* nomThis=M_WPRootFolderGetData(nomSelf);

  if(!_pChrTitle)
    {
      _pChrTitle="Root Folder";
    }

  return _pChrTitle;
#if 0
  /* orbit-idl-c-stubs.c, VoyagerWriteProtoForParentCall line 119 */
  M_WPRootFolder_wpclsQueryTitle_parent(nomSelf,  ev);
#endif
}
