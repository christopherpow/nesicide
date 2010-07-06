;
; NES controller reading code
; Copyright 2010 Damian Yerrick
;
; Copying and distribution of this file, with or without
; modification, are permitted in any medium without royalty
; provided the copyright notice and this notice are preserved.
; This file is offered as-is, without any warranty.
;
.export read_pads, read_trigger
.importzp cur_keys, new_keys, das_keys, das_timer

JOY1      = $4016
JOY2      = $4017

DAS_DELAY = 15
DAS_SPEED = 3

.segment "CODE"
.proc read_pads

  ; store the current keypress state to detect key-down later
  lda cur_keys
  sta 4
  lda cur_keys+1
  sta 5

  ; read the joypads twice
  jsr read_pads_once
  lda 0
  sta 2
  lda 1
  sta 3
  jsr read_pads_once

  ldx #1
@fixupKeys:

  ; if the player's keys read out the same ways both times, update
  lda 0,x
  cmp 2,x
  bne @dontUpdateGlitch
  sta cur_keys,x
@dontUpdateGlitch:
  
  lda 4,x   ; A = keys that were down last frame
  eor #$FF  ; A = keys that were up last frame
  and cur_keys,x  ; A = keys down now and up last frame
  sta new_keys,x
  dex
  bpl @fixupKeys
  rts

read_pads_once:
  lda #1
  sta 0
  sta 1
  sta JOY1
  lda #0
  sta JOY1
  loop:
    lda JOY1
    and #$03
    cmp #1
    rol 0
    lda JOY2
    and #$03
    cmp #1
    rol 1
    bcc loop
  rts
.endproc

.proc read_trigger
  lda cur_keys+2
  sta 4
  lda JOY2
  and #$10
  sta cur_keys+2
  lda 4     ; A = trigger was down last frame
  eor #$FF  ; A = trigger was up last frame
  and cur_keys+2  ; A = trigger down now and up last frame
  sta new_keys+2
  rts
.endproc
