#include "cqtmfc.h"
#include "cqtmfc_famitracker.h"

#include <QToolBar>

#include "stdafx.h"

void qtMfcInitMenuResources()
{
   /////////////////////////////////////////////////////////////////////////////
   //
   // Menu
   //
   
//   IDR_PATTERN_POPUP MENU 
//   BEGIN
//       POPUP "Popup"
//       BEGIN
//           MENUITEM "&Undo",                       ID_EDIT_UNDO
//           MENUITEM "&Redo",                       ID_EDIT_REDO
//           MENUITEM SEPARATOR
//           MENUITEM "Cu&t",                        ID_EDIT_CUT
//           MENUITEM "&Copy",                       ID_EDIT_COPY
//           MENUITEM "&Paste",                      ID_EDIT_PASTE
//           MENUITEM "&Delete",                     ID_POPUP_DELETE
//           MENUITEM "Select &All",                 ID_EDIT_SELECTALL
//           MENUITEM SEPARATOR
//           POPUP "Transpose"
//           BEGIN
//               MENUITEM "Increase Note",               ID_TRANSPOSE_INCREASENOTE
//               MENUITEM "Decrease Note",               ID_TRANSPOSE_DECREASENOTE
//               MENUITEM "Increase Octave",             ID_TRANSPOSE_INCREASEOCTAVE
//               MENUITEM "Decrease Octave",             ID_TRANSPOSE_DECREASEOCTAVE
//           END
//           MENUITEM SEPARATOR
//           MENUITEM "&Interpolate",                ID_EDIT_INTERPOLATE
//           MENUITEM "&Reverse",                    ID_EDIT_REVERSE
//           MENUITEM "Pick up row",                 ID_POPUP_PICKUPROW
//           MENUITEM SEPARATOR
//           MENUITEM "Expand pattern",              ID_EDIT_EXPANDPATTERNS
//           MENUITEM "Shrink pattern",              ID_EDIT_SHRINKPATTERNS
//           MENUITEM SEPARATOR
//           MENUITEM "R&eplace instrument",         ID_EDIT_REPLACEINSTRUMENT
//           MENUITEM SEPARATOR
//           MENUITEM "&Toggle channel",             ID_POPUP_TOGGLECHANNEL
//           MENUITEM "&Solo channel",               ID_POPUP_SOLOCHANNEL
//       END
//   END
   
//   IDR_FRAME_POPUP MENU 
//   BEGIN
//       POPUP "Frame"
//       BEGIN
//           MENUITEM "&Insert frame\tIns",          ID_MODULE_INSERTFRAME
//           MENUITEM "&Remove frame\tDel",          ID_MODULE_REMOVEFRAME
//           MENUITEM "&Duplicate frame",            ID_MODULE_DUPLICATEFRAME
//           MENUITEM "Duplicate patterns",          ID_MODULE_DUPLICATEFRAMEPATTERNS
//           MENUITEM SEPARATOR
//           MENUITEM "Move &up",                    ID_MODULE_MOVEFRAMEUP
//           MENUITEM "Move d&own",                  ID_MODULE_MOVEFRAMEDOWN
//           MENUITEM SEPARATOR
//           MENUITEM "Cu&t\tCtrl+X",                ID_FRAME_CUT
//           MENUITEM "&Copy\tCtrl+C",               ID_FRAME_COPY
//           MENUITEM "&Paste\tCtrl+V",              ID_FRAME_PASTE
//       END
//   END
   
//   IDR_INSTRUMENT_POPUP MENU 
//   BEGIN
//       POPUP "Instrument"
//       BEGIN
//           MENUITEM "Add",                         ID_INSTRUMENT_NEW
//           MENUITEM "Remove",                      ID_INSTRUMENT_REMOVE
//           MENUITEM SEPARATOR
//           MENUITEM "Clone instrument",            ID_INSTRUMENT_CLONE
//           MENUITEM "Deep clone instrument",       ID_INSTRUMENT_DEEPCLONE
//           MENUITEM SEPARATOR
//           MENUITEM "Edit",                        ID_INSTRUMENT_EDIT
//       END
//   END
   
//   IDR_SAMPLES_POPUP MENU 
//   BEGIN
//       POPUP "Popup"
//       BEGIN
//           MENUITEM "Preview",                     IDC_PREVIEW
//           MENUITEM SEPARATOR
//           MENUITEM "Edit",                        IDC_EDIT
//           MENUITEM SEPARATOR
//           MENUITEM "Unload",                      IDC_UNLOAD
//           MENUITEM "Save",                        IDC_SAVE
//           MENUITEM "Load",                        IDC_LOAD
//           MENUITEM "Import",                      IDC_IMPORT
//       END
//   END
   
//   IDR_SAMPLE_WND_POPUP MENU 
//   BEGIN
//       POPUP "Popup"
//       BEGIN
//           MENUITEM "Sample graph 1",              ID_POPUP_SAMPLEGRAPH1
//           MENUITEM "Sample graph 2",              ID_POPUP_SAMPLEGRAPH2
//           MENUITEM "Spectrum analyzer",           ID_POPUP_SPECTRUMANALYZER
//           MENUITEM "Nothing",                     ID_POPUP_NOTHING
//       END
//   END
   
//   IDR_PATTERN_HEADER_POPUP MENU 
//   BEGIN
//       POPUP "Popup"
//       BEGIN
//           MENUITEM "&Toggle channel\tAlt+F9",     ID_POPUP_TOGGLECHANNEL
//           MENUITEM "&Solo channel\tAlt+F10",      ID_POPUP_SOLOCHANNEL
//           MENUITEM "&Unmute all channels",        ID_POPUP_UNMUTEALLCHANNELS
//       END
//   END
}

void qtMfcInitStringResources()
{
   /////////////////////////////////////////////////////////////////////////////
   //
   // String Table
   //
   
// AFX resources
   qtMfcStringResources.insert(AFX_IDS_ALLFILTER,"All files (*.*)");
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDR_MAINFRAME           , "FamiTracker\n\nFamiTracker\nFamiTracker files (*.ftm)\n.ftm\nFamiTracker.Module\nFamiTracker.Module");
   qtMfcStringResources.insert(IDS_CLIPBOARD_ERROR     , "Could not register clipboard format");
   qtMfcStringResources.insert(IDS_FILE_OPEN_ERROR     , "Could not open the file!");
   qtMfcStringResources.insert(IDS_FILE_VALID_ERROR    , "File is not a valid FamiTracker module");
   qtMfcStringResources.insert(IDS_FILE_VERSION_ERROR  , "File version is not supported");
   qtMfcStringResources.insert(IDS_INST_LIMIT          , "You cannot add more instruments");
   qtMfcStringResources.insert(IDS_CLIPBOARD_OPEN_ERROR, "Cannot open clipboard.");
   qtMfcStringResources.insert(IDS_CLIPBOARD_NOT_AVALIABLE, "Clipboard data is not available.");
   qtMfcStringResources.insert(ID_TRACKER_PLAY_START   , "Play module from start");
   qtMfcStringResources.insert(IDS_INSTRUMENT_FILE_FAIL, "Instrument file was not vaild");
   qtMfcStringResources.insert(IDS_SAMPLES_NEEDED      , "Following samples are needed for parent instrument:\n\n");
   qtMfcStringResources.insert(IDS_OPEN                , "Open");
   qtMfcStringResources.insert(IDS_SAVE_ERROR_REASON   , "Could not save file: ");
   qtMfcStringResources.insert(IDS_START_ERROR         , "Program could not start properly, default settings has been restored. Please restart the program.");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(AFX_IDS_APP_TITLE       , "FamiTracker");
   qtMfcStringResources.insert(AFX_IDS_IDLEMESSAGE     , "For Help, press F1");
   qtMfcStringResources.insert(AFX_IDS_HELPMODEMESSAGE, "Select an object on which to get Help");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_INDICATOR_EXT        , "EXT");
   qtMfcStringResources.insert(ID_INDICATOR_CAPS       , "CAP");
   qtMfcStringResources.insert(ID_INDICATOR_NUM        , "NUM");
   qtMfcStringResources.insert(ID_INDICATOR_SCRL       , "SCRL");
   qtMfcStringResources.insert(ID_INDICATOR_OVR        , "OVR");
   qtMfcStringResources.insert(ID_INDICATOR_REC        , "REC");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_FILE_NEW             , "Create a new document\nNew");
   qtMfcStringResources.insert(ID_FILE_OPEN            , "Open an existing document\nOpen");
   qtMfcStringResources.insert(ID_FILE_CLOSE           , "Close the active document\nClose");
   qtMfcStringResources.insert(ID_FILE_SAVE            , "Save the active document\nSave");
   qtMfcStringResources.insert(ID_FILE_SAVE_AS         , "Save the active document with a new name\nSave As");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_APP_ABOUT            , "Display program information, version number and copyright\nAbout");
   qtMfcStringResources.insert(ID_APP_EXIT             , "Quit the application; prompts to save documents\nExit");
   qtMfcStringResources.insert(ID_HELP_INDEX           , "Opens Help\nHelp Topics");
   qtMfcStringResources.insert(ID_HELP_FINDER          , "List Help topics\nHelp Topics");
   qtMfcStringResources.insert(ID_HELP_USING           , "Display instructions about how to use help\nHelp");
   qtMfcStringResources.insert(ID_CONTEXT_HELP         , "Display help for clicked on buttons, menus and windows\nHelp");
   qtMfcStringResources.insert(ID_HELP                 , "Display help for current task or command\nHelp");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_FILE_MRU_FILE1       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE2       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE3       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE4       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE5       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE6       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE7       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE8       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE9       , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE10      , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE11      , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE12      , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE13      , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE14      , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE15      , "Open parent document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE16      , "Open parent document");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_NEXT_PANE            , "Switch to the next window pane\nNext Pane");
   qtMfcStringResources.insert(ID_PREV_PANE            , "Switch back to the previous window pane\nPrevious Pane");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_WINDOW_SPLIT         , "Split the active window into panes\nSplit");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_EDIT_CLEAR           , "Erase the selection\nErase");
   qtMfcStringResources.insert(ID_EDIT_CLEAR_ALL       , "Erase everything\nErase All");
   qtMfcStringResources.insert(ID_EDIT_COPY            , "Copy the selection and put it on the Clipboard\nCopy");
   qtMfcStringResources.insert(ID_EDIT_CUT             , "Cut the selection and put it on the Clipboard\nCut");
   qtMfcStringResources.insert(ID_EDIT_FIND            , "Find the specified text\nFind");
   qtMfcStringResources.insert(ID_EDIT_PASTE           , "Insert Clipboard contents\nPaste");
   qtMfcStringResources.insert(ID_EDIT_REPEAT          , "Repeat the last action\nRepeat");
   qtMfcStringResources.insert(ID_EDIT_REPLACE         , "Replace specific text with different text\nReplace");
   qtMfcStringResources.insert(ID_EDIT_SELECT_ALL      , "Select the entire document\nSelect All");
   qtMfcStringResources.insert(ID_EDIT_UNDO            , "Undo the last action\nUndo");
   qtMfcStringResources.insert(ID_EDIT_REDO            , "Redo the previously undone action\nRedo");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_VIEW_TOOLBAR         , "Show or hide the toolbar\nToggle ToolBar");
   qtMfcStringResources.insert(ID_VIEW_STATUS_BAR      , "Show or hide the status bar\nToggle StatusBar");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(AFX_IDS_SCSIZE          , "Change the window size");
   qtMfcStringResources.insert(AFX_IDS_SCMOVE          , "Change the window position");
   qtMfcStringResources.insert(AFX_IDS_SCMINIMIZE      , "Reduce the window to an icon");
   qtMfcStringResources.insert(AFX_IDS_SCMAXIMIZE      , "Enlarge the window to full size");
   qtMfcStringResources.insert(AFX_IDS_SCNEXTWINDOW    , "Switch to the next document window");
   qtMfcStringResources.insert(AFX_IDS_SCPREVWINDOW    , "Switch to the previous document window");
   qtMfcStringResources.insert(AFX_IDS_SCCLOSE         , "Close the active window and prompts to save the documents");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(AFX_IDS_SCRESTORE       , "Restore the window to normal size");
   qtMfcStringResources.insert(AFX_IDS_SCTASKLIST      , "Activate Task List");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_TRACKER_PLAY         , "Play module\nPlay");
   qtMfcStringResources.insert(ID_TRACKER_PLAYPATTERN  , "Play and loop current pattern\nPlay and loop");
   qtMfcStringResources.insert(ID_TRACKER_STOP         , "Stop playing\nStop");
   qtMfcStringResources.insert(ID_TRACKER_EDIT         , "Enable/disable edit mode\nToggle edit mode");
   qtMfcStringResources.insert(ID_TRACKER_KILLSOUND    , "Promptly stop all channels");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_EDIT_DELETE          , "Delete selection, or note if there is no selection.");
   qtMfcStringResources.insert(ID_EDIT_UNLOCKCURSOR    , "Makes it possible to edit without scrolling the view");
   qtMfcStringResources.insert(ID_TRACKER_NTSC         , "Switch to NTSC emulation");
   qtMfcStringResources.insert(ID_TRACKER_PAL          , "Switch to PAL emulation");
   qtMfcStringResources.insert(ID_SPEED_DEFALUT        , "Default playback rate (60 for NTSC, 50 for PAL)");
   qtMfcStringResources.insert(ID_SPEED_CUSTOM         , "Set the playback rate manually");
   qtMfcStringResources.insert(ID_HELP_PERFORMANCE     , "Application performance");
   qtMfcStringResources.insert(ID_EDIT_PASTEOVERWRITE  , "Pasted notes will overwrite existing notes");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_FILE_CREATE_NSF      , "Brings up the create NSF dialog\nCreate NSF");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_TRANSPOSE_INCREASENOTE, "Increase selected note/notes by one step");
   qtMfcStringResources.insert(ID_TRANSPOSE_DECREASENOTE, "Decrease selected note/notes by one step");
   qtMfcStringResources.insert(ID_TRANSPOSE_INCREASEOCTAVE, "Increase selected octave/octaves by one step");
   qtMfcStringResources.insert(ID_TRANSPOSE_DECREASEOCTAVE, "Decrease selected octave/octaves by one step");
   qtMfcStringResources.insert(ID_EDIT_SELECTALL       , "Select pattern or all patterns\nSelect All");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_FILE_GENERALSETTINGS, "Opens FamiTracker settings\nSettings");
   qtMfcStringResources.insert(ID_EDIT_ENABLEMIDI      , "Temporarly enable / disable commands from the MIDI device");
   qtMfcStringResources.insert(ID_FRAME_INSERT         , "Insert a new frame on current location\nAdd frame");
   qtMfcStringResources.insert(ID_FRAME_REMOVE         , "Remove current frame\nRemove frame");
   qtMfcStringResources.insert(ID_TRACKER_PLAYROW      , "Play a single row");
   qtMfcStringResources.insert(ID_FILE_IMPORTMIDI      , "Imports a MIDI file\nImport MIDI");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_CREATE_NSF           , "Export module to NSF");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_SPEED_DEFAULT        , "Chooses default playback rate corresponding to selected video standard");
   qtMfcStringResources.insert(ID_MODULE_MODULEPROPERTIES, "Opens the module properties window\nModule properties");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDS_FILE_LOAD_ERROR     , "Couldn't load file properly. Make sure you have the latest version of FamiTracker.");
   qtMfcStringResources.insert(IDS_CONFIG_WINDOW       , "FamiTracker configuration");
   qtMfcStringResources.insert(IDS_WELCOME             , "Welcome to FamiTracker, press F1 for help");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_EDIT_PASTEMIX        , "Pasted notes will mix with existing notes");
   qtMfcStringResources.insert(ID_MODULE_MOVEFRAMEDOWN, "Move selected frame one step down\nMove frame down");
   qtMfcStringResources.insert(ID_MODULE_MOVEFRAMEUP   , "Move selected frame one step up\nMove frame up");
   qtMfcStringResources.insert(ID_MODULE_SAVEINSTRUMENT, "Save instrument to file\nSave instrument");
   qtMfcStringResources.insert(ID_MODULE_LOADINSTRUMENT, "Load instrument from file\nLoad instrument");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_MODULE_ADDINSTRUMENT, "Add a new instrument to the module\nAdd instrument");
   qtMfcStringResources.insert(ID_MODULE_REMOVEINSTRUMENT , "Remove instrument from module\nRemove instrument");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_NEXT_SONG            , "Switches to next song\nNext song");
   qtMfcStringResources.insert(ID_PREV_SONG            , "Switches to previous song\nPrevious song");
   qtMfcStringResources.insert(ID_EDIT_INSTRUMENTMASK  , "Disables insertion of instrument numbers in the instrument column");
   qtMfcStringResources.insert(ID_TRACKER_SWITCHTOTRACKINSTRUMENT , "Switch to channel instruments when playing");
   qtMfcStringResources.insert(ID_FRAME_INSERT_UNIQUE  , "Insert new frame with empty patterns");
   qtMfcStringResources.insert(ID_FILE_CREATEWAV       , "Saves the file as a WAV-file\nCreate WAV");
   qtMfcStringResources.insert(ID_VIEW_CONTROLPANEL    , "Show or hide the control panel");
   qtMfcStringResources.insert(ID_EDIT_CLEARPATTERNS   , "Resets all patterns and frames");
   qtMfcStringResources.insert(ID_EDIT_INTERPOLATE     , "Interpolates a selection between two values\nInterpolate");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_TRACKER_SOLOCHANNEL  , "Mutes/unmutes all but the selected channel");
   qtMfcStringResources.insert(ID_TRACKER_TOGGLECHANNEL, "Mute/unmute selected channel");
   qtMfcStringResources.insert(ID_EDIT_GRADIENT        , "Interpolates a selection of values");
   qtMfcStringResources.insert(ID_MODULE_EDITINSTRUMENT, "Open instrument editor\nEdit instrument");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDS_SOUNDGEN_ERROR      , "Couldn't start sound generator thread, default settings has been restored. Please restart the program.");
   qtMfcStringResources.insert(IDS_SOUNDGEN_CLOSE_ERR  , "Could not close sound generator thread!");
   qtMfcStringResources.insert(IDS_OUT_OF_SLOTS        , "Out of sample slots");
   qtMfcStringResources.insert(IDS_OUT_OF_SAMPLEMEM    , "Couldn't load sample, out of sample memory (max %i kB is avaliable)\n");
   qtMfcStringResources.insert(IDS_INVALID_RIFF        , "File is not a valid RIFF.");
   qtMfcStringResources.insert(IDS_INCLUDE_INSTRUMENTS, "Do you want to include instruments?");
   qtMfcStringResources.insert(IDS_IMPORT_FAILED       , "Import failed");
   qtMfcStringResources.insert(IDS_IMPORT_CHIP_MISMATCH, "Imported file must be of the same expansion chip type as current file.");
   qtMfcStringResources.insert(IDS_IMPORT_INSTRUMENT_COUNT, "Can't import file, out of instrument slots!");
   qtMfcStringResources.insert(IDS_IMPORT_SAMPLE_SLOTS, "Could not import all samples, out of sample slots!");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_MODULE_DUPLICATEFRAME, "Inserts a copy of current frame\nDuplicate frame");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_EDIT_REVERSE         , "Reverses a selection\nReverse");
   qtMfcStringResources.insert(ID_TRACKER_DPCM         , "Display information about the DPCM channel");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_INSTRUMENT_REMOVE    , "Remove instrument from module\nRemove instrument");
   qtMfcStringResources.insert(ID_INSTRUMENT_EDIT      , "Open instrument edtor\nEdit instrument");
   qtMfcStringResources.insert(ID_INSTRUMENT_CLONE     , "Create a copy of selected instrument\nClone instrument");
   qtMfcStringResources.insert(ID_EDIT_REPLACEINSTRUMENT, "Replaces all instruments in a selection with the selected instrument\nReplace instrument");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_CLEANUP_REMOVEUNUSEDPATTERNS, "Removes all unused patterns\nRemove unused patterns");
   qtMfcStringResources.insert(ID_HELP_EFFECTTABLE     , "Open help window with effect table");
   qtMfcStringResources.insert(ID_CLEANUP_REMOVEUNUSEDINSTRUMENTS, "Removes all unused instruments\nRemove unused instruments");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_MODULE_FRAME_INSERT  , "Insert a new frame on current location");
   qtMfcStringResources.insert(ID_MODULE_FRAME_REMOVE  , "Removes selected frame");
   qtMfcStringResources.insert(ID_MODULE_REMOVEFRAME   , "Removes selected frame\nRemove frame");
   qtMfcStringResources.insert(ID_MODULE_COMMENTS      , "Open module comments dialog\nComments");
   qtMfcStringResources.insert(ID_MODULE_INSERTFRAME   , "Inserts a new frame");
   qtMfcStringResources.insert(ID_INSTRUMENT_NEW       , "Add a new instrument to the module\nAdd instrument");
   qtMfcStringResources.insert(ID_INSTRUMENT_LOAD      , "Load instrument from file\nLoad instrument");
   qtMfcStringResources.insert(ID_INSTRUMENT_SAVE      , "Save instrument to file\nSave instrument");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDS_SAVE_ERROR          , "Couldn't save file, check that it isn't write protected or shared with other applications and there are free disk space available.");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDC_OPT_WRAPCURSOR      , "Wrap around the cursor when reaching top or bottom in the pattern editor.");
   qtMfcStringResources.insert(IDC_OPT_FREECURSOR      , "Unlocks the cursor from the center of the pattern editor");
   qtMfcStringResources.insert(IDC_OPT_WAVEPREVIEW     , "Preview wave and DPCM files in the open file dialog when loading samples to the module.");
   qtMfcStringResources.insert(IDC_OPT_KEYREPEAT       , "Enable key repetition in the pattern editor.");
   qtMfcStringResources.insert(IDC_OPT_HEXROW          , "Display row numbers in hexadecimal.");
   qtMfcStringResources.insert(IDC_OPT_WRAPFRAMES      , "Move to next or previous frame when reaching top or bottom in the pattern editor.");
   qtMfcStringResources.insert(IDC_OPT_FRAMEPREVIEW    , "Preview next and previous frame in the pattern editor.");
   qtMfcStringResources.insert(IDC_OPT_NODPCMRESET     , "Prevents resetting the DPCM channel on note stop commands.");
   qtMfcStringResources.insert(IDC_OPT_NOSTEPMOVE      , "Ignore the pattern step setting when moving the cursor, only use it when inserting notes.");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDC_OPT_PULLUPDELETE    , "Makes delete key pull up rows rather than only deleting the value.");
   qtMfcStringResources.insert(IDC_OPT_BACKUPS         , "Creates a backup copy of the existing file when saving a module.");
   qtMfcStringResources.insert(IDC_OPT_SINGLEINSTANCE  , "Only allow one single instance of the FamiTracker application.");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_MODULE_DUPLICATEFRAMEPATTERNS, "Inserts a copy of current frame including all patterns\nDuplicate patterns");
   qtMfcStringResources.insert(ID_EDIT_EXPANDPATTERNS  , "Expands selected pattern\nExpand pattern");
   qtMfcStringResources.insert(ID_EDIT_SHRINKPATTERNS  , "Shrinks selected pattern\nShrink pattern");
   qtMfcStringResources.insert(ID_FRAMEEDITOR_TOP      , "Moves the frame editor to the top of the window (default)\nTop");
   qtMfcStringResources.insert(ID_FRAMEEDITOR_LEFT     , "Moves the frame editor to the left of the window\nLeft");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_INSTRUMENT_DEEPCLONE, "Creates a copy of selected instrument including all sequences\nDeep clone instrument");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_TRACKER_PLAY_CURSOR  , "Play pattern from cursor");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDS_CLEARPATTERN        , "Do you want to reset all patterns and frames? There is no undo for parent command.");
   qtMfcStringResources.insert(IDS_CLIPBOARD_COPY_ERROR, "An error occured while trying to copy data.");
   qtMfcStringResources.insert(IDS_CLIPBOARD_PASTE_ERROR, "An error occured while trying to paste data.");
   qtMfcStringResources.insert(IDS_DPCM_SPACE_FORMAT   , "Space used %i kB, left %i kB (%i kB available)");
   qtMfcStringResources.insert(IDS_FILE_VERSION_TOO_NEW, "This file was created in a newer version of FamiTracker. Please download the most recent version to open parent file.");
   qtMfcStringResources.insert(IDS_INST_EDITOR_TITLE   , "Instrument editor - %02X. %s (%s)");
   qtMfcStringResources.insert(IDS_INST_VERSION_UNSUPPORTED, "File version not supported!");
   qtMfcStringResources.insert(IDS_INVALID_INST_FILE   , "Could not open instrument file");
   qtMfcStringResources.insert(IDS_MIDI_ERR_INPUT      , "MIDI Error: Could not open MIDI input device!");
   qtMfcStringResources.insert(IDS_MIDI_ERR_OUTPUT     , "MIDI Error: Could not open MIDI output device!");
   qtMfcStringResources.insert(IDS_NEW_SOUND_CONFIG    , "New sound configuration loaded");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDS_REMOVE_INSTRUMENTS  , "Do you want to remove all unused instruments? There is no undo for parent action.");
   qtMfcStringResources.insert(IDS_REMOVE_PATTERNS     , "Do you want to remove all unused patterns? There is no undo for parent action.");
   qtMfcStringResources.insert(IDS_SONG_DELETE         , "Do you want to delete parent song? There is no undo for parent action.");
   qtMfcStringResources.insert(IDS_SOUND_FAIL          , "It appears the current sound settings aren't working, change settings and try again!");
   qtMfcStringResources.insert(IDS_UNDERRUN_MESSAGE    , "Audio buffer underrun, increase the audio buffer size.");
   qtMfcStringResources.insert(IDS_WELCOME_VER         , "Welcome to FamiTracker %i.%i.%i, press F1 for help");
   qtMfcStringResources.insert(IDS_INVALID_WAVEFILE    , "Cannot load parent sample!\nOnly uncompressed PCM is supported.");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(ID_INDICATOR_CHIP       , "No expansion chip");
   qtMfcStringResources.insert(ID_INDICATOR_INSTRUMENT, "Instrument: 00");
   qtMfcStringResources.insert(ID_INDICATOR_OCTAVE     , "Octave: 0");
   qtMfcStringResources.insert(ID_INDICATOR_RATE       , "000 Hz");
   qtMfcStringResources.insert(ID_INDICATOR_TEMPO      , "000 BPM");
   qtMfcStringResources.insert(ID_INDICATOR_TIME       , "00:00:00 ");
//   END
}

void qtMfcInitBitmapResources()
{
   /////////////////////////////////////////////////////////////////////////////
   //
   // Bitmap
   //
   
//   IDB_SAMPLEBG            BITMAP                  "res\\SampleBg.bmp"
   qtMfcBitmapResources.insert(IDB_SAMPLEBG,CBitmap(":/resources/SampleBg.bmp"));
//   IDB_KEY_BLACK           BITMAP                  "res\\key_black_unpressed.bmp"
   qtMfcBitmapResources.insert(IDB_KEY_BLACK,CBitmap(":/resources/key_black_unpressed.bmp"));
//   IDB_KEY_BLACK_MARK      BITMAP                  "res\\key_black_pressed.bmp"
   qtMfcBitmapResources.insert(IDB_KEY_BLACK_MARK,CBitmap(":/resources/key_black_pressed.bmp"));
//   IDB_KEY_WHITE           BITMAP                  "res\\key_white_unpressed.bmp"
   qtMfcBitmapResources.insert(IDB_KEY_WHITE,CBitmap(":/resources/key_white_unpressed.bmp"));
//   IDB_KEY_WHITE_MARK      BITMAP                  "res\\key_white_pressed.bmp"
   qtMfcBitmapResources.insert(IDB_KEY_WHITE_MARK,CBitmap(":/resources/key_white_pressed.bmp"));
//   IDB_INSTRUMENT_TOOLS    BITMAP                  "res\\toolbar1.bmp"
   qtMfcBitmapResources.insert(IDB_INSTRUMENT_TOOLS,CBitmap(":/resources/toolbar1.bmp"));
//   IDB_TOOLBAR_256         BITMAP                  "res\\Toolbar-d5.bmp"
   qtMfcBitmapResources.insert(IDB_TOOLBAR_256,CBitmap(":/resources/Toolbar-d5.bmp"));
//   IDB_TOOLBAR_INST_256    BITMAP                  "res\\inst_toolbar.bmp"
   qtMfcBitmapResources.insert(IDB_TOOLBAR_INST_256,CBitmap(":/resources/inst_toolbar.bmp"));
}

#include "ControlPanelDlg.h"
void qtMfcInitDialogResource_IDD_MAINFRAME(UINT dlgID,CDialog* parent1)
{
   CControlPanelDlg* parent = dynamic_cast<CControlPanelDlg*>(parent1);
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
   CGroupBox* mfc1 = new CGroupBox(parent);
   CRect r1(CPoint(7,5),CSize(78,71));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Song settings"),WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Edit settings",IDC_STATIC,7,78,78,43,0,WS_EX_TRANSPARENT
   CGroupBox* mfc14 = new CGroupBox(parent);
   CRect r14(CPoint(7,78),CSize(78,43));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Edit settings"),WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Song information",IDC_STATIC,91,5,96,61
   CGroupBox* mfc19 = new CGroupBox(parent);
   CRect r19(CPoint(91,5),CSize(96,61));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Song information"),WS_VISIBLE,r19,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Songs",IDC_STATIC,91,94,96,27
   CGroupBox* mfc23 = new CGroupBox(parent);
   CRect r23(CPoint(91,94),CSize(96,27));
   parent->MapDialogRect(&r23);
   mfc23->Create(_T("Songs"),WS_VISIBLE,r23,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Speed",IDC_STATIC,13,17,29,10
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(13,17),CSize(29,10));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Speed"),WS_VISIBLE,r2,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_SPEED,43,15,38,12,ES_AUTOHSCROLL | ES_READONLY | ES_NUMBER
//       CONTROL         "",IDC_SPEED_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,15,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc3 = new CEdit(parent);
   CSpinButtonCtrl* mfc4 = new CSpinButtonCtrl(parent);
   CRect r3(CPoint(43,15),CSize(38,12));
   CRect r4(CPoint(r3.right-11,r3.top),CSize(11,12));
   parent->MapDialogRect(&r3);
   parent->MapDialogRect(&r4);
   mfc3->Create(ES_AUTOHSCROLL | ES_READONLY | ES_NUMBER | WS_VISIBLE,r3,parent,IDC_SPEED);
   mfc3->setBuddy(mfc4);
   mfcToQtWidget->insert(IDC_SPEED,mfc3);
   QObject::connect(mfc3,SIGNAL(textChanged(QString)),parent,SLOT(speed_textChanged(QString)));
   mfc4->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r4,parent,IDC_SPEED_SPIN);
   mfc4->setBuddy(mfc3);
   mfcToQtWidget->insert(IDC_SPEED_SPIN,mfc4);
   QObject::connect(mfc4,SIGNAL(valueChanged(int)),parent,SLOT(speedSpin_valueChanged(int)));   
//       LTEXT           "Tempo",IDC_STATIC,13,31,29,10
   CStatic* mfc5 = new CStatic(parent);
   CRect r5(CPoint(13,31),CSize(29,10));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Tempo"),WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_TEMPO,43,29,38,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_TEMPO_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,28,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc6 = new CEdit(parent);
   CSpinButtonCtrl* mfc7 = new CSpinButtonCtrl(parent);
   CRect r6(CPoint(43,29),CSize(38,12));
   CRect r7(CPoint(r6.right-11,r6.top),CSize(11,12));
   parent->MapDialogRect(&r6);
   parent->MapDialogRect(&r7);
   mfc6->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r6,parent,IDC_TEMPO);
   mfc6->setBuddy(mfc7);
   mfcToQtWidget->insert(IDC_TEMPO,mfc6);
   QObject::connect(mfc6,SIGNAL(textChanged(QString)),parent,SLOT(tempo_textChanged(QString)));
   mfc7->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r7,parent,IDC_TEMPO_SPIN);
   mfc7->setBuddy(mfc6);
   mfcToQtWidget->insert(IDC_TEMPO_SPIN,mfc7);
   QObject::connect(mfc7,SIGNAL(valueChanged(int)),parent,SLOT(tempoSpin_valueChanged(int)));   
//       LTEXT           "Rows",IDC_STATIC,13,45,29,10
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(13,45),CSize(29,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Rows"),WS_VISIBLE,r8,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_ROWS,43,43,38,12,ES_AUTOHSCROLL | ES_READONLY | ES_WANTRETURN
//       CONTROL         "",IDC_ROWS_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,43,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc9 = new CEdit(parent);
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(parent);
   CRect r9(CPoint(43,43),CSize(38,12));
   CRect r10(CPoint(r9.right-11,r9.top),CSize(11,12));
   parent->MapDialogRect(&r9);
   parent->MapDialogRect(&r10);
   mfc9->Create(ES_AUTOHSCROLL | ES_READONLY | ES_WANTRETURN | WS_VISIBLE,r9,parent,IDC_ROWS);
   mfc9->setBuddy(mfc10);
   mfcToQtWidget->insert(IDC_ROWS,mfc9);
   QObject::connect(mfc9,SIGNAL(textChanged(QString)),parent,SLOT(rows_textChanged(QString)));
   mfc10->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,parent,IDC_ROWS_SPIN);
   mfc10->setBuddy(mfc9);
   mfcToQtWidget->insert(IDC_ROWS_SPIN,mfc10);
   QObject::connect(mfc10,SIGNAL(valueChanged(int)),parent,SLOT(rowsSpin_valueChanged(int)));   
//       LTEXT           "Frames",IDC_STATIC,13,59,29,10
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(13,59),CSize(29,10));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Frames"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_FRAMES,43,57,38,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_FRAME_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,58,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc12 = new CEdit(parent);
   CSpinButtonCtrl* mfc13 = new CSpinButtonCtrl(parent);
   CRect r12(CPoint(43,57),CSize(38,12));
   CRect r13(CPoint(r12.right-11,r12.top),CSize(11,12));
   parent->MapDialogRect(&r12);
   parent->MapDialogRect(&r13);
   mfc12->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r12,parent,IDC_FRAMES);
   mfc12->setBuddy(mfc13);
   mfcToQtWidget->insert(IDC_FRAMES,mfc12);
   QObject::connect(mfc12,SIGNAL(textChanged(QString)),parent,SLOT(frames_textChanged(QString)));
   mfc13->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r13,parent,IDC_FRAME_SPIN);
   mfc13->setBuddy(mfc12);
   mfcToQtWidget->insert(IDC_FRAME_SPIN,mfc13);
   QObject::connect(mfc13,SIGNAL(valueChanged(int)),parent,SLOT(framesSpin_valueChanged(int)));   
//       LTEXT           "Step",IDC_STATIC,13,91,29,10,0,WS_EX_TRANSPARENT
   CStatic* mfc15 = new CStatic(parent);
   CRect r15(CPoint(13,91),CSize(29,10));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("Step"),WS_VISIBLE,r15,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_KEYSTEP,43,90,38,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_KEYSTEP_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,88,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc16 = new CEdit(parent);
   CSpinButtonCtrl* mfc17 = new CSpinButtonCtrl(parent);
   CRect r16(CPoint(43,90),CSize(38,12));
   CRect r17(CPoint(r16.right-11,r16.top),CSize(11,12));
   parent->MapDialogRect(&r16);
   parent->MapDialogRect(&r17);
   mfc16->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r16,parent,IDC_KEYSTEP);
   mfc16->setBuddy(mfc17);
   mfcToQtWidget->insert(IDC_KEYSTEP,mfc16);
   QObject::connect(mfc16,SIGNAL(textChanged(QString)),parent,SLOT(keyStep_textChanged(QString)));
   mfc17->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r17,parent,IDC_KEYSTEP_SPIN);
   mfc17->setBuddy(mfc16);
   mfcToQtWidget->insert(IDC_KEYSTEP_SPIN,mfc17);
   QObject::connect(mfc17,SIGNAL(valueChanged(int)),parent,SLOT(keyStepSpin_valueChanged(int)));   
//       CONTROL         "Key repetition",IDC_KEYREPEAT,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,105,68,9,WS_EX_TRANSPARENT
   CButton* mfc18 = new CButton(parent);
   CRect r18(CPoint(13,105),CSize(68,9));
   parent->MapDialogRect(&r18);
   mfc18->Create(_T("Key repetition"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r18,parent,IDC_KEYREPEAT);
   mfcToQtWidget->insert(IDC_KEYREPEAT,mfc18);
   QObject::connect(mfc18,SIGNAL(clicked()),parent,SLOT(keyRepeat_clicked()));
//       EDITTEXT        IDC_SONG_NAME,97,18,83,12,ES_AUTOHSCROLL
   CEdit* mfc20 = new CEdit(parent);
   CRect r20(CPoint(97,18),CSize(83,12));
   parent->MapDialogRect(&r20);
   mfc20->Create(ES_AUTOHSCROLL | WS_VISIBLE,r20,parent,IDC_SONG_NAME);
   mfcToQtWidget->insert(IDC_SONG_NAME,mfc20);
   QObject::connect(mfc20,SIGNAL(textChanged(QString)),parent,SLOT(songName_textChanged(QString)));
//       EDITTEXT        IDC_SONG_ARTIST,97,33,83,12,ES_AUTOHSCROLL
   CEdit* mfc21 = new CEdit(parent);
   CRect r21(CPoint(97,33),CSize(83,12));
   parent->MapDialogRect(&r21);
   mfc21->Create(ES_AUTOHSCROLL | WS_VISIBLE,r21,parent,IDC_SONG_ARTIST);
   mfcToQtWidget->insert(IDC_SONG_ARTIST,mfc21);
   QObject::connect(mfc21,SIGNAL(textChanged(QString)),parent,SLOT(songArtist_textChanged(QString)));
//       EDITTEXT        IDC_SONG_COPYRIGHT,97,49,83,12,ES_AUTOHSCROLL
   CEdit* mfc22 = new CEdit(parent);
   CRect r22(CPoint(97,49),CSize(83,12));
   parent->MapDialogRect(&r22);
   mfc22->Create(ES_AUTOHSCROLL | WS_VISIBLE,r22,parent,IDC_SONG_COPYRIGHT);
   mfcToQtWidget->insert(IDC_SONG_COPYRIGHT,mfc22);
   QObject::connect(mfc22,SIGNAL(textChanged(QString)),parent,SLOT(songCopyright_textChanged(QString)));
//       COMBOBOX        IDC_SUBTUNE,97,104,84,85,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc24 = new CComboBox(parent);
   CRect r24(CPoint(97,104),CSize(84,85));
   parent->MapDialogRect(&r24);
   mfc24->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r24,parent,IDC_SUBTUNE);
   mfcToQtWidget->insert(IDC_SUBTUNE,mfc24);
   QObject::connect(mfc24,SIGNAL(currentIndexChanged(int)),parent,SLOT(subtune_currentIndexChanged(int)));
//       CONTROL         "",IDC_INSTRUMENTS,"SysListView32",LVS_LIST | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,193,5,202,103
   CListCtrl* mfc25 = new CListCtrl(parent);
   CRect r25(CPoint(193,5),CSize(202,103));
   parent->MapDialogRect(&r25);
   mfc25->Create(LVS_LIST | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r25,parent,IDC_INSTRUMENTS);
   mfcToQtWidget->insert(IDC_INSTRUMENTS,mfc25);
   QObject::connect(mfc25,SIGNAL(cellClicked(int,int)),parent,SLOT(instruments_cellClicked(int,int)));
   QObject::connect(mfc25,SIGNAL(cellDoubleClicked(int,int)),parent,SLOT(instruments_cellDoubleClicked(int,int)));
//       EDITTEXT        IDC_INSTNAME,324,109,71,12,ES_AUTOHSCROLL
   CEdit* mfc26 = new CEdit(parent);
   CRect r26(CPoint(324,109),CSize(71,12));
   parent->MapDialogRect(&r26);
   mfc26->Create(ES_AUTOHSCROLL | WS_VISIBLE,r26,parent,IDC_INSTNAME);
   mfcToQtWidget->insert(IDC_INSTNAME,mfc26);
   QObject::connect(mfc26,SIGNAL(textChanged(QString)),parent,SLOT(instName_textChanged(QString)));
//   END
}

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_PERFORMANCE(UINT dlgID,CDialog* parent)
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

#include "SpeedDlg.h"
void qtMfcInitDialogResource_IDD_SPEED(UINT dlgID,CDialog* parent1)
{
   CSpeedDlg* parent = dynamic_cast<CSpeedDlg*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_SPEED DIALOGEX 0, 0, 196, 44
   CRect rect(CPoint(0,0),CSize(196,44));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Custom speed"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       DEFPUSHBUTTON   "OK",IDOK,139,7,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(139,7),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),parent,SLOT(ok_clicked()));
//       PUSHBUTTON      "Cancel",IDCANCEL,139,23,50,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(139,23),CSize(50,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),parent,SLOT(cancel_clicked()));
//       CONTROL         "",IDC_SPEED_SLD,"msctls_trackbar32",WS_TABSTOP,7,7,101,16
   CSliderCtrl* mfc3 = new CSliderCtrl(parent);
   CRect r3(CPoint(7,7),CSize(101,16));
   parent->MapDialogRect(&r3);
   mfc3->setGeometry(r3);
   mfcToQtWidget->insert(IDC_SPEED_SLD,mfc3);
   QObject::connect(mfc3,SIGNAL(valueChanged(int)),parent,SLOT(speedSld_valueChanged(int)));
//       LTEXT           "60 Hz",IDC_SPEED,112,11,26,12
   CStatic* mfc4 = new CStatic(parent);
   CRect r4(CPoint(112,11),CSize(26,12));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("60 Hz"),WS_VISIBLE,r4,parent,IDC_SPEED);
   mfcToQtWidget->insert(IDC_SPEED,mfc4);
//   END      
}

#include "PCMImport.h"
void qtMfcInitDialogResource_IDD_PCMIMPORT(UINT dlgID,CDialog* parent1)
{
   CPCMImport* parent = dynamic_cast<CPCMImport*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_PCMIMPORT DIALOGEX 0, 0, 211, 134
   CRect rect(CPoint(0,0),CSize(211,134));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());   
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "PCM import"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   GROUPBOX        "Quality: 0",IDC_QUALITY_FRM,7,7,136,36
   CGroupBox* mfc1 = new CGroupBox(parent);
   CRect r1(CPoint(7,7),CSize(136,36));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Quality: 0"),WS_VISIBLE,r1,parent,IDC_QUALITY_FRM);
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
   mfc3->setGeometry(r3);
   mfcToQtWidget->insert(IDC_QUALITY,mfc3);
   QObject::connect(mfc3,SIGNAL(valueChanged(int)),parent,SLOT(quality_valueChanged(int)));
//   LTEXT           "High",IDC_STATIC,123,21,16,10
   CStatic* mfc4 = new CStatic(parent);
   CRect r4(CPoint(123,21),CSize(16,10));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("High"),WS_VISIBLE,r4,parent,IDC_STATIC);
//   GROUPBOX        "Gain: +0dB",IDC_VOLUME_FRM,7,47,136,35
   CGroupBox* mfc5 = new CGroupBox(parent);
   CRect r5(CPoint(7,47),CSize(136,35));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Gain: +0dB"),WS_VISIBLE,r5,parent,IDC_VOLUME_FRM);
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
   mfc7->setGeometry(r7);
   mfcToQtWidget->insert(IDC_VOLUME,mfc7);
   QObject::connect(mfc7,SIGNAL(valueChanged(int)),parent,SLOT(volume_valueChanged(int)));
//   LTEXT           "High",IDC_STATIC,123,61,16,10
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(123,61),CSize(16,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("High"),WS_VISIBLE,r8,parent,IDC_STATIC);
//   GROUPBOX        "File info",IDC_STATIC,7,86,136,40
   CGroupBox* mfc9 = new CGroupBox(parent);
   CRect r9(CPoint(7,86),CSize(136,40));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("File info"),WS_VISIBLE,r9,parent,IDC_STATIC);
//   LTEXT           "Static",IDC_SAMPLE_RATE,15,99,118,8
   CStatic* mfc10 = new CStatic(parent);
   CRect r10(CPoint(15,99),CSize(118,8));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T(""),WS_VISIBLE,r10,parent,IDC_SAMPLE_RATE);
   mfcToQtWidget->insert(IDC_SAMPLE_RATE,mfc10);
//   LTEXT           "Static",IDC_RESAMPLING,15,111,118,8
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(15,111),CSize(118,8));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T(""),WS_VISIBLE,r11,parent,IDC_RESAMPLING);
   mfcToQtWidget->insert(IDC_RESAMPLING,mfc11);
//   PUSHBUTTON      "Preview",IDC_PREVIEW,154,7,50,14
   CButton* mfc12 = new CButton(parent);
   CRect r12(CPoint(154,7),CSize(50,14));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Preview"),WS_VISIBLE,r12,parent,IDC_PREVIEW);
   mfcToQtWidget->insert(IDC_PREVIEW,mfc12);
   QObject::connect(mfc12,SIGNAL(clicked()),parent,SLOT(preview_clicked()));
//   DEFPUSHBUTTON   "OK",IDOK,154,24,50,14
   CButton* mfc13 = new CButton(parent);
   CRect r13(CPoint(154,24),CSize(50,14));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r13,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc13);
   QObject::connect(mfc13,SIGNAL(clicked()),parent,SLOT(ok_clicked()));
//   PUSHBUTTON      "Cancel",IDCANCEL,154,41,50,14
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(154,41),CSize(50,14));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Cancel"),WS_VISIBLE,r14,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc14);
   QObject::connect(mfc14,SIGNAL(clicked()),parent,SLOT(cancel_clicked()));
//   END
}

#include "InstrumentEditor2A03.h"
#include "InstrumentEditorN163.h"
#include "InstrumentEditorS5B.h"
#include "InstrumentEditorVRC6.h"
void qtMfcInitDialogResource_IDD_INSTRUMENT_INTERNAL(UINT dlgID,CDialog* parent1)
{
   CInstrumentEditor2A03* parent2A03 = dynamic_cast<CInstrumentEditor2A03*>(parent1);
   CInstrumentEditorN163* parentN163 = dynamic_cast<CInstrumentEditorN163*>(parent1);
   CInstrumentEditorS5B* parentS5B = dynamic_cast<CInstrumentEditorS5B*>(parent1);
   CInstrumentEditorVRC6* parentVRC6 = dynamic_cast<CInstrumentEditorVRC6*>(parent1);
   CDialog* parent = NULL;
   if ( parent2A03 )
      parent = parent2A03;
   else if ( parentN163 )
      parent = parentN163;
   else if ( parentS5B )
      parent = parentS5B;
   else if ( parentVRC6 )
      parent = parentVRC6;
   else
      qFatal("no parent for dialog?!");
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_INSTRUMENT_INTERNAL DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());   
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   GROUPBOX        "Sequence editor",IDC_STATIC,120,7,245,158
   CGroupBox* mfc5 = new CGroupBox(parent);
   CRect r5(CPoint(120,7),CSize(245,158));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Sequence editor"),WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   GROUPBOX        "Instrument settings",IDC_STATIC,7,7,107,158,0,WS_EX_TRANSPARENT
   CGroupBox* mfc6 = new CGroupBox(parent);
   CRect r6(CPoint(7,7),CSize(107,158));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Instrument settings"),WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_INSTSETTINGS,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,12,18,96,109,WS_EX_TRANSPARENT
   CListCtrl* mfc1 = new CListCtrl(parent);
   CRect r1(CPoint(12,18),CSize(96,109));
   parent->MapDialogRect(&r1);
   mfc1->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r1,parent,IDC_INSTSETTINGS);
   mfcToQtWidget->insert(IDC_INSTSETTINGS,mfc1);
   QObject::connect(mfc1,SIGNAL(itemSelectionChanged()),parent,SLOT(instSettings_itemSelectionChanged()));
//   CONTROL         "Sequence #",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | SS_CENTERIMAGE | WS_GROUP,12,149,53,10,WS_EX_TRANSPARENT
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(12,149),CSize(53,10));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Sequence #"),SS_LEFTNOWORDWRAP | SS_CENTERIMAGE | WS_GROUP | WS_VISIBLE,r2,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   EDITTEXT        IDC_SEQ_INDEX,69,147,39,12,ES_AUTOHSCROLL | ES_NUMBER
//   CONTROL         "",IDC_SEQUENCE_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,66,153,11,9
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc3 = new CEdit(parent);
   CSpinButtonCtrl* mfc4 = new CSpinButtonCtrl(parent);
   CRect r3(CPoint(69,147),CSize(39,12));
   CRect r4(CPoint(r3.right-11,r3.top),CSize(11,12));
   parent->MapDialogRect(&r3);
   parent->MapDialogRect(&r4);
   mfc3->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r3,parent,IDC_SEQ_INDEX);
   mfc3->setBuddy(mfc4);
   mfcToQtWidget->insert(IDC_SEQ_INDEX,mfc3);
   QObject::connect(mfc3,SIGNAL(textChanged(QString)),parent,SLOT(seqIndex_textChanged(QString)));
   mfc4->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r4,parent,IDC_SEQUENCE_SPIN);
   mfc4->setBuddy(mfc3);
   mfcToQtWidget->insert(IDC_SEQUENCE_SPIN,mfc4);
   QObject::connect(mfc4,SIGNAL(valueChanged(int)),parent,SLOT(sequenceSpin_valueChanged(int)));   
//   PUSHBUTTON      "Select next empty slot",IDC_FREE_SEQ,12,129,96,15
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(12,129),CSize(96,15));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Select next empty slot"),WS_VISIBLE,r7,parent,IDC_FREE_SEQ);
   mfcToQtWidget->insert(IDC_FREE_SEQ,mfc7);
   QObject::connect(mfc7,SIGNAL(clicked()),parent,SLOT(freeSeq_clicked()));
//   EDITTEXT        IDC_SEQUENCE_STRING,126,149,232,13,ES_AUTOHSCROLL
   CEdit* mfc8 = new CEdit(parent);
   CRect r8(CPoint(126,149),CSize(232,13));
   parent->MapDialogRect(&r8);
   mfc8->Create(ES_AUTOHSCROLL | WS_VISIBLE,r8,parent,IDC_SEQUENCE_STRING);
   mfcToQtWidget->insert(IDC_SEQUENCE_STRING,mfc8);
//   END
}

#include "InstrumentEditorDPCM.h"
void qtMfcInitDialogResource_IDD_INSTRUMENT_DPCM(UINT dlgID,CDialog* parent1)
{
   CInstrumentEditorDPCM* parent = dynamic_cast<CInstrumentEditorDPCM*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_INSTRUMENT_DPCM DIALOGEX 0, 0, 372, 174
   CRect rect(CPoint(0,0),CSize(372,174));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_CHILD | WS_SYSMENU
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN   
//   GROUPBOX        "Loaded samples",IDC_STATIC,192,7,173,160
   CGroupBox* mfc9 = new CGroupBox(parent);
   CRect r9(CPoint(192,7),CSize(173,160));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("Loaded samples"),WS_VISIBLE,r9,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   GROUPBOX        "Assigned samples",IDC_STATIC,7,7,179,160
   CGroupBox* mfc10 = new CGroupBox(parent);
   CRect r10(CPoint(7,7),CSize(179,160));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Assigned samples"),WS_VISIBLE,r10,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_OCTAVE,138,30,42,53,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc1 = new CComboBox(parent);
   CRect r1(CPoint(138,30),CSize(42,53));
   parent->MapDialogRect(&r1);
   mfc1->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r1,parent,IDC_OCTAVE);
   mfcToQtWidget->insert(IDC_OCTAVE,mfc1);
   QObject::connect(mfc1,SIGNAL(currentIndexChanged(int)),parent,SLOT(octave_currentIndexChanged(int)));
//   LTEXT           "Octave",IDC_STATIC,138,19,30,10
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(138,19),CSize(30,10));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Octave"),WS_VISIBLE,r2,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_SAMPLES,15,148,117,125,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc3 = new CComboBox(parent);
   CRect r3(CPoint(15,148),CSize(117,125));
   parent->MapDialogRect(&r3);
   mfc3->Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_SAMPLES);
   mfcToQtWidget->insert(IDC_SAMPLES,mfc3);
   QObject::connect(mfc3,SIGNAL(currentIndexChanged(int)),parent,SLOT(samples_currentIndexChanged(int)));
//   PUSHBUTTON      "Unload",IDC_UNLOAD,312,35,47,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(312,35),CSize(47,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Unload"),WS_VISIBLE,r4,parent,IDC_UNLOAD);
   mfcToQtWidget->insert(IDC_UNLOAD,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),parent,SLOT(unload_clicked()));
//   DEFPUSHBUTTON   "Load",IDC_LOAD,312,19,47,14
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(312,19),CSize(47,14));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Load"),BS_DEFPUSHBUTTON | WS_VISIBLE,r5,parent,IDC_LOAD);
   mfcToQtWidget->insert(IDC_LOAD,mfc5);
   QObject::connect(mfc5,SIGNAL(clicked()),parent,SLOT(load_clicked()));
//   PUSHBUTTON      "Save",IDC_SAVE,312,51,47,14
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(312,51),CSize(47,14));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Save"),WS_VISIBLE,r6,parent,IDC_SAVE);
   mfcToQtWidget->insert(IDC_SAVE,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),parent,SLOT(save_clicked()));
//   PUSHBUTTON      "Import",IDC_IMPORT,312,67,47,14
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(312,67),CSize(47,14));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Import"),WS_VISIBLE,r7,parent,IDC_IMPORT);
   mfcToQtWidget->insert(IDC_IMPORT,mfc7);
   QObject::connect(mfc7,SIGNAL(clicked()),parent,SLOT(import_clicked()));
//   LTEXT           "Space used 16 / 16 kb",IDC_SPACE,198,151,166,9
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(198,151),CSize(166,9));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Space used 16 / 16 kb"),WS_VISIBLE,r8,parent,IDC_SPACE);
   mfcToQtWidget->insert(IDC_SPACE,mfc8);
//   COMBOBOX        IDC_PITCH,138,58,42,53,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc11 = new CComboBox(parent);
   CRect r11(CPoint(138,58),CSize(42,53));
   parent->MapDialogRect(&r11);
   mfc11->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r11,parent,IDC_PITCH);
   mfcToQtWidget->insert(IDC_PITCH,mfc11);
   QObject::connect(mfc11,SIGNAL(currentIndexChanged(int)),parent,SLOT(pitch_currentIndexChanged(int)));
//   LTEXT           "Pitch",IDC_STATIC,138,48,30,8
   CStatic* mfc12 = new CStatic(parent);
   CRect r12(CPoint(138,48),CSize(30,8));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Pitch"),WS_VISIBLE,r12,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_TABLE,"SysListView32",LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,15,19,117,125
   CListCtrl* mfc13 = new CListCtrl(parent);
   CRect r13(CPoint(15,19),CSize(117,125));
   parent->MapDialogRect(&r13);
   mfc13->Create(LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r13,parent,IDC_TABLE);
   mfcToQtWidget->insert(IDC_TABLE,mfc13);
   QObject::connect(mfc13,SIGNAL(itemSelectionChanged()),parent,SLOT(table_itemSelectionChanged()));
   QObject::connect(mfc13,SIGNAL(cellClicked(int,int)),parent,SLOT(table_cellClicked(int,int)));
   QObject::connect(mfc13,SIGNAL(cellDoubleClicked(int,int)),parent,SLOT(table_cellDoubleClicked(int,int)));
//   CONTROL         "",IDC_SAMPLE_LIST,"SysListView32",LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,198,19,108,125
   CListCtrl* mfc14 = new CListCtrl(parent);
   CRect r14(CPoint(198,19),CSize(108,125));
   parent->MapDialogRect(&r14);
   mfc14->Create(LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r14,parent,IDC_SAMPLE_LIST);
   mfcToQtWidget->insert(IDC_SAMPLE_LIST,mfc14);
   QObject::connect(mfc14,SIGNAL(itemSelectionChanged()),parent,SLOT(sampleList_itemSelectionChanged()));
   QObject::connect(mfc14,SIGNAL(cellClicked(int,int)),parent,SLOT(sampleList_cellClicked(int,int)));
   QObject::connect(mfc14,SIGNAL(cellDoubleClicked(int,int)),parent,SLOT(sampleList_cellDoubleClicked(int,int)));
//   CONTROL         "Loop",IDC_LOOP,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,138,75,42,9
   CButton* mfc15 = new CButton(parent);
   CRect r15(CPoint(138,75),CSize(42,9));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("Loop"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r15,parent,IDC_LOOP);
   mfcToQtWidget->insert(IDC_LOOP,mfc15);
   QObject::connect(mfc15,SIGNAL(clicked()),parent,SLOT(loop_clicked()));
//   PUSHBUTTON      "<-",IDC_ADD,138,130,42,14
   CButton* mfc16 = new CButton(parent);
   CRect r16(CPoint(138,130),CSize(42,14));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("<-"),WS_VISIBLE,r16,parent,IDC_ADD);
   mfcToQtWidget->insert(IDC_ADD,mfc16);
   QObject::connect(mfc16,SIGNAL(clicked()),parent,SLOT(add_clicked()));
//   PUSHBUTTON      "->",IDC_REMOVE,138,146,42,14
   CButton* mfc17 = new CButton(parent);
   CRect r17(CPoint(138,146),CSize(42,14));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("->"),WS_VISIBLE,r17,parent,IDC_REMOVE);
   mfcToQtWidget->insert(IDC_REMOVE,mfc17);
   QObject::connect(mfc15,SIGNAL(clicked()),parent,SLOT(remove_clicked()));
//   EDITTEXT        IDC_LOOP_POINT,138,106,42,13,ES_AUTOHSCROLL | NOT WS_VISIBLE
   CEdit* mfc18 = new CEdit(parent);
   CRect r18(CPoint(138,106),CSize(42,13));
   parent->MapDialogRect(&r18);
   mfc18->Create(ES_AUTOHSCROLL,r18,parent,IDC_LOOP_POINT);
   mfcToQtWidget->insert(IDC_LOOP_POINT,mfc18);
   QObject::connect(mfc18,SIGNAL(textChanged(QString)),parent,SLOT(loopPoint_textChanged(QString)));
//   PUSHBUTTON      "Edit",IDC_EDIT,312,83,47,14
   CButton* mfc19 = new CButton(parent);
   CRect r19(CPoint(312,83),CSize(47,14));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Edit"),WS_VISIBLE,r19,parent,IDC_EDIT);
   mfcToQtWidget->insert(IDC_EDIT,mfc19);
   QObject::connect(mfc19,SIGNAL(clicked()),parent,SLOT(edit_clicked()));
//   PUSHBUTTON      "Preview",IDC_PREVIEW,312,99,47,14
   CButton* mfc20 = new CButton(parent);
   CRect r20(CPoint(312,99),CSize(47,14));
   parent->MapDialogRect(&r20);
   mfc20->Create(_T("Preview"),WS_VISIBLE,r20,parent,IDC_PREVIEW);
   mfcToQtWidget->insert(IDC_PREVIEW,mfc20);
   QObject::connect(mfc20,SIGNAL(clicked()),parent,SLOT(preview_clicked()));
//   LTEXT           "Loop offset",IDC_STATIC,138,94,42,10,NOT WS_VISIBLE
   CStatic* mfc21 = new CStatic(parent);
   CRect r21(CPoint(138,94),CSize(42,10));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("Loop offset"),0,r21,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   END
}   

#include "InstrumentEditDlg.h"
void qtMfcInitDialogResource_IDD_INSTRUMENT(UINT dlgID,CDialog* parent1)
{
   CInstrumentEditDlg* parent = dynamic_cast<CInstrumentEditDlg*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_INSTRUMENT DIALOGEX 0, 0, 389, 242
   CRect rect(CPoint(0,0),CSize(389,242));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());   
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Instrument editor"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   CONTROL         "",IDC_INST_TAB,"SysTabControl32",0x0,7,7,375,185
   CTabCtrl* mfc1 = new CTabCtrl(parent);
   CRect r1(CPoint(7,7),CSize(375,185));
   parent->MapDialogRect(&r1);
   mfc1->setGeometry(r1);
   mfcToQtWidget->insert(IDC_INST_TAB,mfc1);
   QObject::connect(mfc1,SIGNAL(currentChanged(int)),parent,SLOT(instTab_currentChanged(int)));
//   CONTROL         "",IDC_KEYBOARD,"Static",SS_OWNERDRAW | SS_REALSIZEIMAGE | SS_SUNKEN,7,198,375,37
   CStatic* mfc2 = new CStatic(parent);
   CRect r2(CPoint(7,198),CSize(375,37));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T(""),SS_OWNERDRAW | SS_REALSIZEIMAGE | SS_SUNKEN | WS_VISIBLE,r2,parent,IDC_KEYBOARD);
   mfcToQtWidget->insert(IDC_KEYBOARD,mfc2);   
//   END
}   

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_CONFIG_APPEARANCE(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_CONFIG_APPEARANCE DIALOGEX 0, 0, 280, 167
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Appearance"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Color schemes",IDC_STATIC,7,7,149,39
//       COMBOBOX        IDC_SCHEME,15,22,135,126,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
//       GROUPBOX        "Colors",IDC_STATIC,7,54,149,62
//       LTEXT           "Item",IDC_STATIC,15,68,16,8
//       COMBOBOX        IDC_COL_ITEM,35,66,115,172,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       CONTROL         "",IDC_COL_PREVIEW,"Static",SS_OWNERDRAW,15,84,69,15
//       PUSHBUTTON      "Pick color",IDC_PICK_COL,96,84,54,15
//       CONTROL         "Pattern colors",IDC_PATTERNCOLORS,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,15,102,113,9
//       GROUPBOX        "Pattern font and size",IDC_STATIC,7,123,149,37
//       COMBOBOX        IDC_FONT,15,139,102,93,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
//       COMBOBOX        IDC_FONT_SIZE,122,139,28,30,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       GROUPBOX        "Preview",IDC_STATIC,166,7,107,153
//       CONTROL         "",IDC_PREVIEW,"Static",SS_OWNERDRAW,176,18,90,125
//   END
}

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_MIDIIMPORT(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_MIDIIMPORT DIALOGEX 0, 0, 204, 137
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "MIDI file import"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       DEFPUSHBUTTON   "OK",IDOK,147,7,50,14
//       PUSHBUTTON      "Cancel",IDCANCEL,147,24,50,14
//       GROUPBOX        "Channel mapping",IDC_STATIC,7,7,131,101
//       LTEXT           "Square 1",IDC_STATIC,12,19,42,11
//       LTEXT           "Square 2",IDC_STATIC,12,36,42,10
//       LTEXT           "Triangle",IDC_STATIC,12,54,42,10
//       LTEXT           "Noise",IDC_STATIC,12,72,42,10
//       LTEXT           "DPCM",IDC_STATIC,12,90,42,10
//       COMBOBOX        IDC_CHANNEL1,60,18,72,72,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       COMBOBOX        IDC_CHANNEL2,60,36,72,72,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       COMBOBOX        IDC_CHANNEL3,60,54,72,72,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       COMBOBOX        IDC_CHANNEL4,60,72,72,72,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       COMBOBOX        IDC_CHANNEL5,60,90,72,72,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       LTEXT           "Pattern length:",IDC_STATIC,12,118,56,10
//       EDITTEXT        IDC_PATLEN,74,117,58,13,ES_RIGHT | ES_AUTOHSCROLL | ES_NUMBER
//   END
}  

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_CONFIG_GENERAL(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_CONFIG_GENERAL DIALOGEX 0, 0, 280, 167
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "General"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "General settings",IDC_STATIC,7,7,125,153
//       CONTROL         "Wrap cursor",IDC_OPT_WRAPCURSOR,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,18,113,9
//       CONTROL         "Wrap across frames",IDC_OPT_WRAPFRAMES,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,29,113,9
//       CONTROL         "Free cursor edit",IDC_OPT_FREECURSOR,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,40,113,9
//       CONTROL         "Preview wave-files",IDC_OPT_WAVEPREVIEW,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,51,113,9
//       CONTROL         "Key repeat",IDC_OPT_KEYREPEAT,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,62,113,9
//       CONTROL         "Show row numbers in hex",IDC_OPT_HEXROW,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,73,113,9
//       CONTROL         "Preview next/previous frame",IDC_OPT_FRAMEPREVIEW,
//                       "Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,84,113,9
//       CONTROL         "Don't reset DPCM on note stop",IDC_OPT_NODPCMRESET,
//                       "Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,95,113,9
//       CONTROL         "Ignore Step when moving",IDC_OPT_NOSTEPMOVE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,106,113,9
//       CONTROL         "Delete-key pulls up rows",IDC_OPT_PULLUPDELETE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,117,113,9
//       CONTROL         "Backup files",IDC_OPT_BACKUPS,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,128,113,9
//       CONTROL         "Single instance",IDC_OPT_SINGLEINSTANCE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,139,113,9
//       GROUPBOX        "Pattern edit style",IDC_STATIC,138,7,135,48
//       CONTROL         "FastTracker 2",IDC_STYLE1,"Button",BS_AUTORADIOBUTTON,144,18,120,8
//       CONTROL         "ModPlug tracker",IDC_STYLE2,"Button",BS_AUTORADIOBUTTON,144,30,120,8
//       CONTROL         "Impulse Tracker",IDC_STYLE3,"Button",BS_AUTORADIOBUTTON,144,42,120,8
//       GROUPBOX        "Edit settings",IDC_STATIC,137,57,135,30
//       LTEXT           "Page jump length",IDC_STATIC,143,70,56,9
//       COMBOBOX        IDC_PAGELENGTH,215,68,48,65,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP
//       GROUPBOX        "Keys",IDC_STATIC,138,89,135,71
//       LTEXT           "Note cut",IDC_STATIC,144,100,50,11
//       EDITTEXT        IDC_KEY_NOTE_CUT,204,99,60,13,ES_AUTOHSCROLL | ES_READONLY
//       LTEXT           "Clear field",IDC_STATIC,144,115,50,11
//       EDITTEXT        IDC_KEY_CLEAR,204,114,60,13,ES_AUTOHSCROLL | ES_READONLY
//       LTEXT           "Repeat",IDC_STATIC,144,129,50,11
//       EDITTEXT        IDC_KEY_REPEAT,204,128,60,13,ES_AUTOHSCROLL | ES_READONLY
//       LTEXT           "Note release",IDC_STATIC,144,143,50,11
//       EDITTEXT        IDC_KEY_NOTE_RELEASE,204,142,60,13,ES_AUTOHSCROLL | ES_READONLY
//   END
}   

#include "ModulePropertiesDlg.h"
void qtMfcInitDialogResource_IDD_PROPERTIES(UINT dlgID,CDialog* parent1)
{
   CModulePropertiesDlg* parent = dynamic_cast<CModulePropertiesDlg*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_PROPERTIES DIALOGEX 0, 0, 213, 259
   CRect rect(CPoint(0,0),CSize(213,259));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Module properties"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Song editor",IDC_STATIC,7,7,199,147
   CGroupBox* mfc1 = new CGroupBox(parent);
   CRect r1(CPoint(7,7),CSize(199,147));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Song editor"),WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Expansion sound",IDC_STATIC,7,162,127,30
   CGroupBox* mfc10 = new CGroupBox(parent);
   CRect r10(CPoint(7,162),CSize(127,30));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Expansion sound"),WS_VISIBLE,r10,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Vibrato",IDC_STATIC,7,194,199,31
   CGroupBox* mfc12 = new CGroupBox(parent);
   CRect r12(CPoint(7,194),CSize(199,31));
   parent->MapDialogRect(&r12);
   mfc12->Create(_T("Vibrato"),WS_VISIBLE,r12,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Channels: 0",IDC_CHANNELS_NR,141,162,65,30
   CGroupBox* mfc17 = new CGroupBox(parent);
   CRect r17(CPoint(141,162),CSize(65,30));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("Channels: 0"),WS_VISIBLE,r17,parent,IDC_CHANNELS_NR);
   mfcToQtWidget->insert(IDC_CHANNELS_NR,mfc17);
//       CONTROL         "",IDC_SONGLIST,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP,14,18,120,114
   CListCtrl* mfc2 = new CListCtrl(parent);
   CRect r2(CPoint(14,18),CSize(120,114));
   parent->MapDialogRect(&r2);
   mfc2->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_SONGLIST);
   mfcToQtWidget->insert(IDC_SONGLIST,mfc2);
   QObject::connect(mfc2,SIGNAL(itemSelectionChanged()),parent,SLOT(songList_itemSelectionChanged()));
//       PUSHBUTTON      "Add",IDC_SONG_ADD,138,18,60,14
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(138,18),CSize(60,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Add"),WS_VISIBLE,r3,parent,IDC_SONG_ADD);
   mfcToQtWidget->insert(IDC_SONG_ADD,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),parent,SLOT(songAdd_clicked()));
//       PUSHBUTTON      "Remove",IDC_SONG_REMOVE,138,35,60,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(138,35),CSize(60,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Remove"),WS_VISIBLE,r4,parent,IDC_SONG_REMOVE);
   mfcToQtWidget->insert(IDC_SONG_REMOVE,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),parent,SLOT(songRemove_clicked()));
//       PUSHBUTTON      "Move up",IDC_SONG_UP,138,52,60,14
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(138,52),CSize(60,14));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Move up"),WS_VISIBLE,r5,parent,IDC_SONG_UP);
   mfcToQtWidget->insert(IDC_SONG_UP,mfc5);
   QObject::connect(mfc5,SIGNAL(clicked()),parent,SLOT(songUp_clicked()));
//       PUSHBUTTON      "Move down",IDC_SONG_DOWN,138,69,60,14
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(138,69),CSize(60,14));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Move down"),WS_VISIBLE,r6,parent,IDC_SONG_DOWN);
   mfcToQtWidget->insert(IDC_SONG_DOWN,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),parent,SLOT(songDown_clicked()));
//       PUSHBUTTON      "Import file",IDC_SONG_IMPORT,138,86,60,14
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(138,86),CSize(60,14));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Import file"),WS_VISIBLE,r7,parent,IDC_SONG_IMPORT);
   mfcToQtWidget->insert(IDC_SONG_IMPORT,mfc7);
   QObject::connect(mfc7,SIGNAL(clicked()),parent,SLOT(songImport_clicked()));
//       LTEXT           " Title",IDC_STATIC,14,135,17,12,SS_CENTERIMAGE
   CStatic* mfc8 = new CStatic(parent);
   mfc8->setText(" Title");
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
   QObject::connect(mfc9,SIGNAL(textChanged(QString)),parent,SLOT(songName_textChanged(QString)));
//       COMBOBOX        IDC_EXPANSION,14,173,113,61,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc11 = new CComboBox(parent);
   CRect r11(CPoint(14,173),CSize(113,61));
   parent->MapDialogRect(&r11);
   mfc11->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r11,parent,IDC_EXPANSION);
   mfcToQtWidget->insert(IDC_EXPANSION,mfc11);
   QObject::connect(mfc11,SIGNAL(currentIndexChanged(int)),parent,SLOT(expansion_currentIndexChanged(int)));
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
//       IDC_VIBRATO, 0x403, 20, 0
//   0x6c4f, 0x2064, 0x7473, 0x6c79, 0x2065, 0x6228, 0x6e65, 0x2064, 0x7075, 
//   0x0029, 
//       0
//   END
   mfc13->AddString(_T("Old style (bend up)"));
   mfc13->AddString(_T("New style (bend up & down)"));
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,231,199,1
   qDebug("horzline not implemented");
//       DEFPUSHBUTTON   "OK",IDOK,95,238,53,14
   CButton* mfc15 = new CButton(parent);
   CRect r15(CPoint(95,238),CSize(53,14));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r15,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc15);
   QObject::connect(mfc15,SIGNAL(clicked()),parent,SLOT(ok_clicked()));
//       PUSHBUTTON      "Cancel",IDCANCEL,153,238,53,14
   CButton* mfc16 = new CButton(parent);
   CRect r16(CPoint(153,238),CSize(53,14));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Cancel"),WS_VISIBLE,r16,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc16);
   QObject::connect(mfc16,SIGNAL(clicked()),parent,SLOT(cancel_clicked()));
//       CONTROL         "",IDC_CHANNELS,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,147,173,51,13
   CSliderCtrl* mfc18 = new CSliderCtrl(parent);
   CRect r18(CPoint(147,173),CSize(51,13));
   parent->MapDialogRect(&r18);
   mfc18->setGeometry(r18);
   mfcToQtWidget->insert(IDC_CHANNELS,mfc18);
   QObject::connect(mfc18,SIGNAL(valueChanged(int)),parent,SLOT(channels_valueChanged(int)));
//   END
}   

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_CONFIG_MIDI(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_CONFIG_MIDI DIALOGEX 0, 0, 280, 167
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "MIDI"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Input Device",IDC_STATIC,7,7,266,35
//       COMBOBOX        IDC_INDEVICES,13,20,253,50,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       GROUPBOX        "Output Device",IDC_STATIC,7,48,266,35
//       COMBOBOX        IDC_OUTDEVICES,13,61,253,50,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       CONTROL         "Receive sync ticks",IDC_MASTER_SYNC,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,102,173,10
//       CONTROL         "Skip key releases",IDC_KEY_RELEASE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,112,173,10
//       CONTROL         "Map MIDI channels to NES channels",IDC_CHANMAP,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,122,173,10
//       CONTROL         "Record velocities",IDC_VELOCITY,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,132,173,10
//       CONTROL         "Auto arpeggiate chords",IDC_ARPEGGIATE,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,142,173,10
//       GROUPBOX        "Options",IDC_STATIC,7,89,266,71
//   END
}   

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_CONFIG_SOUND(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_CONFIG_SOUND DIALOGEX 0, 0, 280, 167
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   EXSTYLE WS_EX_CONTEXTHELP
//   CAPTION "Sound"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Device",IDC_STATIC,7,7,266,35
//       COMBOBOX        IDC_DEVICES,13,20,253,12,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
//       GROUPBOX        "Sample rate",IDC_STATIC,7,48,113,33
//       COMBOBOX        IDC_SAMPLE_RATE,13,61,101,62,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
//       GROUPBOX        "Sample size",IDC_STATIC,7,90,113,33
//       COMBOBOX        IDC_SAMPLE_SIZE,13,102,101,62,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
//       GROUPBOX        "Buffer length",IDC_STATIC,7,129,113,31
//       CONTROL         "",IDC_BUF_LENGTH,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,13,141,69,12
//       CTEXT           "20 ms",IDC_BUF_LEN,83,142,31,11
//       GROUPBOX        "Bass filtering",IDC_STATIC,126,48,147,33
//       LTEXT           "Frequency",IDC_STATIC,132,63,36,11
//       CONTROL         "",IDC_BASS_FREQ,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,174,63,55,12
//       CTEXT           "16 Hz",IDC_BASS_FREQ_T,234,64,32,10
//       GROUPBOX        "Treble filtering",IDC_STATIC,126,86,147,43
//       LTEXT           "Damping",IDC_STATIC,132,99,36,10
//       CONTROL         "",IDC_TREBLE_DAMP,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,174,99,55,12
//       CTEXT           "-24 dB",IDC_TREBLE_DAMP_T,234,99,32,9
//       LTEXT           "Frequency",IDC_STATIC,132,112,36,10
//       CONTROL         "",IDC_TREBLE_FREQ,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,174,112,55,12
//       CTEXT           "12000 Hz",IDC_TREBLE_FREQ_T,234,110,32,11
//       GROUPBOX        "Volume",IDC_STATIC,126,133,147,27
//       CONTROL         "",IDC_VOLUME,"msctls_trackbar32",TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,132,144,96,12
//       CTEXT           "100 %",IDC_VOLUME_T,234,144,31,8
//   END
}   

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_CONFIG_SHORTCUTS(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_CONFIG_SHORTCUTS DIALOGEX 0, 0, 280, 167
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Shortcuts"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       CONTROL         "",IDC_SHORTCUTS,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP,7,7,266,136
//       LTEXT           "Keys:",IDC_STATIC,7,149,23,12
//       EDITTEXT        IDC_KEY,36,147,144,14,ES_AUTOHSCROLL | ES_READONLY
//       PUSHBUTTON      "Clear",IDC_CLEAR,186,147,36,14
//       PUSHBUTTON      "Default",IDC_DEFAULT,228,147,45,14
//   END
}   

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_OCTAVE(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_OCTAVE DIALOGEX 0, 0, 300, 12
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD | WS_SYSMENU
//   EXSTYLE WS_EX_TRANSPARENT
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       COMBOBOX        IDC_OCTAVE,36,0,30,74,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
//       CONTROL         "Octave",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | WS_GROUP,6,2,24,8
//       CONTROL         "Follow-mode",IDC_FOLLOW,"Button",BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP,72,0,52,13
//       CONTROL         "Row highlight",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | WS_GROUP,129,2,44,8
//       EDITTEXT        IDC_HIGHLIGHT1,177,0,27,12,ES_AUTOHSCROLL | ES_NUMBER
//       CONTROL         "",IDC_HIGHLIGHTSPIN1,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HOTTRACK,197,3,11,6
//       CONTROL         "2nd highlight",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | WS_GROUP,211,2,42,8
//       EDITTEXT        IDC_HIGHLIGHT2,259,0,27,12,ES_AUTOHSCROLL | ES_NUMBER
//       CONTROL         "",IDC_HIGHLIGHTSPIN2,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HOTTRACK,281,3,11,6
//   END
}   

#include "ExportDialog.h"
void qtMfcInitDialogResource_IDD_EXPORT(UINT dlgID,CDialog* parent1)
{
   CExportDialog* parent = dynamic_cast<CExportDialog*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_EXPORT DIALOGEX 0, 0, 247, 283
   CRect rect(CPoint(0,0),CSize(247,283));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Export file"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "NSF file options",IDC_STATIC,7,7,173,75
   CGroupBox* mfc10 = new CGroupBox(parent);
   CRect r10(CPoint(7,7),CSize(173,75));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("NSF file options"),WS_VISIBLE,r10,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Progress",IDC_STATIC,7,120,233,156
   CGroupBox* mfc14 = new CGroupBox(parent);
   CRect r14(CPoint(7,120),CSize(233,156));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Progress"),WS_VISIBLE,r14,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Type of file",IDC_STATIC,7,87,233,29
   CGroupBox* mfc16 = new CGroupBox(parent);
   CRect r16(CPoint(7,87),CSize(233,29));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Type of file"),WS_VISIBLE,r16,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "&Export",IDC_EXPORT,187,7,53,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(187,7),CSize(53,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Export"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDC_EXPORT);
   mfcToQtWidget->insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),parent,SLOT(export_clicked()));
//       PUSHBUTTON      "&Close",IDC_CLOSE,187,23,53,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(187,23),CSize(53,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Close"),WS_VISIBLE,r2,parent,IDC_CLOSE);
   mfcToQtWidget->insert(IDC_CLOSE,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),parent,SLOT(close_clicked()));
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
   QObject::connect(mfc17,SIGNAL(clicked()),parent,SLOT(play_clicked()));
//   END
}   

#include "InstrumentEditorVRC7.h"
void qtMfcInitDialogResource_IDD_INSTRUMENT_VRC7(UINT dlgID,CDialog* parent1)
{
   CInstrumentEditorVRC7* parent = dynamic_cast<CInstrumentEditorVRC7*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_INSTRUMENT_VRC7 DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_CHILD
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN   
//       GROUPBOX        "Patch",IDC_STATIC,7,7,310,30
   CGroupBox* mfc1 = new CGroupBox(parent);
   CRect r1(CPoint(7,7),CSize(310,30));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Patch"),WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Modulator settings",IDC_STATIC,7,42,153,123
   CGroupBox* mfc5 = new CGroupBox(parent);
   CRect r5(CPoint(7,42),CSize(153,123));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Modulator settings"),WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Carrier settings",IDC_STATIC,166,42,152,123
   CGroupBox* mfc23 = new CGroupBox(parent);
   CRect r23(CPoint(166,42),CSize(152,123));
   parent->MapDialogRect(&r23);
   mfc23->Create(_T("Carrier settings"),WS_VISIBLE,r23,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_PATCH,15,18,295,128,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc2 = new CComboBox(parent);
   CRect r2(CPoint(15,18),CSize(295,128));
   parent->MapDialogRect(&r2);
   mfc2->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r2,parent,IDC_PATCH);
   mfcToQtWidget->insert(IDC_PATCH,mfc2);
   QObject::connect(mfc2,SIGNAL(currentIndexChanged(int)),parent,SLOT(patch_currentIndexChanged(int)));
//       PUSHBUTTON      "Copy",IDC_COPY,322,7,43,14
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(322,7),CSize(43,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Copy"),WS_VISIBLE,r3,parent,IDC_COPY);
   mfcToQtWidget->insert(IDC_COPY,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),parent,SLOT(copy_clicked()));
//       PUSHBUTTON      "Paste",IDC_PASTE,322,23,43,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(322,23),CSize(43,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Paste"),WS_VISIBLE,r4,parent,IDC_PASTE);
   mfcToQtWidget->insert(IDC_PASTE,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),parent,SLOT(paste_clicked()));
//       CONTROL         "Amplitude modulation",IDC_M_AM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,54,84,10,WS_EX_TRANSPARENT
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(14,54),CSize(84,10));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Amplitude modulation"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r6,parent,IDC_M_AM);
   mfcToQtWidget->insert(IDC_M_AM,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),parent,SLOT(m_am_clicked()));
//       CONTROL         "Vibrato",IDC_M_VIB,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,104,54,39,10,WS_EX_TRANSPARENT
   CButton* mfc7 = new CButton(parent);
   CRect r7(CPoint(104,54),CSize(39,10));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Vibrato"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_M_VIB);
   mfcToQtWidget->insert(IDC_M_VIB,mfc7);
   QObject::connect(mfc7,SIGNAL(clicked()),parent,SLOT(m_vib_clicked()));
//       CONTROL         "Sustained",IDC_M_EG,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,70,47,10,WS_EX_TRANSPARENT
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(14,70),CSize(47,10));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Sustained"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r8,parent,IDC_M_EG);
   mfcToQtWidget->insert(IDC_M_EG,mfc8);
   QObject::connect(mfc8,SIGNAL(clicked()),parent,SLOT(m_eg_clicked()));
//       CONTROL         "RATE key scale",IDC_M_KSR2,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,89,70,65,10,WS_EX_TRANSPARENT
   CButton* mfc9 = new CButton(parent);
   CRect r9(CPoint(89,70),CSize(65,10));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("RATE key scale"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r9,parent,IDC_M_KSR2);
   mfcToQtWidget->insert(IDC_M_KSR2,mfc9);
   QObject::connect(mfc9,SIGNAL(clicked()),parent,SLOT(m_ksr2_clicked()));
//       CONTROL         "Wave rectification",IDC_M_DM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,86,74,10,WS_EX_TRANSPARENT
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(14,86),CSize(74,10));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Wave rectification"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r10,parent,IDC_M_DM);
   mfcToQtWidget->insert(IDC_M_DM,mfc10);
   QObject::connect(mfc10,SIGNAL(clicked()),parent,SLOT(m_dm_clicked()));
//       LTEXT           "Level",IDC_STATIC,91,87,18,8
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(91,87),CSize(18,8));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Level"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_KSL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,112,86,44,11,WS_EX_TRANSPARENT
   CSliderCtrl* mfc12 = new CSliderCtrl(parent);
   CRect r12(CPoint(112,86),CSize(44,11));
   parent->MapDialogRect(&r12);
   mfc12->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r12,parent,IDC_M_KSL);
   mfcToQtWidget->insert(IDC_M_KSL,mfc12);
   QObject::connect(mfc12,SIGNAL(valueChanged(int)),parent,SLOT(m_ksl_valueChanged(int)));
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
   QObject::connect(mfc14,SIGNAL(valueChanged(int)),parent,SLOT(m_mul_valueChanged(int)));
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
   QObject::connect(mfc16,SIGNAL(valueChanged(int)),parent,SLOT(m_ar_valueChanged(int)));
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
   QObject::connect(mfc18,SIGNAL(valueChanged(int)),parent,SLOT(m_dr_valueChanged(int)));
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
   QObject::connect(mfc20,SIGNAL(valueChanged(int)),parent,SLOT(m_sl_valueChanged(int)));
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
   QObject::connect(mfc22,SIGNAL(valueChanged(int)),parent,SLOT(m_rr_valueChanged(int)));
//       CONTROL         "Amplitude modulation",IDC_C_AM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,54,84,10
   CButton* mfc24 = new CButton(parent);
   CRect r24(CPoint(171,54),CSize(84,10));
   parent->MapDialogRect(&r24);
   mfc24->Create(_T("Amplitude modulation"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r24,parent,IDC_C_AM);
   mfcToQtWidget->insert(IDC_C_AM,mfc24);
   QObject::connect(mfc24,SIGNAL(clicked()),parent,SLOT(c_am_clicked()));
//       CONTROL         "Vibrato",IDC_C_VIB,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,263,54,39,10
   CButton* mfc25 = new CButton(parent);
   CRect r25(CPoint(263,54),CSize(39,10));
   parent->MapDialogRect(&r25);
   mfc25->Create(_T("Vibrato"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r25,parent,IDC_C_VIB);
   mfcToQtWidget->insert(IDC_C_VIB,mfc25);
   QObject::connect(mfc25,SIGNAL(clicked()),parent,SLOT(c_vib_clicked()));
//       CONTROL         "Sustained",IDC_C_EG,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,70,47,10
   CButton* mfc26 = new CButton(parent);
   CRect r26(CPoint(171,70),CSize(47,10));
   parent->MapDialogRect(&r26);
   mfc26->Create(_T("Sustained"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r26,parent,IDC_C_EG);
   mfcToQtWidget->insert(IDC_C_EG,mfc26);
   QObject::connect(mfc26,SIGNAL(clicked()),parent,SLOT(c_eg_clicked()));
//       CONTROL         "RATE key scale",IDC_C_KSR,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,247,70,65,10
   CButton* mfc27 = new CButton(parent);
   CRect r27(CPoint(247,70),CSize(65,10));
   parent->MapDialogRect(&r27);
   mfc27->Create(_T("RATE key scale"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r27,parent,IDC_C_KSR);
   mfcToQtWidget->insert(IDC_C_KSR,mfc27);
   QObject::connect(mfc27,SIGNAL(clicked()),parent,SLOT(c_ksr_clicked()));
//       CONTROL         "Wave rectification",IDC_C_DM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,86,74,10
   CButton* mfc28 = new CButton(parent);
   CRect r28(CPoint(171,86),CSize(74,10));
   parent->MapDialogRect(&r28);
   mfc28->Create(_T("Wave rectification"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r28,parent,IDC_C_DM);
   mfcToQtWidget->insert(IDC_C_DM,mfc28);
   QObject::connect(mfc28,SIGNAL(clicked()),parent,SLOT(c_dm_clicked()));
//       LTEXT           "Level",IDC_STATIC,249,87,18,8
   CStatic* mfc29 = new CStatic(parent);
   CRect r29(CPoint(249,87),CSize(18,8));
   parent->MapDialogRect(&r29);
   mfc29->Create(_T("Level"),WS_VISIBLE,r21,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_KSL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,270,86,40,11
   CSliderCtrl* mfc30 = new CSliderCtrl(parent);
   CRect r30(CPoint(270,86),CSize(40,11));
   parent->MapDialogRect(&r30);
   mfc30->Create(TBS_AUTOTICKS | WS_TABSTOP | WS_VISIBLE,r30,parent,IDC_C_KSL);
   mfcToQtWidget->insert(IDC_C_KSL,mfc30);
   QObject::connect(mfc30,SIGNAL(valueChanged(int)),parent,SLOT(c_ksl_valueChanged(int)));
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
   QObject::connect(mfc32,SIGNAL(valueChanged(int)),parent,SLOT(c_mul_valueChanged(int)));
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
   QObject::connect(mfc34,SIGNAL(valueChanged(int)),parent,SLOT(c_ar_valueChanged(int)));
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
   QObject::connect(mfc36,SIGNAL(valueChanged(int)),parent,SLOT(c_dr_valueChanged(int)));
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
   QObject::connect(mfc38,SIGNAL(valueChanged(int)),parent,SLOT(c_sl_valueChanged(int)));
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
   QObject::connect(mfc40,SIGNAL(valueChanged(int)),parent,SLOT(c_rr_valueChanged(int)));
//       CTEXT           "Modulator\nlevel",IDC_STATIC,322,42,36,17
   CStatic* mfc41 = new CStatic(parent);
   CRect r41(CPoint(322,42),CSize(36,17));
   parent->MapDialogRect(&r41);
   mfc41->Create(_T("Modulator\nlevel"),WS_VISIBLE,r41,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_TL,"msctls_trackbar32",TBS_VERT | TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,329,59,16,53
   CSliderCtrl* mfc42 = new CSliderCtrl(parent);
   CRect r42(CPoint(329,59),CSize(16,53));
   parent->MapDialogRect(&r42);
   mfc42->Create(TBS_VERT | TBS_BOTH | TBS_NOTICKS | WS_TABSTOP | WS_VISIBLE,r42,parent,IDC_TL);
   mfcToQtWidget->insert(IDC_TL,mfc42);
   QObject::connect(mfc42,SIGNAL(valueChanged(int)),parent,SLOT(tl_valueChanged(int)));
//       CTEXT           "Feedback",IDC_STATIC,322,112,36,9
   CStatic* mfc43 = new CStatic(parent);
   CRect r43(CPoint(322,112),CSize(36,9));
   parent->MapDialogRect(&r43);
   mfc43->Create(_T("Feedback"),WS_VISIBLE,r43,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_FB,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,325,124,25,41
   CSliderCtrl* mfc44 = new CSliderCtrl(parent);
   CRect r44(CPoint(325,124),CSize(25,41));
   parent->MapDialogRect(&r44);
   mfc44->Create(TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP | WS_VISIBLE,r44,parent,IDC_FB);
   mfcToQtWidget->insert(IDC_FB,mfc44);
   QObject::connect(mfc44,SIGNAL(valueChanged(int)),parent,SLOT(fb_valueChanged(int)));
//   END
}   

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_CREATEWAV(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_CREATEWAV DIALOGEX 0, 0, 151, 173
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Create wave file"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       DEFPUSHBUTTON   "Begin",IDC_BEGIN,37,152,52,14
//       PUSHBUTTON      "Cancel",IDCANCEL,92,152,52,14
//       GROUPBOX        "Song length",IDC_STATIC,7,7,137,47
//       CONTROL         "Play the song",IDC_RADIO_LOOP,"Button",BS_AUTORADIOBUTTON,14,20,59,10
//       CONTROL         "Play for",IDC_RADIO_TIME,"Button",BS_AUTORADIOBUTTON,14,38,41,10
//       EDITTEXT        IDC_TIMES,73,19,36,12,ES_AUTOHSCROLL
//       CONTROL         "",IDC_SPIN_LOOP,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,105,17,11,17
//       LTEXT           "time(s)",IDC_STATIC,115,20,21,10,SS_CENTERIMAGE
//       EDITTEXT        IDC_SECONDS,53,37,44,12,ES_AUTOHSCROLL
//       CONTROL         "",IDC_SPIN_TIME,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,93,36,11,14
//       LTEXT           "mm:ss",IDC_STATIC,106,38,21,10,SS_CENTERIMAGE
//       GROUPBOX        "Channels",IDC_STATIC,7,60,137,87
//       LISTBOX         IDC_CHANNELS,14,71,124,70,LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP
//   END
}   

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_WAVE_PROGRESS(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_WAVE_PROGRESS DIALOGEX 0, 0, 220, 111
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Creating WAV..."
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       PUSHBUTTON      "Cancel",IDC_CANCEL,84,90,50,14
//       CONTROL         "",IDC_PROGRESS_BAR,"msctls_progress32",WS_BORDER,7,65,206,12
//       CTEXT           "Progress",IDC_PROGRESS_LBL,7,37,206,11
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDFRAME,7,83,206,1
//       CTEXT           "File",IDC_PROGRESS_FILE,7,7,206,18,SS_CENTERIMAGE
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDFRAME,7,29,206,1
//       CTEXT           "Progress",IDC_TIME,7,49,206,11
//   END
}   

void qtMfcInitDialogResource_IDD_MAINBAR(UINT dlgID,CDialog* parent1)
{
   CDialog* parent = dynamic_cast<CDialog*>(parent1);
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

#include "InstrumentEditorFDS.h"
void qtMfcInitDialogResource_IDD_INSTRUMENT_FDS(UINT dlgID,CDialog* parent1)
{
   CInstrumentEditorFDS* parent = dynamic_cast<CInstrumentEditorFDS*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_INSTRUMENT_FDS DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());   
//   STYLE DS_SETFONT | DS_FIXEDSYS | DS_CONTROL | WS_CHILD
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Wave editor",IDC_STATIC,7,7,228,111
   CGroupBox* mfc1 = new CGroupBox(parent);
   CRect r1(CPoint(7,7),CSize(228,111));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Wave editor"),WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Frequency modulation",IDC_STATIC,240,7,124,79
   CGroupBox* mfc16 = new CGroupBox(parent);
   CRect r16(CPoint(240,7),CSize(124,79));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Frequency modulation"),WS_VISIBLE,r16,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       PUSHBUTTON      "Sine",IDC_PRESET_SINE,14,101,41,12
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(14,101),CSize(41,12));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Sine"),WS_VISIBLE,r2,parent,IDC_PRESET_SINE);
   mfcToQtWidget->insert(IDC_PRESET_SINE,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),parent,SLOT(presetSine_clicked()));
//       PUSHBUTTON      "Triangle",IDC_PRESET_TRIANGLE,57,101,41,12
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(57,101),CSize(41,12));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Triangle"),WS_VISIBLE,r3,parent,IDC_PRESET_TRIANGLE);
   mfcToQtWidget->insert(IDC_PRESET_TRIANGLE,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),parent,SLOT(presetTriangle_clicked()));
//       PUSHBUTTON      "Sawtooth",IDC_PRESET_SAWTOOTH,100,101,41,12
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(100,101),CSize(41,12));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Sawtooth"),WS_VISIBLE,r4,parent,IDC_PRESET_SAWTOOTH);
   mfcToQtWidget->insert(IDC_PRESET_SAWTOOTH,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),parent,SLOT(presetSawtooth_clicked()));
//       LTEXT           "Modulation rate",IDC_STATIC,246,24,51,8
   CStatic* mfc5 = new CStatic(parent);
   CRect r5(CPoint(246,24),CSize(51,8));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Modulation rate"),WS_VISIBLE,r5,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_RATE,318,22,37,14,ES_AUTOHSCROLL | ES_NUMBER
//       CONTROL         "",IDC_MOD_RATE_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_NOTHOUSANDS,344,25,11,14
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc6 = new CEdit(parent);
   CSpinButtonCtrl* mfc7 = new CSpinButtonCtrl(parent);
   CRect r6(CPoint(318,22),CSize(37,14));
   CRect r7(CPoint(r6.right-11,r6.top),CSize(11,14));
   parent->MapDialogRect(&r6);
   parent->MapDialogRect(&r7);
   mfc6->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r6,parent,IDC_MOD_RATE);
   mfc6->setBuddy(mfc7);
   mfcToQtWidget->insert(IDC_MOD_RATE,mfc6);
   QObject::connect(mfc6,SIGNAL(textChanged(QString)),parent,SLOT(modRate_textChanged(QString)));
   mfc7->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_NOTHOUSANDS | WS_VISIBLE,r7,parent,IDC_MOD_RATE_SPIN);
   mfc7->setBuddy(mfc6);
   mfcToQtWidget->insert(IDC_MOD_RATE_SPIN,mfc7);
   QObject::connect(mfc7,SIGNAL(valueChanged(int)),parent,SLOT(modRateSpin_valueChanged(int)));
//       LTEXT           "Modulation depth",IDC_STATIC,246,46,56,8
   CStatic* mfc8 = new CStatic(parent);
   CRect r8(CPoint(246,46),CSize(56,8));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Modulation depth"),WS_VISIBLE,r8,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_DEPTH,318,43,37,14,ES_AUTOHSCROLL | ES_NUMBER
//       CONTROL         "",IDC_MOD_DEPTH_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,344,43,11,14
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc9 = new CEdit(parent);
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(parent);
   CRect r9(CPoint(318,43),CSize(37,14));
   CRect r10(CPoint(r9.right-11,r9.top),CSize(11,14));
   parent->MapDialogRect(&r9);
   parent->MapDialogRect(&r10);
   mfc9->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r9,parent,IDC_MOD_DEPTH);
   mfc9->setBuddy(mfc10);
   mfcToQtWidget->insert(IDC_MOD_DEPTH,mfc9);
   QObject::connect(mfc9,SIGNAL(textChanged(QString)),parent,SLOT(modDepth_textChanged(QString)));
   mfc10->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,parent,IDC_MOD_DEPTH_SPIN);
   mfc10->setBuddy(mfc9);
   mfcToQtWidget->insert(IDC_MOD_DEPTH_SPIN,mfc10);
   QObject::connect(mfc10,SIGNAL(valueChanged(int)),parent,SLOT(modDepthSpin_valueChanged(int)));
//       LTEXT           "Modulation delay",IDC_STATIC,246,68,55,8
   CStatic* mfc11 = new CStatic(parent);
   CRect r11(CPoint(246,68),CSize(55,8));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Modulation delay"),WS_VISIBLE,r11,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_DELAY,318,65,37,14,ES_AUTOHSCROLL | ES_NUMBER
//       CONTROL         "",IDC_MOD_DELAY_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,344,63,11,14
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc12 = new CEdit(parent);
   CSpinButtonCtrl* mfc13 = new CSpinButtonCtrl(parent);
   CRect r12(CPoint(318,65),CSize(37,14));
   CRect r13(CPoint(r12.right-11,r12.top),CSize(11,14));
   parent->MapDialogRect(&r12);
   parent->MapDialogRect(&r13);
   mfc12->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r12,parent,IDC_MOD_DELAY);
   mfc12->setBuddy(mfc13);
   mfcToQtWidget->insert(IDC_MOD_DELAY,mfc12);
   QObject::connect(mfc12,SIGNAL(textChanged(QString)),parent,SLOT(modDelay_textChanged(QString)));
   mfc13->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r13,parent,IDC_MOD_DELAY_SPIN);
   mfc13->setBuddy(mfc12);
   mfcToQtWidget->insert(IDC_MOD_DELAY_SPIN,mfc13);
   QObject::connect(mfc13,SIGNAL(valueChanged(int)),parent,SLOT(modDelaySpin_valueChanged(int)));
//       PUSHBUTTON      "Flat",IDC_MOD_PRESET_FLAT,318,131,46,12
   CButton* mfc14 = new CButton(parent);
   CRect r14(CPoint(318,131),CSize(46,12));
   parent->MapDialogRect(&r14);
   mfc14->Create(_T("Flat"),WS_VISIBLE,r14,parent,IDC_MOD_PRESET_FLAT);
   mfcToQtWidget->insert(IDC_MOD_PRESET_FLAT,mfc14);
   QObject::connect(mfc14,SIGNAL(clicked()),parent,SLOT(modPresetFlat_clicked()));
//       PUSHBUTTON      "Sine",IDC_MOD_PRESET_SINE,318,148,46,12
   CButton* mfc15 = new CButton(parent);
   CRect r15(CPoint(318,148),CSize(46,12));
   parent->MapDialogRect(&r15);
   mfc15->Create(_T("Sine"),WS_VISIBLE,r15,parent,IDC_MOD_PRESET_SINE);
   mfcToQtWidget->insert(IDC_MOD_PRESET_SINE,mfc15);
   QObject::connect(mfc15,SIGNAL(clicked()),parent,SLOT(modPresetSine_clicked()));
//       PUSHBUTTON      "Copy wave",IDC_COPY_WAVE,240,90,52,14
   CButton* mfc17 = new CButton(parent);
   CRect r17(CPoint(240,90),CSize(52,14));
   parent->MapDialogRect(&r17);
   mfc17->Create(_T("Copy wave"),WS_VISIBLE,r17,parent,IDC_COPY_WAVE);
   mfcToQtWidget->insert(IDC_COPY_WAVE,mfc17);
   QObject::connect(mfc17,SIGNAL(clicked()),parent,SLOT(copyWave_clicked()));
//       PUSHBUTTON      "Paste wave",IDC_PASTE_WAVE,240,104,52,14
   CButton* mfc18 = new CButton(parent);
   CRect r18(CPoint(240,104),CSize(52,14));
   parent->MapDialogRect(&r18);
   mfc18->Create(_T("Paste wave"),WS_VISIBLE,r18,parent,IDC_PASTE_WAVE);
   mfcToQtWidget->insert(IDC_PASTE_WAVE,mfc18);
   QObject::connect(mfc18,SIGNAL(clicked()),parent,SLOT(pasteWave_clicked()));
//       PUSHBUTTON      "Copy table",IDC_COPY_TABLE,305,90,50,14
   CButton* mfc19 = new CButton(parent);
   CRect r19(CPoint(305,90),CSize(50,14));
   parent->MapDialogRect(&r19);
   mfc19->Create(_T("Copy table"),WS_VISIBLE,r19,parent,IDC_COPY_TABLE);
   mfcToQtWidget->insert(IDC_COPY_TABLE,mfc19);
   QObject::connect(mfc19,SIGNAL(clicked()),parent,SLOT(copyTable_clicked()));
//       PUSHBUTTON      "Paste table",IDC_PASTE_TABLE,305,104,50,14
   CButton* mfc20 = new CButton(parent);
   CRect r20(CPoint(305,104),CSize(50,14));
   parent->MapDialogRect(&r20);
   mfc20->Create(_T("Paste table"),WS_VISIBLE,r20,parent,IDC_PASTE_TABLE);
   mfcToQtWidget->insert(IDC_PASTE_TABLE,mfc20);
   QObject::connect(mfc20,SIGNAL(clicked()),parent,SLOT(pasteTable_clicked()));
//       PUSHBUTTON      "50% pulse",IDC_PRESET_PULSE_50,143,101,41,12
   CButton* mfc21 = new CButton(parent);
   CRect r21(CPoint(143,101),CSize(41,12));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("50% pulse"),WS_VISIBLE,r21,parent,IDC_PRESET_PULSE_50);
   mfcToQtWidget->insert(IDC_PRESET_PULSE_50,mfc21);
   QObject::connect(mfc21,SIGNAL(clicked()),parent,SLOT(presetPulse50_clicked()));
//       PUSHBUTTON      "25% pulse",IDC_PRESET_PULSE_25,186,101,41,12
   CButton* mfc22 = new CButton(parent);
   CRect r22(CPoint(186,101),CSize(41,12));
   parent->MapDialogRect(&r22);
   mfc22->Create(_T("25% pulse"),WS_VISIBLE,r22,parent,IDC_PRESET_PULSE_25);
   mfcToQtWidget->insert(IDC_PRESET_PULSE_25,mfc22);
   QObject::connect(mfc22,SIGNAL(clicked()),parent,SLOT(presetPulse25_clicked()));
//   END
}   

void qtMfcInitDialogResource_IDD_FRAMECONTROLS(UINT dlgID,CDialog* parent1)
{
   CControlPanelDlg* parent = dynamic_cast<CControlPanelDlg*>(parent1);
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
   QObject::connect(mfc1,SIGNAL(clicked()),parent,SLOT(frameInc_clicked()));
//       PUSHBUTTON      "-",IDC_FRAME_DEC,23,0,17,12
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(23,0),CSize(17,12));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("-"),WS_VISIBLE,r2,parent,IDC_FRAME_DEC);
   mfcToQtWidget->insert(IDC_FRAME_DEC,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),parent,SLOT(frameDec_clicked()));
//       CONTROL         "Change all",IDC_CHANGE_ALL,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,51,2,56,10
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(51,2),CSize(56,10));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Change all"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_CHANGE_ALL);
   mfcToQtWidget->insert(IDC_CHANGE_ALL,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),parent,SLOT(changeAll_clicked()));
//   END
}   

#include "SampleEditorDlg.h"
void qtMfcInitDialogResource_IDD_SAMPLE_EDITOR(UINT dlgID,CDialog* parent1)
{
   CSampleEditorDlg* parent = dynamic_cast<CSampleEditorDlg*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_SAMPLE_EDITOR DIALOGEX 0, 0, 481, 255
   CRect rect(CPoint(0,0),CSize(481,255));
   parent->MapDialogRect(&rect);
   parent->setBaseSize(rect.Width(),rect.Height());
   parent->setSizeIncrement(1,1);
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_MAXIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
//   CAPTION "DPCM sample editor"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//   DEFPUSHBUTTON   "OK",IDOK,372,234,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(372,234),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),parent,SLOT(ok_clicked()));
//   PUSHBUTTON      "Cancel",IDCANCEL,424,234,50,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(424,234),CSize(50,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),parent,SLOT(cancel_clicked()));
//   CONTROL         "",IDC_SAMPLE,"Static",SS_WHITERECT | SS_NOTIFY | SS_SUNKEN,7,7,467,204
   CStatic *mfc3 = new CStatic(parent);
   CRect r3(CPoint(7,7),CSize(467,204));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T(""),SS_WHITERECT | SS_NOTIFY | SS_SUNKEN | WS_VISIBLE,r3,parent,IDC_SAMPLE);
   mfcToQtWidget->insert(IDC_SAMPLE,mfc3);
//   PUSHBUTTON      "Play",IDC_PLAY,67,234,50,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(67,234),CSize(50,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Play"),WS_VISIBLE,r4,parent,IDC_PLAY);
   mfcToQtWidget->insert(IDC_PLAY,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),parent,SLOT(play_clicked()));
//   LTEXT           "Offset: 0, Pos: 0",IDC_POS,7,217,79,11,SS_SUNKEN
   CStatic* mfc5 = new CStatic(parent);
   CRect r5(CPoint(7,217),CSize(79,11));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("Offset: 0, Pos: 0"),SS_SUNKEN | WS_VISIBLE,r5,parent,IDC_POS);
   mfcToQtWidget->insert(IDC_POS,mfc5);
//   PUSHBUTTON      "Delete",IDC_DELETE,239,234,50,14
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(239,234),CSize(50,14));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Delete"),WS_VISIBLE,r6,parent,IDC_DELETE);
   mfcToQtWidget->insert(IDC_DELETE,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),parent,SLOT(delete_clicked()));
//   CONTROL         "",IDC_PITCH,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,143,236,79,12
   CSliderCtrl* mfc7 = new CSliderCtrl(parent);
   CRect r7(CPoint(143,236),CSize(79,12));
   parent->MapDialogRect(&r7);
   mfc7->setGeometry(r7);
   mfcToQtWidget->insert(IDC_PITCH,mfc7);
   QObject::connect(mfc7,SIGNAL(valueChanged(int)),parent,SLOT(pitch_valueChanged(int)));
//   PUSHBUTTON      "Tilt",IDC_TILT,292,234,50,14
   CButton* mfc8 = new CButton(parent);
   CRect r8(CPoint(292,234),CSize(50,14));
   parent->MapDialogRect(&r8);
   mfc8->Create(_T("Tilt"),WS_VISIBLE,r8,parent,IDC_TILT);
   mfcToQtWidget->insert(IDC_TILT,mfc8);
   QObject::connect(mfc8,SIGNAL(clicked()),parent,SLOT(tilt_clicked()));
//   LTEXT           "0 bytes",IDC_INFO,88,217,87,11,SS_SUNKEN
   CStatic* mfc9 = new CStatic(parent);
   CRect r9(CPoint(88,217),CSize(87,11));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("0 bytes"),SS_SUNKEN | WS_VISIBLE,r9,parent,IDC_INFO);
   mfcToQtWidget->insert(IDC_INFO,mfc9);
//   LTEXT           "Pitch",IDC_STATIC_PITCH,123,237,19,11
   CStatic* mfc10 = new CStatic(parent);
   CRect r10(CPoint(123,237),CSize(19,11));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Pitch"),WS_VISIBLE,r10,parent,IDC_STATIC_PITCH);
   mfcToQtWidget->insert(IDC_STATIC_PITCH,mfc10);
//   CONTROL         "Start from 64",IDC_DELTASTART,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,7,238,57,10
   CButton* mfc11 = new CButton(parent);
   CRect r11(CPoint(7,238),CSize(57,10));
   parent->MapDialogRect(&r11);
   mfc11->Create(_T("Start from 64"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r11,parent,IDC_DELTASTART);
   mfcToQtWidget->insert(IDC_DELTASTART,mfc11);
   QObject::connect(mfc11,SIGNAL(clicked()),parent,SLOT(deltaStart_clicked()));
//   END
}   

#include "InstrumentEditorFDSEnvelope.h"
void qtMfcInitDialogResource_IDD_INSTRUMENT_FDS_ENVELOPE(UINT dlgID,CDialog* parent1)
{
   CInstrumentEditorFDSEnvelope* parent = dynamic_cast<CInstrumentEditorFDSEnvelope*>(parent1);
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
//       IDC_TYPE, 0x403, 9, 0
//   0x7241, 0x6570, 0x6767, 0x6f69, "\000" 
//       IDC_TYPE, 0x403, 6, 0
//   0x6950, 0x6374, 0x0068, 
//       0
//   END
   mfc2->AddString(_T("Volume"));
   mfc2->AddString(_T("Arpeggio"));
   mfc2->AddString(_T("Pitch"));
   QObject::connect(mfc2,SIGNAL(currentIndexChanged(int)),parent,SLOT(type_currentIndexChanged(int)));
//   END
}   

#include "ChannelsDlg.h"
void qtMfcInitDialogResource_IDD_CHANNELS(UINT dlgID,CDialog* parent1)
{
   CChannelsDlg* parent = dynamic_cast<CChannelsDlg*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_CHANNELS DIALOGEX 0, 0, 303, 236
   CRect rect(CPoint(0,0),CSize(303,236));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Channels"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Available channels",IDC_STATIC,7,7,138,197
   CGroupBox* mfc6 = new CGroupBox(parent);
   CRect r6(CPoint(7,7),CSize(138,197));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Available channels"),WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Added channels",IDC_STATIC,151,7,145,197
   CGroupBox* mfc7 = new CGroupBox(parent);
   CRect r7(CPoint(151,7),CSize(145,197));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("Added channels"),WS_VISIBLE,r7,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "OK",IDOK,194,215,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(194,215),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),parent,SLOT(ok_clicked()));
//       PUSHBUTTON      "Cancel",IDCANCEL,246,215,50,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(246,215),CSize(50,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),parent,SLOT(cancel_clicked()));
//       PUSHBUTTON      "Move up",IDC_MOVE_UP,7,215,50,14
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(7,215),CSize(50,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Move up"),WS_VISIBLE,r3,parent,IDC_MOVE_UP);
   mfcToQtWidget->insert(IDC_MOVE_UP,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),parent,SLOT(moveUp_clicked()));
//       PUSHBUTTON      "Move down",IDC_MOVE_DOWN,62,215,50,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(62,215),CSize(50,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Move down"),WS_VISIBLE,r4,parent,IDC_MOVE_DOWN);
   mfcToQtWidget->insert(IDC_MOVE_DOWN,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),parent,SLOT(moveDown_clicked()));
//       CONTROL         "",IDC_ADDED_LIST,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP,159,17,129,181
   CListCtrl* mfc5 = new CListCtrl(parent);
   CRect r5(CPoint(159,17),CSize(129,181));
   parent->MapDialogRect(&r5);
   mfc5->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r5,parent,IDC_ADDED_LIST);
   mfcToQtWidget->insert(IDC_ADDED_LIST,mfc5);
   QObject::connect(mfc5,SIGNAL(cellDoubleClicked(int,int)),parent,SLOT(addedList_cellDoubleClicked(int,int)));
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,209,289,1
   qDebug("Static horz line...");
//       CONTROL         "",IDC_AVAILABLE_TREE,"SysTreeView32",TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | WS_BORDER | WS_HSCROLL | WS_TABSTOP,14,17,122,181
   CTreeCtrl* mfc8 = new CTreeCtrl(parent);
   CRect r8(CPoint(14,17),CSize(122,181));
   parent->MapDialogRect(&r8);
   mfc8->Create(TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | WS_BORDER | WS_HSCROLL | WS_TABSTOP | WS_VISIBLE,r8,parent,IDC_AVAILABLE_TREE);
   mfcToQtWidget->insert(IDC_AVAILABLE_TREE,mfc8);
   QObject::connect(mfc8,SIGNAL(itemClicked(QTreeWidgetItem*,int)),parent,SLOT(availableTree_itemClicked(QTreeWidgetItem*,int)));
   QObject::connect(mfc8,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),parent,SLOT(availableTree_itemDoubleClicked(QTreeWidgetItem*,int)));
//   END
}

#include "CommentsDlg.h"
void qtMfcInitDialogResource_IDD_COMMENTS(UINT dlgID,CDialog* parent1)
{
   CCommentsDlg* parent = dynamic_cast<CCommentsDlg*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_COMMENTS DIALOGEX 0, 0, 358, 230
   CRect rect(CPoint(0,0),CSize(358,230));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
//   CAPTION "Module message"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       DEFPUSHBUTTON   "OK",IDC_OK,251,211,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(251,211),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDC_OK);
   mfcToQtWidget->insert(IDC_OK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),parent,SLOT(ok_clicked()));
//       EDITTEXT        IDC_COMMENTS,0,0,358,206,ES_MULTILINE | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL
   CEdit* mfc2 = new CEdit(parent);
   CRect r2(CPoint(0,0),CSize(358,206));
   parent->MapDialogRect(&r2);
   mfc2->Create(ES_MULTILINE | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_VISIBLE,r2,parent,IDC_COMMENTS);
   mfcToQtWidget->insert(IDC_COMMENTS,mfc2);
//       CONTROL         "Show when loading file",IDC_SHOWONOPEN,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,5,215,89,10
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(5,215),CSize(89,10));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Show when loading file"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r3,parent,IDC_SHOWONOPEN);
   mfcToQtWidget->insert(IDC_SHOWONOPEN,mfc3);
//       PUSHBUTTON      "Cancel",IDC_CANCEL,303,211,50,14
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(303,211),CSize(50,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Cancel"),WS_VISIBLE,r4,parent,IDC_CANCEL);
   mfcToQtWidget->insert(IDC_CANCEL,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),parent,SLOT(cancel_clicked()));
//   END
}   

//#include "dialog-file"
void qtMfcInitDialogResource_IDD_CONFIG_LEVELS(UINT dlgID,CDialog* parent)
{
//  dialog-type* parent = dynamic_cast<dialog-type*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_CONFIG_LEVELS DIALOGEX 0, 0, 280, 167
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   EXSTYLE WS_EX_CONTEXTHELP
//   CAPTION "Sound levels"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "2A03",IDC_STATIC,7,7,47,131
//       CONTROL         "",IDC_VOL_2A03,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,17,16,26,110
//       CTEXT           "0 dB",IDC_LEVEL_2A03,19,126,23,8
//       CONTROL         "Enable normalization",IDC_CHECK1,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,7,151,81,10
//       GROUPBOX        "VRC6",IDC_STATIC,63,7,47,131
//       CONTROL         "",IDC_VOL_VRC6,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,73,16,26,110
//       CTEXT           "0 dB",IDC_LEVEL_VRC6,75,126,23,8
//       GROUPBOX        "VRC7",IDC_STATIC,117,7,47,131
//       CONTROL         "",IDC_VOL_VRC7,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,127,16,26,110
//       CTEXT           "0 dB",IDC_LEVEL_VRC7,130,126,21,8
//       GROUPBOX        "MMC5",IDC_STATIC,171,7,47,131
//       CONTROL         "",IDC_VOL_MMC5,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,181,16,26,110
//       CTEXT           "0 dB",IDC_LEVEL_MMC5,182,126,24,8
//       GROUPBOX        "FDS",IDC_STATIC,226,7,47,131
//       CONTROL         "",IDC_VOL_FDS,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,236,16,26,110
//       CTEXT           "0 dB",IDC_LEVEL_FDS,238,126,22,8
//       PUSHBUTTON      "Default",IDC_DEFAULT,228,147,45,14
//   END
}   

#include "InstrumentEditorN163Wave.h"
void qtMfcInitDialogResource_IDD_INSTRUMENT_N163_WAVE(UINT dlgID,CDialog* parent1)
{
   CInstrumentEditorN163Wave* parent = dynamic_cast<CInstrumentEditorN163Wave*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_INSTRUMENT_N163_WAVE DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());   
//   STYLE DS_SETFONT | DS_FIXEDSYS | WS_CHILD | WS_SYSMENU
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Wave editor",IDC_STATIC,7,7,228,116
   CGroupBox* mfc1 = new CGroupBox(parent);
   CRect r1(CPoint(7,7),CSize(228,116));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("Wave editor"),WS_VISIBLE,r1,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Wave #",IDC_STATIC,244,7,121,35
   CGroupBox* mfc21 = new CGroupBox(parent);
   CRect r21(CPoint(244,7),CSize(121,35));
   parent->MapDialogRect(&r21);
   mfc21->Create(_T("Wave #"),WS_VISIBLE,r21,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "MML string",IDC_STATIC,7,132,358,33
   CGroupBox* mfc7 = new CGroupBox(parent);
   CRect r7(CPoint(7,132),CSize(358,33));
   parent->MapDialogRect(&r7);
   mfc7->Create(_T("MML string"),WS_VISIBLE,r7,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Wave RAM settings",IDC_STATIC,244,45,121,52
   CGroupBox* mfc16 = new CGroupBox(parent);
   CRect r16(CPoint(244,45),CSize(121,52));
   parent->MapDialogRect(&r16);
   mfc16->Create(_T("Wave RAM settings"),WS_VISIBLE,r16,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       PUSHBUTTON      "Sine",IDC_PRESET_SINE,14,101,41,12
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(14,104),CSize(41,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Sine"),WS_VISIBLE,r2,parent,IDC_PRESET_SINE);
   mfcToQtWidget->insert(IDC_PRESET_SINE,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),parent,SLOT(presetSine_clicked()));
//       PUSHBUTTON      "Triangle",IDC_PRESET_TRIANGLE,57,101,41,12
   CButton* mfc3 = new CButton(parent);
   CRect r3(CPoint(57,104),CSize(41,14));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Triangle"),WS_VISIBLE,r3,parent,IDC_PRESET_TRIANGLE);
   mfcToQtWidget->insert(IDC_PRESET_TRIANGLE,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),parent,SLOT(presetTriangle_clicked()));
//       PUSHBUTTON      "Sawtooth",IDC_PRESET_SAWTOOTH,100,101,41,12
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(100,104),CSize(41,14));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Sawtooth"),WS_VISIBLE,r4,parent,IDC_PRESET_SAWTOOTH);
   mfcToQtWidget->insert(IDC_PRESET_SAWTOOTH,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),parent,SLOT(presetSawtooth_clicked()));
//       PUSHBUTTON      "50% pulse",IDC_PRESET_PULSE_50,143,101,41,12
   CButton* mfc5 = new CButton(parent);
   CRect r5(CPoint(143,104),CSize(41,14));
   parent->MapDialogRect(&r5);
   mfc5->Create(_T("50% pulse"),WS_VISIBLE,r5,parent,IDC_PRESET_PULSE_50);
   mfcToQtWidget->insert(IDC_PRESET_PULSE_50,mfc5);
   QObject::connect(mfc5,SIGNAL(clicked()),parent,SLOT(presetPulse50_clicked()));
//       PUSHBUTTON      "25% pulse",IDC_PRESET_PULSE_25,186,101,41,12
   CButton* mfc6 = new CButton(parent);
   CRect r6(CPoint(186,104),CSize(41,14));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("25% pulse"),WS_VISIBLE,r6,parent,IDC_PRESET_PULSE_25);
   mfcToQtWidget->insert(IDC_PRESET_PULSE_25,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),parent,SLOT(presetPulse25_clicked()));
//       EDITTEXT        IDC_MML,16,145,342,14,ES_AUTOHSCROLL
   CEdit* mfc8 = new CEdit(parent);
   CRect r8(CPoint(16,145),CSize(342,14));
   parent->MapDialogRect(&r8);
   mfc8->Create(ES_AUTOHSCROLL | WS_VISIBLE,r8,parent,IDC_MML);
   mfcToQtWidget->insert(IDC_MML,mfc8);
//       PUSHBUTTON      "Copy wave",IDC_COPY_WAVE,240,90,52,14
   CButton* mfc9 = new CButton(parent);
   CRect r9(CPoint(249,105),CSize(50,14));
   parent->MapDialogRect(&r9);
   mfc9->Create(_T("Copy wave"),WS_VISIBLE,r9,parent,IDC_COPY_WAVE);
   mfcToQtWidget->insert(IDC_COPY,mfc9);
   QObject::connect(mfc9,SIGNAL(clicked()),parent,SLOT(copy_clicked()));
//       PUSHBUTTON      "Paste wave",IDC_PASTE_WAVE,240,104,52,14
   CButton* mfc10 = new CButton(parent);
   CRect r10(CPoint(308,105),CSize(50,14));
   parent->MapDialogRect(&r10);
   mfc10->Create(_T("Paste wave"),WS_VISIBLE,r10,parent,IDC_PASTE_WAVE);
   mfcToQtWidget->insert(IDC_PASTE,mfc10);
   QObject::connect(mfc10,SIGNAL(clicked()),parent,SLOT(paste_clicked()));
//       EDITTEXT        IDC_INDEX,253,20,39,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_INDEX_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,281,26,11,14
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc11 = new CEdit(parent);
   CSpinButtonCtrl* mfc12 = new CSpinButtonCtrl(parent);
   CRect r11(CPoint(253,20),CSize(39,12));
   CRect r12(CPoint(r11.right-11,r11.top),CSize(11,12));
   parent->MapDialogRect(&r11);
   parent->MapDialogRect(&r12);
   mfc11->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r11,parent,IDC_INDEX);
   mfc11->setBuddy(mfc12);
   mfcToQtWidget->insert(IDC_INDEX,mfc11);
   QObject::connect(mfc11,SIGNAL(textChanged(QString)),parent,SLOT(index_textChanged(QString)));
   mfc12->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r12,parent,IDC_INDEX_SPIN);
   mfc12->setBuddy(mfc11);
   mfcToQtWidget->insert(IDC_INDEX_SPIN,mfc12);
   QObject::connect(mfc12,SIGNAL(valueChanged(int)),parent,SLOT(indexSpin_valueChanged(int)));
//       LTEXT           "of",IDC_STATIC,300,22,8,8
   CStatic* mfc13 = new CStatic(parent);
   CRect r13(CPoint(300,22),CSize(8,8));
   parent->MapDialogRect(&r13);
   mfc13->Create(_T("of"),WS_VISIBLE,r13,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_WAVES,316,20,37,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_WAVES_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,342,19,11,14
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc14 = new CEdit(parent);
   CSpinButtonCtrl* mfc15 = new CSpinButtonCtrl(parent);
   CRect r14(CPoint(316,20),CSize(37,12));
   CRect r15(CPoint(r14.right-11,r14.top),CSize(11,12));
   parent->MapDialogRect(&r14);
   parent->MapDialogRect(&r15);
   mfc14->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r14,parent,IDC_WAVES);
   mfc14->setBuddy(mfc15);
   mfcToQtWidget->insert(IDC_WAVES,mfc14);
   QObject::connect(mfc14,SIGNAL(textChanged(QString)),parent,SLOT(waves_textChanged(QString)));
   mfc15->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r15,parent,IDC_WAVES_SPIN);
   mfc15->setBuddy(mfc14);
   mfcToQtWidget->insert(IDC_WAVES_SPIN,mfc15);
   QObject::connect(mfc15,SIGNAL(valueChanged(int)),parent,SLOT(wavesSpin_valueChanged(int)));
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
   QObject::connect(mfc18,SIGNAL(currentIndexChanged(int)),parent,SLOT(waveSize_currentIndexChanged(int)));
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
   QObject::connect(mfc20,SIGNAL(currentIndexChanged(int)),parent,SLOT(wavePos_currentIndexChanged(int)));
//   END
}

#include "ModuleImportDlg.h"
void qtMfcInitDialogResource_IDD_IMPORT(UINT dlgID,CDialog* parent1)
{
   CModuleImportDlg* parent = dynamic_cast<CModuleImportDlg*>(parent1);
   QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
   
//   IDD_IMPORT DIALOGEX 0, 0, 161, 209
   CRect rect(CPoint(0,0),CSize(161,209));
   parent->MapDialogRect(&rect);
   parent->setFixedSize(rect.Width(),rect.Height());
//   STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
//   CAPTION "Import options"
//   FONT 8, "MS Shell Dlg", 400, 0, 0x1
//   BEGIN
//       GROUPBOX        "Tracks",IDC_STATIC,7,7,147,137
   CGroupBox* mfc3 = new CGroupBox(parent);
   CRect r3(CPoint(7,7),CSize(147,137));
   parent->MapDialogRect(&r3);
   mfc3->Create(_T("Tracks"),WS_VISIBLE,r3,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Options",IDC_STATIC,7,148,147,29
   CGroupBox* mfc6 = new CGroupBox(parent);
   CRect r6(CPoint(7,148),CSize(147,29));
   parent->MapDialogRect(&r6);
   mfc6->Create(_T("Options"),WS_VISIBLE,r6,parent,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "OK",IDOK,51,188,50,14
   CButton* mfc1 = new CButton(parent);
   CRect r1(CPoint(51,188),CSize(50,14));
   parent->MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,parent,IDOK);
   mfcToQtWidget->insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),parent,SLOT(ok_clicked()));
//       PUSHBUTTON      "Cancel",IDCANCEL,104,188,50,14
   CButton* mfc2 = new CButton(parent);
   CRect r2(CPoint(104,188),CSize(50,14));
   parent->MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,parent,IDCANCEL);
   mfcToQtWidget->insert(IDCANCEL,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),parent,SLOT(cancel_clicked()));
//       CONTROL         "Include instruments",IDC_INSTRUMENTS,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,161,116,10
   CButton* mfc4 = new CButton(parent);
   CRect r4(CPoint(14,161),CSize(116,10));
   parent->MapDialogRect(&r4);
   mfc4->Create(_T("Include instruments"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r4,parent,IDC_INSTRUMENTS);
   mfcToQtWidget->insert(IDC_INSTRUMENTS,mfc4);
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,183,147,1
   qDebug("horzline not implemented");
//       CONTROL         "",IDC_TRACKS,"SysListView32",LVS_REPORT | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP,14,18,133,120
   CListCtrl* mfc7 = new CListCtrl(parent);
   CRect r7(CPoint(14,18),CSize(133,120));
   parent->MapDialogRect(&r7);
   mfc7->Create(LVS_REPORT | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r7,parent,IDC_TRACKS);
   mfcToQtWidget->insert(IDC_TRACKS,mfc7);
//   END
}

void qtMfcInitDialogResource(UINT dlgID,CDialog* parent)
{
   switch ( dlgID )
   {
   case IDD_PERFORMANCE:
      qtMfcInitDialogResource_IDD_PERFORMANCE(dlgID,parent);
      break;
   case IDD_SPEED:
      qtMfcInitDialogResource_IDD_SPEED(dlgID,parent);
      break;
   case IDD_PCMIMPORT:
      qtMfcInitDialogResource_IDD_PCMIMPORT(dlgID,parent);
      break;
   case IDD_INSTRUMENT_INTERNAL:
      qtMfcInitDialogResource_IDD_INSTRUMENT_INTERNAL(dlgID,parent);
      break;
   case IDD_INSTRUMENT_DPCM:
      qtMfcInitDialogResource_IDD_INSTRUMENT_DPCM(dlgID,parent);
      break;
   case IDD_INSTRUMENT:
      qtMfcInitDialogResource_IDD_INSTRUMENT(dlgID,parent);
      break;
   case IDD_CONFIG_APPEARANCE:
      qtMfcInitDialogResource_IDD_CONFIG_APPEARANCE(dlgID,parent);
      break;
   case IDD_MIDIIMPORT:
      qtMfcInitDialogResource_IDD_MIDIIMPORT(dlgID,parent);
      break;
   case IDD_CONFIG_GENERAL:
      qtMfcInitDialogResource_IDD_CONFIG_GENERAL(dlgID,parent);
      break;
   case IDD_PROPERTIES:
      qtMfcInitDialogResource_IDD_PROPERTIES(dlgID,parent);
      break;
   case IDD_CONFIG_MIDI:
      qtMfcInitDialogResource_IDD_CONFIG_MIDI(dlgID,parent);
      break;
   case IDD_CONFIG_SOUND:
      qtMfcInitDialogResource_IDD_CONFIG_SOUND(dlgID,parent);
      break;
   case IDD_CONFIG_SHORTCUTS:
      qtMfcInitDialogResource_IDD_CONFIG_SHORTCUTS(dlgID,parent);
      break;
   case IDD_EXPORT:
      qtMfcInitDialogResource_IDD_EXPORT(dlgID,parent);
      break;
   case IDD_INSTRUMENT_VRC7:
      qtMfcInitDialogResource_IDD_INSTRUMENT_VRC7(dlgID,parent);
      break;
   case IDD_CREATEWAV:
      qtMfcInitDialogResource_IDD_CREATEWAV(dlgID,parent);
      break;
   case IDD_WAVE_PROGRESS:
      qtMfcInitDialogResource_IDD_WAVE_PROGRESS(dlgID,parent);
      break;
   case IDD_INSTRUMENT_FDS:
      qtMfcInitDialogResource_IDD_INSTRUMENT_FDS(dlgID,parent);
      break;
   case IDD_SAMPLE_EDITOR:
      qtMfcInitDialogResource_IDD_SAMPLE_EDITOR(dlgID,parent);
      break;
   case IDD_INSTRUMENT_FDS_ENVELOPE:
      qtMfcInitDialogResource_IDD_INSTRUMENT_FDS_ENVELOPE(dlgID,parent);
      break;
   case IDD_CHANNELS:
      qtMfcInitDialogResource_IDD_CHANNELS(dlgID,parent);
      break;
   case IDD_COMMENTS:
      qtMfcInitDialogResource_IDD_COMMENTS(dlgID,parent);
      break;
   case IDD_CONFIG_LEVELS:
      qtMfcInitDialogResource_IDD_CONFIG_LEVELS(dlgID,parent);
      break;
   case IDD_INSTRUMENT_N163_WAVE:
      qtMfcInitDialogResource_IDD_INSTRUMENT_N163_WAVE(dlgID,parent);
      break;
   case IDD_IMPORT:
      qtMfcInitDialogResource_IDD_IMPORT(dlgID,parent);
      break;
   case IDD_FRAMECONTROLS:
      qtMfcInitDialogResource_IDD_FRAMECONTROLS(dlgID,parent);
      break;
   case IDD_MAINFRAME:
      qtMfcInitDialogResource_IDD_MAINFRAME(dlgID,parent);
      break;
   case IDD_OCTAVE:
      qtMfcInitDialogResource_IDD_OCTAVE(dlgID,parent);
      break;
   case IDD_MAINBAR:
      qtMfcInitDialogResource_IDD_MAINBAR(dlgID,parent);
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
   QHash<int,QAction*>* mfcToQtAction = parent->mfcToQtActionMap();

   QImage toolBarImage(":/resources/Toolbar-d5.bmp");
   QToolBar* toolBar = dynamic_cast<QToolBar*>(parent->toQWidget());
   QPixmap toolBarActionPixmap;
   QAction* toolBarAction;

//IDR_MAINFRAME TOOLBAR 16, 15
//BEGIN
//    BUTTON      ID_FILE_NEW
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(0,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_FILE_NEW,toolBarAction);
//    BUTTON      ID_FILE_OPEN
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(16,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_FILE_OPEN,toolBarAction);
//    BUTTON      ID_FILE_SAVE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(32,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_FILE_SAVE,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_EDIT_CUT
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(48,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_EDIT_CUT,toolBarAction);
//    BUTTON      ID_EDIT_COPY
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(64,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_EDIT_COPY,toolBarAction);
//    BUTTON      ID_EDIT_PASTE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(80,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_EDIT_PASTE,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_APP_ABOUT
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(96,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_APP_ABOUT,toolBarAction);
//    BUTTON      ID_CONTEXT_HELP
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(112,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_CONTEXT_HELP,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_MODULE_INSERTFRAME
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(128,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_MODULE_INSERTFRAME,toolBarAction);
//    BUTTON      ID_MODULE_REMOVEFRAME
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(144,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_MODULE_REMOVEFRAME,toolBarAction);
//    BUTTON      ID_MODULE_MOVEFRAMEDOWN
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(160,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_MODULE_MOVEFRAMEDOWN,toolBarAction);
//    BUTTON      ID_MODULE_MOVEFRAMEUP
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(176,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_MODULE_MOVEFRAMEUP,toolBarAction);
//    BUTTON      ID_MODULE_DUPLICATEFRAME
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(192,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_MODULE_DUPLICATEFRAME,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_MODULE_MODULEPROPERTIES
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(208,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_MODULE_MODULEPROPERTIES,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_TRACKER_PLAY
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(224,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_TRACKER_PLAY,toolBarAction);
//    BUTTON      ID_TRACKER_PLAYPATTERN
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(240,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_TRACKER_PLAYPATTERN,toolBarAction);
//    BUTTON      ID_TRACKER_STOP
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(256,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_TRACKER_STOP,toolBarAction);
//    BUTTON      ID_TRACKER_EDIT
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(272,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_TRACKER_EDIT,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_PREV_SONG
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(288,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_PREV_SONG,toolBarAction);
//    BUTTON      ID_NEXT_SONG
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(304,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_NEXT_SONG,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_FILE_GENERALSETTINGS
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(320,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_FILE_GENERALSETTINGS,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_FILE_CREATE_NSF
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(336,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_FILE_CREATE_NSF,toolBarAction);
//END
}

void qtMfcInitToolBarResource_IDR_INSTRUMENT_TOOLBAR(UINT dlgID,CToolBar* parent)
{
   QHash<int,QAction*>* mfcToQtAction = parent->mfcToQtActionMap();

   QImage toolBarImage(":/resources/inst_toolbar.bmp");
   QToolBar* toolBar = dynamic_cast<QToolBar*>(parent->toQWidget());
   QPixmap toolBarActionPixmap;
   QAction* toolBarAction;

//IDR_INSTRUMENT_TOOLBAR TOOLBAR 16, 15
//BEGIN
//    BUTTON      ID_INSTRUMENT_NEW
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(0,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_INSTRUMENT_NEW,toolBarAction);
//    BUTTON      ID_INSTRUMENT_REMOVE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(16,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_INSTRUMENT_REMOVE,toolBarAction);
//    BUTTON      ID_INSTRUMENT_CLONE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(32,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_INSTRUMENT_CLONE,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_INSTRUMENT_LOAD
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(48,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_INSTRUMENT_LOAD,toolBarAction);
//    BUTTON      ID_INSTRUMENT_SAVE
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(64,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_INSTRUMENT_SAVE,toolBarAction);
//    SEPARATOR
   toolBar->addSeparator();
//    BUTTON      ID_INSTRUMENT_EDIT
   toolBarAction = new QAction(parent);
   toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy(80,0,16,15)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   toolBarAction->setIcon(QIcon(toolBarActionPixmap));
   QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
   parent->toolBarActions()->append(toolBarAction);
   toolBar->addAction(toolBarAction); 
   mfcToQtAction->insert(ID_INSTRUMENT_EDIT,toolBarAction);
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

void qtMfcInit()
{
   qtMfcInitStringResources();
   qtMfcInitMenuResources();
   qtMfcInitBitmapResources();
}
