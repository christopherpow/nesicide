# FamiTracker Qt BURN-DOWN LIST

## Bugs/things to work on

- [x] FrameEditor accelerator keys need handling (PreTranslate...)
- [x] TrackPopupMenu needs to hook into #2 so that pattern-editor menu actions go up the proper chain and get handled.
- [x] Support non-numeric spin-controls for CreateWave dialog.
- [x] Figure out why CreateWave dialog's subclassed CListBox isn't subclassing properly.
- [x] Module importing doesn't work properly.
- [ ] Frame Editor doesn't move properly between top/left sides and doesn't size appropriately on the left side for non-2A03 channel inclusions.


## Features/things to get to DONE

- [ ] Finish implementation of accelerator keys.
- [x] Finish implementation of custom controls [instrument list!]
- [x] MIDI
- [ ] Test, test, test...

## NESICIDE TO-DO LIST

The following items are being tracked here because otherwise they'd be forgotten:

- [X] Add folders to project panel and support loading different file types (headers, etc.)
- [ ] Finish Code Editor lexer for syntax highlights.
   - It's still a bit buggy with highlighting numbers in label references.
   - It incorrectly comment-highlights ;'s in "'s.
   - Comic Sans font in C file comments is ridiculous
- [ ] VIM, perhaps.
- [ ] Add operator highlighting to Code Editor lexer.
- [ ] Arbitrary expressions in the Symbol Watch window.
- [ ] Interactive Lua console for debugging.
- [ ] Make clean does not remove .nes or .chr files.
- [ ] Add icons to source navigator and add files to list from project so that files can be shown whether or not they are in the list because they were in the project
or because they were pulled in by the build.
- [ ] Figure out how to adjust lines of errors, breakpoints, and symbol lookups...probably have to do background compilation.
- [ ] Extend drag-drop capability to allow adding source files (use specified extensions to check) to a project.
- [X] Project still does not get cleared out properly when a ROM is loaded after a project has been loaded.  Saving the "project" overwrites it with crap.
- [X] Execution Visualizer doesn't open file for address that's clicked on and doesn't bring that file to view in the project.
- [X] Somehow tabs aren't being removed from the Window menu.
- [ ] Add option to automatically assign names of new elements added to a project based on what they are instead of asking all the time.
- [X] Implement "modes" of the UI.  "Coding" mode would close all opened debug windows and emulator windows, allowing most screen real-estate for coding.  "Debugging".  Alternatively this could just be a "remove clutter" button somewhere prominent in the UI that has the same effect.
- [X] Fix emulator window disappearing on project reload and debug mode switch.
- [X] Limit symbol watch debug updates to only the symbols in the currently visible tab in the symbol watch window.  Makes no sense to update the values of the RAM symbols if the watch tab is selected.  Updating of symbols on a non-visible tab can be done in the tab change handler.
- [ ] Add OpenGL monospace font for overlays for Execution Visualizer, Code/Data Logger, maybe other viewers, and Emulator [with Lua capabilities?]
- [ ] Rework CCC65Interface object so that it creates Symbol structures containing all of the relevant information for each symbol in easy-to-access means, rather than using individual CCC65Interface methods to retrieve different pieces of information about symbols all the time.  The symbol structures can be built whenever a debug file is loaded, which is either when a project is loaded or when a compile is completed.  CCC65Interface::captureDebugInfo.
- [ ] Move linker config file into Project Browser folder.  Project Properties can still specify the linker config file name, perhaps, but should not contain the linker config file content.

## NESICIDE UX NOTES FROM GGJ2020

- [ ] Make mixed-mode checkbox accessible via keyboard shortcut, toolbar item, or etc.
- [ ] Mixed-mode assembly in side-bar
- [ ] Mixed-mode assembly updates in real-time (requires background quiet compilation)
- [ ] Attribute Table and Tile/Stamp data should *not* be in project file as they are thus inaccessible to the project source to reference
- [ ] Editors should be detachable
- [X] Some glitches in long term use, probably related to OpenGL misuse. Find an OpenGL debugger?
This included some weird switching of contexts from one widget to another, such as from exevis to emu, etc.
- [X] Should have option to set both start/end of execution visualization markers with one command
- [X] Should have option to remove ALL breakpoints
- [ ] Removing execution visualizer markers by line(s) of selected code only, not just ALL
- [ ] Alpha-blend of execution markers on visual instead of replace
- [X] Fix linker config file browser -- shouldn't be looking to SAVE a file
