;
; Dice rolling
; Copyright 2010 Damian Yerrick
;
; Copying and distribution of this file, with or without
; modification, are permitted in any medium without royalty
; provided the copyright notice and this notice are preserved.
; This file is offered as-is, without any warranty.
;
.include "src/ram.h"
;.include "src/nes.h"  ; for debugging high level shit
.segment "ZEROPAGE"
rand0: .res 1
rand1: .res 1
rand2: .res 1
rand3: .res 1

.segment "CODE"
;; random
; Uses the crc32 polynomial to generate Y
; pseudorandom bits as the low_order bits of rand3.
; Average 48 cycles per bit.
;
.proc random
  asl rand3
  rol rand2
  rol rand1
  rol rand0
  bcc @no_xor
  lda rand0
  eor #$04
  sta rand0
  lda rand1
  eor #$c1
  sta rand1
  lda rand2
  eor #$1d
  sta rand2
  lda rand3
  eor #$b7
  sta rand3
@no_xor:
  dey
  bne random
  rts
.endproc

;;
; Rolls a single X-sided die
; @param X number of sides
; @return rolled number from 0 to one less than the number of sides
.proc roll
  ldy #8
  jsr random
  lda #0
  tay
loop:
  clc
  adc rand3
  bcc :+
  iny
:
  dex
  bne loop
  tya
  rts
.endproc


