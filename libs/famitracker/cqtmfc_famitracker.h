#ifndef CQTMFC_FAMITRACKER_H
#define CQTMFC_FAMITRACKER_H

#include "cqtmfc.h"

extern QMap<int,CString> qtMfcStringResources;

extern QMap<int,CMenu> qtMfcMenuResources;

void qtMfcInit();

// Define resources here that are "hidden under the hood" of MFC...
enum
{
   __UNDER_THE_HOOD_START = 0x8000000,

//   STRINGTABLE 
//   BEGIN
   AFX_IDS_APP_TITLE       ,
   AFX_IDS_IDLEMESSAGE     ,
   AFX_IDS_HELPMODEMESSAGE,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_INDICATOR_EXT        ,
   ID_INDICATOR_CAPS       ,
   ID_INDICATOR_NUM        ,
   ID_INDICATOR_SCRL       ,
   ID_INDICATOR_OVR        ,
   ID_INDICATOR_REC        ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_FILE_NEW             ,
   ID_FILE_OPEN            ,
   ID_FILE_CLOSE           ,
   ID_FILE_SAVE            ,
   ID_FILE_SAVE_AS         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_APP_ABOUT            ,
   ID_APP_EXIT             ,
   ID_HELP_INDEX           ,
   ID_HELP_FINDER          ,
   ID_HELP_USING           ,
   ID_CONTEXT_HELP         ,
   ID_HELP                 ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_FILE_MRU_FILE1       ,
   ID_FILE_MRU_FILE2       ,
   ID_FILE_MRU_FILE3       ,
   ID_FILE_MRU_FILE4       ,
   ID_FILE_MRU_FILE5       ,
   ID_FILE_MRU_FILE6       ,
   ID_FILE_MRU_FILE7       ,
   ID_FILE_MRU_FILE8       ,
   ID_FILE_MRU_FILE9       ,
   ID_FILE_MRU_FILE10      ,
   ID_FILE_MRU_FILE11      ,
   ID_FILE_MRU_FILE12      ,
   ID_FILE_MRU_FILE13      ,
   ID_FILE_MRU_FILE14      ,
   ID_FILE_MRU_FILE15      ,
   ID_FILE_MRU_FILE16      ,
//   END

//   STRINGTABLE 
//   BEGIN
   ID_NEXT_PANE            ,
   ID_PREV_PANE            ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_WINDOW_SPLIT         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_EDIT_CLEAR           ,
   ID_EDIT_CLEAR_ALL       ,
   ID_EDIT_COPY            ,
   ID_EDIT_CUT             ,
   ID_EDIT_FIND            ,
   ID_EDIT_PASTE           ,
   ID_EDIT_REPEAT          ,
   ID_EDIT_REPLACE         ,
   ID_EDIT_SELECT_ALL      ,
   ID_EDIT_UNDO            ,
   ID_EDIT_REDO            ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_VIEW_TOOLBAR         ,
   ID_VIEW_STATUS_BAR      ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   AFX_IDS_SCSIZE          ,
   AFX_IDS_SCMOVE          ,
   AFX_IDS_SCMINIMIZE      ,
   AFX_IDS_SCMAXIMIZE      ,
   AFX_IDS_SCNEXTWINDOW    ,
   AFX_IDS_SCPREVWINDOW    ,
   AFX_IDS_SCCLOSE         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   AFX_IDS_SCRESTORE       ,
   AFX_IDS_SCTASKLIST      ,
//   END
};

#endif // CQTMFC_FAMITRACKER_H
