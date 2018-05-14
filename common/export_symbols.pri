win32-msvc* {
    # Export *all* symbols, using same method as VS2017 HertzDevil.
    # Also behaves like MinGW.

    EXPORT = ""
#    EXPORT += cd & cd & cd & cd & cd & cd & cd
#    C:\Users\jimbo1qaz\src\nesicide\deps\build-rtmidi-Desktop_Qt_5_10_1_MSVC2017_64bit-Debug\debug

#    EXPORT += "dir $$DESTDIR\*.obj"
#2018-05-14  05:28           697,546 RtMidi.obj

    EXPORT += \
cd $$DESTDIR &\
del $${TARGET}.lib &\
lib -out:$${TARGET}.lib *.obj

#    QMAKE_POST_LINK += $$EXPORT
    export_symbols.commands = "$$EXPORT"
    export_symbols.target = export_symbols
    QMAKE_EXTRA_TARGETS += export_symbols

#    MAKEFILE = RealMakefile
#    data = $$cat($$TOP/common/export_symbols.makefile)
#    write_file("Makefile", data)

    # https://stackoverflow.com/a/5948236
#    POST_TARGETDEPS += export_symbols
#    message($$PWD)
#    message("foo")
    QMAKE_POST_LINK += "$$EXPORT"
}
