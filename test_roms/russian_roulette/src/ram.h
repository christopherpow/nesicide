; Copyright 2010 Damian Yerrick
;
; Copying and distribution of this file, with or without
; modification, are permitted in any medium without royalty
; provided the copyright notice and this notice are preserved.
; This file is offered as-is, without any warranty.

xferBuf = $0100
OAM = $0200

; main fields
.globalzp nmis, oamIndex, debugHex1, debugHex2

; pads fields
.globalzp cur_keys, new_keys, das_timer, das_keys
; pads methods
.global read_pads, read_trigger

; random fields
.globalzp rand0, rand1, rand2, rand3
; random methods
.global random, roll

