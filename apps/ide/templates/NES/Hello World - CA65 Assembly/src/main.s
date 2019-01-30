;
; Template based on
; Russian Roulette game for NES
; Copyright 2010 Damian Yerrick
;
; Copying and distribution of this file, with or without
; modification, are permitted in any medium without royalty
; provided the copyright notice and this notice are preserved.
; This file is offered as-is, without any warranty.
;
.include "src/nes.h"
.p02

.segment "ZEROPAGE"

.segment "VECTORS"
  .addr nmi, reset, irq

.segment "CODE"

; we don't use irqs yet
.proc irq
  rti
.endproc

.proc nmi
  rti
.endproc

.proc reset
  sei

  ; Acknowledge and disable interrupt sources during bootup
  ldx #0
  stx PPUCTRL    ; disable vblank NMI
  stx PPUMASK    ; disable rendering (and rendering-triggered mapper IRQ)
  lda #$40
  sta $4017      ; disable frame IRQ
  stx $4010      ; disable DPCM IRQ
  bit PPUSTATUS  ; ack vblank NMI
  bit $4015      ; ack DPCM IRQ
  cld            ; disable decimal mode to help generic 6502 debuggers
                 ; http://magweasel.com/2009/08/29/hidden-messagin/
  dex            ; set up the stack
  txs

  ; Wait for the PPU to warm up (part 1 of 2)
vwait1:
  bit PPUSTATUS
  bpl vwait1

  ; While waiting for the PPU to finish warming up, we have about
  ; 29000 cycles to burn without touching the PPU.  So we have time
  ; to initialize some of RAM to known values.
  ; Ordinarily the "new game" initializes everything that the game
  ; itself needs, so we'll just do zero page and shadow OAM.
  ldy #$00
  lda #$F0
  ldx #$00
clear_zp:
  sty $00,x
  inx
  bne clear_zp
  ; the most basic sound engine possible
  lda #$0F
  sta $4015

  ; Wait for the PPU to warm up (part 2 of 2)
vwait2:
  bit PPUSTATUS
  bpl vwait2

  ; Draw HELLO WORLD text
  jsr drawHelloWorld

  ; Turn screen on
  lda #0
  sta PPUSCROLL
  sta PPUSCROLL
  lda #VBLANK_NMI|BG_1000
  sta PPUCTRL
  lda #BG_ON
  sta PPUMASK


mainLoop:
  jmp mainLoop
.endproc


.proc cls
  lda #VBLANK_NMI
  sta PPUCTRL
  lda #$20
  ldx #$00
  stx PPUMASK
  sta PPUADDR
  stx PPUADDR
  ldx #240
:
  sta PPUDATA
  sta PPUDATA
  sta PPUDATA
  sta PPUDATA
  dex
  bne :-
  ldx #64
  lda #0
:
  sta PPUDATA
  dex
  bne :-
  rts
.endproc

.proc drawHelloWorld
  jsr cls

  ; set monochrome palette
  lda #$3F
  sta PPUADDR
  lda #$00
  sta PPUADDR
  ldx #8
:
  lda #$17
  sta PPUDATA
  lda #$38
  sta PPUDATA
  sta PPUDATA
  sta PPUDATA
  dex
  bne :-

  ; load source and destination addresses
  lda #>helloWorld
  sta 1
  lda #<helloWorld
  sta 0
  lda #$20
  sta 3
  lda #$62
  sta 2
  ; fall through
.endproc
.proc printMsg
dstLo = 2
dstHi = 3
src = 0
  lda dstHi
  sta PPUADDR
  lda dstLo
  sta PPUADDR
  ldy #0
loop:
  lda (src),y
  beq done
  iny
  bne :+
  inc src+1
:
  cmp #10
  beq newline
  sta PPUDATA
  bne loop
newline:
  lda #32
  clc
  adc dstLo
  sta dstLo
  lda #0
  adc dstHi
  sta dstHi
  sta PPUADDR
  lda dstLo
  sta PPUADDR
  jmp loop
done:
  rts
.endproc

.segment "RODATA"
helloWorld:
  .byt "HELLO WORLD",0
