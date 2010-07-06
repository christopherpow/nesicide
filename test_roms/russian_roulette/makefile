#!/usr/bin/make -f
#
# Makefile for Russian Roulette
# Copyright 2010 Damian Yerrick
#
# Copying and distribution of this file, with or without
# modification, are permitted in any medium without royalty
# provided the copyright notice and this notice are preserved.
# This file is offered as-is, without any warranty.
#
version = 0.01
objlist = main pads random

CC65 = /usr/local/bin
AS65 = ca65
LD65 = ld65
#EMU := "/C/Program Files/nintendulator/Nintendulator.exe"
EMU := fceu -input2 zapper
CC = gcc
ifdef COMSPEC
DOTEXE=.exe
else
DOTEXE=
endif
CFLAGS = -std=gnu99 -Wall -DNDEBUG -O
CFLAGS65 = 
objdir = obj/nes
srcdir = src
imgdir = tilesets

# -f while debugging code; -r while adding shuffle markup;
# neither once a module has stabilized
shufflemode = -r

objlistntsc = $(foreach o,$(objlist),$(objdir)/$(o).o)

.PHONY: run dist zip

run: roulette.nes
	$(EMU) $<

# Actually this depends on every single file in zip.in, but currently
# we use changes to roulette.nes, makefile, and README as a heuristic
# for when something was changed.  Limitation: it won't see changes
# to docs or tools.
dist: zip
zip: roulette-$(version).zip
roulette-$(version).zip: zip.in roulette.nes README.html $(objdir)/index.txt
	zip -9 -u $@ -@ < $<

$(objdir)/index.txt: makefile
	echo Files produced by build tools go here, but caulk goes where? > $@

$(objdir)/%.o: $(srcdir)/%.s $(srcdir)/nes.h $(srcdir)/ram.h
	$(AS65) $(CFLAGS65) $< -o $@

$(objdir)/%.o: $(objdir)/%.s
	$(AS65) $(CFLAGS65) $< -o $@

$(objdir)/title.o: todo.txt

$(objdir)/ntscPeriods.s: tools/mktables.py
	$< period $@

$(objdir)/palPeriods.s: tools/mktables.py
	$< period $@

map.txt roulette.prg: nes.ini $(objlistntsc)
	$(LD65) -C $^ -m map.txt -o roulette.prg

$(objdir)/titlegfx.chr: $(imgdir)/titlegfx.png
	tools/pilbmp2nes.py $< $@

$(objdir)/gamegfx.chr: $(imgdir)/gamegfx.png
	tools/pilbmp2nes.py $< $@

%.nes: %.prg %.chr
	cat $^ > $@

roulette.chr: $(objdir)/titlegfx.chr $(objdir)/gamegfx.chr
	cat $^ > $@

