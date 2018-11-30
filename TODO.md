# FamiTracker Qt BURN-DOWN LIST

## Bugs/things to work on

- [x] FrameEditor accelerator keys need handling (PreTranslate...)
- [x] MFC message map implement proper handlers.
- [x] Paint event handling for subclassed widgets needs to support paint-over of standard widgets.
- [x] TrackPopupMenu needs to hook into #2 so that pattern-editor menu actions go up the proper chain and get handled.
- [x] Implement CEdit/CSpinButtonCtrl solution that ties in better with Qt widgets.
- [x] Support non-numeric spin-controls for CreateWave dialog.
- [x] Figure out why CreateWave dialog's subclassed CListBox isn't subclassing properly.
- [x] Module importing doesn't work properly.
- [ ] Frame Editor doesn't move properly between top/left sides and doesn't size appropriately on the left side for non-2A03 channel inclusions.


## Features/things to get to DONE

- [x] Finish implementing configuration dialogs.
- [ ] Finish implementation of accelerator keys.
   - [x] Implement hooks to UI for triggers.
- [x] Finish implementation of custom controls [instrument list!]
- [x] MIDI
- [ ] Test, test, test...

## NESICIDE TO-DO LIST

The following items are being tracked here because otherwise they'd be forgotten:

- [x] Files that have been modified need to have a modification indicator on the tab bar and/or project panel.
- [x] External modifications of files needs to be tracked and reacted to when NESICIDE gains focus.
- [x] Add folders to project panel and support loading different file types (headers, etc.)
- [x] Finish Code Editor lexer for syntax highlights.
   - It's still a bit buggy with highlighting numbers in label references.
   - It incorrectly comment-highlights ;'s in "'s.
- [x] Finish implementation of Code Editor context menu to pull in breakpoint/marker setting and any other context-menu stuff.
- [x] Customizations for cursor size/color in Code Editor.
- [x] Changes that effect the display of open Code Editors don't take effect immediately like they should.  It should not require closing/reopening the Code Editor.
- [ ] VIM, perhaps.
- [ ] Add operator highlighting to Code Editor lexer.
- [ ] Arbitrary expressions in the Symbol Watch window.
- [ ] Interactive Lua console for debugging.
- [x] Add "global persistence" for debuggers so that "most recent settings" can be remembered in a project.
- [x] GNU Make support as an option alongside direct CA65/LD65 invocation.
- [x] Figure out why Code Browser doesn't snap to reset vector when project is loaded at application startup, but does when project is loaded after startup.
- [ ] Make clean does not remove .nes or .chr files.
- [ ] Add icons to source navigator and add files to list from project so that files can be shown whether or not they are in the list because they were in the project
or because they were pulled in by the build.
- [x] Figure out how to adjust lines of errors, breakpoints, and symbol lookups...probably have to do background compilation.
(This is done because the IDE now puts up a message if a file is newer than the running ROM).
- [x] Adding multiple files to a project at once.
- [ ] Extend drag-drop capability to allow adding source files (use specified extensions to check) to a project.
- [x] Project still does not get cleared out properly when a ROM is loaded after a project has been loaded.  Saving the "project" overwrites it with crap.
- [x] Execution Visualizer doesn't open file for address that's clicked on and doesn't bring that file to view in the project.
- [x] Somehow tabs aren't being removed from the Window menu.
- [ ] Add option to automatically assign names of new elements added to a project based on what they are instead of asking all the time.
- [x] Implement "modes" of the UI.  "Coding" mode would close all opened debug windows and emulator windows, allowing most screen real-estate for coding.  "Debugging".  Alternatively this could just be a "remove clutter" button somewhere prominent in the UI that has the same effect.
- [x] Limit symbol watch debug updates to only the symbols in the currently visible tab in the symbol watch window.  Makes no sense to update the values of the RAM symbols if the watch tab is selected.  Updating of symbols on a non-visible tab can be done in the tab change handler.
- [ ] Add OpenGL monospace font for overlays for Execution Visualizer, Code/Data Logger, maybe other viewers, and Emulator [with Lua capabilities?]
- [ ] Rework CCC65Interface object so that it creates Symbol structures containing all of the relevant information for each symbol in easy-to-access means, rather than using individual CCC65Interface methods to retrieve different pieces of information about symbols all the time.  The symbol structures can be built whenever a debug file is loaded, which is either when a project is loaded or when a compile is completed.  CCC65Interface::captureDebugInfo.
- [ ] Move linker config file into Project Browser folder.  Project Properties can still specify the linker config file name, perhaps, but should not contain the linker config file content.
