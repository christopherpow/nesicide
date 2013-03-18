#include "cqtmfc.h"
#include "cqtmfc_famitracker.h"

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
   qtMfcStringResources.insert(IDS_SAMPLES_NEEDED      , "Following samples are needed for this instrument:\n\n");
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
   qtMfcStringResources.insert(ID_FILE_MRU_FILE1       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE2       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE3       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE4       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE5       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE6       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE7       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE8       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE9       , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE10      , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE11      , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE12      , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE13      , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE14      , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE15      , "Open this document");
   qtMfcStringResources.insert(ID_FILE_MRU_FILE16      , "Open this document");
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
   qtMfcStringResources.insert(IDS_CLEARPATTERN        , "Do you want to reset all patterns and frames? There is no undo for this command.");
   qtMfcStringResources.insert(IDS_CLIPBOARD_COPY_ERROR, "An error occured while trying to copy data.");
   qtMfcStringResources.insert(IDS_CLIPBOARD_PASTE_ERROR, "An error occured while trying to paste data.");
   qtMfcStringResources.insert(IDS_DPCM_SPACE_FORMAT   , "Space used %i kB, left %i kB (%i kB available)");
   qtMfcStringResources.insert(IDS_FILE_VERSION_TOO_NEW, "This file was created in a newer version of FamiTracker. Please download the most recent version to open this file.");
   qtMfcStringResources.insert(IDS_INST_EDITOR_TITLE   , "Instrument editor - %02X. %s (%s)");
   qtMfcStringResources.insert(IDS_INST_VERSION_UNSUPPORTED, "File version not supported!");
   qtMfcStringResources.insert(IDS_INVALID_INST_FILE   , "Could not open instrument file");
   qtMfcStringResources.insert(IDS_MIDI_ERR_INPUT      , "MIDI Error: Could not open MIDI input device!");
   qtMfcStringResources.insert(IDS_MIDI_ERR_OUTPUT     , "MIDI Error: Could not open MIDI output device!");
   qtMfcStringResources.insert(IDS_NEW_SOUND_CONFIG    , "New sound configuration loaded");
//   END
   
//   STRINGTABLE 
//   BEGIN
   qtMfcStringResources.insert(IDS_REMOVE_INSTRUMENTS  , "Do you want to remove all unused instruments? There is no undo for this action.");
   qtMfcStringResources.insert(IDS_REMOVE_PATTERNS     , "Do you want to remove all unused patterns? There is no undo for this action.");
   qtMfcStringResources.insert(IDS_SONG_DELETE         , "Do you want to delete this song? There is no undo for this action.");
   qtMfcStringResources.insert(IDS_SOUND_FAIL          , "It appears the current sound settings aren't working, change settings and try again!");
   qtMfcStringResources.insert(IDS_UNDERRUN_MESSAGE    , "Audio buffer underrun, increase the audio buffer size.");
   qtMfcStringResources.insert(IDS_WELCOME_VER         , "Welcome to FamiTracker %i.%i.%i, press F1 for help");
   qtMfcStringResources.insert(IDS_INVALID_WAVEFILE    , "Cannot load this sample!\nOnly uncompressed PCM is supported.");
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

void qtMfcInit()
{
   qtMfcInitStringResources();
   qtMfcInitMenuResources();
   qtMfcInitBitmapResources();
}
