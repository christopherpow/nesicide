#include "cqtmfc.h"
#include "cqtmfc_famitracker.h"
#include "resource.h"

#include "stdafx.h"

#include "FamiTrackerDoc.h"

/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

void qtMfcInitMenuResource_IDR_MAINFRAME(CMenu* parent)
{
   QAction* menuAction;

//IDR_MAINFRAME MENU
//BEGIN
   CMenu* subMenuTree[2];
   CMenu* subMenu;
//    POPUP "&File"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"&File");
//    BEGIN
//        MENUITEM "&New\tCtrl+N",                ID_FILE_NEW
   subMenu->AppendMenu(MF_STRING,ID_FILE_NEW,"&New\tCtrl+N");
//        MENUITEM "&Open...\tCtrl+O",            ID_FILE_OPEN
   subMenu->AppendMenu(MF_STRING,ID_FILE_OPEN,"&Open...\tCtrl+O");
//        MENUITEM "&Save\tCtrl+S",               ID_FILE_SAVE
   subMenu->AppendMenu(MF_STRING,ID_FILE_SAVE,"&Save\tCtrl+S");
//        MENUITEM "Save &As...",                 ID_FILE_SAVE_AS
   subMenu->AppendMenu(MF_STRING,ID_FILE_SAVE_AS,"Save &As...");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "&Create NSF...\tCtrl+E",      ID_FILE_CREATE_NSF
   subMenu->AppendMenu(MF_STRING,ID_FILE_CREATE_NSF,"&Create NSF...\tCtrl+E");
//        MENUITEM "Create WAV...",               ID_FILE_CREATEWAV
   subMenu->AppendMenu(MF_STRING,ID_FILE_CREATEWAV,"Create WAV...");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//   MENUITEM "Import text...",              ID_FILE_IMPORTTEXT
   subMenu->AppendMenu(MF_STRING,ID_FILE_IMPORTTEXT,"Import text...");
//   MENUITEM "Export text...",              ID_FILE_EXPORTTEXT
   subMenu->AppendMenu(MF_STRING,ID_FILE_EXPORTTEXT,"Export text...");
//   MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "&Configuration",              ID_FILE_GENERALSETTINGS
   subMenu->AppendMenu(MF_STRING,ID_FILE_GENERALSETTINGS,"&Configuration");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Recent File",                 ID_FILE_MRU_FILE1
   subMenu->AppendMenu(MF_STRING,ID_FILE_MRU_FILE1,"Recent File");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "E&xit\tAlt+F4",               ID_APP_EXIT
   subMenu->AppendMenu(MF_STRING,ID_APP_EXIT,"E&xit");
//    END
//    POPUP "&Edit"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"&Edit");
//    BEGIN
//        MENUITEM "&Undo\tCtrl+Z",               ID_EDIT_UNDO
   subMenu->AppendMenu(MF_STRING,ID_EDIT_UNDO,"&Undo\tCtrl+Z");
//        MENUITEM "&Redo\tCtrl+Y",               ID_EDIT_REDO
   subMenu->AppendMenu(MF_STRING,ID_EDIT_REDO,"&Redo\tCtrl+Y");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Cu&t\tCtrl+X",                ID_EDIT_CUT
   subMenu->AppendMenu(MF_STRING,ID_EDIT_CUT,"Cu&t\tCtrl+X");
//        MENUITEM "&Copy\tCtrl+C",               ID_EDIT_COPY
   subMenu->AppendMenu(MF_STRING,ID_EDIT_COPY,"&Copy\tCtrl+C");
//        MENUITEM "&Paste\tCtrl+V",              ID_EDIT_PASTE
   subMenu->AppendMenu(MF_STRING,ID_EDIT_PASTE,"&Paste\tCtrl+V");
//   MENUITEM "Paste (mix)",                 ID_EDIT_PASTEMIX
   subMenu->AppendMenu(MF_STRING,ID_EDIT_PASTEMIX,"Paste (mix)");
//        MENUITEM "&Delete\tDel",                ID_EDIT_DELETE
   subMenu->AppendMenu(MF_STRING,ID_EDIT_DELETE,"&Delete\tDel");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Select &All\tCtrl+A",         ID_EDIT_SELECTALL
   subMenu->AppendMenu(MF_STRING,ID_EDIT_SELECTALL,"Select &All\tCtrl+A");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        POPUP "Transpose"
   subMenuTree[1] = new CMenu;
   subMenu = subMenuTree[1];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   subMenuTree[0]->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Transpose");
//        BEGIN
//            MENUITEM "Decrease Note\tCtrl+F1",      ID_TRANSPOSE_DECREASENOTE
   subMenu->AppendMenu(MF_STRING,ID_TRANSPOSE_DECREASENOTE,"Decrease Note\tCtrl+F1");
//            MENUITEM "Increase Note\tCtrl+F2",      ID_TRANSPOSE_INCREASENOTE
   subMenu->AppendMenu(MF_STRING,ID_TRANSPOSE_INCREASENOTE,"Increase Note\tCtrl+F2");
//            MENUITEM "Decrease Octave\tCtrl+F3",    ID_TRANSPOSE_DECREASEOCTAVE
   subMenu->AppendMenu(MF_STRING,ID_TRANSPOSE_DECREASEOCTAVE,"Decrease Octave\tCtrl+F3");
//            MENUITEM "Increase Octave\tCtrl+F4",    ID_TRANSPOSE_INCREASEOCTAVE
   subMenu->AppendMenu(MF_STRING,ID_TRANSPOSE_INCREASEOCTAVE,"Increase Octave\tCtrl+F4");
//        END
   subMenu = subMenuTree[0];
//        MENUITEM "Interpolate\tCtrl+G",         ID_EDIT_INTERPOLATE
   subMenu->AppendMenu(MF_STRING,ID_EDIT_INTERPOLATE,"Interpolate\tCtrl+G");
//        MENUITEM "&Reverse\tCtrl+R",            ID_EDIT_REVERSE
   subMenu->AppendMenu(MF_STRING,ID_EDIT_REVERSE,"&Reverse\tCtrl+R");
//        MENUITEM "R&eplace instrument\tAlt+S",  ID_EDIT_REPLACEINSTRUMENT
   subMenu->AppendMenu(MF_STRING,ID_EDIT_REPLACEINSTRUMENT,"R&eplace instrument\tAlt+S");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Expand pattern",              ID_EDIT_EXPANDPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_EDIT_EXPANDPATTERNS,"Expand pattern");
//        MENUITEM "Shrink pattern",              ID_EDIT_SHRINKPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_EDIT_SHRINKPATTERNS,"Shrink pattern");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Clear patterns",              ID_EDIT_CLEARPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_EDIT_CLEARPATTERNS,"Clear pattern");
//        POPUP "Clean up"
   subMenuTree[1] = new CMenu;
   subMenu = subMenuTree[1];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   subMenuTree[0]->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Clean up");
//        BEGIN
//            MENUITEM "Remove unused instruments",   ID_CLEANUP_REMOVEUNUSEDINSTRUMENTS
   subMenu->AppendMenu(MF_STRING,ID_CLEANUP_REMOVEUNUSEDINSTRUMENTS,"Remove unused instruments");
//            MENUITEM "Remove unused patterns",      ID_CLEANUP_REMOVEUNUSEDPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_CLEANUP_REMOVEUNUSEDPATTERNS,"Remove unused patterns");
//   MENUITEM "Merge duplicated patterns",   ID_CLEANUP_MERGEDUPLICATEDPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_CLEANUP_MERGEDUPLICATEDPATTERNS,"Merge duplicated patterns");
//        END
   subMenu = subMenuTree[0];
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Instrument Mask\tAlt+T",      ID_EDIT_INSTRUMENTMASK
   subMenu->AppendMenu(MF_STRING,ID_EDIT_INSTRUMENTMASK,"Instrument Mask\tAlt+T");
//   MENUITEM "Volume Mask",                 ID_EDIT_VOLUMEMASK, CHECKED
   subMenu->AppendMenu(MF_STRING,ID_EDIT_VOLUMEMASK,"Volume Mask");
   subMenu->CheckMenuItem(ID_EDIT_VOLUMEMASK,TRUE);
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Enable MIDI",                 ID_EDIT_ENABLEMIDI
   subMenu->AppendMenu(MF_STRING,ID_EDIT_ENABLEMIDI,"Enable MIDI");
//    END
//    POPUP "&Module"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"&Module");
//    BEGIN
//        MENUITEM "Insert frame",                ID_MODULE_INSERTFRAME
   subMenu->AppendMenu(MF_STRING,ID_MODULE_INSERTFRAME,"Insert frame");
//        MENUITEM "Remove frame",                ID_MODULE_REMOVEFRAME
   subMenu->AppendMenu(MF_STRING,ID_MODULE_REMOVEFRAME,"Remove frame");
//        MENUITEM "&Duplicate frame",            ID_MODULE_DUPLICATEFRAME
   subMenu->AppendMenu(MF_STRING,ID_MODULE_DUPLICATEFRAME,"&Duplicate frame");
//        MENUITEM "Duplicate patterns",          ID_MODULE_DUPLICATEFRAMEPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_MODULE_DUPLICATEFRAMEPATTERNS,"&Duplicate patterns");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Move Frame Up",               ID_MODULE_MOVEFRAMEUP
   subMenu->AppendMenu(MF_STRING,ID_MODULE_MOVEFRAMEUP,"Move Frame Up");
//        MENUITEM "Move Frame Down",             ID_MODULE_MOVEFRAMEDOWN
   subMenu->AppendMenu(MF_STRING,ID_MODULE_MOVEFRAMEDOWN,"Move Frame Down");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Comments",                    ID_MODULE_COMMENTS
   subMenu->AppendMenu(MF_STRING,ID_MODULE_COMMENTS,"Comments");
//        MENUITEM "Module Properties\tCtrl+P",   ID_MODULE_MODULEPROPERTIES
   subMenu->AppendMenu(MF_STRING,ID_MODULE_MODULEPROPERTIES,"Module Properties\tCtrl+P");
//        MENUITEM "Channels",                    ID_MODULE_CHANNELS
   subMenu->AppendMenu(MF_STRING,ID_MODULE_CHANNELS,"Channels");
//    END
//    POPUP "&Instrument"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"&Instrument");
//    BEGIN
//        MENUITEM "New instrument",              ID_INSTRUMENT_NEW
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_NEW,"New instrument");
//        MENUITEM "Remove instrument",           ID_INSTRUMENT_REMOVE
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_REMOVE,"Remove instrument");
//        MENUITEM "Clone instrument",            ID_INSTRUMENT_CLONE
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_CLONE,"Clone instrument");
//        MENUITEM "Deep clone instrument",       ID_INSTRUMENT_DEEPCLONE
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_DEEPCLONE,"Deep clone instrument");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Load from file...",           ID_INSTRUMENT_LOAD
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_LOAD,"Load from file...");
//        MENUITEM "Save to file...",             ID_INSTRUMENT_SAVE
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_SAVE,"Save to file...");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Edit\tCtrl+I",                ID_INSTRUMENT_EDIT
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_EDIT,"Edit\tCtrl+I");
//    END
//    POPUP "T&racker"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"T&racker");
//    BEGIN
//        MENUITEM "&Play\tReturn",               ID_TRACKER_PLAY
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_PLAY,"&Play\tReturn");
//        MENUITEM "P&lay Pattern\tShift+Return", ID_TRACKER_PLAYPATTERN
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_PLAYPATTERN,"P&lay Pattern\tShift+Return");
//        MENUITEM "Play from start",             ID_TRACKER_PLAY_START
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_PLAY_START,"Play from start");
//        MENUITEM "Play from cursor",            ID_TRACKER_PLAY_CURSOR
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_PLAY_CURSOR,"Play from cursor");
//        MENUITEM "&Stop\tReturn",               ID_TRACKER_STOP
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_STOP,"&Stop\tReturn");
//        MENUITEM "&Edit\tSpace",                ID_TRACKER_EDIT
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_EDIT,"Toggle &Edit mode\tSpace");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Toggle channel\tAlt+F9",      ID_TRACKER_TOGGLECHANNEL
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_TOGGLECHANNEL,"Toggle channel\tAlt+F9");
//        MENUITEM "Solo channel\tAlt+F10",       ID_TRACKER_SOLOCHANNEL
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_SOLOCHANNEL,"Solo channel\tAlt+F10");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Switch to track instrument",  ID_TRACKER_SWITCHTOTRACKINSTRUMENT
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_SWITCHTOTRACKINSTRUMENT,"Switch to track instrument");
//   MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//   MENUITEM "Display register state",      ID_TRACKER_DISPLAYREGISTERSTATE
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_DISPLAYREGISTERSTATE,"Display register state");
//        MENUITEM "&Display DPCM state",         ID_TRACKER_DPCM
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_DPCM,"&Display DPCM state");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "&Kill Sound\tF12",            ID_TRACKER_KILLSOUND
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_KILLSOUND,"&Kill Sound\tF12");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "NTSC",                        ID_TRACKER_NTSC
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_NTSC,"NTSC");
//        MENUITEM "PAL",                         ID_TRACKER_PAL
   subMenu->AppendMenu(MF_STRING,ID_TRACKER_PAL,"PAL");
//        POPUP "Engine Speed"
   subMenuTree[1] = new CMenu;
   subMenu = subMenuTree[1];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   subMenuTree[0]->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Engine Speed");
//        BEGIN
//            MENUITEM "Default",                     ID_SPEED_DEFAULT
   subMenu->AppendMenu(MF_STRING,ID_SPEED_DEFAULT,"Default");
//            MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//            MENUITEM "Custom",                      ID_SPEED_CUSTOM
   subMenu->AppendMenu(MF_STRING,ID_SPEED_CUSTOM,"Custom");
//        END
//    END
   subMenu = subMenuTree[0];
//    POPUP "&View"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"&View");
//    BEGIN
//        MENUITEM "&Toolbar",                    ID_VIEW_TOOLBAR
   subMenu->AppendMenu(MF_STRING,ID_VIEW_TOOLBAR,"&Toolbar");
//        MENUITEM "&Status Bar",                 ID_VIEW_STATUS_BAR
   subMenu->AppendMenu(MF_STRING,ID_VIEW_STATUS_BAR,"&Status Bar");
//        MENUITEM "Control panel",               ID_VIEW_CONTROLPANEL
   subMenu->AppendMenu(MF_STRING,ID_VIEW_CONTROLPANEL,"Control panel");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        POPUP "Frame editor position"
   subMenuTree[1] = new CMenu;
   subMenu = subMenuTree[1];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   subMenuTree[0]->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Frame editor position");
//        BEGIN
//            MENUITEM "Top",                         ID_FRAMEEDITOR_TOP, CHECKED
   subMenu->AppendMenu(MF_STRING,ID_FRAMEEDITOR_TOP,"Top");
//            MENUITEM "Left",                        ID_FRAMEEDITOR_LEFT
   subMenu->AppendMenu(MF_STRING,ID_FRAMEEDITOR_LEFT,"Left");
//        END
//    END
   subMenu = subMenuTree[0];
//    POPUP "&Help"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"&Help");
//    BEGIN
//        MENUITEM "&Help Topics\tF1",            ID_HELP_FINDER
   subMenu->AppendMenu(MF_STRING,ID_HELP_FINDER,"&Help Topics\tF1");
//        MENUITEM "Effect table",                ID_HELP_EFFECTTABLE
   subMenu->AppendMenu(MF_STRING,ID_HELP_EFFECTTABLE,"Effect table");
//   MENUITEM "FAQ",                         ID_HELP_FAQ
   subMenu->AppendMenu(MF_STRING,ID_HELP_FAQ,"FAQ");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Performance",                 ID_HELP_PERFORMANCE
   subMenu->AppendMenu(MF_STRING,ID_HELP_PERFORMANCE,"Performance");
//        MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "&About FamiTracker...",       ID_APP_ABOUT
   subMenu->AppendMenu(MF_STRING,ID_APP_ABOUT,"&About FamiTracker...");
//    END
//END
}
void qtMfcInitMenuResource_IDR_PATTERN_POPUP(CMenu* parent)
{
   QAction* menuAction;

//   IDR_PATTERN_POPUP MENU
//   BEGIN
   CMenu* subMenuTree[2];
   CMenu* subMenu;
//       POPUP "Popup"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"&Popup");
//       BEGIN
//           MENUITEM "&Undo",                       ID_EDIT_UNDO
   subMenu->AppendMenu(MF_STRING,ID_EDIT_UNDO,"&Undo");
//           MENUITEM "&Redo",                       ID_EDIT_REDO
   subMenu->AppendMenu(MF_STRING,ID_EDIT_REDO,"&Redo");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "Cu&t",                        ID_EDIT_CUT
   subMenu->AppendMenu(MF_STRING,ID_EDIT_CUT,"Cu&t");
//           MENUITEM "&Copy",                       ID_EDIT_COPY
   subMenu->AppendMenu(MF_STRING,ID_EDIT_COPY,"&Copy");
//           MENUITEM "&Paste",                      ID_EDIT_PASTE
   subMenu->AppendMenu(MF_STRING,ID_EDIT_PASTE,"&Paste");
//           MENUITEM "&Delete",                     ID_POPUP_DELETE
   subMenu->AppendMenu(MF_STRING,ID_POPUP_DELETE,"&Delete");
//           MENUITEM "Select &All",                 ID_EDIT_SELECTALL
   subMenu->AppendMenu(MF_STRING,ID_EDIT_SELECTALL,"Select &All");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           POPUP "Transpose"
   subMenuTree[1] = new CMenu;
   subMenu = subMenuTree[1];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   subMenuTree[0]->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Transpose");
//           BEGIN
//               MENUITEM "Increase Note",               ID_TRANSPOSE_INCREASENOTE
   subMenu->AppendMenu(MF_STRING,ID_TRANSPOSE_INCREASENOTE,"Increase Note");
//               MENUITEM "Decrease Note",               ID_TRANSPOSE_DECREASENOTE
   subMenu->AppendMenu(MF_STRING,ID_TRANSPOSE_DECREASENOTE,"Decrease Note");
//               MENUITEM "Increase Octave",             ID_TRANSPOSE_INCREASEOCTAVE
   subMenu->AppendMenu(MF_STRING,ID_TRANSPOSE_INCREASEOCTAVE,"Increase Octave");
//               MENUITEM "Decrease Octave",             ID_TRANSPOSE_DECREASEOCTAVE
   subMenu->AppendMenu(MF_STRING,ID_TRANSPOSE_DECREASEOCTAVE,"Decrease Octave");
//           END
   subMenu = subMenuTree[0];
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "&Interpolate",                ID_EDIT_INTERPOLATE
   subMenu->AppendMenu(MF_STRING,ID_EDIT_INTERPOLATE,"&Interpolate");
//           MENUITEM "&Reverse",                    ID_EDIT_REVERSE
   subMenu->AppendMenu(MF_STRING,ID_EDIT_REVERSE,"&Reverse");
//           MENUITEM "Pick up row",                 ID_POPUP_PICKUPROW
   subMenu->AppendMenu(MF_STRING,ID_POPUP_PICKUPROW,"Pick up row");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "Expand pattern",              ID_EDIT_EXPANDPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_EDIT_EXPANDPATTERNS,"Expand pattern");
//           MENUITEM "Shrink pattern",              ID_EDIT_SHRINKPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_EDIT_SHRINKPATTERNS,"Shrink pattern");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "R&eplace instrument",         ID_EDIT_REPLACEINSTRUMENT
   subMenu->AppendMenu(MF_STRING,ID_EDIT_REPLACEINSTRUMENT,"R&eplace instrument");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "&Toggle channel",             ID_POPUP_TOGGLECHANNEL
   subMenu->AppendMenu(MF_STRING,ID_POPUP_TOGGLECHANNEL,"&Toggle channel");
//           MENUITEM "&Solo channel",               ID_POPUP_SOLOCHANNEL
   subMenu->AppendMenu(MF_STRING,ID_POPUP_SOLOCHANNEL,"&Solo channel");
//       END
//   END
}

void qtMfcInitMenuResource_IDR_FRAME_POPUP(CMenu* parent)
{
   QAction* menuAction;

//   IDR_FRAME_POPUP MENU
//   BEGIN
   CMenu* subMenuTree[2];
   CMenu* subMenu;
//       POPUP "Frame"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"&Frame");
//       BEGIN
//           MENUITEM "&Insert frame\tIns",          ID_MODULE_INSERTFRAME
   subMenu->AppendMenu(MF_STRING,ID_MODULE_INSERTFRAME,"&Insert frame\tIns");
//           MENUITEM "&Remove frame\tDel",          ID_MODULE_REMOVEFRAME
   subMenu->AppendMenu(MF_STRING,ID_MODULE_REMOVEFRAME,"&Remove frame\tDel");
//           MENUITEM "&Duplicate frame",            ID_MODULE_DUPLICATEFRAME
   subMenu->AppendMenu(MF_STRING,ID_MODULE_DUPLICATEFRAME,"&Duplicate frame");
//           MENUITEM "D&uplicate patterns",          ID_MODULE_DUPLICATEFRAMEPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_MODULE_DUPLICATEFRAMEPATTERNS,"Duplicate patterns");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "Move &up",                    ID_MODULE_MOVEFRAMEUP
   subMenu->AppendMenu(MF_STRING,ID_MODULE_MOVEFRAMEUP,"Move &up");
//           MENUITEM "Move d&own",                  ID_MODULE_MOVEFRAMEDOWN
   subMenu->AppendMenu(MF_STRING,ID_MODULE_MOVEFRAMEDOWN,"Move d&own");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "Cu&t\tCtrl+X",                ID_FRAME_CUT
   subMenu->AppendMenu(MF_STRING,ID_FRAME_CUT,"Cu&t\tCtrl+X");
//           MENUITEM "&Copy\tCtrl+C",               ID_FRAME_COPY
   subMenu->AppendMenu(MF_STRING,ID_FRAME_COPY,"&Copy\tCtrl+C");
//           MENUITEM "&Paste\tCtrl+V",              ID_FRAME_PASTE
   subMenu->AppendMenu(MF_STRING,ID_FRAME_PASTE,"&Paste\tCtrl+V");
//   MENUITEM "Paste && duplicate",          ID_FRAME_PASTENEWPATTERNS
   subMenu->AppendMenu(MF_STRING,ID_FRAME_PASTENEWPATTERNS,"Paste && duplicate");
//   MENUITEM "D&elete\tDel",                ID_FRAME_DELETE
   subMenu->AppendMenu(MF_STRING,ID_FRAME_DELETE,"D&elete\tDel");
//       END
//   END
}

void qtMfcInitMenuResource_IDR_INSTRUMENT_POPUP(CMenu* parent)
{
   QAction* menuAction;

//   IDR_INSTRUMENT_POPUP MENU
//   BEGIN
   CMenu* subMenuTree[2];
   CMenu* subMenu;
//       POPUP "Instrument"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Instrument");
//       BEGIN
//           MENUITEM "Add",                         ID_INSTRUMENT_NEW
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_NEW,"Add");
//           MENUITEM "Remove",                      ID_INSTRUMENT_REMOVE
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_REMOVE,"Remove");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//   MENUITEM "&Load from file...",          ID_INSTRUMENT_LOAD
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_LOAD,"&Load from file...");
//   MENUITEM "&Save to file...",            ID_INSTRUMENT_SAVE
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_SAVE,"&Save to file...");
//   MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "Clone instrument",            ID_INSTRUMENT_CLONE
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_CLONE,"Clone instrument");
//           MENUITEM "Deep clone instrument",       ID_INSTRUMENT_DEEPCLONE
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_DEEPCLONE,"Deep clone instrument");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "Edit",                        ID_INSTRUMENT_EDIT
   subMenu->AppendMenu(MF_STRING,ID_INSTRUMENT_EDIT,"Edit");
//       END
//   END
}

void qtMfcInitMenuResource_IDR_SAMPLES_POPUP(CMenu* parent)
{
   QAction* menuAction;

//   IDR_SAMPLES_POPUP MENU
//   BEGIN
   CMenu* subMenuTree[2];
   CMenu* subMenu;
//       POPUP "Popup"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Popup");
//       BEGIN
//           MENUITEM "Preview",                     IDC_PREVIEW
   subMenu->AppendMenu(MF_STRING,IDC_PREVIEW,"Preview");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "Edit",                        IDC_EDIT
   subMenu->AppendMenu(MF_STRING,IDC_EDIT,"Edit");
//           MENUITEM SEPARATOR
   subMenu->AppendMenu(MF_SEPARATOR);
//           MENUITEM "Unload",                      IDC_UNLOAD
   subMenu->AppendMenu(MF_STRING,IDC_UNLOAD,"Unload");
//           MENUITEM "Save",                        IDC_SAVE
   subMenu->AppendMenu(MF_STRING,IDC_SAVE,"Save");
//           MENUITEM "Load",                        IDC_LOAD
   subMenu->AppendMenu(MF_STRING,IDC_LOAD,"Load");
//           MENUITEM "Import",                      IDC_IMPORT
   subMenu->AppendMenu(MF_STRING,IDC_IMPORT,"Import");
//       END
//   END
}

void qtMfcInitMenuResource_IDR_SAMPLE_WND_POPUP(CMenu* parent)
{
   QAction* menuAction;

//   IDR_SAMPLE_WND_POPUP MENU
//   BEGIN
   CMenu* subMenuTree[2];
   CMenu* subMenu;
//       POPUP "Popup"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Popup");
//       BEGIN
//           MENUITEM "Sample scope",              ID_POPUP_SAMPLESCOPE1
   subMenu->AppendMenu(MF_STRING,ID_POPUP_SAMPLESCOPE1,"Sample scope 1");
//           MENUITEM "Sample scope (blur)",              ID_POPUP_SAMPLESCOPE2
   subMenu->AppendMenu(MF_STRING,ID_POPUP_SAMPLESCOPE2,"Sample scope 2");
//           MENUITEM "Spectrum analyzer",           ID_POPUP_SPECTRUMANALYZER
   subMenu->AppendMenu(MF_STRING,ID_POPUP_SPECTRUMANALYZER,"Spectrum analyzer");
//           MENUITEM "Nothing",                     ID_POPUP_NOTHING
   subMenu->AppendMenu(MF_STRING,ID_POPUP_NOTHING,"Nothing");
//       END
//   END
}

void qtMfcInitMenuResource_IDR_PATTERN_HEADER_POPUP(CMenu* parent)
{
   QAction* menuAction;

//   IDR_PATTERN_HEADER_POPUP MENU
//   BEGIN
   CMenu* subMenuTree[2];
   CMenu* subMenu;
//       POPUP "Popup"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Popup");
//       BEGIN
//           MENUITEM "&Toggle channel\tAlt+F9",     ID_POPUP_TOGGLECHANNEL
   subMenu->AppendMenu(MF_STRING,ID_POPUP_TOGGLECHANNEL,"&Toggle channel\tAlt+F9");
//           MENUITEM "&Solo channel\tAlt+F10",      ID_POPUP_SOLOCHANNEL
   subMenu->AppendMenu(MF_STRING,ID_POPUP_SOLOCHANNEL,"&Solo channel\tAlt+F10");
//           MENUITEM "&Unmute all channels",        ID_POPUP_UNMUTEALLCHANNELS
   subMenu->AppendMenu(MF_STRING,ID_POPUP_UNMUTEALLCHANNELS,"&Unmute all channels");
//       END
//   END
}

void qtMfcInitMenuResource_IDR_SAMPLE_EDITOR_POPUP(CMenu* parent)
{
   QAction* menuAction;

//IDR_SAMPLE_EDITOR_POPUP MENU
      CMenu* subMenuTree[2];
      CMenu* subMenu;
//BEGIN
//    POPUP "Popup"
      subMenuTree[0] = new CMenu;
      subMenu = subMenuTree[0];
      QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
      parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Popup");
//    BEGIN
//        MENUITEM "Play\tP",                     IDC_PLAY
      subMenu->AppendMenu(MF_STRING,IDC_PLAY,"Play\tP");
//        MENUITEM SEPARATOR
      subMenu->AppendMenu(MF_SEPARATOR);
//        MENUITEM "Delete\tDel",                 IDC_DELETE
      subMenu->AppendMenu(MF_STRING,IDC_DELETE,"Delete\tDel");
//        MENUITEM "Tilt",                        IDC_TILT
      subMenu->AppendMenu(MF_STRING,IDC_TILT,"Tilt");
//    END
//END
}

void qtMfcInitMenuResource_IDR_SEQUENCE_POPUP(CMenu* parent)
{
   QAction* menuAction;

//IDR_SEQUENCE_POPUP MENU
   CMenu* subMenuTree[2];
   CMenu* subMenu;
//BEGIN
//    POPUP "Popup"
   subMenuTree[0] = new CMenu;
   subMenu = subMenuTree[0];
   QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
   parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,"Popup");
//    BEGIN
//        MENUITEM "&Clone sequence",             ID_CLONE_SEQUENCE
   subMenu->AppendMenu(MF_STRING,ID_CLONE_SEQUENCE,"&Clone sequence");
//    END
//END
}

void qtMfcInitMenuResource(UINT menuID,CMenu* parent)
{
   switch ( menuID )
   {
   case IDR_MAINFRAME:
      qtMfcInitMenuResource_IDR_MAINFRAME(parent);
      break;
   case IDR_PATTERN_POPUP:
      qtMfcInitMenuResource_IDR_PATTERN_POPUP(parent);
      break;
   case IDR_FRAME_POPUP:
      qtMfcInitMenuResource_IDR_FRAME_POPUP(parent);
      break;
   case IDR_INSTRUMENT_POPUP:
      qtMfcInitMenuResource_IDR_INSTRUMENT_POPUP(parent);
      break;
   case IDR_SAMPLES_POPUP:
      qtMfcInitMenuResource_IDR_SAMPLES_POPUP(parent);
      break;
   case IDR_SAMPLE_WND_POPUP:
      qtMfcInitMenuResource_IDR_SAMPLE_WND_POPUP(parent);
      break;
   case IDR_PATTERN_HEADER_POPUP:
      qtMfcInitMenuResource_IDR_PATTERN_HEADER_POPUP(parent);
      break;
   case IDR_SAMPLE_EDITOR_POPUP:
      qtMfcInitMenuResource_IDR_SAMPLE_EDITOR_POPUP(parent);
      break;
   case IDR_SEQUENCE_POPUP:
      qtMfcInitMenuResource_IDR_SEQUENCE_POPUP(parent);
      break;
   }
   // Fixup shortcuts
   int menu = 0;
   CMenu* subMenu = parent->GetSubMenu(menu);
   while ( subMenu )
   {
      foreach ( QAction* action, subMenu->toQMenu()->actions() )
      {
         if ( action->text().contains("\t") )
         {
            action->setShortcut(QKeySequence(action->text().split("\t").at(1)));
         }
      }
      menu++;
      subMenu = parent->GetSubMenu(menu);
   }
}

void qtMfcInitStringResources()
{
   /////////////////////////////////////////////////////////////////////////////
   //
   // String Table
   //

   qtMfcStringResources.clear();

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDR_MAINFRAME,"FamiTracker\n\nFamiTracker\nFamiTracker files (*.ftm)\n.ftm\nFamiTracker.Module\nFamiTracker.Module");
      qtMfcStringResources.insert(IDS_FRAME_DROP_MOVE,"Drop selection to move frames");
      qtMfcStringResources.insert(IDS_DPCM_IMPORT_QUALITY_FORMAT,"Quality: %1");
      qtMfcStringResources.insert(IDS_CLIPBOARD_ERROR,"Could not register clipboard format");
      qtMfcStringResources.insert(IDS_FILE_OPEN_ERROR,"Could not open the file!");
      qtMfcStringResources.insert(IDS_FILE_VALID_ERROR,"File is not a valid FamiTracker module");
      qtMfcStringResources.insert(IDS_FILE_VERSION_ERROR,"File version is not supported");
      qtMfcStringResources.insert(IDS_INST_LIMIT,"You cannot add more instruments");
      qtMfcStringResources.insert(IDS_CLIPBOARD_OPEN_ERROR,"Cannot open clipboard.");
      qtMfcStringResources.insert(IDS_CLIPBOARD_NOT_AVALIABLE,"Clipboard data is not available.");
      qtMfcStringResources.insert(ID_TRACKER_PLAY_START,"Play module from start");
      qtMfcStringResources.insert(IDS_INSTRUMENT_FILE_FAIL,"Instrument file was not vaild");
      qtMfcStringResources.insert(IDS_CLIPPING_MESSAGE,"Warning: Audio is clipping, decrease mixing levels!");
      qtMfcStringResources.insert(IDS_FILTER_FTI,"FamiTracker Instrument (*.fti)");
      qtMfcStringResources.insert(IDS_SAVE_FILE_ERROR,"Could not save file: %1");
      qtMfcStringResources.insert(IDS_START_ERROR,"Program could not start properly, default settings has been restored. Please restart the program.");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(AFX_IDS_APP_TITLE,"FamiTracker");
      qtMfcStringResources.insert(AFX_IDS_IDLEMESSAGE,"For Help, press F1");
      qtMfcStringResources.insert(AFX_IDS_HELPMODEMESSAGE,"Select an object on which to get Help");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_INDICATOR_EXT,"EXT");
      qtMfcStringResources.insert(ID_INDICATOR_CAPS,"CAP");
      qtMfcStringResources.insert(ID_INDICATOR_NUM,"NUM");
      qtMfcStringResources.insert(ID_INDICATOR_SCRL,"SCRL");
      qtMfcStringResources.insert(ID_INDICATOR_OVR,"OVR");
      qtMfcStringResources.insert(ID_INDICATOR_REC,"REC");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_FILE_NEW,"Create a new document\nNew");
      qtMfcStringResources.insert(ID_FILE_OPEN,"Open an existing document\nOpen");
      qtMfcStringResources.insert(ID_FILE_CLOSE,"Close the active document\nClose");
      qtMfcStringResources.insert(ID_FILE_SAVE,"Save the active document\nSave");
      qtMfcStringResources.insert(ID_FILE_SAVE_AS,"Save the active document with a new name\nSave As");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_APP_ABOUT,"Display program information, version number and copyright\nAbout");
      qtMfcStringResources.insert(ID_APP_EXIT,"Quit the application; prompts to save documents\nExit");
      qtMfcStringResources.insert(ID_HELP_INDEX,"Opens Help\nHelp Topics");
      qtMfcStringResources.insert(ID_HELP_FINDER,"List Help topics\nHelp Topics");
      qtMfcStringResources.insert(ID_HELP_USING,"Display instructions about how to use help\nHelp");
      qtMfcStringResources.insert(ID_CONTEXT_HELP,"Display help for clicked on buttons, menus and windows\nHelp");
      qtMfcStringResources.insert(ID_HELP,"Display help for current task or command\nHelp");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_FILE_MRU_FILE1,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE2,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE3,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE4,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE5,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE6,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE7,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE8,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE9,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE10,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE11,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE12,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE13,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE14,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE15,"Open this document");
      qtMfcStringResources.insert(ID_FILE_MRU_FILE16,"Open this document");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_NEXT_PANE,"Switch to the next window pane\nNext Pane");
      qtMfcStringResources.insert(ID_PREV_PANE,"Switch back to the previous window pane\nPrevious Pane");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_WINDOW_SPLIT,"Split the active window into panes\nSplit");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_EDIT_CLEAR,"Erase the selection\nErase");
      qtMfcStringResources.insert(ID_EDIT_CLEAR_ALL,"Erase everything\nErase All");
      qtMfcStringResources.insert(ID_EDIT_COPY,"Copy the selection and put it on the Clipboard\nCopy");
      qtMfcStringResources.insert(ID_EDIT_CUT,"Cut the selection and put it on the Clipboard\nCut");
      qtMfcStringResources.insert(ID_EDIT_FIND,"Find the specified text\nFind");
      qtMfcStringResources.insert(ID_EDIT_PASTE,"Insert Clipboard contents\nPaste");
      qtMfcStringResources.insert(ID_EDIT_REPEAT,"Repeat the last action\nRepeat");
      qtMfcStringResources.insert(ID_EDIT_REPLACE,"Replace specific text with different text\nReplace");
      qtMfcStringResources.insert(ID_EDIT_SELECT_ALL,"Select the entire document\nSelect All");
      qtMfcStringResources.insert(ID_EDIT_UNDO,"Undo the last action\nUndo");
      qtMfcStringResources.insert(ID_EDIT_REDO,"Redo the previously undone action\nRedo");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_VIEW_TOOLBAR,"Show or hide the toolbar\nToggle ToolBar");
      qtMfcStringResources.insert(ID_VIEW_STATUS_BAR,"Show or hide the status bar\nToggle StatusBar");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(AFX_IDS_SCSIZE,"Change the window size");
      qtMfcStringResources.insert(AFX_IDS_SCMOVE,"Change the window position");
      qtMfcStringResources.insert(AFX_IDS_SCMINIMIZE,"Reduce the window to an icon");
      qtMfcStringResources.insert(AFX_IDS_SCMAXIMIZE,"Enlarge the window to full size");
      qtMfcStringResources.insert(AFX_IDS_SCNEXTWINDOW,"Switch to the next document window");
      qtMfcStringResources.insert(AFX_IDS_SCPREVWINDOW,"Switch to the previous document window");
      qtMfcStringResources.insert(AFX_IDS_SCCLOSE,"Close the active window and prompts to save the documents");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(AFX_IDS_SCRESTORE,"Restore the window to normal size");
      qtMfcStringResources.insert(AFX_IDS_SCTASKLIST,"Activate Task List");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_TRACKER_PLAY,"Play module\nPlay");
      qtMfcStringResources.insert(ID_TRACKER_PLAYPATTERN,"Play and loop current pattern\nPlay and loop");
      qtMfcStringResources.insert(ID_TRACKER_STOP,"Stop playing\nStop");
      qtMfcStringResources.insert(ID_TRACKER_EDIT,"Enable/disable edit mode\nToggle edit mode");
      qtMfcStringResources.insert(ID_TRACKER_KILLSOUND,"Promptly stop all channels");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_EDIT_DELETE,"Delete selection, or note if there is no selection.");
      qtMfcStringResources.insert(ID_EDIT_UNLOCKCURSOR,"Makes it possible to edit without scrolling the view");
      qtMfcStringResources.insert(ID_TRACKER_NTSC,"Switch to NTSC emulation");
      qtMfcStringResources.insert(ID_TRACKER_PAL,"Switch to PAL emulation");
      qtMfcStringResources.insert(ID_SPEED_DEFALUT,"Default playback rate (60 Hz for NTSC, 50 Hz for PAL)");
      qtMfcStringResources.insert(ID_SPEED_CUSTOM,"Set the playback rate manually");
      qtMfcStringResources.insert(ID_HELP_PERFORMANCE,"Display application performance");
      qtMfcStringResources.insert(ID_EDIT_PASTEOVERWRITE,"Pasted notes will overwrite existing notes");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_FILE_CREATE_NSF,"Brings up the create NSF dialog\nCreate NSF");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_TRANSPOSE_INCREASENOTE,"Increase selected note/notes by one step");
      qtMfcStringResources.insert(ID_TRANSPOSE_DECREASENOTE,"Decrease selected note/notes by one step");
      qtMfcStringResources.insert(ID_TRANSPOSE_INCREASEOCTAVE,"Increase selected octave/octaves by one step");
      qtMfcStringResources.insert(ID_TRANSPOSE_DECREASEOCTAVE,"Decrease selected octave/octaves by one step");
      qtMfcStringResources.insert(ID_EDIT_SELECTALL,"Select pattern or all patterns\nSelect All");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_FILE_GENERALSETTINGS,"Opens FamiTracker settings\nSettings");
      qtMfcStringResources.insert(ID_EDIT_ENABLEMIDI,"Temporarly enable / disable commands from the MIDI device");
      qtMfcStringResources.insert(ID_FRAME_INSERT,"Insert a new frame on current location\nAdd frame");
      qtMfcStringResources.insert(ID_FRAME_REMOVE,"Remove current frame\nRemove frame");
      qtMfcStringResources.insert(ID_TRACKER_PLAYROW,"Play a single row");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_CREATE_NSF,"Export module to NSF");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_SPEED_DEFAULT,"Chooses default playback rate corresponding to selected video standard");
      qtMfcStringResources.insert(ID_MODULE_MODULEPROPERTIES,
                               "Opens the module properties window\nModule properties");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDS_FILE_LOAD_ERROR,"Couldn't load file properly. Make sure you have the latest version of FamiTracker.");
      qtMfcStringResources.insert(IDS_CONFIG_WINDOW,"FamiTracker configuration");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_EDIT_PASTEMIX,"Pasted notes will mix with existing notes");
      qtMfcStringResources.insert(ID_MODULE_MOVEFRAMEDOWN,"Move selected frame one step down\nMove frame down");
      qtMfcStringResources.insert(ID_MODULE_MOVEFRAMEUP,"Move selected frame one step up\nMove frame up");
      qtMfcStringResources.insert(ID_MODULE_SAVEINSTRUMENT,"Save instrument to file\nSave instrument");
      qtMfcStringResources.insert(ID_MODULE_LOADINSTRUMENT,"Load instrument from file\nLoad instrument");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_MODULE_ADDINSTRUMENT,"Add a new instrument to the module\nAdd instrument");
      qtMfcStringResources.insert(ID_MODULE_REMOVEINSTRUMENT,
                               "Remove instrument from module\nRemove instrument");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_NEXT_SONG,"Select next song\nNext song");
      qtMfcStringResources.insert(ID_PREV_SONG,"Select previous song\nPrevious song");
      qtMfcStringResources.insert(ID_EDIT_INSTRUMENTMASK,"Disables insertion of values in the instrument column");
      qtMfcStringResources.insert(ID_TRACKER_SWITCHTOTRACKINSTRUMENT,
                               "Switch to channel instruments when playing");
      qtMfcStringResources.insert(ID_FRAME_INSERT_UNIQUE,"Insert new frame with empty patterns");
      qtMfcStringResources.insert(ID_FILE_CREATEWAV,"Saves the file as a WAV-file\nCreate WAV");
      qtMfcStringResources.insert(ID_VIEW_CONTROLPANEL,"Show or hide the control panel");
      qtMfcStringResources.insert(ID_EDIT_CLEARPATTERNS,"Resets all patterns and frames");
      qtMfcStringResources.insert(ID_EDIT_INTERPOLATE,"Interpolates a selection between two values\nInterpolate");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_TRACKER_SOLOCHANNEL,"Mutes/unmutes all but the selected channel");
      qtMfcStringResources.insert(ID_TRACKER_TOGGLECHANNEL,"Mute/unmute selected channel");
      qtMfcStringResources.insert(ID_EDIT_GRADIENT,"Interpolates a selection of values");
      qtMfcStringResources.insert(ID_MODULE_EDITINSTRUMENT,"Open instrument editor\nEdit instrument");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDS_INFO_TITLE,"(title)");
      qtMfcStringResources.insert(IDS_INFO_AUTHOR,"(author)");
      qtMfcStringResources.insert(IDS_OUT_OF_SLOTS,"Out of sample slots");
      qtMfcStringResources.insert(IDS_OUT_OF_SAMPLEMEM_FORMAT,
                               "Couldn't load sample, out of sample memory (max %1 kB is avaliable)");
      qtMfcStringResources.insert(IDS_DPCM_IMPORT_GAIN_FORMAT,"Gain: %1 dB");
      qtMfcStringResources.insert(IDS_OPEN_FILE_ERROR,"Could not open file: %1");
      qtMfcStringResources.insert(IDS_INFO_COPYRIGHT,"(copyright)");
      qtMfcStringResources.insert(IDS_IMPORT_FAILED,"Import module failed");
      qtMfcStringResources.insert(IDS_IMPORT_CHIP_MISMATCH,
                               "Imported file must be of the same expansion chip type as current file.");
      qtMfcStringResources.insert(IDS_IMPORT_INSTRUMENT_COUNT,"Can't import file, out of instrument slots!");
      qtMfcStringResources.insert(IDS_IMPORT_SAMPLE_SLOTS,"Could not import all samples, out of sample slots!");
      qtMfcStringResources.insert(IDS_DPCM_IMPORT_TITLE_FORMAT,"PCM Import (%1)");
      qtMfcStringResources.insert(IDS_DPCM_IMPORT_WAVE_FORMAT,"%1 Hz, %2 bits, %3");
      qtMfcStringResources.insert(IDS_DPCM_IMPORT_TARGET_FORMAT,"Target sample rate: %1 Hz");
      qtMfcStringResources.insert(IDS_PERFORMANCE_FRAMERATE_FORMAT,"Frame rate: %1 Hz");
      qtMfcStringResources.insert(IDS_PERFORMANCE_UNDERRUN_FORMAT,"Underruns: %1");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_MODULE_DUPLICATEFRAME,
                               "Inserts a copy of current frame\nDuplicate frame");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_EDIT_REVERSE,"Reverses a selection\nReverse");
      qtMfcStringResources.insert(ID_TRACKER_DPCM,"Display information about the DPCM channel");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_INSTRUMENT_REMOVE,"Remove instrument from module\nRemove instrument");
      qtMfcStringResources.insert(ID_INSTRUMENT_EDIT,"Open instrument edtor\nEdit instrument");
      qtMfcStringResources.insert(ID_INSTRUMENT_CLONE,"Create a copy of selected instrument\nClone instrument");
      qtMfcStringResources.insert(ID_EDIT_REPLACEINSTRUMENT,
                               "Replaces all instruments in a selection with the selected instrument\nReplace instrument");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_CLEANUP_REMOVEUNUSEDPATTERNS,
                               "Removes all unused patterns\nRemove unused patterns");
      qtMfcStringResources.insert(ID_HELP_EFFECTTABLE,"Open help window with effect table");
      qtMfcStringResources.insert(ID_CLEANUP_REMOVEUNUSEDINSTRUMENTS,
                               "Removes all unused instruments\nRemove unused instruments");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_MODULE_FRAME_INSERT,"Insert a new frame on current location");
      qtMfcStringResources.insert(ID_MODULE_FRAME_REMOVE,"Removes selected frame");
      qtMfcStringResources.insert(ID_MODULE_REMOVEFRAME,"Removes selected frame\nRemove frame");
      qtMfcStringResources.insert(ID_MODULE_COMMENTS,"Open module comments dialog\nComments");
      qtMfcStringResources.insert(ID_MODULE_INSERTFRAME,"Inserts a new frame\nInsert frame");
      qtMfcStringResources.insert(ID_INSTRUMENT_NEW,"Add a new instrument to the module\nAdd instrument");
      qtMfcStringResources.insert(ID_INSTRUMENT_LOAD,"Load instrument from file\nLoad instrument");
      qtMfcStringResources.insert(ID_INSTRUMENT_SAVE,"Save instrument to file\nSave instrument");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDS_SAVE_ERROR,"Couldn't save file, check that it isn't write protected or shared with other applications and enough disk space is available.");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDC_OPT_WRAPCURSOR,"Wrap around the cursor when reaching top or bottom in the pattern editor.");
      qtMfcStringResources.insert(IDC_OPT_FREECURSOR,"Unlocks the cursor from the center of the pattern editor.");
      qtMfcStringResources.insert(IDC_OPT_WAVEPREVIEW,"Preview wave and DPCM files in the open file dialog when loading samples to the module.");
      qtMfcStringResources.insert(IDC_OPT_KEYREPEAT,"Enable key repetition in the pattern editor.");
      qtMfcStringResources.insert(IDC_OPT_HEXROW,"Display row numbers in hexadecimal.");
      qtMfcStringResources.insert(IDC_OPT_WRAPFRAMES,"Move to next or previous frame when reaching top or bottom in the pattern editor.");
      qtMfcStringResources.insert(IDC_OPT_FRAMEPREVIEW,"Preview next and previous frame in the pattern editor.");
      qtMfcStringResources.insert(IDC_OPT_NODPCMRESET,"Prevents resetting the DPCM channel on note stop commands.");
      qtMfcStringResources.insert(IDC_OPT_NOSTEPMOVE,"Ignore the pattern step setting when moving the cursor, only use it when inserting notes.");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDC_OPT_PULLUPDELETE,"Makes delete key pull up rows rather than only deleting the value.");
      qtMfcStringResources.insert(IDC_OPT_BACKUPS,"Creates a backup copy of the existing file when saving a module.");
      qtMfcStringResources.insert(IDC_OPT_SINGLEINSTANCE,"Only allow one single instance of the FamiTracker application.");
      qtMfcStringResources.insert(IDC_OPT_PREVIEWFULLROW,"Preview all channels when inserting notes in the pattern editor.");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_MODULE_DUPLICATEFRAMEPATTERNS,
                               "Inserts a copy of current frame including all patterns\nDuplicate patterns");
      qtMfcStringResources.insert(ID_EDIT_EXPANDPATTERNS,"Expands selected pattern\nExpand pattern");
      qtMfcStringResources.insert(ID_EDIT_SHRINKPATTERNS,"Shrinks selected pattern\nShrink pattern");
      qtMfcStringResources.insert(ID_FRAMEEDITOR_TOP,"Moves the frame editor to the top of the window (default)\nTop");
      qtMfcStringResources.insert(ID_FRAMEEDITOR_LEFT,"Moves the frame editor to the left of the window\nLeft");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_INSTRUMENT_DEEPCLONE,"Creates a copy of selected instrument including all sequences\nDeep clone instrument");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_TRACKER_PLAY_CURSOR,"Play pattern from cursor");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDS_CLEARPATTERN,"Do you want to reset all patterns and frames? There is no undo for this command.");
      qtMfcStringResources.insert(IDS_CLIPBOARD_COPY_ERROR,"An error occured while trying to copy data to the clipboard.");
      qtMfcStringResources.insert(IDS_CLIPBOARD_PASTE_ERROR,"An error occured while trying to paste data from the clipboard.");
      qtMfcStringResources.insert(IDS_DPCM_SPACE_FORMAT,"Space used %1 kB, left %2 kB (%3 kB available)");
      qtMfcStringResources.insert(IDS_FILE_VERSION_TOO_NEW,
                               "This file was created in a newer version of FamiTracker. Please download the most recent version to open this file.");
      qtMfcStringResources.insert(IDS_INSTRUMENT_EDITOR_TITLE,"Instrument editor - %1");
      qtMfcStringResources.insert(IDS_INST_VERSION_UNSUPPORTED,"File version not supported!");
      qtMfcStringResources.insert(IDS_FILE_SAVED,"File saved (%1 bytes)");
      qtMfcStringResources.insert(IDS_MIDI_ERR_INPUT,"MIDI Error: Could not open MIDI input device!");
      qtMfcStringResources.insert(IDS_MIDI_ERR_OUTPUT,"MIDI Error: Could not open MIDI output device!");
      qtMfcStringResources.insert(IDS_NEW_SOUND_CONFIG,"New sound configuration has been loaded");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDS_REMOVE_INSTRUMENTS,"Do you want to remove all unused instruments? There is no undo for this action.");
      qtMfcStringResources.insert(IDS_REMOVE_PATTERNS,"Do you want to remove all unused patterns? There is no undo for this action.");
      qtMfcStringResources.insert(IDS_SONG_DELETE,"Do you want to delete this song? There is no undo for this action.");
      qtMfcStringResources.insert(IDS_SOUND_FAIL,"It appears the current sound settings aren't working, change settings and try again.");
      qtMfcStringResources.insert(IDS_UNDERRUN_MESSAGE,"Warning: Audio buffer underrun, increase the audio buffer size!");
      qtMfcStringResources.insert(IDS_WELCOME_VER_FORMAT,"Welcome to FamiTracker %s, press F1 for help");
      qtMfcStringResources.insert(IDS_DPCM_IMPORT_INVALID_WAVEFILE,
                               "Cannot load this sample, file is invalid or incompatible. (Only uncompressed PCM is supported.)");
      qtMfcStringResources.insert(IDS_INST_FILE_ERROR,"Could not load instrument file! Check that you have the most recent version of FamiTracker.");
      qtMfcStringResources.insert(IDS_NEW_2A03_INSTRUMENT,"New 2A03 instrument");
      qtMfcStringResources.insert(IDS_NEW_VRC6_INSTRUMENT,"New VRC6 instrument");
      qtMfcStringResources.insert(IDS_NEW_VRC7_INSTRUMENT,"New VRC7 instrument");
      qtMfcStringResources.insert(IDS_NEW_FDS_INSTRUMENT,"New FDS instrument");
      qtMfcStringResources.insert(IDS_NEW_N163_INSTRUMENT,"New Namco instrument");
      qtMfcStringResources.insert(IDS_NEW_S5B_INSTRUMENT,"New Sunsoft instrument");
      qtMfcStringResources.insert(IDS_FRAME_DROP_COPY_NEW,"Drop selection to copy frames to new pattern numbers");
      qtMfcStringResources.insert(IDS_FRAME_DROP_COPY,"Drop selection to copy frames");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_INDICATOR_CHIP,"No expansion chip");
      qtMfcStringResources.insert(ID_INDICATOR_INSTRUMENT,"Instrument: %1");
      qtMfcStringResources.insert(ID_INDICATOR_OCTAVE,"Octave: %1");
      qtMfcStringResources.insert(ID_INDICATOR_RATE,"000 Hz");
      qtMfcStringResources.insert(ID_INDICATOR_TEMPO,"000.00 BPM");
      qtMfcStringResources.insert(ID_INDICATOR_TIME,"00:00:00 ");
      qtMfcStringResources.insert(ID_INDICATOR_POS,"00 / 00    ");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_HELP_FAQ,"Open help window with the FAQ");
      qtMfcStringResources.insert(ID_TRACKER_DISPLAYREGISTERSTATE,
                              "Display channel register values and tuning information");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDS_WAVE_PROGRESS_FILE_FORMAT,"Saving to: %1");
      qtMfcStringResources.insert(IDS_WAVE_PROGRESS_FRAME_FORMAT,"Frame: %1 (%2 done)");
      qtMfcStringResources.insert(IDS_WAVE_PROGRESS_TIME_FORMAT,"Time: %1 (%2 done)");
      qtMfcStringResources.insert(IDS_WAVE_PROGRESS_ELAPSED_FORMAT,"Elapsed time: %1");
      qtMfcStringResources.insert(IDS_DSOUND_ERROR,"DirectX: DirectSound error!");
      qtMfcStringResources.insert(IDS_DSOUND_BUFFER_ERROR,"DirectX: DirectSound error: Could not create buffer!");
      qtMfcStringResources.insert(IDS_EDIT_MODE,"Changed to edit mode");
      qtMfcStringResources.insert(IDS_NORMAL_MODE,"Changed to normal mode");
      qtMfcStringResources.insert(IDS_MIDI_MESSAGE_ON_FORMAT,
                               "MIDI message: Note on (note = %1, octave = %2, velocity = %3)");
      qtMfcStringResources.insert(IDS_MIDI_MESSAGE_OFF,"MIDI message: Note off");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDC_OPT_DOUBLECLICK,"Don't select the whole channel when double-clicking in the pattern editor.");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(IDS_LOADING_FILE,"Loading file...");
      qtMfcStringResources.insert(IDS_FILTER_DMC,"Delta modulated samples (*.dmc)");
      qtMfcStringResources.insert(IDS_FILTER_WAV,"Waveform Audio File Format (*.wav)");
      qtMfcStringResources.insert(IDS_FILTER_TXT,"Text files (*.txt)");
      qtMfcStringResources.insert(IDS_DPCM_EDIT_INFO_FORMAT,"Wave end: %1, Size: %2 bytes ");
      qtMfcStringResources.insert(IDS_WAVE_EXPORT_DONE,"Done");
      qtMfcStringResources.insert(IDS_WAVE_EXPORT_FINISHED,"finished");
      qtMfcStringResources.insert(IDS_ABOUT_TOOLTIP_MAIL,"Send mail to jsr@famitracker.com");
      qtMfcStringResources.insert(IDS_ABOUT_TOOLTIP_WEB,"Go to http://www.famitracker.com");
      qtMfcStringResources.insert(IDS_ABOUT_VERSION_FORMAT,"FamiTracker version %1");
      qtMfcStringResources.insert(IDS_PROPERTIES_CHANNELS,"Channels:");
      qtMfcStringResources.insert(IDS_LOADING_DONE,"File loaded");
      qtMfcStringResources.insert(IDS_DPCM_IMPORT_SIZE_FORMAT,"Converted size: %1 bytes");
      qtMfcStringResources.insert(IDS_INSTRUMENT_FOLDER,"Choose a folder where you have instrument files");
//   END

//   STRINGTABLE
//   BEGIN
      qtMfcStringResources.insert(ID_FILE_IMPORTTEXT,"Imports pattern text files");
      qtMfcStringResources.insert(ID_FILE_EXPORTTEXT,"Exports pattern text files");
      qtMfcStringResources.insert(ID_EDIT_VOLUMEMASK,"Disables insertion of values in the volume column");
//   END

// AFX resources
   qtMfcStringResources.insert(AFX_IDS_ALLFILTER,"All files|");
   qtMfcStringResources.insert(AFX_IDS_OPENFILE,"Open");
   qtMfcStringResources.insert(AFX_IDS_SAVEFILE,"Save As");
   qtMfcStringResources.insert(AFX_IDS_SAVEFILECOPY,"Save As");
   qtMfcStringResources.insert(AFX_IDS_UNTITLED,"Untitled");
   qtMfcStringResources.insert(AFX_IDP_ASK_TO_SAVE,"Save changes to %s?");
   qtMfcStringResources.insert(AFX_IDP_FAILED_TO_CREATE_DOC,"Failed to create empty document.");
}

/////////////////////////////////////////////////////////////////////////////
//
// Accelerator
//

//IDR_FRAMEWND ACCELERATORS
ACCEL ACCEL_IDR_FRAMEWND[] = 
{
//BEGIN
//    "C",            ID_EDIT_COPY,           VIRTKEY, CONTROL, NOINVERT
   { FVIRTKEY|FCONTROL|FNOINVERT, 'C', ID_EDIT_COPY },
//   VK_DELETE,      ID_EDIT_DELETE,         VIRTKEY, NOINVERT
   { FVIRTKEY|FNOINVERT, VK_DELETE, ID_EDIT_DELETE },
//    "V",            ID_EDIT_PASTE,          VIRTKEY, CONTROL, NOINVERT
   { FVIRTKEY|FCONTROL|FNOINVERT, 'V', ID_EDIT_PASTE },
//    "Y",            ID_EDIT_REDO,           VIRTKEY, CONTROL, NOINVERT
   { FVIRTKEY|FCONTROL|FNOINVERT, 'Y', ID_EDIT_REDO },
//    "Z",            ID_EDIT_UNDO,           VIRTKEY, CONTROL, NOINVERT
   { FVIRTKEY|FCONTROL|FNOINVERT, 'Z', ID_EDIT_UNDO },
//    VK_DOWN,        ID_MODULE_MOVEFRAMEDOWN, VIRTKEY, CONTROL, NOINVERT
   { FVIRTKEY|FCONTROL|FNOINVERT, VK_DOWN, ID_MODULE_MOVEFRAMEDOWN },
//    VK_UP,          ID_MODULE_MOVEFRAMEUP,  VIRTKEY, CONTROL, NOINVERT
   { FVIRTKEY|FCONTROL|FNOINVERT, VK_UP, ID_MODULE_MOVEFRAMEUP },
//   "X",            ID_EDIT_CUT,            VIRTKEY, CONTROL, NOINVERT
   { FVIRTKEY|FCONTROL|FNOINVERT, 'X', ID_EDIT_CUT },
//END
   { 0, 0, 0 },
};

ACCEL* qtMfcAcceleratorResource(UINT id)
{
   switch ( id )
   {
   case IDR_FRAMEWND:
      return ACCEL_IDR_FRAMEWND;
      break;
   }
   return NULL;
}

void qtMfcInitBitmapResources()
{
   /////////////////////////////////////////////////////////////////////////////
   //
   // Bitmap
   //
   qtMfcBitmapResources.clear();

//   IDB_ABOUT               BITMAP                  "res\\About.bmp"
   qtMfcBitmapResources.insert(IDB_ABOUT,new CBitmap(":/resources/About.bmp"));
//   IDB_KEY_BLACK           BITMAP                  "res\\key_black_unpressed.bmp"
   qtMfcBitmapResources.insert(IDB_KEY_BLACK,new CBitmap(":/resources/key_black_unpressed.bmp"));
//   IDR_MAINFRAME           BITMAP                  "res\\MainToolbar-16.bmp"
   qtMfcBitmapResources.insert(IDR_MAINFRAME,new CBitmap(":/resources/MainToolbar-16.bmp"));
//   IDT_INSTRUMENT          BITMAP                  "res\\InstrumentToolbar-16.bmp"
   qtMfcBitmapResources.insert(IDT_INSTRUMENT,new CBitmap(":/resources/InstrumentToolbar-16.bmp"));
//   IDB_VISUALIZER          BITMAP                  "res\\VisualizerBg.bmp"
   qtMfcBitmapResources.insert(IDB_VISUALIZER,new CBitmap(":/resources/VisualizerBg.bmp"));
//   IDB_KEY_BLACK_MARK      BITMAP                  "res\\key_black_pressed.bmp"
   qtMfcBitmapResources.insert(IDB_KEY_BLACK_MARK,new CBitmap(":/resources/key_black_pressed.bmp"));
//   IDB_KEY_WHITE           BITMAP                  "res\\key_white_unpressed.bmp"
   qtMfcBitmapResources.insert(IDB_KEY_WHITE,new CBitmap(":/resources/key_white_unpressed.bmp"));
//   IDB_KEY_WHITE_MARK      BITMAP                  "res\\key_white_pressed.bmp"
   qtMfcBitmapResources.insert(IDB_KEY_WHITE_MARK,new CBitmap(":/resources/key_white_pressed.bmp"));
//   IDB_TOOLBAR_256         BITMAP                  "res\\MainToolbar-256.bmp"
   qtMfcBitmapResources.insert(IDB_TOOLBAR_256,new CBitmap(":/resources/MainToolbar-256.bmp"));
//   IDB_TOOLBAR_INST_256    BITMAP                  "res\\InstrumentToolbar-256.bmp"
   qtMfcBitmapResources.insert(IDB_TOOLBAR_INST_256,new CBitmap(":/resources/InstrumentToolbar-256.bmp"));
}

void qtInitIconResources()
{
   /////////////////////////////////////////////////////////////////////////////
   //
   // Bitmap
   //
   qtIconNames.clear();
   qtIconResources.clear();

   // Icon with lowest ID value placed first to ensure application icon
   // remains consistent on all systems.

//   IDR_MAINFRAME           ICON                    "res\\Application.ico"
   qtIconNames.insert(IDR_MAINFRAME,":/resources/Application.ico");
//   IDI_INST_2A03           ICON                    "res\\Inst_2A03.ico"
   qtIconNames.insert(IDI_INST_2A03,":/resources/Inst_2A03.ico");
//   IDI_INST_VRC6           ICON                    "res\\Inst_VRC6.ico"
   qtIconNames.insert(IDI_INST_VRC6,":/resources/Inst_VRC6.ico");
//   IDI_INST_VRC7           ICON                    "res\\Inst_VRC7.ico"
   qtIconNames.insert(IDI_INST_VRC7,":/resources/Inst_VRC7.ico");
//   IDR_FamiTrackerTYPE     ICON                    "res\\Document.ico"
   qtIconNames.insert(IDR_FamiTrackerTYPE,":/resources/Document.ico");
//   IDI_INST_N163           ICON                    "res\\Inst_N163.ico"
   qtIconNames.insert(IDI_INST_N163,":/resources/Inst_N163.ico");
//   IDI_INST_FDS            ICON                    "res\\Inst_FDS.ico"
   qtIconNames.insert(IDI_INST_FDS,":/resources/Inst_FDS.ico");
//   IDI_INST_S5B             ICON                    "res\\Inst_S5B.ico"
   qtIconNames.insert(IDI_INST_S5B,":/resources/Inst_S5B.ico");
//   IDI_INST_2A07           ICON                    "res\\Inst_2A07.ico"
   qtIconNames.insert(IDI_INST_2A07,":/resources/Inst_2A07.ico");
//   IDI_LEFT                ICON                    "res\\LeftArrow.ico"
   qtIconNames.insert(IDI_LEFT,":/resources/LeftArrow.ico");
//   IDI_RIGHT               ICON                    "res\\RightArrow.ico"
   qtIconNames.insert(IDI_RIGHT,":/resources/RightArrow.ico");
}

void qtMfcInitDialogResource_IDD_ABOUTBOX(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//IDD_ABOUTBOX DIALOGEX 0, 0, 285, 336
//STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//CAPTION "About FamiTracker"
   parent->SetWindowText("About FamiTracker");
//FONT 8, "MS Shell Dlg", 0, 0, 0x1
      CRect rect(CPoint(0,0),CSize(285,336));
      parent->MapDialogRect(&rect);
      parent->setFixedSize(rect.Width(),rect.Height());
//BEGIN
//      LTEXT           "Copyright (C) 2005 - 2015 jsr",IDC_ABOUT3,54,92,167,11
      CStatic* mfc2 = new CStatic(parent);
      CRect r2(CPoint(54,92),CSize(167,11));
      parent->MapDialogRect(&r2);
      mfc2->Create(_T("Copyright (C) 2005 - 2015 jsr"),SS_NOPREFIX | WS_VISIBLE,r2,parent,IDC_ABOUT3);
      mfcToQtWidget->insert(IDC_ABOUT3,mfc2);
//      DEFPUSHBUTTON   "OK",IDOK,228,64,50,16,WS_GROUP
      CButton* mfc3 = new CButton(parent);
      CRect r3(CPoint(228,64),CSize(50,16));
      parent->MapDialogRect(&r3);
      mfc3->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_GROUP | WS_VISIBLE,r3,parent,IDOK);
      mfcToQtWidget->insert(IDOK,mfc3);
//      LTEXT           "? FDS sound emulator from nezplug (including a fix by rainwarrior)",IDC_STATIC,54,313,224,8
      CStatic* mfc4 = new CStatic(parent);
      CRect r4(CPoint(54,313),CSize(224,8));
      parent->MapDialogRect(&r4);
      mfc4->Create(_T("? FDS sound emulator from nezplug (including a fix by rainwarrior)"),WS_VISIBLE,r4,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "E-mail:",IDC_STATIC,54,108,24,10
      CStatic* mfc5 = new CStatic(parent);
      CRect r5(CPoint(54,108),CSize(24,10));
      parent->MapDialogRect(&r5);
      mfc5->Create(_T("E-mail:"),WS_VISIBLE,r5,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "This software is licensed under the GNU General Public\n License. Source is available on the webpage.",IDC_STATIC,54,167,224,17
      CStatic* mfc6 = new CStatic(parent);
      CRect r6(CPoint(54,167),CSize(224,17));
      parent->MapDialogRect(&r6);
      mfc6->Create(_T("This software is licensed under the GNU General Public\n License. Source is available on the webpage."),WS_VISIBLE,r6,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      ICON            IDR_MAINFRAME,IDC_STATIC,7,63,20,20,SS_REALSIZEIMAGE
      CBitmap* mfc7 = new CBitmap(qtIconName(IDR_MAINFRAME));
      CRect r7(CPoint(7,63),CSize(20,20));
      parent->MapDialogRect(&r7);
      CStatic* mfc0 = new CStatic(parent);
      CRect r0(CPoint(7,63),CSize(20,20));
      parent->MapDialogRect(&r0);
      mfc0->Create(_T(""),SS_REALSIZEIMAGE | WS_VISIBLE,r0,parent,IDC_STATIC);
      mfc0->SetBitmap((HBITMAP)mfc7);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "Contributions:",IDC_STATIC,54,191,224,9
      CStatic* mfc8 = new CStatic(parent);
      CRect r8(CPoint(54,191),CSize(224,9));
      parent->MapDialogRect(&r8);
      mfc8->Create(_T("Contributions:"),WS_VISIBLE,r8,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "All comments and feedback are welcome\nGreets to everyone at the 2A03.org forum and everyone who has contributed with bug reports and suggestions!",IDC_STATIC,54,137,224,26
      CStatic* mfc9 = new CStatic(parent);
      CRect r9(CPoint(54,137),CSize(224,26));
      parent->MapDialogRect(&r9);
      mfc9->Create(_T("All comments and feedback are welcome\nGreets to everyone at the 2A03.org forum and everyone who has contributed with bug reports and suggestions!"),WS_VISIBLE,r9,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "jsr@famitracker.com",IDC_MAIL,84,108,70,10,SS_NOTIFY
      CStatic* mfc10 = new CStatic(parent);
      CRect r10(CPoint(84,108),CSize(70,10));
      parent->MapDialogRect(&r10);
      mfc10->Create(_T("jsr@famitracker.com"),SS_NOTIFY | WS_VISIBLE,r10,parent,IDC_MAIL);
      mfcToQtWidget->insert(IDC_MAIL,mfc10);
//      LTEXT           "http://www.famitracker.com",IDC_WEBPAGE,84,121,95,9,SS_NOTIFY
      CStatic* mfc11 = new CStatic(parent);
      CRect r11(CPoint(84,121),CSize(95,9));
      parent->MapDialogRect(&r11);
      mfc11->Create(_T("http://www.famitracker.com"),SS_NOTIFY | WS_VISIBLE,r11,parent,IDC_WEBPAGE);
      mfcToQtWidget->insert(IDC_WEBPAGE,mfc11);
//      LTEXT           "Web:",IDC_STATIC,54,121,24,9
      CStatic* mfc12 = new CStatic(parent);
      CRect r12(CPoint(54,121),CSize(24,9));
      parent->MapDialogRect(&r12);
      mfc12->Create(_T("Web:"),WS_VISIBLE,r12,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "Libraries:",IDC_STATIC,54,260,224,8
      CStatic* mfc13 = new CStatic(parent);
      CRect r13(CPoint(54,260),CSize(224,8));
      parent->MapDialogRect(&r13);
      mfc13->Create(_T("Libraries:"),WS_VISIBLE,r13,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "* Blip_buffer 0.4.0 is Copyright (C) blargg (http://www.slack.net/~ant/nes-emu/)\n",IDC_STATIC,54,273,224,16
      CStatic* mfc14 = new CStatic(parent);
      CRect r14(CPoint(54,273),CSize(224,16));
      parent->MapDialogRect(&r14);
      mfc14->Create(_T("* Blip_buffer 0.4.0 is Copyright (C) blargg (http://www.slack.net/~ant/nes-emu/)\n"),WS_VISIBLE,r14,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "* Fast Fourier Transform code is (C) Reliable Software",IDC_STATIC,54,292,224,8
      CStatic* mfc15 = new CStatic(parent);
      CRect r15(CPoint(54,292),CSize(224,8));
      parent->MapDialogRect(&r15);
      mfc15->Create(_T("* Fast Fourier Transform code is (C) Reliable Software"),WS_VISIBLE,r15,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "* YM2413 && YM2149 emulators are written by Mitsutaka Okazaki",IDC_STATIC,54,303,224,8
      CStatic* mfc16 = new CStatic(parent);
      CRect r16(CPoint(54,303),CSize(224,8));
      parent->MapDialogRect(&r16);
      mfc16->Create(_T("* YM2413 && YM2149 emulators are written by Mitsutaka Okazaki"),WS_VISIBLE,r16,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,54,271,215,1
//      LTEXT           "* Export plugin support by Gradualore",IDC_STATIC,54,204,224,9
      CStatic* mfc18 = new CStatic(parent);
      CRect r18(CPoint(54,204),CSize(224,9));
      parent->MapDialogRect(&r18);
      mfc18->Create(_T("* Export plugin support by Gradualore"),WS_VISIBLE,r18,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "* Icon is made by Kuhneghetz",IDC_STATIC,54,214,224,9
      CStatic* mfc19 = new CStatic(parent);
      CRect r19(CPoint(54,214),CSize(224,9));
      parent->MapDialogRect(&r19);
      mfc19->Create(_T("* Icon is made by Kuhneghetz"),WS_VISIBLE,r19,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "* Toolbar icons are made by ilkke",IDC_STATIC,54,223,224,9
      CStatic* mfc20 = new CStatic(parent);
      CRect r20(CPoint(54,223),CSize(224,9));
      parent->MapDialogRect(&r20);
      mfc20->Create(_T("* Toolbar icons are made by ilkke"),WS_VISIBLE,r20,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,54,201,211,1
//      LTEXT           "* DPCM import resampler by Jarhmander",IDC_STATIC,54,233,224,9
      CStatic* mfc22 = new CStatic(parent);
      CRect r22(CPoint(54,233),CSize(224,9));
      parent->MapDialogRect(&r22);
      mfc22->Create(_T("* DPCM import resampler by Jarhmander"),WS_VISIBLE,r22,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      LTEXT           "A Famicom/NES music tracker",IDC_ABOUT2,54,76,167,11,SS_NOPREFIX
      CStatic* mfc23 = new CStatic(parent);
      CRect r23(CPoint(54,76),CSize(167,11));
      parent->MapDialogRect(&r23);
      mfc23->Create(_T("A Famicom/NES music tracker"),SS_NOPREFIX | WS_VISIBLE,r23,parent,IDC_ABOUT2);
      mfcToQtWidget->insert(IDC_ABOUT2,mfc23);
//      LTEXT           "* Module text import/export by rainwarrior",IDC_STATIC,54,244,224,9
      CStatic* mfc24 = new CStatic(parent);
      CRect r24(CPoint(54,244),CSize(224,9));
      parent->MapDialogRect(&r24);
      mfc24->Create(_T("* Module text import/export by rainwarrior"),WS_VISIBLE,r24,parent,IDC_STATIC);
      // IDC_STATIC do not get added to MFC-to-Qt map.
//      CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,0,49,287,1
//      CONTROL         "",IDC_HEAD,"Static",SS_OWNERDRAW,0,0,287,49
      CStatic* mfc25 = new CStatic(parent);
      CRect r25(CPoint(0,0),CSize(287,49));
      parent->MapDialogRect(&r25);
      mfc25->Create(_T(""),SS_OWNERDRAW | WS_VISIBLE,r25,parent,IDC_HEAD);
      mfcToQtWidget->insert(IDC_HEAD,mfc25);
//      LTEXT           "A Famicom/NES music tracker",IDC_ABOUT1,54,58,167,11,SS_NOPREFIX
      CStatic* mfc26 = new CStatic(parent);
      CRect r26(CPoint(54,58),CSize(167,11));
      parent->MapDialogRect(&r26);
      mfc26->Create(_T("A Famicom/NES music tracker"),SS_NOPREFIX | WS_VISIBLE,r26,parent,IDC_ABOUT1);
      mfcToQtWidget->insert(IDC_ABOUT1,mfc26);
//END
}

void qtMfcInitDialogResource_IDD_MAINFRAME(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_MAINFRAME DIALOGEX 0, 0, 413, 126
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   EXSTYLE WS_EX_TRANSPARENT | WS_EX_CONTROLPARENT
//   FONT 8, "MS Shell Dlg", 400, 0, 0x0
//   BEGIN
   CRect rect(CPoint(0,0),CSize(413,126));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//       GROUPBOX        "Song settings",IDC_STATIC,7,5,78,71
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,5),CSize(78,71));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Song settings"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Edit settings",IDC_STATIC,7,78,78,43,0,WS_EX_TRANSPARENT
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(7,78),CSize(78,43));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Edit settings"),BS_GROUPBOX | WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Song information",IDC_STATIC,91,5,96,61
   CButton* mfc19 = new CButton(parent);
   CRect r19(CPoint(91,5),CSize(96,61));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Song information"),BS_GROUPBOX | WS_VISIBLE,r19,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Songs",IDC_STATIC,91,94,96,27
   CButton* mfc23 = new CButton(parent);
   CRect r23(CPoint(91,94),CSize(96,27));
   parent->MapDialogRect(&r23);
   mfc23->Create(_T("Songs"),BS_GROUPBOX | WS_VISIBLE,r23,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Speed",IDC_STATIC,13,17,29,10
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(13,17),CSize(29,10));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Speed"),WS_VISIBLE,r2,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_SPEED,43,15,38,12,ES_AUTOHSCROLL | ES_READONLY | ES_NUMBER
   CEdit* mfc3 = new CEdit(parent);
   CRect r3(CPoint(43,15),CSize(38,12));
   parent->MapDialogRect(&r3);
   mfc3->Create(ES_AUTOHSCROLL | ES_READONLY | ES_NUMBER | WS_VISIBLE,r3,parent,IDC_SPEED);
   mfcToQtWidget->insert(IDC_SPEED,mfc3);
//       CONTROL         "",IDC_SPEED_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,15,11,13
   CSpinButtonCtrl* mfc4 = new CSpinButtonCtrl(parent);
   CRect r4(CPoint(81,15),CSize(11,13));
   parent->MapDialogRect(&r4);
   mfc4->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r4,parent,IDC_SPEED_SPIN);
   mfcToQtWidget->insert(IDC_SPEED_SPIN,mfc4);
//       LTEXT           "Tempo",IDC_STATIC,13,31,29,10
   CStatic* mfc5 = new CStatic(parent);
   CRect r5(CPoint(13,31),CSize(29,10));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Tempo"),WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_TEMPO,43,29,38,12,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc6 = new CEdit(parent);
   CRect r6(CPoint(43,29),CSize(38,12));
   parent->MapDialogRect(&r6);
   mfc6->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r6,parent,IDC_TEMPO);
   mfcToQtWidget->insert(IDC_TEMPO,mfc6);
//       CONTROL         "",IDC_TEMPO_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,28,11,13
   CSpinButtonCtrl* mfc7 = new CSpinButtonCtrl(parent);
   CRect r7(CPoint(81,28),CSize(11,13));
   parent->MapDialogRect(&r7);
   mfc7->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r7,parent,IDC_TEMPO_SPIN);
   mfcToQtWidget->insert(IDC_TEMPO_SPIN,mfc7);
//       LTEXT           "Rows",IDC_STATIC,13,45,29,10
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(13,45),CSize(29,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Rows"),WS_VISIBLE,r8,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_ROWS,43,43,38,12,ES_AUTOHSCROLL | ES_READONLY | ES_WANTRETURN
   CEdit* mfc9 = new CEdit(parent);
   CRect r9(CPoint(43,43),CSize(38,12));
   parent->MapDialogRect(&r9);
   mfc9->Create(ES_AUTOHSCROLL | ES_READONLY | ES_WANTRETURN | WS_VISIBLE,r9,parent,IDC_ROWS);
   mfcToQtWidget->insert(IDC_ROWS,mfc9);
//       CONTROL         "",IDC_ROWS_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,43,11,13
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(parent);
   CRect r10(CPoint(81,43),CSize(11,13));
   parent->MapDialogRect(&r10);
   mfc10->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,parent,IDC_ROWS_SPIN);
   mfcToQtWidget->insert(IDC_ROWS_SPIN,mfc10);
//       LTEXT           "Frames",IDC_STATIC,13,59,29,10
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(13,59),CSize(29,10));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Frames"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_FRAMES,43,57,38,12,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc12 = new CEdit(parent);
   CRect r12(CPoint(43,57),CSize(38,12));
   parent->MapDialogRect(&r12);
   mfc12->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r12,parent,IDC_FRAMES);
   mfcToQtWidget->insert(IDC_FRAMES,mfc12);
//       CONTROL         "",IDC_FRAME_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,58,11,13
   CSpinButtonCtrl* mfc13 = new CSpinButtonCtrl(parent);
   CRect r13(CPoint(81,58),CSize(11,13));
   parent->MapDialogRect(&r13);
   mfc13->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r13,parent,IDC_FRAME_SPIN);
   mfcToQtWidget->insert(IDC_FRAME_SPIN,mfc13);
//       LTEXT           "Step",IDC_STATIC,13,91,29,10,0,WS_EX_TRANSPARENT
   CStatic* mfc15 = new CStatic(parent);
   CRect r15(CPoint(13,91),CSize(29,10));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("Step"),WS_VISIBLE,r15,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_KEYSTEP,43,90,38,12,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc16 = new CEdit(parent);
   CRect r16(CPoint(43,90),CSize(38,12));
   parent->MapDialogRect(&r16);
   mfc16->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r16,parent,IDC_KEYSTEP);
   mfcToQtWidget->insert(IDC_KEYSTEP,mfc16);
//       CONTROL         "",IDC_KEYSTEP_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,88,11,13
   CSpinButtonCtrl* mfc17 = new CSpinButtonCtrl(parent);
   CRect r17(CPoint(81,88),CSize(11,13));
   parent->MapDialogRect(&r17);
   mfc17->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r17,parent,IDC_KEYSTEP_SPIN);
   mfcToQtWidget->insert(IDC_KEYSTEP_SPIN,mfc17);
//       CONTROL         "Key repetition",IDC_KEYREPEAT,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,105,68,9,WS_EX_TRANSPARENT
   CButton* mfc18 = new CButton(parent);
   CRect r18(CPoint(13,105),CSize(68,9));
   parent->MapDialogRect(&r18);
   mfc18->Create(_T("Key repetition"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r18,parent,IDC_KEYREPEAT);
   mfcToQtWidget->insert(IDC_KEYREPEAT,mfc18);
//       EDITTEXT        IDC_SONG_NAME,97,18,83,12,ES_AUTOHSCROLL
   CEdit* mfc20 = new CEdit(parent);
   CRect r20(CPoint(97,18),CSize(83,12));
   parent->MapDialogRect(&r20);
   mfc20->Create(ES_AUTOHSCROLL | WS_VISIBLE,r20,parent,IDC_SONG_NAME);
   mfcToQtWidget->insert(IDC_SONG_NAME,mfc20);
//       EDITTEXT        IDC_SONG_ARTIST,97,33,83,12,ES_AUTOHSCROLL
   CEdit* mfc21 = new CEdit(parent);
   CRect r21(CPoint(97,33),CSize(83,12));
   parent->MapDialogRect(&r21);
   mfc21->Create(ES_AUTOHSCROLL | WS_VISIBLE,r21,parent,IDC_SONG_ARTIST);
   mfcToQtWidget->insert(IDC_SONG_ARTIST,mfc21);
//       EDITTEXT        IDC_SONG_COPYRIGHT,97,49,83,12,ES_AUTOHSCROLL
   CEdit* mfc22 = new CEdit(parent);
   CRect r22(CPoint(97,49),CSize(83,12));
   parent->MapDialogRect(&r22);
   mfc22->Create(ES_AUTOHSCROLL | WS_VISIBLE,r22,parent,IDC_SONG_COPYRIGHT);
   mfcToQtWidget->insert(IDC_SONG_COPYRIGHT,mfc22);
//       COMBOBOX        IDC_SUBTUNE,97,104,84,85,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc24 = new CComboBox(parent);
   CRect r24(CPoint(97,104),CSize(84,85));
   parent->MapDialogRect(&r24);
   mfc24->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r24,parent,IDC_SUBTUNE);
   mfcToQtWidget->insert(IDC_SUBTUNE,mfc24);
//   CONTROL         "",IDC_INSTRUMENTS,"SysListView32",LVS_LIST | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_EDITLABELS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,193,5,202,103
   CListCtrl* mfc25 = new CListCtrl(parent);
   CRect r25(CPoint(193,5),CSize(202,103));
   parent->MapDialogRect(&r25);
   mfc25->Create(LVS_LIST | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_EDITLABELS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r25,parent,IDC_INSTRUMENTS);
   mfcToQtWidget->insert(IDC_INSTRUMENTS,mfc25);
//       EDITTEXT        IDC_INSTNAME,324,109,71,12,ES_AUTOHSCROLL
   CEdit* mfc26 = new CEdit(parent);
   CRect r26(CPoint(324,109),CSize(71,12));
   parent->MapDialogRect(&r26);
   mfc26->Create(ES_AUTOHSCROLL | WS_VISIBLE,r26,parent,IDC_INSTNAME);
   mfcToQtWidget->insert(IDC_INSTNAME,mfc26);
//   END
}

void qtMfcInitDialogResource_IDD_PERFORMANCE(CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_PERFORMANCE DIALOGEX 0, 0, 177, 95
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Performance"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       DEFPUSHBUTTON   "Close",IDOK,58,74,60,14
//       GROUPBOX        "CPU usage",IDC_STATIC,7,7,68,53
//       CTEXT           "--%",IDC_CPU,43,30,29,10
//       CONTROL         "",IDC_CPU_BAR,"msctls_progress32",PBS_SMOOTH | PBS_VERTICAL | WS_BORDER,18,19,18,34
//       LTEXT           "Frame rate: 0 Hz",IDC_FRAMERATE,89,18,72,8
//       LTEXT           "Underruns: 0",IDC_UNDERRUN,89,45,66,8
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,67,162,1
//       GROUPBOX        "Other",IDC_STATIC,81,7,88,26
//       GROUPBOX        "Audio",IDC_STATIC,81,34,88,26
//   END
}

void qtMfcInitDialogResource_IDD_SPEED(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_SPEED DIALOGEX 0, 0, 196, 44
   CRect rect(CPoint(0,0),CSize(196,44));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Custom speed"
   parent->SetWindowText("Custom speed");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       DEFPUSHBUTTON   "OK",IDOK,139,7,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(139,7),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc1);
//       PUSHBUTTON      "Cancel",IDCANCEL,139,23,50,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(139,23),CSize(50,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
//       CONTROL         "",IDC_SPEED_SLD,"msctls_trackbar32",WS_TABSTOP,7,7,101,16
   CSliderCtrl* mfc3 = new CSliderCtrl(parent);
   CRect r3(CPoint(7,7),CSize(101,16));
   parent->MapDialogRect(&r3);
   mfc3->Create(WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_SPEED_SLD);
   mfcToQtWidget->insert(IDC_SPEED_SLD,mfc3);
//       LTEXT           "60 Hz",IDC_SPEED,112,11,26,12
   CStatic* mfc4 = new CStatic(parent);
   CRect r4(CPoint(112,11),CSize(26,12));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("60 Hz"),WS_VISIBLE,r4,parent,IDC_SPEED);
   mfcToQtWidget->insert(IDC_SPEED,mfc4);
//   END
}

void qtMfcInitDialogResource_IDD_PCMIMPORT(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_PCMIMPORT DIALOGEX 0, 0, 211, 144
   CRect rect(CPoint(0,0),CSize(211,134));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "PCM import"
   parent->SetWindowText("PCM import");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   GROUPBOX        "Quality: 0",IDC_QUALITY_FRM,7,7,136,36
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(136,36));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Quality: 0"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_QUALITY_FRM);
   mfcToQtWidget->insert(IDC_QUALITY_FRM,mfc1);
//   LTEXT           "Low",IDC_STATIC,15,21,15,10
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(15,21),CSize(15,10));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Low"),WS_VISIBLE,r2,parent,IDC_STATIC);
//   CONTROL         "",IDC_QUALITY,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,30,19,90,15
   CSliderCtrl* mfc3 = new CSliderCtrl(parent);
   CRect r3(CPoint(30,19),CSize(90,15));
   parent->MapDialogRect(&r3);
   mfc3->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_QUALITY);
   mfcToQtWidget->insert(IDC_QUALITY,mfc3);
//   LTEXT           "High",IDC_STATIC,123,21,16,10
   CStatic* mfc4 = new CStatic(parent);
   CRect r4(CPoint(123,21),CSize(16,10));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("High"),WS_VISIBLE,r4,parent,IDC_STATIC);
//   GROUPBOX        "Gain: +0dB",IDC_VOLUME_FRM,7,47,136,35
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(7,47),CSize(136,35));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Gain: +0dB"),BS_GROUPBOX | WS_VISIBLE,r5,parent,IDC_VOLUME_FRM);
   mfcToQtWidget->insert(IDC_VOLUME_FRM,mfc5);
//   LTEXT           "Low",IDC_STATIC,15,61,15,10
   CStatic* mfc6 = new CStatic(parent);
   CRect r6(CPoint(15,61),CSize(15,10));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Low"),WS_VISIBLE,r6,parent,IDC_STATIC);
//   CONTROL         "",IDC_VOLUME,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,30,59,90,15
   CSliderCtrl* mfc7 = new CSliderCtrl(parent);
   CRect r7(CPoint(30,59),CSize(90,15));
   parent->MapDialogRect(&r7);
   mfc7->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_VOLUME);
   mfcToQtWidget->insert(IDC_VOLUME,mfc7);
//   LTEXT           "High",IDC_STATIC,123,61,16,10
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(123,61),CSize(16,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("High"),WS_VISIBLE,r8,parent,IDC_STATIC);
//   GROUPBOX        "File info",IDC_STATIC,7,86,136,511
   CButton* mfc9 = new CButton(parent);
   CRect r9(CPoint(7,86),CSize(136,51));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("File info"),BS_GROUPBOX | WS_VISIBLE,r9,parent,IDC_STATIC);
//   LTEXT           "Static",IDC_SAMPLE_RATE,15,99,118,8
   CStatic* mfc10 = new CStatic(parent);
   CRect r10(CPoint(15,99),CSize(118,8));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T(""),WS_VISIBLE,r10,parent,IDC_SAMPLE_RATE);
   mfcToQtWidget->insert(IDC_SAMPLE_RATE,mfc10);
//   LTEXT           "Static",IDC_RESAMPLING,15,110,118,8
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(15,110),CSize(118,8));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T(""),WS_VISIBLE,r11,parent,IDC_RESAMPLING);
   mfcToQtWidget->insert(IDC_RESAMPLING,mfc11);
//   PUSHBUTTON      "Preview",IDC_PREVIEW,154,7,50,14
   CButton* mfc12 = new CButton(parent);
   CRect r12(CPoint(154,7),CSize(50,14));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Preview"),WS_VISIBLE,r12,parent,IDC_PREVIEW);
   mfcToQtWidget->insert(IDC_PREVIEW,mfc12);
//   DEFPUSHBUTTON   "OK",IDOK,154,24,50,14
   CButton* mfc13 = new CButton(parent);
   CRect r13(CPoint(154,24),CSize(50,14));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r13,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc13);
//   PUSHBUTTON      "Cancel",IDCANCEL,154,41,50,14
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(154,41),CSize(50,14));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Cancel"),WS_VISIBLE,r14,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc14);
//   LTEXT           "Static",IDC_SAMPLESIZE,15,121,118,8
   CStatic* mfc15 = new CStatic(parent);
   CRect r15(CPoint(15,121),CSize(118,8));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T(""),WS_VISIBLE,r15,parent,IDC_SAMPLESIZE);
   mfcToQtWidget->insert(IDC_SAMPLESIZE,mfc15);
//   END
}

void qtMfcInitDialogResource_IDD_INSTRUMENT_INTERNAL(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_INSTRUMENT_INTERNAL DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   GROUPBOX        "Sequence editor",IDC_STATIC,120,7,245,158
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(120,7),CSize(245,158));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Sequence editor"),BS_GROUPBOX | WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   GROUPBOX        "Instrument settings",IDC_STATIC,7,7,107,158,0,WS_EX_TRANSPARENT
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(7,7),CSize(107,158));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Instrument settings"),BS_GROUPBOX | WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_INSTSETTINGS,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,12,18,96,109,WS_EX_TRANSPARENT
   CListCtrl* mfc1 = new CListCtrl(parent);
   CRect r1(CPoint(12,18),CSize(96,109));
   parent->MapDialogRect(&r1);
   mfc1->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r1,parent,IDC_INSTSETTINGS);
   mfcToQtWidget->insert(IDC_INSTSETTINGS,mfc1);
//   CONTROL         "Sequence #",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | SS_CENTERIMAGE | WS_GROUP,12,149,53,10,WS_EX_TRANSPARENT
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(12,149),CSize(53,10));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Sequence #"),SS_LEFTNOWORDWRAP | SS_CENTERIMAGE | WS_GROUP | WS_VISIBLE,r2,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   EDITTEXT        IDC_SEQ_INDEX,69,147,39,12,ES_AUTOHSCROLL | ES_NUMBER
   CEdit* mfc3 = new CEdit(parent);
   CRect r3(CPoint(69,147),CSize(39,12));
   parent->MapDialogRect(&r3);
   mfc3->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r3,parent,IDC_SEQ_INDEX);
   mfcToQtWidget->insert(IDC_SEQ_INDEX,mfc3);
//   CONTROL         "",IDC_SEQUENCE_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,66,153,11,9
   CSpinButtonCtrl* mfc4 = new CSpinButtonCtrl(parent);
   CRect r4(CPoint(66,153),CSize(11,9));
   parent->MapDialogRect(&r4);
   mfc4->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r4,parent,IDC_SEQUENCE_SPIN);
   mfcToQtWidget->insert(IDC_SEQUENCE_SPIN,mfc4);
//   PUSHBUTTON      "Select next empty slot",IDC_FREE_SEQ,12,129,96,15
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(12,129),CSize(96,15));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Select next empty slot"),WS_VISIBLE,r7,parent,IDC_FREE_SEQ);
   mfcToQtWidget->insert(IDC_FREE_SEQ,mfc7);
//   EDITTEXT        IDC_SEQUENCE_STRING,126,149,232,13,ES_AUTOHSCROLL
   CEdit* mfc8 = new CEdit(parent);
   CRect r8(CPoint(126,149),CSize(232,13));
   parent->MapDialogRect(&r8);
   mfc8->Create(ES_AUTOHSCROLL | WS_VISIBLE,r8,parent,IDC_SEQUENCE_STRING);
   mfcToQtWidget->insert(IDC_SEQUENCE_STRING,mfc8);
//   END
}

void qtMfcInitDialogResource_IDD_INSTRUMENT_DPCM(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_INSTRUMENT_DPCM DIALOGEX 0, 0, 372, 174
   CRect rect(CPoint(0,0),CSize(372,174));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   GROUPBOX        "Assigned samples",IDC_STATIC,7,7,179,160
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(179,160));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Assigned samples"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_TABLE,"SysListView32",LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,15,19,117,125
   CListCtrl* mfc2 = new CListCtrl(parent);
   CRect r2(CPoint(15,19),CSize(117,125));
   parent->MapDialogRect(&r2);
   mfc2->Create(LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_TABLE);
   mfcToQtWidget->insert(IDC_TABLE,mfc2);
//   COMBOBOX        IDC_SAMPLES,15,148,117,125,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc3 = new CComboBox(parent);
   CRect r3(CPoint(15,148),CSize(117,125));
   parent->MapDialogRect(&r3);
   mfc3->Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_SAMPLES);
   mfcToQtWidget->insert(IDC_SAMPLES,mfc3);
//   LTEXT           "Octave",IDC_STATIC,138,19,30,9
   CStatic* mfc4 = new CStatic(parent);
   CRect r4(CPoint(138,19),CSize(30,9));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Octave"),WS_VISIBLE,r4,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_OCTAVE,138,29,42,53,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc5 = new CComboBox(parent);
   CRect r5(CPoint(138,29),CSize(42,53));
   parent->MapDialogRect(&r5);
   mfc5->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r5,parent,IDC_OCTAVE);
   mfcToQtWidget->insert(IDC_OCTAVE,mfc5);
//   LTEXT           "Pitch",IDC_STATIC,138,44,30,8
   CStatic* mfc6 = new CStatic(parent);
   CRect r6(CPoint(138,44),CSize(30,8));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Pitch"),WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_PITCH,138,54,42,53,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc7 = new CComboBox(parent);
   CRect r7(CPoint(138,54),CSize(42,53));
   parent->MapDialogRect(&r7);
   mfc7->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_PITCH);
   mfcToQtWidget->insert(IDC_PITCH,mfc7);
//   CONTROL         "Loop",IDC_LOOP,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,138,71,42,9
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(138,71),CSize(42,9));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Loop"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r8,parent,IDC_LOOP);
   mfcToQtWidget->insert(IDC_LOOP,mfc8);
//   EDITTEXT        IDC_DELTA_COUNTER,138,97,42,14,ES_AUTOHSCROLL
   CEdit* mfc9 = new CEdit(parent);
   CRect r9(CPoint(138,97),CSize(42,14));
   parent->MapDialogRect(&r9);
   mfc9->Create(ES_AUTOHSCROLL | WS_VISIBLE,r9,parent,IDC_DELTA_COUNTER);
   mfcToQtWidget->insert(IDC_DELTA_COUNTER,mfc9);
//   CONTROL         "",IDC_DELTA_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,170,106,11,14
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(parent);
   CRect r10(CPoint(170,106),CSize(11,14));
   parent->MapDialogRect(&r10);
   mfc10->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,parent,IDC_DELTA_SPIN);
   mfcToQtWidget->insert(IDC_DELTA_SPIN,mfc10);
//   PUSHBUTTON      "",IDC_ADD,138,130,42,14,BS_ICON
   CButton* mfc11 = new CButton(parent);
   CRect r11(CPoint(138,130),CSize(42,14));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T(""),BS_ICON | WS_VISIBLE,r11,parent,IDC_ADD);
   mfcToQtWidget->insert(IDC_ADD,mfc11);
//   PUSHBUTTON      "",IDC_REMOVE,138,148,42,14,BS_ICON
   CButton* mfc12 = new CButton(parent);
   CRect r12(CPoint(138,148),CSize(42,14));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T(""),BS_ICON | WS_VISIBLE,r12,parent,IDC_REMOVE);
   mfcToQtWidget->insert(IDC_REMOVE,mfc12);
//   GROUPBOX        "Loaded samples",IDC_STATIC,192,7,173,160
   CButton* mfc13 = new CButton(parent);
   CRect r13(CPoint(192,7),CSize(173,160));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("Loaded samples"),BS_GROUPBOX | WS_VISIBLE,r13,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_SAMPLE_LIST,"SysListView32",LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,198,19,108,125
   CListCtrl* mfc14 = new CListCtrl(parent);
   CRect r14(CPoint(198,19),CSize(108,125));
   parent->MapDialogRect(&r14);
   mfc14->Create(LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r14,parent,IDC_SAMPLE_LIST);
   mfcToQtWidget->insert(IDC_SAMPLE_LIST,mfc14);
//   LTEXT           "Space used 16 / 16 kb",IDC_SPACE,198,151,166,9
   CStatic* mfc15 = new CStatic(parent);
   CRect r15(CPoint(198,151),CSize(166,9));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("Space used 16 / 16 kb"),WS_VISIBLE,r15,parent,IDC_SPACE);
   mfcToQtWidget->insert(IDC_SPACE,mfc15);
//   DEFPUSHBUTTON   "Load",IDC_LOAD,312,19,47,14
   CButton* mfc16 = new CButton(parent);
   CRect r16(CPoint(312,19),CSize(47,14));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Load"),BS_DEFPUSHBUTTON | WS_VISIBLE,r16,parent,IDC_LOAD);
   mfcToQtWidget->insert(IDC_LOAD,mfc16);
//   PUSHBUTTON      "Unload",IDC_UNLOAD,312,35,47,14
   CButton* mfc17 = new CButton(parent);
   CRect r17(CPoint(312,35),CSize(47,14));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("Unload"),WS_VISIBLE,r17,parent,IDC_UNLOAD);
   mfcToQtWidget->insert(IDC_UNLOAD,mfc17);
//   PUSHBUTTON      "Save",IDC_SAVE,312,51,47,14
   CButton* mfc18 = new CButton(parent);
   CRect r18(CPoint(312,51),CSize(47,14));
   parent->MapDialogRect(&r18);
   mfc18->Create(_T("Save"),WS_VISIBLE,r18,parent,IDC_SAVE);
   mfcToQtWidget->insert(IDC_SAVE,mfc18);
//   PUSHBUTTON      "Import",IDC_IMPORT,312,67,47,14
   CButton* mfc19 = new CButton(parent);
   CRect r19(CPoint(312,67),CSize(47,14));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Import"),WS_VISIBLE,r19,parent,IDC_IMPORT);
   mfcToQtWidget->insert(IDC_IMPORT,mfc19);
//   PUSHBUTTON      "Edit",IDC_EDIT,312,83,47,14
   CButton* mfc20 = new CButton(parent);
   CRect r20(CPoint(312,83),CSize(47,14));
   parent->MapDialogRect(&r20);
   mfc20->Create(_T("Edit"),WS_VISIBLE,r20,parent,IDC_EDIT);
   mfcToQtWidget->insert(IDC_EDIT,mfc20);
//   PUSHBUTTON      "Preview",IDC_PREVIEW,312,99,47,14
   CButton* mfc21 = new CButton(parent);
   CRect r21(CPoint(312,99),CSize(47,14));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("Preview"),WS_VISIBLE,r21,parent,IDC_PREVIEW);
   mfcToQtWidget->insert(IDC_PREVIEW,mfc21);
//   LTEXT           "D-counter",IDC_STATIC,138,86,33,8
   CStatic* mfc22 = new CStatic(parent);
   CRect r22(CPoint(138,86),CSize(33,8));
   parent->MapDialogRect(&r22);
   mfc22->Create(_T("D-counter"),WS_VISIBLE,r22,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   END
}

void qtMfcInitDialogResource_IDD_INSTRUMENT(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_INSTRUMENT DIALOGEX 0, 0, 389, 242
   CRect rect(CPoint(0,0),CSize(389,242));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
//   CAPTION "Instrument editor"
   parent->SetWindowText("Instrument editor");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   CONTROL         "",IDC_INST_TAB,"SysTabControl32",0x0,7,7,375,185
   CTabCtrl* mfc1 = new CTabCtrl(parent);
   CRect r1(CPoint(7,7),CSize(375,185));
   parent->MapDialogRect(&r1);
   mfc1->Create(WS_VISIBLE,r1,parent,IDC_INST_TAB);
   mfcToQtWidget->insert(IDC_INST_TAB,mfc1);
//   CONTROL         "",IDC_KEYBOARD,"Static",SS_OWNERDRAW | SS_REALSIZEIMAGE | SS_SUNKEN,7,198,375,37
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(7,198),CSize(375,37));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T(""),SS_OWNERDRAW | SS_REALSIZEIMAGE | SS_SUNKEN | WS_VISIBLE,r2,parent,IDC_KEYBOARD);
   mfcToQtWidget->insert(IDC_KEYBOARD,mfc2);
//   END
}

void qtMfcInitDialogResource_IDD_CONFIG_APPEARANCE(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_CONFIG_APPEARANCE DIALOGEX 0, 0, 280, 167
   CRect rect(CPoint(0,0),CSize(280,167));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Appearance"
   parent->SetWindowText("Appearance");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   GROUPBOX        "Color schemes",IDC_STATIC,7,7,149,39
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(149,39));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Color schemes"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_SCHEME,15,22,135,126,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc2 = new CComboBox(parent);
   CRect r2(CPoint(15,22),CSize(135,126));
   parent->MapDialogRect(&r2);
   mfc2->Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_SCHEME);
   mfcToQtWidget->insert(IDC_SCHEME,mfc2);
//   GROUPBOX        "Colors",IDC_STATIC,7,54,149,65
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(7,54),CSize(149,65));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Colors"),BS_GROUPBOX | WS_VISIBLE,r3,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   LTEXT           "Item",IDC_STATIC,15,68,16,8
   CStatic* mfc4 = new CStatic(parent);
   CRect r4(CPoint(15,68),CSize(16,8));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Item"),WS_VISIBLE,r4,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_COL_ITEM,35,66,115,172,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc5 = new CComboBox(parent);
   CRect r5(CPoint(35,66),CSize(115,172));
   parent->MapDialogRect(&r5);
   mfc5->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r5,parent,IDC_COL_ITEM);
   mfcToQtWidget->insert(IDC_COL_ITEM,mfc5);
//   CONTROL         "",IDC_COL_PREVIEW,"Static",SS_OWNERDRAW,15,85,69,15
   CStatic* mfc6 = new CStatic(parent);
   CRect r6(CPoint(15,85),CSize(69,15));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T(""),SS_OWNERDRAW | WS_VISIBLE,r6,parent,IDC_COL_PREVIEW);
   mfcToQtWidget->insert(IDC_COL_PREVIEW,mfc6);
   //   PUSHBUTTON      "Pick color",IDC_PICK_COL,96,85,54,15
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(96,85),CSize(54,15));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Pick color"),WS_VISIBLE,r7,parent,IDC_PICK_COL);
   mfcToQtWidget->insert(IDC_PICK_COL,mfc7);
//   CONTROL         "Pattern colors",IDC_PATTERNCOLORS,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,15,105,58,9
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(15,105),CSize(58,9));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Pattern colors"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r8,parent,IDC_PATTERNCOLORS);
   mfcToQtWidget->insert(IDC_PATTERNCOLORS,mfc8);
//   GROUPBOX        "Pattern font and size",IDC_STATIC,7,126,149,34
   CButton* mfc9 = new CButton(parent);
   CRect r9(CPoint(7,126),CSize(149,34));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("Pattern font and size"),BS_GROUPBOX | WS_VISIBLE,r9,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_FONT,15,139,102,93,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc10 = new CComboBox(parent);
   CRect r10(CPoint(15,139),CSize(102,93));
   parent->MapDialogRect(&r10);
   mfc10->Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r10,parent,IDC_FONT);
   mfcToQtWidget->insert(IDC_FONT,mfc10);
//   COMBOBOX        IDC_FONT_SIZE,122,139,28,30,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc11 = new CComboBox(parent);
   CRect r11(CPoint(122,139),CSize(28,30));
   parent->MapDialogRect(&r11);
   mfc11->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r11,parent,IDC_FONT_SIZE);
   mfcToQtWidget->insert(IDC_FONT_SIZE,mfc11);
//   GROUPBOX        "Preview",IDC_STATIC,166,7,107,153
   CButton* mfc12 = new CButton(parent);
   CRect r12(CPoint(166,7),CSize(107,153));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Preview"),BS_GROUPBOX | WS_VISIBLE,r12,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_PREVIEW,"Static",SS_OWNERDRAW,176,18,90,135
   CStatic* mfc13 = new CStatic(parent);
   CRect r13(CPoint(176,18),CSize(90,135));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T(""),SS_OWNERDRAW | WS_VISIBLE,r13,parent,IDC_PREVIEW);
   mfcToQtWidget->insert(IDC_PREVIEW,mfc13);
//   CONTROL         "Display flats",IDC_DISPLAYFLATS,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,83,105,58,9
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(83,105),CSize(58,9));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Display flats"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r14,parent,IDC_DISPLAYFLATS);
   mfcToQtWidget->insert(IDC_DISPLAYFLATS,mfc14);
//   END
}

void qtMfcInitDialogResource_IDD_CONFIG_GENERAL(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_CONFIG_GENERAL DIALOGEX 0, 0, 280, 167
   CRect rect(CPoint(0,0),CSize(280,167));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "General"
   parent->SetWindowText("General");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "General settings",IDC_STATIC,7,7,125,153
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(125,153));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("General settings"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "Wrap cursor",IDC_OPT_WRAPCURSOR,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,18,113,9
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(13,18),CSize(113,9));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Wrap cursor"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_OPT_WRAPCURSOR);
   mfcToQtWidget->insert(IDC_OPT_WRAPCURSOR,mfc2);
//   CONTROL         "Wrap across frames",IDC_OPT_WRAPFRAMES,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,28,113,9
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(13,28),CSize(113,9));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Wrap across frames"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_OPT_WRAPFRAMES);
   mfcToQtWidget->insert(IDC_OPT_WRAPFRAMES,mfc3);
//   CONTROL         "Free cursor edit",IDC_OPT_FREECURSOR,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,38,113,9
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(13,38),CSize(113,9));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Free cursor edit"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r4,parent,IDC_OPT_FREECURSOR);
   mfcToQtWidget->insert(IDC_OPT_FREECURSOR,mfc4);
//   CONTROL         "Preview wave-files",IDC_OPT_WAVEPREVIEW,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,48,113,9
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(13,48),CSize(113,9));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Preview wave-files"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r5,parent,IDC_OPT_WAVEPREVIEW);
   mfcToQtWidget->insert(IDC_OPT_WAVEPREVIEW,mfc5);
//   CONTROL         "Key repeat",IDC_OPT_KEYREPEAT,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,58,113,9
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(13,58),CSize(113,9));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Key repeat"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r6,parent,IDC_OPT_KEYREPEAT);
   mfcToQtWidget->insert(IDC_OPT_KEYREPEAT,mfc6);
//   CONTROL         "Show row numbers in hex",IDC_OPT_HEXROW,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,68,113,9
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(13,68),CSize(113,9));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Show row numbers in hex"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_OPT_HEXROW);
   mfcToQtWidget->insert(IDC_OPT_HEXROW,mfc7);
//       CONTROL         "Preview next/previous frame",IDC_OPT_FRAMEPREVIEW,
//   "Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,78,113,9
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(13,78),CSize(113,9));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Preview next/previous frame"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r8,parent,IDC_OPT_FRAMEPREVIEW);
   mfcToQtWidget->insert(IDC_OPT_FRAMEPREVIEW,mfc8);
//       CONTROL         "Don't reset DPCM on note stop",IDC_OPT_NODPCMRESET,
//   "Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,88,113,9
   CButton* mfc9 = new CButton(parent);
   CRect r9(CPoint(13,88),CSize(113,9));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("Don't reset DPCM on note stop"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r9,parent,IDC_OPT_NODPCMRESET);
   mfcToQtWidget->insert(IDC_OPT_NODPCMRESET,mfc9);
//CONTROL         "Ignore Step when moving",IDC_OPT_NOSTEPMOVE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,98,113,9
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(13,98),CSize(113,9));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Ignore Step when moving"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r10,parent,IDC_OPT_NOSTEPMOVE);
   mfcToQtWidget->insert(IDC_OPT_NOSTEPMOVE,mfc10);
//CONTROL         "Delete-key pulls up rows",IDC_OPT_PULLUPDELETE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,108,113,9
   CButton* mfc11 = new CButton(parent);
   CRect r11(CPoint(13,108),CSize(113,9));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Delete-key pulls up rows"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r11,parent,IDC_OPT_PULLUPDELETE);
   mfcToQtWidget->insert(IDC_OPT_PULLUPDELETE,mfc11);
//CONTROL         "Backup files",IDC_OPT_BACKUPS,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,118,113,9
   CButton* mfc12 = new CButton(parent);
   CRect r12(CPoint(13,118),CSize(113,9));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Backup files"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r12,parent,IDC_OPT_BACKUPS);
   mfcToQtWidget->insert(IDC_OPT_BACKUPS,mfc12);   
//CONTROL         "Single instance",IDC_OPT_SINGLEINSTANCE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,128,113,9
   CButton* mfc13 = new CButton(parent);
   CRect r13(CPoint(13,128),CSize(113,9));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("Single instance"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r13,parent,IDC_OPT_SINGLEINSTANCE);
   mfcToQtWidget->insert(IDC_OPT_SINGLEINSTANCE,mfc13);
//       GROUPBOX        "Pattern edit style",IDC_STATIC,138,7,135,48
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(138,7),CSize(135,48));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Pattern edit style"),BS_GROUPBOX | WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "FastTracker 2",IDC_STYLE1,"Button",BS_AUTORADIOBUTTON,144,18,120,8
   CButton* mfc15 = new CButton(parent);
   CRect r15(CPoint(144,18),CSize(120,8));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("FastTracker 2"),BS_AUTORADIOBUTTON | WS_VISIBLE,r15,parent,IDC_STYLE1);
   mfcToQtWidget->insert(IDC_STYLE1,mfc15);
//       CONTROL         "ModPlug tracker",IDC_STYLE2,"Button",BS_AUTORADIOBUTTON,144,30,120,8
   CButton* mfc16 = new CButton(parent);
   CRect r16(CPoint(144,30),CSize(120,8));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("ModPlug tracker"),BS_AUTORADIOBUTTON | WS_VISIBLE,r16,parent,IDC_STYLE2);
   mfcToQtWidget->insert(IDC_STYLE2,mfc16);
//       CONTROL         "Impulse Tracker",IDC_STYLE3,"Button",BS_AUTORADIOBUTTON,144,42,120,8
   CButton* mfc17 = new CButton(parent);
   CRect r17(CPoint(144,42),CSize(120,8));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("Impulse Tracker"),BS_AUTORADIOBUTTON | WS_VISIBLE,r17,parent,IDC_STYLE3);
   mfcToQtWidget->insert(IDC_STYLE3,mfc17);
//       GROUPBOX        "Edit settings",IDC_STATIC,137,57,135,30
   CButton* mfc18 = new CButton(parent);
   CRect r18(CPoint(137,57),CSize(135,30));
   parent->MapDialogRect(&r18);
   mfc18->Create(_T("Edit settings"),BS_GROUPBOX | WS_VISIBLE,r18,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Page jump length",IDC_STATIC,143,70,56,9
   CStatic* mfc19 = new CStatic(parent);
   CRect r19(CPoint(143,70),CSize(56,9));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Page jump length"),WS_VISIBLE,r19,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_PAGELENGTH,215,68,48,65,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc20 = new CComboBox(parent);
   CRect r20(CPoint(215,68),CSize(48,65));
   parent->MapDialogRect(&r20);
   mfc20->Create(CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r20,parent,IDC_PAGELENGTH);
   mfcToQtWidget->insert(IDC_PAGELENGTH,mfc20);
//   IDD_CONFIG_GENERAL DLGINIT
//   BEGIN
//       IDC_PAGELENGTH, 0x403, 2, 0
//   0x0032,
   mfc20->AddString(_T("2"));
//       IDC_PAGELENGTH, 0x403, 2, 0
//   0x0034,
   mfc20->AddString(_T("4"));
//       IDC_PAGELENGTH, 0x403, 2, 0
//   0x0038,
   mfc20->AddString(_T("8"));
//       IDC_PAGELENGTH, 0x403, 3, 0
//   0x3631, "\000"
   mfc20->AddString(_T("16"));
//       0
//   END
//       GROUPBOX        "Keys",IDC_STATIC,138,89,135,71
   CButton* mfc21 = new CButton(parent);
   CRect r21(CPoint(138,89),CSize(135,71));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("Keys"),BS_GROUPBOX | WS_VISIBLE,r21,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Note cut",IDC_STATIC,144,100,50,11
   CStatic* mfc22 = new CStatic(parent);
   CRect r22(CPoint(144,100),CSize(50,11));
   parent->MapDialogRect(&r22);
   mfc22->Create(_T("Note cut"),WS_VISIBLE,r22,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_KEY_NOTE_CUT,204,99,60,13,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc23 = new CEdit(parent);
   CRect r23(CPoint(204,99),CSize(60,13));
   parent->MapDialogRect(&r23);
   mfc23->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r23,parent,IDC_KEY_NOTE_CUT);
   mfcToQtWidget->insert(IDC_KEY_NOTE_CUT,mfc23);
//       LTEXT           "Clear field",IDC_STATIC,144,115,50,11
   CStatic* mfc24 = new CStatic(parent);
   CRect r24(CPoint(144,115),CSize(50,11));
   parent->MapDialogRect(&r24);
   mfc24->Create(_T("Clear field"),WS_VISIBLE,r24,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_KEY_CLEAR,204,114,60,13,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc25 = new CEdit(parent);
   CRect r25(CPoint(204,114),CSize(60,13));
   parent->MapDialogRect(&r25);
   mfc25->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r25,parent,IDC_KEY_CLEAR);
   mfcToQtWidget->insert(IDC_KEY_CLEAR,mfc25);
//       LTEXT           "Repeat",IDC_STATIC,144,129,50,11
   CStatic* mfc26 = new CStatic(parent);
   CRect r26(CPoint(144,129),CSize(50,11));
   parent->MapDialogRect(&r26);
   mfc26->Create(_T("Repeat"),WS_VISIBLE,r26,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_KEY_REPEAT,204,128,60,13,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc27 = new CEdit(parent);
   CRect r27(CPoint(204,128),CSize(60,13));
   parent->MapDialogRect(&r27);
   mfc27->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r27,parent,IDC_KEY_REPEAT);
   mfcToQtWidget->insert(IDC_KEY_REPEAT,mfc27);
//       LTEXT           "Note release",IDC_STATIC,144,143,50,11
   CStatic* mfc28 = new CStatic(parent);
   CRect r28(CPoint(144,143),CSize(50,11));
   parent->MapDialogRect(&r28);
   mfc28->Create(_T("Note release"),WS_VISIBLE,r28,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_KEY_NOTE_RELEASE,204,142,60,13,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc29 = new CEdit(parent);
   CRect r29(CPoint(204,142),CSize(60,13));
   parent->MapDialogRect(&r29);
   mfc29->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r29,parent,IDC_KEY_NOTE_RELEASE);
   mfcToQtWidget->insert(IDC_KEY_NOTE_RELEASE,mfc29);
//   CONTROL         "Preview full row",IDC_OPT_PREVIEWFULLROW,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,138,113,9
   CButton* mfc30 = new CButton(parent);
   CRect r30(CPoint(13,138),CSize(113,9));
   parent->MapDialogRect(&r30);
   mfc30->Create(_T("Preview full row"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r30,parent,IDC_OPT_PREVIEWFULLROW);
   mfcToQtWidget->insert(IDC_OPT_PREVIEWFULLROW,mfc30);
//   CONTROL         "Don't select on double-click",IDC_OPT_DOUBLECLICK,
//                   "Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,148,113,9
   CButton* mfc31 = new CButton(parent);
   CRect r31(CPoint(13,148),CSize(113,9));
   parent->MapDialogRect(&r31);
   mfc31->Create(_T("Don't select on double-click"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r31,parent,IDC_OPT_DOUBLECLICK);
   mfcToQtWidget->insert(IDC_OPT_DOUBLECLICK,mfc31);
//   END
}

void qtMfcInitDialogResource_IDD_PROPERTIES(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_PROPERTIES DIALOGEX 0, 0, 213, 259
   CRect rect(CPoint(0,0),CSize(213,259));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Module properties"
   parent->SetWindowText("Module properties");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Song editor",IDC_STATIC,7,7,199,147
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(199,147));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Song editor"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Expansion sound",IDC_STATIC,7,162,127,30
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(7,162),CSize(127,30));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Expansion sound"),BS_GROUPBOX | WS_VISIBLE,r10,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Vibrato",IDC_STATIC,7,194,199,31
   CButton* mfc12 = new CButton(parent);
   CRect r12(CPoint(7,194),CSize(199,31));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Vibrato"),BS_GROUPBOX | WS_VISIBLE,r12,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Channels: 0",IDC_CHANNELS_NR,141,162,65,30
   CButton* mfc17 = new CButton(parent);
   CRect r17(CPoint(141,162),CSize(65,30));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("Channels: 0"),BS_GROUPBOX | WS_VISIBLE,r17,parent,IDC_CHANNELS_NR);
   mfcToQtWidget->insert(IDC_CHANNELS_NR,mfc17);
//       CONTROL         "",IDC_SONGLIST,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP,14,18,120,114
   CListCtrl* mfc2 = new CListCtrl(parent);
   CRect r2(CPoint(14,18),CSize(120,114));
   parent->MapDialogRect(&r2);
   mfc2->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_SONGLIST);
   mfcToQtWidget->insert(IDC_SONGLIST,mfc2);
//       PUSHBUTTON      "Add",IDC_SONG_ADD,138,18,60,14
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(138,18),CSize(60,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Add"),WS_VISIBLE,r3,parent,IDC_SONG_ADD);
   mfcToQtWidget->insert(IDC_SONG_ADD,mfc3);
//       PUSHBUTTON      "Remove",IDC_SONG_REMOVE,138,35,60,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(138,35),CSize(60,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Remove"),WS_VISIBLE,r4,parent,IDC_SONG_REMOVE);
   mfcToQtWidget->insert(IDC_SONG_REMOVE,mfc4);
//       PUSHBUTTON      "Move up",IDC_SONG_UP,138,52,60,14
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(138,52),CSize(60,14));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Move up"),WS_VISIBLE,r5,parent,IDC_SONG_UP);
   mfcToQtWidget->insert(IDC_SONG_UP,mfc5);
//       PUSHBUTTON      "Move down",IDC_SONG_DOWN,138,69,60,14
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(138,69),CSize(60,14));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Move down"),WS_VISIBLE,r6,parent,IDC_SONG_DOWN);
   mfcToQtWidget->insert(IDC_SONG_DOWN,mfc6);
//       PUSHBUTTON      "Import file",IDC_SONG_IMPORT,138,86,60,14
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(138,86),CSize(60,14));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Import file"),WS_VISIBLE,r7,parent,IDC_SONG_IMPORT);
   mfcToQtWidget->insert(IDC_SONG_IMPORT,mfc7);
//       LTEXT           " Title",IDC_STATIC,14,135,17,12,SS_CENTERIMAGE
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(14,135),CSize(17,12));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T(" Title"),SS_CENTERIMAGE | WS_VISIBLE,r8,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_SONGNAME,34,135,100,12,ES_AUTOHSCROLL
   CEdit* mfc9 = new CEdit(parent);
   CRect r9(CPoint(34,135),CSize(100,12));
   parent->MapDialogRect(&r9);
   mfc9->Create(ES_AUTOHSCROLL | WS_VISIBLE,r9,parent,IDC_SONGNAME);
   mfcToQtWidget->insert(IDC_SONGNAME,mfc9);
//       COMBOBOX        IDC_EXPANSION,14,173,113,61,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc11 = new CComboBox(parent);
   CRect r11(CPoint(14,173),CSize(113,61));
   parent->MapDialogRect(&r11);
   mfc11->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r11,parent,IDC_EXPANSION);
   mfcToQtWidget->insert(IDC_EXPANSION,mfc11);
//       COMBOBOX        IDC_VIBRATO,14,204,184,61,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc13 = new CComboBox(parent);
   CRect r13(CPoint(14,204),CSize(184,61));
   parent->MapDialogRect(&r13);
   mfc13->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r13,parent,IDC_VIBRATO);
   mfcToQtWidget->insert(IDC_VIBRATO,mfc13);
//   IDD_PROPERTIES DLGINIT
//   BEGIN
//       IDC_VIBRATO, 0x403, 27, 0
//   0x654e, 0x2077, 0x7473, 0x6c79, 0x2065, 0x6228, 0x6e65, 0x2064, 0x7075,
//   0x2620, 0x6420, 0x776f, 0x296e, "\000"
   mfc13->AddString(_T("Old style (bend up)"));
//       IDC_VIBRATO, 0x403, 20, 0
//   0x6c4f, 0x2064, 0x7473, 0x6c79, 0x2065, 0x6228, 0x6e65, 0x2064, 0x7075,
//   0x0029,
   mfc13->AddString(_T("New style (bend up & down)"));
//       0
//   END
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,231,199,1
   CStatic* mfc14 = new CStatic(parent);
   CRect r14(CPoint(7,231),CSize(199,1));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T(""),SS_ETCHEDHORZ | WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "OK",IDOK,95,238,53,14
   CButton* mfc15 = new CButton(parent);
   CRect r15(CPoint(95,238),CSize(53,14));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r15,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc15);
//       PUSHBUTTON      "Cancel",IDCANCEL,153,238,53,14
   CButton* mfc16 = new CButton(parent);
   CRect r16(CPoint(153,238),CSize(53,14));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Cancel"),WS_VISIBLE,r16,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc16);
//       CONTROL         "",IDC_CHANNELS,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,147,173,51,13
   CSliderCtrl* mfc18 = new CSliderCtrl(parent);
   CRect r18(CPoint(147,173),CSize(51,13));
   parent->MapDialogRect(&r18);
   mfc18->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r18,parent,IDC_CHANNELS);
   mfcToQtWidget->insert(IDC_CHANNELS,mfc18);
//   END
}

void qtMfcInitDialogResource_IDD_CONFIG_MIDI(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_CONFIG_MIDI DIALOGEX 0, 0, 280, 167
   CRect rect(CPoint(0,0),CSize(280,167));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "MIDI"
   parent->SetWindowText("MIDI");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Options",IDC_STATIC,7,89,266,71
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(7,89),CSize(266,71));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Options"),BS_GROUPBOX | WS_VISIBLE,r10,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Input Device",IDC_STATIC,7,7,266,35
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(266,35));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Input Device"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_INDEVICES,13,20,253,50,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc2 = new CComboBox(parent);
   CRect r2(CPoint(13,20),CSize(253,50));
   parent->MapDialogRect(&r2);
   mfc2->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_INDEVICES);
   mfcToQtWidget->insert(IDC_INDEVICES,mfc2);
//       GROUPBOX        "Output Device",IDC_STATIC,7,48,266,35
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(7,48),CSize(266,35));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Output Device"),BS_GROUPBOX | WS_VISIBLE,r3,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_OUTDEVICES,13,61,253,50,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc4 = new CComboBox(parent);
   CRect r4(CPoint(13,61),CSize(253,50));
   parent->MapDialogRect(&r4);
   mfc4->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r4,parent,IDC_OUTDEVICES);
   mfcToQtWidget->insert(IDC_OUTDEVICES,mfc4);
//       CONTROL         "Receive sync ticks",IDC_MASTER_SYNC,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,102,173,10
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(13,102),CSize(173,10));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Receive sync ticks"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r5,parent,IDC_MASTER_SYNC);
   mfcToQtWidget->insert(IDC_MASTER_SYNC,mfc5);
//       CONTROL         "Skip key releases",IDC_KEY_RELEASE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,112,173,10
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(13,112),CSize(173,10));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Skip key releases"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r6,parent,IDC_KEY_RELEASE);
   mfcToQtWidget->insert(IDC_KEY_RELEASE,mfc6);
//       CONTROL         "Map MIDI channels to NES channels",IDC_CHANMAP,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,122,173,10
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(13,122),CSize(173,10));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Map MIDI channels to NES channels"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_CHANMAP);
   mfcToQtWidget->insert(IDC_CHANMAP,mfc7);
//       CONTROL         "Record velocities",IDC_VELOCITY,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,132,173,10
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(13,132),CSize(173,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Record velocities"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r8,parent,IDC_VELOCITY);
   mfcToQtWidget->insert(IDC_VELOCITY,mfc8);
//       CONTROL         "Auto arpeggiate chords",IDC_ARPEGGIATE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,142,173,10
   CButton* mfc9 = new CButton(parent);
   CRect r9(CPoint(13,142),CSize(173,10));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("Auto arpeggiate chords"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r9,parent,IDC_ARPEGGIATE);
   mfcToQtWidget->insert(IDC_ARPEGGIATE,mfc9);
//   END
}

void qtMfcInitDialogResource_IDD_CONFIG_SOUND(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_CONFIG_SOUND DIALOGEX 0, 0, 280, 167
   CRect rect(CPoint(0,0),CSize(280,167));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   EXSTYLE WS_EX_CONTEXTHELP
//   CAPTION "Sound"
   parent->SetWindowText("Sound");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Device",IDC_STATIC,7,7,266,35
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(266,35));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Device"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_DEVICES,13,20,253,12,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc2 = new CComboBox(parent);
   CRect r2(CPoint(13,20),CSize(253,12));
   parent->MapDialogRect(&r2);
   mfc2->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_DEVICES);
   mfcToQtWidget->insert(IDC_DEVICES,mfc2);
//       GROUPBOX        "Sample rate",IDC_STATIC,7,48,113,33
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(7,48),CSize(113,33));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Sample rate"),BS_GROUPBOX | WS_VISIBLE,r3,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_SAMPLE_RATE,13,61,101,62,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc4 = new CComboBox(parent);
   CRect r4(CPoint(13,61),CSize(101,62));
   parent->MapDialogRect(&r4);
   mfc4->Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r4,parent,IDC_SAMPLE_RATE);
   mfcToQtWidget->insert(IDC_SAMPLE_RATE,mfc4);
//       GROUPBOX        "Sample size",IDC_STATIC,7,90,113,33
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(7,90),CSize(113,33));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Sample size"),BS_GROUPBOX | WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_SAMPLE_SIZE,13,102,101,62,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc6 = new CComboBox(parent);
   CRect r6(CPoint(13,102),CSize(101,62));
   parent->MapDialogRect(&r6);
   mfc6->Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r6,parent,IDC_SAMPLE_SIZE);
   mfcToQtWidget->insert(IDC_SAMPLE_SIZE,mfc6);
//   IDD_CONFIG_SOUND DLGINIT
//   BEGIN
//       IDC_SAMPLE_RATE, 0x403, 10, 0
//   0x3131, 0x3020, 0x3532, 0x4820, 0x007a,
   mfc4->AddString(_T("11 025 Hz"));
//       IDC_SAMPLE_RATE, 0x403, 10, 0
//   0x3232, 0x3020, 0x3035, 0x4820, 0x007a,
   mfc4->AddString(_T("22 050 Hz"));
//       IDC_SAMPLE_RATE, 0x403, 10, 0
//   0x3434, 0x3120, 0x3030, 0x4820, 0x007a,
   mfc4->AddString(_T("44 100 Hz"));
//       IDC_SAMPLE_RATE, 0x403, 10, 0
//   0x3834, 0x3020, 0x3030, 0x4820, 0x007a,
   mfc4->AddString(_T("48 000 Hz"));
//       IDC_SAMPLE_RATE, 0x403, 10, 0
//   0x3639, 0x3020, 0x3030, 0x4820, 0x007a,
   mfc4->AddString(_T("96 000 Hz"));
//       IDC_SAMPLE_SIZE, 0x403, 7, 0
//   0x3631, 0x6220, 0x7469, "\000"
   mfc6->AddString(_T("16 bit"));
//       IDC_SAMPLE_SIZE, 0x403, 6, 0
//   0x2038, 0x6962, 0x0074,
   mfc6->AddString(_T("8 bit"));
//       0
//   END
//       GROUPBOX        "Buffer length",IDC_STATIC,7,129,113,31
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(7,129),CSize(113,31));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Buffer length"),BS_GROUPBOX | WS_VISIBLE,r7,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_BUF_LENGTH,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,13,141,69,12
   CSliderCtrl* mfc8 = new CSliderCtrl(parent);
   CRect r8(CPoint(13,141),CSize(69,12));
   parent->MapDialogRect(&r8);
   mfc8->Create(TBS_BOTH | TBS_NOTICKS | WS_TABSTOP | WS_VISIBLE,r8,parent,IDC_BUF_LENGTH);
   mfcToQtWidget->insert(IDC_BUF_LENGTH,mfc8);
//       CTEXT           "20 ms",IDC_BUF_LEN,83,142,31,11
   CStatic* mfc9 = new CStatic(parent);
   CRect r9(CPoint(83,142),CSize(31,11));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("20 ms"),SS_CENTER | WS_VISIBLE,r9,parent,IDC_BUF_LEN);
   mfcToQtWidget->insert(IDC_BUF_LEN,mfc9);
//       GROUPBOX        "Bass filtering",IDC_STATIC,126,48,147,33
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(126,48),CSize(147,33));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Bass filtering"),BS_GROUPBOX | WS_VISIBLE,r10,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Frequency",IDC_STATIC,132,63,36,11
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(132,63),CSize(36,11));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Frequency"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_BASS_FREQ,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,174,63,55,12
   CSliderCtrl* mfc12 = new CSliderCtrl(parent);
   CRect r12(CPoint(174,63),CSize(55,12));
   parent->MapDialogRect(&r12);
   mfc12->Create(TBS_BOTH | TBS_NOTICKS | WS_TABSTOP | WS_VISIBLE,r12,parent,IDC_BASS_FREQ);
   mfcToQtWidget->insert(IDC_BASS_FREQ,mfc12);
//       CTEXT           "16 Hz",IDC_BASS_FREQ_T,234,64,32,10
   CStatic* mfc13 = new CStatic(parent);
   CRect r13(CPoint(234,64),CSize(32,10));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("16 Hz"),SS_CENTER | WS_VISIBLE,r13,parent,IDC_BASS_FREQ_T);
   mfcToQtWidget->insert(IDC_BASS_FREQ_T,mfc13);
//       GROUPBOX        "Treble filtering",IDC_STATIC,126,86,147,43
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(126,86),CSize(147,43));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Treble filtering"),BS_GROUPBOX | WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Damping",IDC_STATIC,132,99,36,10
   CStatic* mfc15 = new CStatic(parent);
   CRect r15(CPoint(132,99),CSize(36,10));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("Damping"),WS_VISIBLE,r15,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_TREBLE_DAMP,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,174,99,55,12
   CSliderCtrl* mfc16 = new CSliderCtrl(parent);
   CRect r16(CPoint(174,99),CSize(55,12));
   parent->MapDialogRect(&r16);
   mfc16->Create(TBS_BOTH | TBS_NOTICKS | WS_TABSTOP | WS_VISIBLE,r16,parent,IDC_TREBLE_DAMP);
   mfcToQtWidget->insert(IDC_TREBLE_DAMP,mfc16);
//       CTEXT           "-24 dB",IDC_TREBLE_DAMP_T,234,99,32,9
   CStatic* mfc17 = new CStatic(parent);
   CRect r17(CPoint(234,99),CSize(32,9));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("-24 dB"),SS_CENTER | WS_VISIBLE,r17,parent,IDC_TREBLE_DAMP_T);
   mfcToQtWidget->insert(IDC_TREBLE_DAMP_T,mfc17);
//       LTEXT           "Frequency",IDC_STATIC,132,112,36,10
   CStatic* mfc18 = new CStatic(parent);
   CRect r18(CPoint(132,112),CSize(36,10));
   parent->MapDialogRect(&r18);
   mfc18->Create(_T("Frequency"),WS_VISIBLE,r18,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_TREBLE_FREQ,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,174,112,55,12
   CSliderCtrl* mfc19 = new CSliderCtrl(parent);
   CRect r19(CPoint(174,112),CSize(55,12));
   parent->MapDialogRect(&r19);
   mfc19->Create(TBS_BOTH | TBS_NOTICKS | WS_TABSTOP | WS_VISIBLE,r19,parent,IDC_TREBLE_FREQ);
   mfcToQtWidget->insert(IDC_TREBLE_FREQ,mfc19);
//       CTEXT           "12000 Hz",IDC_TREBLE_FREQ_T,234,110,32,11
   CStatic* mfc20 = new CStatic(parent);
   CRect r20(CPoint(234,110),CSize(32,11));
   parent->MapDialogRect(&r20);
   mfc20->Create(_T("12000 Hz"),SS_CENTER | WS_VISIBLE,r20,parent,IDC_TREBLE_FREQ_T);
   mfcToQtWidget->insert(IDC_TREBLE_FREQ_T,mfc20);
//       GROUPBOX        "Volume",IDC_STATIC,126,133,147,27
   CButton* mfc21 = new CButton(parent);
   CRect r21(CPoint(126,133),CSize(147,27));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("Volume"),BS_GROUPBOX | WS_VISIBLE,r21,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_VOLUME,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,132,144,96,12
   CSliderCtrl* mfc22 = new CSliderCtrl(parent);
   CRect r22(CPoint(132,144),CSize(96,12));
   parent->MapDialogRect(&r22);
   mfc22->Create(TBS_BOTH | TBS_NOTICKS | WS_TABSTOP | WS_VISIBLE,r22,parent,IDC_VOLUME);
   mfcToQtWidget->insert(IDC_VOLUME,mfc22);
//       CTEXT           "100 %",IDC_VOLUME_T,234,144,31,8
   CStatic* mfc23 = new CStatic(parent);
   CRect r23(CPoint(234,144),CSize(32,8));
   parent->MapDialogRect(&r23);
   mfc23->Create(_T("100 %"),SS_CENTER | WS_VISIBLE,r23,parent,IDC_VOLUME_T);
   mfcToQtWidget->insert(IDC_VOLUME_T,mfc23);
//   END
}

void qtMfcInitDialogResource_IDD_CONFIG_SHORTCUTS(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_CONFIG_SHORTCUTS DIALOGEX 0, 0, 280, 167
   CRect rect(CPoint(0,0),CSize(280,167));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Shortcuts"
   parent->SetWindowText("Shortcuts");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       CONTROL         "",IDC_SHORTCUTS,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP,7,7,266,136
   CListCtrl* mfc1 = new CListCtrl(parent);
   CRect r1(CPoint(7,7),CSize(266,136));
   parent->MapDialogRect(&r1);
   mfc1->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r1,parent,IDC_SHORTCUTS);
   mfcToQtWidget->insert(IDC_SHORTCUTS,mfc1);
//       LTEXT           "Keys:",IDC_STATIC,7,149,23,12
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(7,149),CSize(23,12));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Keys:"),WS_VISIBLE,r2,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_KEY,36,147,144,14,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc3 = new CEdit(parent);
   CRect r3(CPoint(36,147),CSize(144,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r3,parent,IDC_KEY);
   mfcToQtWidget->insert(IDC_KEY,mfc3);
//       PUSHBUTTON      "Clear",IDC_CLEAR,186,147,36,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(186,147),CSize(36,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Clear"),WS_VISIBLE,r4,parent,IDC_CLEAR);
   mfcToQtWidget->insert(IDC_CLEAR,mfc4);
//       PUSHBUTTON      "Default",IDC_DEFAULT,228,147,45,14
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(228,147),CSize(45,14));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Default"),WS_VISIBLE,r5,parent,IDC_DEFAULT);
   mfcToQtWidget->insert(IDC_DEFAULT,mfc5);
//   END
}

void qtMfcInitDialogResource_IDD_OCTAVE(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_OCTAVE DIALOGEX 0, 0, 276, 12
   CRect rect(CPoint(0,0),CSize(276,12));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   EXSTYLE WS_EX_CONTROLPARENT
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       COMBOBOX        IDC_OCTAVE,36,0,30,74,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc1 = new CComboBox(parent);
   CRect r1(CPoint(36,0),CSize(30,74));
   parent->MapDialogRect(&r1);
   mfc1->Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r1,parent,IDC_OCTAVE);
   mfcToQtWidget->insert(IDC_OCTAVE,mfc1);
//   IDD_OCTAVE DLGINIT
//   BEGIN
//       IDC_OCTAVE, 0x403, 2, 0
//   0x0030,
   mfc1->AddString(_T("0"));
//       IDC_OCTAVE, 0x403, 2, 0
//   0x0031,
   mfc1->AddString(_T("1"));
//       IDC_OCTAVE, 0x403, 2, 0
//   0x0032,
   mfc1->AddString(_T("2"));
//       IDC_OCTAVE, 0x403, 2, 0
//   0x0033,
   mfc1->AddString(_T("3"));
//       IDC_OCTAVE, 0x403, 2, 0
//   0x0034,
   mfc1->AddString(_T("4"));
//       IDC_OCTAVE, 0x403, 2, 0
//   0x0035,
   mfc1->AddString(_T("5"));
//       IDC_OCTAVE, 0x403, 2, 0
//   0x0036,
   mfc1->AddString(_T("6"));
//       IDC_OCTAVE, 0x403, 2, 0
//   0x0037,
   mfc1->AddString(_T("7"));
//       0
//   END
//       CONTROL         "Octave",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | WS_GROUP,6,1,24,8
   CStatic *mfc2 = new CStatic(parent);
   CRect r2(CPoint(6,1),CSize(24,8));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Octave"),SS_LEFTNOWORDWRAP | WS_GROUP | WS_VISIBLE,r2,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "Follow-mode",IDC_FOLLOW,"Button",BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP,72,0,52,13
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(72,0),CSize(52,13));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Follow-mode"),BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_FOLLOW);
   mfcToQtWidget->insert(IDC_FOLLOW,mfc3);
//   CONTROL         "Row highlight, 1:st",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | WS_GROUP,129,1,58,8
   CStatic *mfc4 = new CStatic(parent);
   CRect r4(CPoint(129,1),CSize(58,8));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Row highlight, 1:st"),SS_LEFTNOWORDWRAP | WS_GROUP | WS_VISIBLE,r4,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_HIGHLIGHT1,191,0,27,12,ES_AUTOHSCROLL | ES_NUMBER
   CEdit* mfc5 = new CEdit(parent);
   CRect r5(CPoint(191,0),CSize(27,12));
   parent->MapDialogRect(&r5);
   mfc5->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r5,parent,IDC_HIGHLIGHT1);
   mfcToQtWidget->insert(IDC_HIGHLIGHT1,mfc5);
//       CONTROL         "",IDC_HIGHLIGHTSPIN1,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HOTTRACK,213,2,11,6
   CSpinButtonCtrl* mfc6 = new CSpinButtonCtrl(parent);
   CRect r6(CPoint(213,2),CSize(11,6));
   parent->MapDialogRect(&r6);
   mfc6->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HOTTRACK | WS_VISIBLE,r6,parent,IDC_HIGHLIGHTSPIN1);
   mfcToQtWidget->insert(IDC_HIGHLIGHTSPIN1,mfc6);
//       CONTROL         "2:nd",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | WS_GROUP,225,1,13,8
   CStatic *mfc7 = new CStatic(parent);
   CRect r7(CPoint(225,1),CSize(13,8));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("2:nd"),SS_LEFTNOWORDWRAP | WS_GROUP | WS_VISIBLE,r7,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_HIGHLIGHT2,241,0,27,12,ES_AUTOHSCROLL | ES_NUMBER
   CEdit* mfc8 = new CEdit(parent);
   CRect r8(CPoint(241,0),CSize(27,12));
   parent->MapDialogRect(&r8);
   mfc8->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r8,parent,IDC_HIGHLIGHT2);
   mfcToQtWidget->insert(IDC_HIGHLIGHT2,mfc8);
//       CONTROL         "",IDC_HIGHLIGHTSPIN2,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HOTTRACK,263,2,11,6
   CSpinButtonCtrl* mfc9 = new CSpinButtonCtrl(parent);
   CRect r9(CPoint(263,2),CSize(11,6));
   parent->MapDialogRect(&r9);
   mfc9->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HOTTRACK | WS_VISIBLE,r9,parent,IDC_HIGHLIGHTSPIN2);
   mfcToQtWidget->insert(IDC_HIGHLIGHTSPIN2,mfc9);
//   END
}

void qtMfcInitDialogResource_IDD_EXPORT(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_EXPORT DIALOGEX 0, 0, 247, 283
   CRect rect(CPoint(0,0),CSize(247,283));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Export file"
   parent->SetWindowText("Export file");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "NSF file options",IDC_STATIC,7,7,173,75
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(7,7),CSize(173,75));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("NSF file options"),BS_GROUPBOX | WS_VISIBLE,r10,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Progress",IDC_STATIC,7,120,233,156
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(7,120),CSize(233,156));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Progress"),BS_GROUPBOX | WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Type of file",IDC_STATIC,7,87,233,29
   CButton* mfc16 = new CButton(parent);
   CRect r16(CPoint(7,87),CSize(233,29));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Type of file"),BS_GROUPBOX | WS_VISIBLE,r16,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "&Export",IDC_EXPORT,187,7,53,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(187,7),CSize(53,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Export"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDC_EXPORT);
   mfcToQtWidget->insert(IDOK,mfc1);
//       PUSHBUTTON      "&Close",IDC_CLOSE,187,23,53,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(187,23),CSize(53,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Close"),WS_VISIBLE,r2,parent,IDC_CLOSE);
   mfcToQtWidget->insert(IDC_CLOSE,mfc2);
//       EDITTEXT        IDC_NAME,60,18,114,13,ES_AUTOHSCROLL
   CEdit* mfc3 = new CEdit(parent);
   CRect r3(CPoint(60,18),CSize(114,13));
   parent->MapDialogRect(&r3);
   mfc3->Create(ES_AUTOHSCROLL | WS_VISIBLE,r3,parent,IDC_NAME);
   mfcToQtWidget->insert(IDC_NAME,mfc3);
//       EDITTEXT        IDC_ARTIST,60,33,114,13,ES_AUTOHSCROLL
   CEdit* mfc4 = new CEdit(parent);
   CRect r4(CPoint(60,33),CSize(114,13));
   parent->MapDialogRect(&r4);
   mfc4->Create(ES_AUTOHSCROLL | WS_VISIBLE,r4,parent,IDC_ARTIST);
   mfcToQtWidget->insert(IDC_ARTIST,mfc4);
//       EDITTEXT        IDC_COPYRIGHT,60,49,114,13,ES_AUTOHSCROLL
   CEdit* mfc5 = new CEdit(parent);
   CRect r5(CPoint(60,49),CSize(114,13));
   parent->MapDialogRect(&r5);
   mfc5->Create(ES_AUTOHSCROLL | WS_VISIBLE,r5,parent,IDC_COPYRIGHT);
   mfcToQtWidget->insert(IDC_COPYRIGHT,mfc5);
//       CONTROL         "NTSC",IDC_NTSC,"Button",BS_AUTORADIOBUTTON,16,67,33,10
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(16,67),CSize(33,10));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("NTSC"),BS_AUTORADIOBUTTON | WS_VISIBLE,r6,parent,IDC_NTSC);
   mfcToQtWidget->insert(IDC_NTSC,mfc6);
//       CONTROL         "PAL",IDC_PAL,"Button",BS_AUTORADIOBUTTON,54,67,28,10
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(54,67),CSize(28,10));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("PAL"),BS_AUTORADIOBUTTON | WS_VISIBLE,r7,parent,IDC_PAL);
   mfcToQtWidget->insert(IDC_PAL,mfc7);
//       CONTROL         "Dual",IDC_DUAL,"Button",BS_AUTORADIOBUTTON,88,67,30,10
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(88,67),CSize(30,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Dual"),BS_AUTORADIOBUTTON | WS_VISIBLE,r8,parent,IDC_DUAL);
   mfcToQtWidget->insert(IDC_DUAL,mfc8);
//       COMBOBOX        IDC_TYPE,16,97,218,100,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc9 = new CComboBox(parent);
   CRect r9(CPoint(16,97),CSize(218,100));
   parent->MapDialogRect(&r9);
   mfc9->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r9,parent,IDC_TYPE);
   mfcToQtWidget->insert(IDC_TYPE,mfc9);
//       LTEXT           "Name",IDC_STATIC,16,20,38,11
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(16,20),CSize(38,11));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Name"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Artist",IDC_STATIC,16,35,38,11
   CStatic* mfc12 = new CStatic(parent);
   CRect r12(CPoint(16,35),CSize(38,11));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Artist"),WS_VISIBLE,r12,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Copyright",IDC_STATIC,16,51,38,11
   CStatic* mfc13 = new CStatic(parent);
   CRect r13(CPoint(16,51),CSize(38,11));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("Copyright"),WS_VISIBLE,r13,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_OUTPUT,16,132,218,139,ES_MULTILINE | ES_AUTOHSCROLL | ES_READONLY | NOT WS_BORDER | WS_VSCROLL
   CEdit* mfc15 = new CEdit(parent);
   CRect r15(CPoint(16,132),CSize(218,139));
   parent->MapDialogRect(&r15);
   mfc15->Create(ES_MULTILINE | ES_AUTOHSCROLL | ES_READONLY | WS_VSCROLL | WS_VISIBLE,r15,parent,IDC_COPYRIGHT);
   mfcToQtWidget->insert(IDC_OUTPUT,mfc15);
//       PUSHBUTTON      "&Play",IDC_PLAY,187,68,53,14,NOT WS_VISIBLE
   CButton* mfc17 = new CButton(parent);
   CRect r17(CPoint(187,68),CSize(53,14));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("&Play"),0,r17,parent,IDC_PLAY);
   mfcToQtWidget->insert(IDC_PLAY,mfc17);
//   END
}

void qtMfcInitDialogResource_IDD_INSTRUMENT_VRC7(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_INSTRUMENT_VRC7 DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_CHILD
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Patch",IDC_STATIC,7,7,310,30
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(310,30));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Patch"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Modulator settings",IDC_STATIC,7,42,153,123
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(7,42),CSize(153,123));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Modulator settings"),BS_GROUPBOX | WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Carrier settings",IDC_STATIC,166,42,152,123
   CButton* mfc23 = new CButton(parent);
   CRect r23(CPoint(166,42),CSize(152,123));
   parent->MapDialogRect(&r23);
   mfc23->Create(_T("Carrier settings"),BS_GROUPBOX | WS_VISIBLE,r23,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_PATCH,15,18,295,128,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc2 = new CComboBox(parent);
   CRect r2(CPoint(15,18),CSize(295,128));
   parent->MapDialogRect(&r2);
   mfc2->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_PATCH);
   mfcToQtWidget->insert(IDC_PATCH,mfc2);
//       PUSHBUTTON      "Copy",IDC_COPY,322,7,43,14
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(322,7),CSize(43,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Copy"),WS_VISIBLE,r3,parent,IDC_COPY);
   mfcToQtWidget->insert(IDC_COPY,mfc3);
//       PUSHBUTTON      "Paste",IDC_PASTE,322,23,43,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(322,23),CSize(43,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Paste"),WS_VISIBLE,r4,parent,IDC_PASTE);
   mfcToQtWidget->insert(IDC_PASTE,mfc4);
//       CONTROL         "Amplitude modulation",IDC_M_AM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,54,84,10,WS_EX_TRANSPARENT
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(14,54),CSize(84,10));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Amplitude modulation"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r6,parent,IDC_M_AM);
   mfcToQtWidget->insert(IDC_M_AM,mfc6);
//   CONTROL         "Vibrato",IDC_M_VIB,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,109,54,39,10,WS_EX_TRANSPARENT
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(109,54),CSize(39,10));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Vibrato"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_M_VIB);
   mfcToQtWidget->insert(IDC_M_VIB,mfc7);
//       CONTROL         "Sustained",IDC_M_EG,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,70,47,10,WS_EX_TRANSPARENT
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(14,70),CSize(47,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Sustained"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r8,parent,IDC_M_EG);
   mfcToQtWidget->insert(IDC_M_EG,mfc8);
//   CONTROL         "Key scaling",IDC_M_KSR2,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,86,51,10,WS_EX_TRANSPARENT
   CButton* mfc9 = new CButton(parent);
   CRect r9(CPoint(14,86),CSize(51,10));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("Key scaling"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r9,parent,IDC_M_KSR2);
   mfcToQtWidget->insert(IDC_M_KSR2,mfc9);
//   CONTROL         "Wave rectification",IDC_M_DM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,74,70,74,10,WS_EX_TRANSPARENT
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(74,70),CSize(74,10));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Wave rectification"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r10,parent,IDC_M_DM);
   mfcToQtWidget->insert(IDC_M_DM,mfc10);
//   LTEXT           "Level",IDC_STATIC,69,87,18,8
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(69,87),CSize(18,8));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Level"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_M_KSL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,90,86,58,11,WS_EX_TRANSPARENT
   CSliderCtrl* mfc12 = new CSliderCtrl(parent);
   CRect r12(CPoint(90,86),CSize(58,11));
   parent->MapDialogRect(&r12);
   mfc12->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r12,parent,IDC_M_KSL);
   mfcToQtWidget->insert(IDC_M_KSL,mfc12);
//       LTEXT           "Mult. factor",IDC_STATIC,14,108,38,8
   CStatic* mfc13 = new CStatic(parent);
   CRect r13(CPoint(14,108),CSize(38,8));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("Mult. factor"),WS_VISIBLE,r13,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_MUL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,51,107,105,11,WS_EX_TRANSPARENT
   CSliderCtrl* mfc14 = new CSliderCtrl(parent);
   CRect r14(CPoint(51,107),CSize(105,11));
   parent->MapDialogRect(&r14);
   mfc14->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r14,parent,IDC_M_MUL);
   mfcToQtWidget->insert(IDC_M_MUL,mfc14);
//       LTEXT           "Attack",IDC_STATIC,14,127,22,8
   CStatic* mfc15 = new CStatic(parent);
   CRect r15(CPoint(14,127),CSize(22,8));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("Attack"),WS_VISIBLE,r15,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_AR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,40,126,42,11
   CSliderCtrl* mfc16 = new CSliderCtrl(parent);
   CRect r16(CPoint(40,126),CSize(42,11));
   parent->MapDialogRect(&r16);
   mfc16->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r16,parent,IDC_M_AR);
   mfcToQtWidget->insert(IDC_M_AR,mfc16);
//       LTEXT           "Decay",IDC_STATIC,87,127,21,8
   CStatic* mfc17 = new CStatic(parent);
   CRect r17(CPoint(87,127),CSize(21,8));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("Decay"),WS_VISIBLE,r17,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_DR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,114,126,42,11
   CSliderCtrl* mfc18 = new CSliderCtrl(parent);
   CRect r18(CPoint(114,126),CSize(42,11));
   parent->MapDialogRect(&r18);
   mfc18->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r18,parent,IDC_M_DR);
   mfcToQtWidget->insert(IDC_M_DR,mfc18);
//       LTEXT           "Sustain",IDC_STATIC,14,144,24,8
   CStatic* mfc19 = new CStatic(parent);
   CRect r19(CPoint(14,144),CSize(24,8));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Sustain"),WS_VISIBLE,r19,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_SL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,39,143,42,11
   CSliderCtrl* mfc20 = new CSliderCtrl(parent);
   CRect r20(CPoint(39,143),CSize(42,11));
   parent->MapDialogRect(&r20);
   mfc20->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r20,parent,IDC_M_SL);
   mfcToQtWidget->insert(IDC_M_SL,mfc20);
//       LTEXT           "Release",IDC_STATIC,85,144,26,8
   CStatic* mfc21 = new CStatic(parent);
   CRect r21(CPoint(85,144),CSize(26,8));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("Release"),WS_VISIBLE,r21,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_RR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,114,143,42,11
   CSliderCtrl* mfc22 = new CSliderCtrl(parent);
   CRect r22(CPoint(114,143),CSize(42,11));
   parent->MapDialogRect(&r22);
   mfc22->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r22,parent,IDC_M_RR);
   mfcToQtWidget->insert(IDC_M_RR,mfc22);
//       CONTROL         "Amplitude modulation",IDC_C_AM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,54,84,10
   CButton* mfc24 = new CButton(parent);
   CRect r24(CPoint(171,54),CSize(84,10));
   parent->MapDialogRect(&r24);
   mfc24->Create(_T("Amplitude modulation"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r24,parent,IDC_C_AM);
   mfcToQtWidget->insert(IDC_C_AM,mfc24);
//   CONTROL         "Vibrato",IDC_C_VIB,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,267,54,39,10
   CButton* mfc25 = new CButton(parent);
   CRect r25(CPoint(267,54),CSize(39,10));
   parent->MapDialogRect(&r25);
   mfc25->Create(_T("Vibrato"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r25,parent,IDC_C_VIB);
   mfcToQtWidget->insert(IDC_C_VIB,mfc25);
//       CONTROL         "Sustained",IDC_C_EG,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,70,47,10
   CButton* mfc26 = new CButton(parent);
   CRect r26(CPoint(171,70),CSize(47,10));
   parent->MapDialogRect(&r26);
   mfc26->Create(_T("Sustained"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r26,parent,IDC_C_EG);
   mfcToQtWidget->insert(IDC_C_EG,mfc26);
//   CONTROL         "Key scaling",IDC_C_KSR,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,86,51,10
   CButton* mfc27 = new CButton(parent);
   CRect r27(CPoint(171,86),CSize(51,10));
   parent->MapDialogRect(&r27);
   mfc27->Create(_T("Key scaling"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r27,parent,IDC_C_KSR);
   mfcToQtWidget->insert(IDC_C_KSR,mfc27);
//   CONTROL         "Wave rectification",IDC_C_DM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,232,70,74,10
   CButton* mfc28 = new CButton(parent);
   CRect r28(CPoint(232,70),CSize(74,10));
   parent->MapDialogRect(&r28);
   mfc28->Create(_T("Wave rectification"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r28,parent,IDC_C_DM);
   mfcToQtWidget->insert(IDC_C_DM,mfc28);
//   LTEXT           "Level",IDC_STATIC,226,87,18,8
   CStatic* mfc29 = new CStatic(parent);
   CRect r29(CPoint(226,87),CSize(18,8));
   parent->MapDialogRect(&r29);
   mfc29->Create(_T("Level"),WS_VISIBLE,r29,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_C_KSL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,247,86,59,11
   CSliderCtrl* mfc30 = new CSliderCtrl(parent);
   CRect r30(CPoint(247,86),CSize(59,11));
   parent->MapDialogRect(&r30);
   mfc30->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r30,parent,IDC_C_KSL);
   mfcToQtWidget->insert(IDC_C_KSL,mfc30);
//       LTEXT           "Mult. factor",IDC_STATIC,171,108,38,8
   CStatic* mfc31 = new CStatic(parent);
   CRect r31(CPoint(171,108),CSize(38,8));
   parent->MapDialogRect(&r31);
   mfc31->Create(_T("Mult. factor"),WS_VISIBLE,r31,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_MUL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,208,107,106,11
   CSliderCtrl* mfc32 = new CSliderCtrl(parent);
   CRect r32(CPoint(208,107),CSize(106,11));
   parent->MapDialogRect(&r32);
   mfc32->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r32,parent,IDC_C_MUL);
   mfcToQtWidget->insert(IDC_C_MUL,mfc32);
//       LTEXT           "Attack",IDC_STATIC,171,127,22,8
   CStatic* mfc33 = new CStatic(parent);
   CRect r33(CPoint(171,127),CSize(22,8));
   parent->MapDialogRect(&r33);
   mfc33->Create(_T("Attack"),WS_VISIBLE,r33,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_AR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,199,126,42,11
   CSliderCtrl* mfc34 = new CSliderCtrl(parent);
   CRect r34(CPoint(199,126),CSize(42,11));
   parent->MapDialogRect(&r34);
   mfc34->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r34,parent,IDC_C_AR);
   mfcToQtWidget->insert(IDC_C_AR,mfc34);
//       LTEXT           "Decay",IDC_STATIC,247,127,21,8
   CStatic* mfc35 = new CStatic(parent);
   CRect r35(CPoint(247,127),CSize(21,8));
   parent->MapDialogRect(&r35);
   mfc35->Create(_T("Decay"),WS_VISIBLE,r35,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_DR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,271,126,43,11
   CSliderCtrl* mfc36 = new CSliderCtrl(parent);
   CRect r36(CPoint(271,126),CSize(43,11));
   parent->MapDialogRect(&r36);
   mfc36->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r36,parent,IDC_C_DR);
   mfcToQtWidget->insert(IDC_C_DR,mfc36);
//       LTEXT           "Sustain",IDC_STATIC,171,144,24,8
   CStatic* mfc37 = new CStatic(parent);
   CRect r37(CPoint(171,144),CSize(24,8));
   parent->MapDialogRect(&r37);
   mfc37->Create(_T("Sustain"),WS_VISIBLE,r37,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_SL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,199,143,42,11
   CSliderCtrl* mfc38 = new CSliderCtrl(parent);
   CRect r38(CPoint(199,143),CSize(42,11));
   parent->MapDialogRect(&r38);
   mfc38->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r38,parent,IDC_C_SL);
   mfcToQtWidget->insert(IDC_C_SL,mfc38);
//       LTEXT           "Release",IDC_STATIC,245,144,26,8
   CStatic* mfc39 = new CStatic(parent);
   CRect r39(CPoint(245,144),CSize(26,8));
   parent->MapDialogRect(&r39);
   mfc39->Create(_T("Release"),WS_VISIBLE,r39,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_RR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,271,143,43,11
   CSliderCtrl* mfc40 = new CSliderCtrl(parent);
   CRect r40(CPoint(271,143),CSize(43,11));
   parent->MapDialogRect(&r40);
   mfc40->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r40,parent,IDC_C_RR);
   mfcToQtWidget->insert(IDC_C_RR,mfc40);
//       CTEXT           "Modulator\nlevel",IDC_STATIC,322,42,36,17
   CStatic* mfc41 = new CStatic(parent);
   CRect r41(CPoint(322,42),CSize(36,17));
   parent->MapDialogRect(&r41);
   mfc41->Create(_T("Modulator\nlevel"),SS_CENTER | WS_VISIBLE,r41,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_TL,"msctls_trackbar32",TBS_VERT | TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,329,59,16,53
   CSliderCtrl* mfc42 = new CSliderCtrl(parent);
   CRect r42(CPoint(329,59),CSize(16,53));
   parent->MapDialogRect(&r42);
   mfc42->Create(TBS_VERT | TBS_BOTH | TBS_NOTICKS | WS_TABSTOP | WS_VISIBLE,r42,parent,IDC_TL);
   mfcToQtWidget->insert(IDC_TL,mfc42);
//       CTEXT           "Feedback",IDC_STATIC,322,112,36,9
   CStatic* mfc43 = new CStatic(parent);
   CRect r43(CPoint(322,112),CSize(36,9));
   parent->MapDialogRect(&r43);
   mfc43->Create(_T("Feedback"),SS_CENTER | WS_VISIBLE,r43,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_FB,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,325,124,25,41
   CSliderCtrl* mfc44 = new CSliderCtrl(parent);
   CRect r44(CPoint(325,124),CSize(25,41));
   parent->MapDialogRect(&r44);
   mfc44->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r44,parent,IDC_FB);
   mfcToQtWidget->insert(IDC_FB,mfc44);
//   END
}

void qtMfcInitDialogResource_IDD_CREATEWAV(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_CREATEWAV DIALOGEX 0, 0, 151, 208
   CRect rect(CPoint(0,0),CSize(151,208));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
   
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Create wave file"
   parent->SetWindowText("Create wave file");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       DEFPUSHBUTTON   "Begin",IDC_BEGIN,37,187,52,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(37,187),CSize(52,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Begin"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDC_BEGIN);
   mfcToQtWidget->insert(IDC_BEGIN,mfc1);
//       PUSHBUTTON      "Cancel",IDCANCEL,92,187,52,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(92,187),CSize(52,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
//       GROUPBOX        "Song length",IDC_STATIC,7,7,137,47
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(7,7),CSize(137,47));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Song length"),BS_GROUPBOX | WS_VISIBLE,r3,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "Play the song",IDC_RADIO_LOOP,"Button",BS_AUTORADIOBUTTON,14,20,59,10
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(14,20),CSize(59,10));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Play the song"),BS_AUTORADIOBUTTON | WS_VISIBLE,r4,parent,IDC_RADIO_LOOP);
   mfcToQtWidget->insert(IDC_RADIO_LOOP,mfc4);
//       CONTROL         "Play for",IDC_RADIO_TIME,"Button",BS_AUTORADIOBUTTON,14,38,41,10
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(14,38),CSize(41,10));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Play for"),BS_AUTORADIOBUTTON | WS_VISIBLE,r5,parent,IDC_RADIO_TIME);
   mfcToQtWidget->insert(IDC_RADIO_TIME,mfc5);
//       EDITTEXT        IDC_TIMES,73,19,36,12,ES_AUTOHSCROLL
   CEdit* mfc6 = new CEdit(parent);
   CRect r6(CPoint(73,19),CSize(36,12));
   parent->MapDialogRect(&r6);
   mfc6->Create(ES_AUTOHSCROLL | WS_VISIBLE,r6,parent,IDC_TIMES);
   mfcToQtWidget->insert(IDC_TIMES,mfc6);
//       CONTROL         "",IDC_SPIN_LOOP,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,105,17,11,17
   CSpinButtonCtrl* mfc7 = new CSpinButtonCtrl(parent);
   CRect r7(CPoint(105,17),CSize(11,17));
   parent->MapDialogRect(&r7);
   mfc7->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r7,parent,IDC_SPIN_LOOP);
   mfcToQtWidget->insert(IDC_SPIN_LOOP,mfc7);
//       LTEXT           "time(s)",IDC_STATIC,115,20,21,10,SS_CENTERIMAGE
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(115,20),CSize(21,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("time(s)"),WS_VISIBLE,r8,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_SECONDS,53,37,44,12,ES_AUTOHSCROLL
   CEdit* mfc9 = new CEdit(parent);
   CRect r9(CPoint(53,37),CSize(44,12));
   parent->MapDialogRect(&r9);
   mfc9->Create(ES_AUTOHSCROLL | WS_VISIBLE,r9,parent,IDC_SECONDS);
   mfcToQtWidget->insert(IDC_SECONDS,mfc9);
//       CONTROL         "",IDC_SPIN_TIME,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,93,36,11,14
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(parent);
   CRect r10(CPoint(93,36),CSize(11,14));
   parent->MapDialogRect(&r10);
   mfc10->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,parent,IDC_SPIN_TIME);
   mfcToQtWidget->insert(IDC_SPIN_TIME,mfc10);
//       LTEXT           "mm:ss",IDC_STATIC,106,38,21,10,SS_CENTERIMAGE
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(106,38),CSize(21,10));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("mm:ss"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Channels",IDC_STATIC,7,96,137,87
   CButton* mfc12 = new CButton(parent);
   CRect r12(CPoint(7,96),CSize(137,87));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Channels"),BS_GROUPBOX | WS_VISIBLE,r12,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LISTBOX         IDC_CHANNELS,14,107,124,70,LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP
   CListBox* mfc13 = new CListBox(parent);
   CRect r13(CPoint(14,107),CSize(124,70));
   parent->MapDialogRect(&r13);
   mfc13->Create(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r13,parent,IDC_CHANNELS);
   mfcToQtWidget->insert(IDC_CHANNELS,mfc13);
//   GROUPBOX        "Song",IDC_STATIC,7,60,137,30
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(7,60),CSize(137,30));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Song"),BS_GROUPBOX | WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_TRACKS,14,72,124,30,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc15 = new CComboBox(parent);
   CRect r15(CPoint(14,72),CSize(124,30));
   parent->MapDialogRect(&r15);
   mfc15->Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r15,parent,IDC_TRACKS);
   mfcToQtWidget->insert(IDC_TRACKS,mfc15);
//   END
}

void qtMfcInitDialogResource_IDD_WAVE_PROGRESS(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_WAVE_PROGRESS DIALOGEX 0, 0, 220, 111
   CRect rect(CPoint(0,0),CSize(220,111));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Creating WAV..."
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       PUSHBUTTON      "Cancel",IDC_CANCEL,84,90,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(84,90),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Cancel"),WS_VISIBLE,r1,parent,IDC_CANCEL);
   mfcToQtWidget->insert(IDC_CANCEL,mfc1);
//       CONTROL         "",IDC_PROGRESS_BAR,"msctls_progress32",WS_BORDER,7,65,206,12
   CProgressCtrl* mfc2 = new CProgressCtrl(parent);
   CRect r2(CPoint(7,65),CSize(206,12));
   parent->MapDialogRect(&r2);
   mfc2->Create(WS_BORDER | WS_VISIBLE,r2,parent,IDC_PROGRESS_BAR);
   mfcToQtWidget->insert(IDC_PROGRESS_BAR,mfc2);
//       CTEXT           "Progress",IDC_PROGRESS_LBL,7,37,206,11
   CStatic* mfc3 = new CStatic(parent);
   CRect r3(CPoint(7,37),CSize(206,11));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Progress"),SS_CENTER | WS_VISIBLE,r3,parent,IDC_PROGRESS_LBL);
   mfcToQtWidget->insert(IDC_PROGRESS_LBL,mfc3);
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDFRAME,7,83,206,1
   CStatic* mfc4 = new CStatic(parent);
   CRect r4(CPoint(7,83),CSize(206,1));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T(""),SS_ETCHEDFRAME | WS_VISIBLE,r4,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CTEXT           "File",IDC_PROGRESS_FILE,7,7,206,18,SS_CENTERIMAGE
   CStatic* mfc5 = new CStatic(parent);
   CRect r5(CPoint(7,7),CSize(206,18));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("File"),SS_CENTER | SS_CENTERIMAGE | WS_VISIBLE,r5,parent,IDC_PROGRESS_FILE);
   mfcToQtWidget->insert(IDC_PROGRESS_FILE,mfc5);
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDFRAME,7,29,206,1
   CStatic* mfc6 = new CStatic(parent);
   CRect r6(CPoint(7,29),CSize(206,1));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T(""),SS_ETCHEDFRAME | WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CTEXT           "Progress",IDC_TIME,7,49,206,11
   CStatic* mfc7 = new CStatic(parent);
   CRect r7(CPoint(7,49),CSize(206,11));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Progress"),SS_CENTER | WS_VISIBLE,r7,parent,IDC_TIME);
   mfcToQtWidget->insert(IDC_TIME,mfc7);
//   END
}

void qtMfcInitDialogResource_IDD_MAINBAR(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_MAINBAR DIALOGEX 0, 0, 111, 128
   CRect rect(CPoint(0,0),CSize(111,128));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   EXSTYLE WS_EX_TRANSPARENT | WS_EX_CONTROLPARENT
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   END
}

void qtMfcInitDialogResource_IDD_INSTRUMENT_FDS(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_INSTRUMENT_FDS DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Wave editor",IDC_STATIC,7,7,228,111
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(228,111));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Wave editor"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Frequency modulation",IDC_STATIC,240,7,124,79
   CButton* mfc16 = new CButton(parent);
   CRect r16(CPoint(240,7),CSize(124,79));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Frequency modulation"),BS_GROUPBOX | WS_VISIBLE,r16,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       PUSHBUTTON      "Sine",IDC_PRESET_SINE,14,101,41,12
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(14,101),CSize(41,12));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Sine"),WS_VISIBLE,r2,parent,IDC_PRESET_SINE);
   mfcToQtWidget->insert(IDC_PRESET_SINE,mfc2);
//       PUSHBUTTON      "Triangle",IDC_PRESET_TRIANGLE,57,101,41,12
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(57,101),CSize(41,12));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Triangle"),WS_VISIBLE,r3,parent,IDC_PRESET_TRIANGLE);
   mfcToQtWidget->insert(IDC_PRESET_TRIANGLE,mfc3);
//       PUSHBUTTON      "Sawtooth",IDC_PRESET_SAWTOOTH,100,101,41,12
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(100,101),CSize(41,12));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Sawtooth"),WS_VISIBLE,r4,parent,IDC_PRESET_SAWTOOTH);
   mfcToQtWidget->insert(IDC_PRESET_SAWTOOTH,mfc4);
//       LTEXT           "Modulation rate",IDC_STATIC,246,24,51,8
   CStatic* mfc5 = new CStatic(parent);
   CRect r5(CPoint(246,24),CSize(51,8));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Modulation rate"),WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_RATE,318,22,37,14,ES_AUTOHSCROLL | ES_NUMBER
   CEdit* mfc6 = new CEdit(parent);
   CRect r6(CPoint(318,22),CSize(37,14));
   parent->MapDialogRect(&r6);
   mfc6->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r6,parent,IDC_MOD_RATE);
   mfcToQtWidget->insert(IDC_MOD_RATE,mfc6);
//       CONTROL         "",IDC_MOD_RATE_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_NOTHOUSANDS,344,25,11,14
   CSpinButtonCtrl* mfc7 = new CSpinButtonCtrl(parent);
   CRect r7(CPoint(344,25),CSize(11,14));
   parent->MapDialogRect(&r7);
   mfc7->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_NOTHOUSANDS | WS_VISIBLE,r7,parent,IDC_MOD_RATE_SPIN);
   mfcToQtWidget->insert(IDC_MOD_RATE_SPIN,mfc7);
//       LTEXT           "Modulation depth",IDC_STATIC,246,46,56,8
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(246,46),CSize(56,8));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Modulation depth"),WS_VISIBLE,r8,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_DEPTH,318,43,37,14,ES_AUTOHSCROLL | ES_NUMBER
   CEdit* mfc9 = new CEdit(parent);
   CRect r9(CPoint(318,43),CSize(37,14));
   parent->MapDialogRect(&r9);
   mfc9->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r9,parent,IDC_MOD_DEPTH);
   mfcToQtWidget->insert(IDC_MOD_DEPTH,mfc9);
//       CONTROL         "",IDC_MOD_DEPTH_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,344,43,11,14
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(parent);
   CRect r10(CPoint(344,43),CSize(11,14));
   parent->MapDialogRect(&r10);
   mfc10->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,parent,IDC_MOD_DEPTH_SPIN);
   mfcToQtWidget->insert(IDC_MOD_DEPTH_SPIN,mfc10);
//       LTEXT           "Modulation delay",IDC_STATIC,246,68,55,8
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(246,68),CSize(55,8));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Modulation delay"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_DELAY,318,65,37,14,ES_AUTOHSCROLL | ES_NUMBER
   CEdit* mfc12 = new CEdit(parent);
   CRect r12(CPoint(318,65),CSize(37,14));
   parent->MapDialogRect(&r12);
   mfc12->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r12,parent,IDC_MOD_DELAY);
   mfcToQtWidget->insert(IDC_MOD_DELAY,mfc12);
//       CONTROL         "",IDC_MOD_DELAY_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,344,63,11,14
   CSpinButtonCtrl* mfc13 = new CSpinButtonCtrl(parent);
   CRect r13(CPoint(344,63),CSize(11,14));
   parent->MapDialogRect(&r13);
   mfc13->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r13,parent,IDC_MOD_DELAY_SPIN);
   mfcToQtWidget->insert(IDC_MOD_DELAY_SPIN,mfc13);
//       PUSHBUTTON      "Flat",IDC_MOD_PRESET_FLAT,318,131,46,12
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(318,131),CSize(46,12));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Flat"),WS_VISIBLE,r14,parent,IDC_MOD_PRESET_FLAT);
   mfcToQtWidget->insert(IDC_MOD_PRESET_FLAT,mfc14);
//       PUSHBUTTON      "Sine",IDC_MOD_PRESET_SINE,318,148,46,12
   CButton* mfc15 = new CButton(parent);
   CRect r15(CPoint(318,148),CSize(46,12));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("Sine"),WS_VISIBLE,r15,parent,IDC_MOD_PRESET_SINE);
   mfcToQtWidget->insert(IDC_MOD_PRESET_SINE,mfc15);
//       PUSHBUTTON      "Copy wave",IDC_COPY_WAVE,240,90,52,14
   CButton* mfc17 = new CButton(parent);
   CRect r17(CPoint(240,90),CSize(52,14));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("Copy wave"),WS_VISIBLE,r17,parent,IDC_COPY_WAVE);
   mfcToQtWidget->insert(IDC_COPY_WAVE,mfc17);
//       PUSHBUTTON      "Paste wave",IDC_PASTE_WAVE,240,104,52,14
   CButton* mfc18 = new CButton(parent);
   CRect r18(CPoint(240,104),CSize(52,14));
   parent->MapDialogRect(&r18);
   mfc18->Create(_T("Paste wave"),WS_VISIBLE,r18,parent,IDC_PASTE_WAVE);
   mfcToQtWidget->insert(IDC_PASTE_WAVE,mfc18);
//       PUSHBUTTON      "Copy table",IDC_COPY_TABLE,305,90,50,14
   CButton* mfc19 = new CButton(parent);
   CRect r19(CPoint(305,90),CSize(50,14));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Copy table"),WS_VISIBLE,r19,parent,IDC_COPY_TABLE);
   mfcToQtWidget->insert(IDC_COPY_TABLE,mfc19);
//       PUSHBUTTON      "Paste table",IDC_PASTE_TABLE,305,104,50,14
   CButton* mfc20 = new CButton(parent);
   CRect r20(CPoint(305,104),CSize(50,14));
   parent->MapDialogRect(&r20);
   mfc20->Create(_T("Paste table"),WS_VISIBLE,r20,parent,IDC_PASTE_TABLE);
   mfcToQtWidget->insert(IDC_PASTE_TABLE,mfc20);
//       PUSHBUTTON      "50% pulse",IDC_PRESET_PULSE_50,143,101,41,12
   CButton* mfc21 = new CButton(parent);
   CRect r21(CPoint(143,101),CSize(41,12));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("50% pulse"),WS_VISIBLE,r21,parent,IDC_PRESET_PULSE_50);
   mfcToQtWidget->insert(IDC_PRESET_PULSE_50,mfc21);
//       PUSHBUTTON      "25% pulse",IDC_PRESET_PULSE_25,186,101,41,12
   CButton* mfc22 = new CButton(parent);
   CRect r22(CPoint(186,101),CSize(41,12));
   parent->MapDialogRect(&r22);
   mfc22->Create(_T("25% pulse"),WS_VISIBLE,r22,parent,IDC_PRESET_PULSE_25);
   mfcToQtWidget->insert(IDC_PRESET_PULSE_25,mfc22);
//   END
}

void qtMfcInitDialogResource_IDD_FRAMECONTROLS(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_FRAMECONTROLS DIALOGEX 0, 0, 107, 12
   CRect rect(CPoint(0,0),CSize(107,12));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD | WS_SYSMENU
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       PUSHBUTTON      "+",IDC_FRAME_INC,0,0,17,12
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(0,0),CSize(17,12));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("+"),WS_VISIBLE,r1,parent,IDC_FRAME_INC);
   mfcToQtWidget->insert(IDC_FRAME_INC,mfc1);
//       PUSHBUTTON      "-",IDC_FRAME_DEC,23,0,17,12
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(23,0),CSize(17,12));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("-"),WS_VISIBLE,r2,parent,IDC_FRAME_DEC);
   mfcToQtWidget->insert(IDC_FRAME_DEC,mfc2);
//       CONTROL         "Change all",IDC_CHANGE_ALL,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,51,2,56,10
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(51,2),CSize(56,10));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Change all"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_CHANGE_ALL);
   mfcToQtWidget->insert(IDC_CHANGE_ALL,mfc3);
//   END
}

void qtMfcInitDialogResource_IDD_SAMPLE_EDITOR(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_SAMPLE_EDITOR DIALOGEX 0, 0, 481, 255
   CRect rect(CPoint(0,0),CSize(481,255));
   parent->MapDialogRect(&rect);
   parent->toQWidget()->resize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_MAXIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowMaximizeButtonHint|Qt::WindowTitleHint);
//   CAPTION "DPCM sample editor"
   parent->SetWindowText("DPCM sample editor");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN   
//   DEFPUSHBUTTON   "OK",IDOK,424,217,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(424,217),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc1);
//   PUSHBUTTON      "Cancel",IDCANCEL,424,234,50,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(424,234),CSize(50,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
//   CONTROL         "",IDC_SAMPLE,"Static",SS_WHITERECT | SS_NOTIFY | SS_SUNKEN,7,7,467,204
   CStatic *mfc3 = new CStatic(parent);
   CRect r3(CPoint(7,7),CSize(467,204));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T(""),SS_WHITERECT | SS_NOTIFY | SS_SUNKEN | WS_VISIBLE,r3,parent,IDC_SAMPLE);
   mfcToQtWidget->insert(IDC_SAMPLE,mfc3);
//   PUSHBUTTON      "Play",IDC_PLAY,7,233,50,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(7,233),CSize(50,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Play"),WS_VISIBLE,r4,parent,IDC_PLAY);
   mfcToQtWidget->insert(IDC_PLAY,mfc4);
//   LTEXT           "Offset: 0, Pos: 0",IDC_POS,7,217,105,11,SS_SUNKEN
   CStatic* mfc5 = new CStatic(parent);
   CRect r5(CPoint(7,217),CSize(105,11));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Offset: 0, Pos: 0"),SS_SUNKEN | WS_VISIBLE,r5,parent,IDC_POS);
   mfcToQtWidget->insert(IDC_POS,mfc5);
//   PUSHBUTTON      "Delete",IDC_DELETE,204,234,50,14
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(204,234),CSize(50,14));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Delete"),WS_VISIBLE,r6,parent,IDC_DELETE);
   mfcToQtWidget->insert(IDC_DELETE,mfc6);
//   CONTROL         "",IDC_PITCH,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,110,236,79,12
   CSliderCtrl* mfc7 = new CSliderCtrl(parent);
   CRect r7(CPoint(110,236),CSize(79,12));
   parent->MapDialogRect(&r7);
   mfc7->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_PITCH);
   mfcToQtWidget->insert(IDC_PITCH,mfc7);
//   PUSHBUTTON      "Tilt",IDC_TILT,257,234,50,14
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(257,234),CSize(50,14));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Tilt"),WS_VISIBLE,r8,parent,IDC_TILT);
   mfcToQtWidget->insert(IDC_TILT,mfc8);
//   LTEXT           "0 bytes",IDC_INFO,115,217,87,11,SS_SUNKEN
   CStatic* mfc9 = new CStatic(parent);
   CRect r9(CPoint(115,217),CSize(87,11));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("0 bytes"),SS_SUNKEN | WS_VISIBLE,r9,parent,IDC_INFO);
   mfcToQtWidget->insert(IDC_INFO,mfc9);
//   LTEXT           "Pitch (15)",IDC_STATIC_PITCH,63,236,34,11
   CStatic* mfc10 = new CStatic(parent);
   CRect r10(CPoint(63,236),CSize(34,11));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Pitch (15)"),WS_VISIBLE,r10,parent,IDC_STATIC_PITCH);
   mfcToQtWidget->insert(IDC_STATIC_PITCH,mfc10);
//   CONTROL         "Center wave",IDC_DELTASTART,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,314,238,57,10
   CButton* mfc11 = new CButton(parent);
   CRect r11(CPoint(314,238),CSize(57,10));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Center wave"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r11,parent,IDC_DELTASTART);
   mfcToQtWidget->insert(IDC_DELTASTART,mfc11);
//   CONTROL         "",IDC_ZOOM,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,360,217,61,12
   CSliderCtrl* mfc12 = new CSliderCtrl(parent);
   CRect r12(CPoint(360,217),CSize(61,12));
   parent->MapDialogRect(&r12);
   mfc12->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r12,parent,IDC_ZOOM);
   mfcToQtWidget->insert(IDC_ZOOM,mfc12);
//   END
}

void qtMfcInitDialogResource_IDD_INSTRUMENT_FDS_ENVELOPE(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_INSTRUMENT_FDS_ENVELOPE DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       EDITTEXT        IDC_SEQUENCE_STRING,75,151,290,14,ES_AUTOHSCROLL
   CEdit* mfc1 = new CEdit(parent);
   CRect r1(CPoint(75,151),CSize(290,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(ES_AUTOHSCROLL | WS_VISIBLE,r1,parent,IDC_SEQUENCE_STRING);
   mfcToQtWidget->insert(IDC_SEQUENCE_STRING,mfc1);
//       COMBOBOX        IDC_TYPE,8,151,63,30,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc2 = new CComboBox(parent);
   CRect r2(CPoint(8,151),CSize(63,30));
   parent->MapDialogRect(&r2);
   mfc2->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_TYPE);
   mfcToQtWidget->insert(IDC_TYPE,mfc2);
//   IDD_INSTRUMENT_FDS_ENVELOPE DLGINIT
//   BEGIN
//       IDC_TYPE, 0x403, 7, 0
//   0x6f56, 0x756c, 0x656d, "\000"
   mfc2->AddString(_T("Volume"));
//       IDC_TYPE, 0x403, 9, 0
//   0x7241, 0x6570, 0x6767, 0x6f69, "\000"
   mfc2->AddString(_T("Arpeggio"));
//       IDC_TYPE, 0x403, 6, 0
//   0x6950, 0x6374, 0x0068,
//       0
   mfc2->AddString(_T("Pitch"));
//   END
//   END
}

void qtMfcInitDialogResource_IDD_CHANNELS(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_CHANNELS DIALOGEX 0, 0, 303, 236
   CRect rect(CPoint(0,0),CSize(303,236));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Channels"
   parent->SetWindowText("Channels");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Available channels",IDC_STATIC,7,7,138,197
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(7,7),CSize(138,197));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Available channels"),BS_GROUPBOX | WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Added channels",IDC_STATIC,151,7,145,197
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(151,7),CSize(145,197));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Added channels"),BS_GROUPBOX | WS_VISIBLE,r7,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "OK",IDOK,194,215,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(194,215),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc1);
//       PUSHBUTTON      "Cancel",IDCANCEL,246,215,50,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(246,215),CSize(50,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
//       PUSHBUTTON      "Move up",IDC_MOVE_UP,7,215,50,14
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(7,215),CSize(50,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Move up"),WS_VISIBLE,r3,parent,IDC_MOVE_UP);
   mfcToQtWidget->insert(IDC_MOVE_UP,mfc3);
//       PUSHBUTTON      "Move down",IDC_MOVE_DOWN,62,215,50,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(62,215),CSize(50,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Move down"),WS_VISIBLE,r4,parent,IDC_MOVE_DOWN);
   mfcToQtWidget->insert(IDC_MOVE_DOWN,mfc4);
//       CONTROL         "",IDC_ADDED_LIST,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP,159,17,129,181
   CListCtrl* mfc5 = new CListCtrl(parent);
   CRect r5(CPoint(159,17),CSize(129,181));
   parent->MapDialogRect(&r5);
   mfc5->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r5,parent,IDC_ADDED_LIST);
   mfcToQtWidget->insert(IDC_ADDED_LIST,mfc5);
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,209,289,1
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(7,209),CSize(289,1));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T(""),SS_ETCHEDHORZ | WS_VISIBLE,r8,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_AVAILABLE_TREE,"SysTreeView32",TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | WS_BORDER | WS_HSCROLL | WS_TABSTOP,14,17,122,181
   CTreeCtrl* mfc9 = new CTreeCtrl(parent);
   CRect r9(CPoint(14,17),CSize(122,181));
   parent->MapDialogRect(&r9);
   mfc9->Create(TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | WS_BORDER | WS_HSCROLL | WS_TABSTOP | WS_VISIBLE,r9,parent,IDC_AVAILABLE_TREE);
   mfcToQtWidget->insert(IDC_AVAILABLE_TREE,mfc9);
//   END
}

void qtMfcInitDialogResource_IDD_COMMENTS(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_COMMENTS DIALOGEX 0, 0, 358, 230
   CRect rect(CPoint(0,0),CSize(358,230));
   parent->MapDialogRect(&rect);
   parent->toQWidget()->resize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowTitleHint);
//   CAPTION "Module comments"
   parent->SetWindowText("Module comments");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   PUSHBUTTON      "Cancel",IDCANCEL,304,211,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(304,211),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Cancel"),WS_VISIBLE,r1,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc1);
//   EDITTEXT        IDC_COMMENTS,0,0,358,206,ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL
   CEdit* mfc2 = new CEdit(parent);
   CRect r2(CPoint(0,0),CSize(358,206));
   parent->MapDialogRect(&r2);
   mfc2->Create(ES_MULTILINE | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_VISIBLE,r2,parent,IDC_COMMENTS);
   mfcToQtWidget->insert(IDC_COMMENTS,mfc2);
//   CONTROL         "Show when loading file",IDC_SHOWONOPEN,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,5,215,89,10
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(5,215),CSize(89,10));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Show when loading file"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_SHOWONOPEN);
   mfcToQtWidget->insert(IDC_SHOWONOPEN,mfc3);
//   DEFPUSHBUTTON   "OK",IDOK,248,211,50,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(248,211),CSize(50,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r4,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc4);
//   END
}

void qtMfcInitDialogResource_IDD_CONFIG_MIXER(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//IDD_CONFIG_MIXER DIALOGEX 0, 0, 280, 167
   CRect rect(CPoint(0,0),CSize(280,167));
   parent->MapDialogRect(&rect);
   parent->toQWidget()->setFixedSize(rect.Width(),rect.Height());
//STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//EXSTYLE WS_EX_CONTEXTHELP
//CAPTION "Mixer"
   parent->SetWindowText("Mixer");
//FONT 8, "MS Shell Dlg", 400, 0, 0x1
//BEGIN
//    CONTROL         "",IDC_SLIDER_APU1,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,7,30,26,114
   CSliderCtrl* mfc1 = new CSliderCtrl(parent);
   CRect r1(CPoint(7,30),CSize(26,114));
   parent->MapDialogRect(&r1);
   mfc1->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r1,parent,IDC_SLIDER_APU1);
   mfcToQtWidget->insert(IDC_SLIDER_APU1,mfc1);
//    CTEXT           "APU1",IDC_STATIC,7,22,26,8
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(7,22),CSize(26,8));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("APU1"),SS_CENTER | WS_VISIBLE,r2,parent,IDC_STATIC);
   // IDC_STATIC do not get added to widget list
//    CONTROL         "",IDC_SLIDER_APU2,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,41,30,26,114
   CSliderCtrl* mfc3 = new CSliderCtrl(parent);
   CRect r3(CPoint(41,30),CSize(26,114));
   parent->MapDialogRect(&r3);
   mfc3->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_SLIDER_APU2);
   mfcToQtWidget->insert(IDC_SLIDER_APU2,mfc3);
//    CTEXT           "APU2",IDC_STATIC,41,22,26,8
   CStatic* mfc4 = new CStatic(parent);
   CRect r4(CPoint(41,22),CSize(26,8));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("APU2"),SS_CENTER | WS_VISIBLE,r4,parent,IDC_STATIC);
   // IDC_STATIC do not get added to widget list
//    CONTROL         "",IDC_SLIDER_VRC6,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,75,30,26,114
   CSliderCtrl* mfc5 = new CSliderCtrl(parent);
   CRect r5(CPoint(75,30),CSize(26,114));
   parent->MapDialogRect(&r5);
   mfc5->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r5,parent,IDC_SLIDER_VRC6);
   mfcToQtWidget->insert(IDC_SLIDER_VRC6,mfc5);
//    CTEXT           "VRC6",IDC_STATIC,75,22,26,8
   CStatic* mfc6 = new CStatic(parent);
   CRect r6(CPoint(75,22),CSize(26,8));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("VRC6"),SS_CENTER | WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to widget list
//    CONTROL         "",IDC_SLIDER_VRC7,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,109,30,26,114
   CSliderCtrl* mfc7 = new CSliderCtrl(parent);
   CRect r7(CPoint(109,30),CSize(26,114));
   parent->MapDialogRect(&r7);
   mfc7->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_SLIDER_VRC7);
   mfcToQtWidget->insert(IDC_SLIDER_VRC7,mfc7);
//    CTEXT           "VRC7",IDC_STATIC,109,22,26,8
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(109,22),CSize(26,8));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("VRC7"),SS_CENTER | WS_VISIBLE,r8,parent,IDC_STATIC);
   // IDC_STATIC do not get added to widget list
//    CONTROL         "",IDC_SLIDER_MMC5,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,143,30,26,114
   CSliderCtrl* mfc9 = new CSliderCtrl(parent);
   CRect r9(CPoint(143,30),CSize(26,114));
   parent->MapDialogRect(&r9);
   mfc9->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r9,parent,IDC_SLIDER_MMC5);
   mfcToQtWidget->insert(IDC_SLIDER_MMC5,mfc9);
//    CTEXT           "MMC5",IDC_STATIC,143,22,26,8
   CStatic* mfc10 = new CStatic(parent);
   CRect r10(CPoint(143,22),CSize(26,8));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("MMC5"),SS_CENTER | WS_VISIBLE,r10,parent,IDC_STATIC);
   // IDC_STATIC do not get added to widget list
//    CONTROL         "",IDC_SLIDER_FDS,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,177,30,26,114
   CSliderCtrl* mfc11 = new CSliderCtrl(parent);
   CRect r11(CPoint(177,30),CSize(26,114));
   parent->MapDialogRect(&r11);
   mfc11->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r11,parent,IDC_SLIDER_FDS);
   mfcToQtWidget->insert(IDC_SLIDER_FDS,mfc11);
//    CTEXT           "FDS",IDC_STATIC,177,22,26,8
   CStatic* mfc12 = new CStatic(parent);
   CRect r12(CPoint(177,22),CSize(26,8));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("FDS"),SS_CENTER | WS_VISIBLE,r12,parent,IDC_STATIC);
   // IDC_STATIC do not get added to widget list
//    CONTROL         "",IDC_SLIDER_N163,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,211,30,26,114
   CSliderCtrl* mfc13 = new CSliderCtrl(parent);
   CRect r13(CPoint(211,30),CSize(26,114));
   parent->MapDialogRect(&r13);
   mfc13->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r13,parent,IDC_SLIDER_N163);
   mfcToQtWidget->insert(IDC_SLIDER_N163,mfc13);
//    CTEXT           "N163",IDC_STATIC,211,22,26,8
   CStatic* mfc14 = new CStatic(parent);
   CRect r14(CPoint(211,22),CSize(26,8));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("N163"),SS_CENTER | WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to widget list
//    CONTROL         "",IDC_SLIDER_S5B,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_DISABLED | WS_TABSTOP,245,30,26,114
   CSliderCtrl* mfc15 = new CSliderCtrl(parent);
   CRect r15(CPoint(245,30),CSize(26,114));
   parent->MapDialogRect(&r15);
   mfc15->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r15,parent,IDC_SLIDER_S5B);
   mfcToQtWidget->insert(IDC_SLIDER_S5B,mfc15);
//    CTEXT           "S5B",IDC_STATIC,245,22,26,8
   CStatic* mfc16 = new CStatic(parent);
   CRect r16(CPoint(245,22),CSize(26,8));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("S5B"),SS_CENTER | WS_VISIBLE,r16,parent,IDC_STATIC);
   // IDC_STATIC do not get added to widget list
//    CTEXT           "0.0dB",IDC_LEVEL_APU1,7,149,30,8
   CStatic* mfc17 = new CStatic(parent);
   CRect r17(CPoint(7,149),CSize(30,8));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("0.0dB"),SS_CENTER | WS_VISIBLE,r17,parent,IDC_LEVEL_APU1);
   mfcToQtWidget->insert(IDC_LEVEL_APU1,mfc17);
//    CTEXT           "0.0dB",IDC_LEVEL_APU2,40,149,30,8
   CStatic* mfc18 = new CStatic(parent);
   CRect r18(CPoint(40,149),CSize(30,8));
   parent->MapDialogRect(&r18);
   mfc18->Create(_T("0.0dB"),SS_CENTER | WS_VISIBLE,r18,parent,IDC_LEVEL_APU2);
   mfcToQtWidget->insert(IDC_LEVEL_APU2,mfc18);
//    CTEXT           "0.0dB",IDC_LEVEL_VRC6,73,149,30,8
   CStatic* mfc19 = new CStatic(parent);
   CRect r19(CPoint(73,149),CSize(30,8));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("0.0dB"),SS_CENTER | WS_VISIBLE,r19,parent,IDC_LEVEL_VRC6);
   mfcToQtWidget->insert(IDC_LEVEL_VRC6,mfc19);
//    CTEXT           "0.0dB",IDC_LEVEL_VRC7,108,149,30,8
   CStatic* mfc20 = new CStatic(parent);
   CRect r20(CPoint(108,149),CSize(30,8));
   parent->MapDialogRect(&r20);
   mfc20->Create(_T("0.0dB"),SS_CENTER | WS_VISIBLE,r20,parent,IDC_LEVEL_VRC7);
   mfcToQtWidget->insert(IDC_LEVEL_VRC7,mfc20);
//    CTEXT           "0.0dB",IDC_LEVEL_MMC5,142,149,30,8
   CStatic* mfc21 = new CStatic(parent);
   CRect r21(CPoint(142,149),CSize(30,8));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("0.0dB"),SS_CENTER | WS_VISIBLE,r21,parent,IDC_LEVEL_MMC5);
   mfcToQtWidget->insert(IDC_LEVEL_MMC5,mfc21);
//    CTEXT           "0.0dB",IDC_LEVEL_FDS,176,149,30,8
   CStatic* mfc22 = new CStatic(parent);
   CRect r22(CPoint(176,149),CSize(30,8));
   parent->MapDialogRect(&r22);
   mfc22->Create(_T("0.0dB"),SS_CENTER | WS_VISIBLE,r22,parent,IDC_LEVEL_FDS);
   mfcToQtWidget->insert(IDC_LEVEL_FDS,mfc22);
//    CTEXT           "0.0dB",IDC_LEVEL_N163,209,149,30,8
   CStatic* mfc23 = new CStatic(parent);
   CRect r23(CPoint(209,149),CSize(30,8));
   parent->MapDialogRect(&r23);
   mfc23->Create(_T("0.0dB"),SS_CENTER | WS_VISIBLE,r23,parent,IDC_LEVEL_N163);
   mfcToQtWidget->insert(IDC_LEVEL_N163,mfc23);
//    CTEXT           "0.0dB",IDC_LEVEL_S5B,243,149,30,8
   CStatic* mfc24 = new CStatic(parent);
   CRect r24(CPoint(243,149),CSize(30,8));
   parent->MapDialogRect(&r24);
   mfc24->Create(_T("0.0dB"),SS_CENTER | WS_VISIBLE,r24,parent,IDC_LEVEL_S5B);
   mfcToQtWidget->insert(IDC_LEVEL_S5B,mfc24);
//    LTEXT           "Hardware mixing levels",IDC_STATIC,7,7,74,8
   CStatic* mfc25 = new CStatic(parent);
   CRect r25(CPoint(7,7),CSize(74,8));
   parent->MapDialogRect(&r25);
   mfc25->Create(_T("Hardware mixing levels"),WS_VISIBLE,r25,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//END
}

void qtMfcInitDialogResource_IDD_INSTRUMENT_N163_WAVE(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_INSTRUMENT_N163_WAVE DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_CHILD | WS_SYSMENU
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Wave editor",IDC_STATIC,7,7,228,116
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(7,7),CSize(228,116));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Wave editor"),BS_GROUPBOX | WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Wave #",IDC_STATIC,244,7,121,35
   CButton* mfc21 = new CButton(parent);
   CRect r21(CPoint(244,7),CSize(121,35));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("Wave #"),BS_GROUPBOX | WS_VISIBLE,r21,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "MML string",IDC_STATIC,7,132,358,33
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(7,132),CSize(358,33));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("MML string"),BS_GROUPBOX | WS_VISIBLE,r7,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Wave RAM settings",IDC_STATIC,244,45,121,52
   CButton* mfc16 = new CButton(parent);
   CRect r16(CPoint(244,45),CSize(121,52));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Wave RAM settings"),BS_GROUPBOX | WS_VISIBLE,r16,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       PUSHBUTTON      "Sine",IDC_PRESET_SINE,14,101,41,12
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(14,104),CSize(41,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Sine"),WS_VISIBLE,r2,parent,IDC_PRESET_SINE);
   mfcToQtWidget->insert(IDC_PRESET_SINE,mfc2);
//   PUSHBUTTON      "Triangle",IDC_PRESET_TRIANGLE,57,104,41,14
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(57,104),CSize(41,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Triangle"),WS_VISIBLE,r3,parent,IDC_PRESET_TRIANGLE);
   mfcToQtWidget->insert(IDC_PRESET_TRIANGLE,mfc3);
//   PUSHBUTTON      "Sawtooth",IDC_PRESET_SAWTOOTH,100,104,41,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(100,104),CSize(41,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Sawtooth"),WS_VISIBLE,r4,parent,IDC_PRESET_SAWTOOTH);
   mfcToQtWidget->insert(IDC_PRESET_SAWTOOTH,mfc4);
//   PUSHBUTTON      "50% pulse",IDC_PRESET_PULSE_50,143,104,41,14
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(143,104),CSize(41,14));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("50% pulse"),WS_VISIBLE,r5,parent,IDC_PRESET_PULSE_50);
   mfcToQtWidget->insert(IDC_PRESET_PULSE_50,mfc5);
//   PUSHBUTTON      "25% pulse",IDC_PRESET_PULSE_25,186,104,41,14
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(186,104),CSize(41,14));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("25% pulse"),WS_VISIBLE,r6,parent,IDC_PRESET_PULSE_25);
   mfcToQtWidget->insert(IDC_PRESET_PULSE_25,mfc6);
//       EDITTEXT        IDC_MML,16,145,342,14,ES_AUTOHSCROLL
   CEdit* mfc8 = new CEdit(parent);
   CRect r8(CPoint(16,145),CSize(342,14));
   parent->MapDialogRect(&r8);
   mfc8->Create(ES_AUTOHSCROLL | WS_VISIBLE,r8,parent,IDC_MML);
   mfcToQtWidget->insert(IDC_MML,mfc8);
//   PUSHBUTTON      "Copy wave",IDC_COPY,249,105,50,14
   CButton* mfc9 = new CButton(parent);
   CRect r9(CPoint(249,105),CSize(50,14));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("Copy wave"),WS_VISIBLE,r9,parent,IDC_COPY);
   mfcToQtWidget->insert(IDC_COPY,mfc9);
//   PUSHBUTTON      "Paste wave",IDC_PASTE,308,105,50,14
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(308,105),CSize(50,14));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Paste wave"),WS_VISIBLE,r10,parent,IDC_PASTE);
   mfcToQtWidget->insert(IDC_PASTE,mfc10);
//       EDITTEXT        IDC_INDEX,253,20,39,12,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc11 = new CEdit(parent);
   CRect r11(CPoint(253,20),CSize(39,12));
   parent->MapDialogRect(&r11);
   mfc11->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r11,parent,IDC_INDEX);
   mfcToQtWidget->insert(IDC_INDEX,mfc11);
//       CONTROL         "",IDC_INDEX_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,281,26,11,14
   CSpinButtonCtrl* mfc12 = new CSpinButtonCtrl(parent);
   CRect r12(CPoint(281,26),CSize(11,12));
   parent->MapDialogRect(&r12);
   mfc12->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r12,parent,IDC_INDEX_SPIN);
   mfcToQtWidget->insert(IDC_INDEX_SPIN,mfc12);
//       LTEXT           "of",IDC_STATIC,300,22,8,8
   CStatic* mfc13 = new CStatic(parent);
   CRect r13(CPoint(300,22),CSize(8,8));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("of"),WS_VISIBLE,r13,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_WAVES,316,20,37,12,ES_AUTOHSCROLL | ES_READONLY
   CEdit* mfc14 = new CEdit(parent);
   CRect r14(CPoint(316,20),CSize(37,12));
   parent->MapDialogRect(&r14);
   mfc14->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r14,parent,IDC_WAVES);
   mfcToQtWidget->insert(IDC_WAVES,mfc14);
//       CONTROL         "",IDC_WAVES_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,342,19,11,14
   CSpinButtonCtrl* mfc15 = new CSpinButtonCtrl(parent);
   CRect r15(CPoint(342,19),CSize(11,14));
   parent->MapDialogRect(&r15);
   mfc15->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r15,parent,IDC_WAVES_SPIN);
   mfcToQtWidget->insert(IDC_WAVES_SPIN,mfc15);
//       LTEXT           "Wave size",IDC_STATIC,253,59,34,8
   CStatic* mfc17 = new CStatic(parent);
   CRect r17(CPoint(253,59),CSize(34,8));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("Wave size"),WS_VISIBLE,r17,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_WAVE_SIZE,305,57,48,30,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc18 = new CComboBox(parent);
   CRect r18(CPoint(305,57),CSize(48,30));
   parent->MapDialogRect(&r18);
   mfc18->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r18,parent,IDC_WAVE_SIZE);
   mfcToQtWidget->insert(IDC_WAVE_SIZE,mfc18);
//       LTEXT           "Wave position",IDC_STATIC,253,78,46,8
   CStatic* mfc19 = new CStatic(parent);
   CRect r19(CPoint(253,78),CSize(46,8));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Wave position"),WS_VISIBLE,r19,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_WAVE_POS,305,75,48,30,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc20 = new CComboBox(parent);
   CRect r20(CPoint(305,75),CSize(48,30));
   parent->MapDialogRect(&r20);
   mfc20->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r20,parent,IDC_WAVE_POS);
   mfcToQtWidget->insert(IDC_WAVE_POS,mfc20);
//   END
}

void qtMfcInitDialogResource_IDD_IMPORT(CDialog* parent)
{
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();

//   IDD_IMPORT DIALOGEX 0, 0, 161, 209
   CRect rect(CPoint(0,0),CSize(161,209));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
   parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags()|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint);
//   CAPTION "Import options"
   parent->SetWindowText("Import options");
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Tracks",IDC_STATIC,7,7,147,137
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(7,7),CSize(147,137));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Tracks"),BS_GROUPBOX | WS_VISIBLE,r3,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Options",IDC_STATIC,7,148,147,29
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(7,148),CSize(147,29));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Options"),BS_GROUPBOX | WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "OK",IDOK,51,188,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(51,188),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc1);
//       PUSHBUTTON      "Cancel",IDCANCEL,104,188,50,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(104,188),CSize(50,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
//       CONTROL         "Include instruments",IDC_INSTRUMENTS,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,161,116,10
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(14,161),CSize(116,10));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Include instruments"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r4,parent,IDC_INSTRUMENTS);
   mfcToQtWidget->insert(IDC_INSTRUMENTS,mfc4);
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,183,147,1
   CStatic* mfc5 = new CStatic(parent);
   CRect r5(CPoint(7,183),CSize(147,1));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T(""),SS_ETCHEDHORZ | WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   LISTBOX         IDC_TRACKS,14,18,133,120,LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP
   CListBox* mfc7 = new CListBox(parent);
   CRect r7(CPoint(14,18),CSize(133,120));
   parent->MapDialogRect(&r7);
   mfc7->Create(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_TRACKS);
   mfcToQtWidget->insert(IDC_TRACKS,mfc7);
//   END
}

void qtMfcInitDialogResource(UINT dlgID,CDialog* parent)
{
   switch ( dlgID )
   {
   case IDD_ABOUTBOX:
      qtMfcInitDialogResource_IDD_ABOUTBOX(parent);
      break;
   case IDD_PERFORMANCE:
      qtMfcInitDialogResource_IDD_PERFORMANCE(parent);
      break;
   case IDD_SPEED:
      qtMfcInitDialogResource_IDD_SPEED(parent);
      break;
   case IDD_PCMIMPORT:
      qtMfcInitDialogResource_IDD_PCMIMPORT(parent);
      break;
   case IDD_INSTRUMENT_INTERNAL:
      qtMfcInitDialogResource_IDD_INSTRUMENT_INTERNAL(parent);
      break;
   case IDD_INSTRUMENT_DPCM:
      qtMfcInitDialogResource_IDD_INSTRUMENT_DPCM(parent);
      break;
   case IDD_INSTRUMENT:
      qtMfcInitDialogResource_IDD_INSTRUMENT(parent);
      break;
   case IDD_CONFIG_APPEARANCE:
      qtMfcInitDialogResource_IDD_CONFIG_APPEARANCE(parent);
      break;
   case IDD_CONFIG_GENERAL:
      qtMfcInitDialogResource_IDD_CONFIG_GENERAL(parent);
      break;
   case IDD_PROPERTIES:
      qtMfcInitDialogResource_IDD_PROPERTIES(parent);
      break;
   case IDD_CONFIG_MIDI:
      qtMfcInitDialogResource_IDD_CONFIG_MIDI(parent);
      break;
   case IDD_CONFIG_SOUND:
      qtMfcInitDialogResource_IDD_CONFIG_SOUND(parent);
      break;
   case IDD_CONFIG_SHORTCUTS:
      qtMfcInitDialogResource_IDD_CONFIG_SHORTCUTS(parent);
      break;
   case IDD_EXPORT:
      qtMfcInitDialogResource_IDD_EXPORT(parent);
      break;
   case IDD_INSTRUMENT_VRC7:
      qtMfcInitDialogResource_IDD_INSTRUMENT_VRC7(parent);
      break;
   case IDD_CREATEWAV:
      qtMfcInitDialogResource_IDD_CREATEWAV(parent);
      break;
   case IDD_WAVE_PROGRESS:
      qtMfcInitDialogResource_IDD_WAVE_PROGRESS(parent);
      break;
   case IDD_INSTRUMENT_FDS:
      qtMfcInitDialogResource_IDD_INSTRUMENT_FDS(parent);
      break;
   case IDD_SAMPLE_EDITOR:
      qtMfcInitDialogResource_IDD_SAMPLE_EDITOR(parent);
      break;
   case IDD_INSTRUMENT_FDS_ENVELOPE:
      qtMfcInitDialogResource_IDD_INSTRUMENT_FDS_ENVELOPE(parent);
      break;
   case IDD_CHANNELS:
      qtMfcInitDialogResource_IDD_CHANNELS(parent);
      break;
   case IDD_COMMENTS:
      qtMfcInitDialogResource_IDD_COMMENTS(parent);
      break;
   case IDD_CONFIG_MIXER:
      qtMfcInitDialogResource_IDD_CONFIG_MIXER(parent);
      break;
   case IDD_INSTRUMENT_N163_WAVE:
      qtMfcInitDialogResource_IDD_INSTRUMENT_N163_WAVE(parent);
      break;
   case IDD_IMPORT:
      qtMfcInitDialogResource_IDD_IMPORT(parent);
      break;
   case IDD_FRAMECONTROLS:
      qtMfcInitDialogResource_IDD_FRAMECONTROLS(parent);
      break;
   case IDD_MAINFRAME:
      qtMfcInitDialogResource_IDD_MAINFRAME(parent);
      break;
   case IDD_OCTAVE:
      qtMfcInitDialogResource_IDD_OCTAVE(parent);
      break;
   case IDD_MAINBAR:
      qtMfcInitDialogResource_IDD_MAINBAR(parent);
      break;
   case 0:
      // CP: Allow blank dialogs.
      break;
   default:
      qFatal("dialog resource not implemented...");
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// Toolbar
//

void qtMfcInitToolBarResource_IDR_MAINFRAME(UINT dlgID,CToolBar* parent)
{
   QImage toolBarImage(":/resources/Toolbar-d5.bmp");
   QToolBar* toolBar = dynamic_cast<QToolBar*>(parent->toQWidget());
   QPixmap toolBarActionPixmap;
   QAction* toolBarAction;
   QRgb pixel00;
   int x,y;
   
   toolBarImage = toolBarImage.convertToFormat(QImage::Format_ARGB32);
   pixel00 = toolBarImage.pixel(0,0);
   for ( y = 0; y < toolBarImage.height(); y++ )
   {
      for ( x = 0; x < toolBarImage.width(); x++ )
      {
         if ( toolBarImage.pixel(x,y) == pixel00 )
         {
            toolBarImage.setPixel(x,y,qRgba(0,0,0,0));
         }
      }
   }
   
//IDR_MAINFRAME TOOLBAR 16, 15
   toolBar->setIconSize(QSize(16,15));
//BEGIN
//    BUTTON      ID_FILE_NEW
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(0,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_FILE_NEW);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_FILE_OPEN
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(16,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_FILE_OPEN);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_FILE_SAVE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(32,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_FILE_SAVE);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_EDIT_CUT
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(48,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_EDIT_CUT);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_EDIT_COPY
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(64,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_EDIT_COPY);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_EDIT_PASTE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(80,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_EDIT_PASTE);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_APP_ABOUT
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(96,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_APP_ABOUT);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_CONTEXT_HELP
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(112,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_CONTEXT_HELP);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_MODULE_INSERTFRAME
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(128,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_MODULE_INSERTFRAME);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_MODULE_REMOVEFRAME
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(144,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_MODULE_REMOVEFRAME);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_MODULE_MOVEFRAMEDOWN
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(160,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_MODULE_MOVEFRAMEDOWN);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_MODULE_MOVEFRAMEUP
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(176,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_MODULE_MOVEFRAMEUP);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_MODULE_DUPLICATEFRAME
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(192,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_MODULE_DUPLICATEFRAME);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_MODULE_MODULEPROPERTIES
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(208,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_MODULE_MODULEPROPERTIES);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_TRACKER_PLAY
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(224,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_TRACKER_PLAY);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_TRACKER_PLAYPATTERN
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(240,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_TRACKER_PLAYPATTERN);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_TRACKER_STOP
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(256,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_TRACKER_STOP);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_TRACKER_EDIT
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(272,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_TRACKER_EDIT);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_PREV_SONG
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(288,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_PREV_SONG);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_NEXT_SONG
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(304,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_NEXT_SONG);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_FILE_GENERALSETTINGS
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(320,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_FILE_GENERALSETTINGS);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_FILE_CREATE_NSF
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(336,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_FILE_CREATE_NSF);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//END
   
   for ( x = 0; x < toolBar->actions().count(); x++ )
   {
      CString toolTipCString = qtMfcStringResource(toolBar->actions().at(x)->data().toInt());
      QString toolTip = toolTipCString;
      if ( toolTip.indexOf('\n') >= 0 )
      {
         toolBar->actions().value(x)->setToolTip(toolTip.right(toolTip.length()-toolTip.indexOf('\n')-1));
      }
   }
}

void qtMfcInitToolBarResource_IDR_INSTRUMENT_TOOLBAR(UINT dlgID,CToolBar* parent)
{
   QImage toolBarImage(":/resources/inst_toolbar.bmp");
   QToolBar* toolBar = dynamic_cast<QToolBar*>(parent->toQWidget());
   QPixmap toolBarActionPixmap;
   QAction* toolBarAction;
   QRgb pixel00;
   int x,y;
   
   toolBarImage = toolBarImage.convertToFormat(QImage::Format_ARGB32);
   pixel00 = toolBarImage.pixel(0,0);
   for ( y = 0; y < toolBarImage.height(); y++ )
   {
      for ( x = 0; x < toolBarImage.width(); x++ )
      {
         if ( toolBarImage.pixel(x,y) == pixel00 )
         {
            toolBarImage.setPixel(x,y,qRgba(0,0,0,0));
         }
      }
   }
   
//IDR_INSTRUMENT_TOOLBAR TOOLBAR 16, 15
   toolBar->setIconSize(QSize(16,15));
//BEGIN
//    BUTTON      ID_INSTRUMENT_NEW
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(0,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_INSTRUMENT_NEW);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_INSTRUMENT_REMOVE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(16,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_INSTRUMENT_REMOVE);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_INSTRUMENT_CLONE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(32,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_INSTRUMENT_CLONE);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_INSTRUMENT_LOAD
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(48,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_INSTRUMENT_LOAD);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    BUTTON      ID_INSTRUMENT_SAVE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(64,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_INSTRUMENT_SAVE);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_INSTRUMENT_EDIT
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(80,0,16,15));
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   toolBarAction->setData(ID_INSTRUMENT_EDIT);
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   toolBar->addAction(toolBarAction);
//END
}

void qtMfcInitToolBarResource(UINT dlgID,CToolBar* parent)
{
   switch ( dlgID )
   {
   case IDR_MAINFRAME:
      qtMfcInitToolBarResource_IDR_MAINFRAME(dlgID,parent);
      break;
   case IDR_INSTRUMENT_TOOLBAR:
      qtMfcInitToolBarResource_IDR_INSTRUMENT_TOOLBAR(dlgID,parent);
      break;
   }
}

void qtMfcInit(QMainWindow* parent)
{
   // Hook Qt to this MFC app...
   AfxGetApp()->qtMainWindow = parent;
   AfxGetApp()->moveToThread(QApplication::instance()->thread());

   qtMfcInitStringResources();
   qtMfcInitBitmapResources();
   qtInitIconResources();
}
