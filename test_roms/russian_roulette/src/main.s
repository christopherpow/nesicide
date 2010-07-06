;
; Russian Roulette game for NES
; Copyright 2010 Damian Yerrick
;
; Copying and distribution of this file, with or without
; modification, are permitted in any medium without royalty
; provided the copyright notice and this notice are preserved.
; This file is offered as-is, without any warranty.
;
.include "src/nes.h"
.include "src/ram.h"
.p02

.segment "ZEROPAGE"
nmis: .res 1
cur_keys: .res 3  ; 0: player 1; 1: player 2; 2: zapper trigger
new_keys: .res 3
num_players: .res 1
cur_turn: .res 1
alive_players: .res 1

.segment "INESHDR"
  .byt "NES",$1A
  .byt 1  ; 16 KiB PRG ROM
  .byt 1  ; 8 KiB CHR ROM
  .byt 1  ; vertical mirroring; low mapper nibble: 0
  .byt 0  ; high mapper nibble: 0

.segment "VECTORS"
  .addr nmi, reset, irq

.segment "CODE"

; we don't use irqs yet
.proc irq
  rti
.endproc

.proc nmi
  inc nmis
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
  sta OAM,x
  inx
  bne clear_zp
  ; the most basic sound engine possible
  lda #$0F
  sta $4015
  
  lda #2
  sta num_players

  
  ; Wait for the PPU to warm up (part 2 of 2)
vwait2:
  bit PPUSTATUS
  bpl vwait2


startOver:
  jsr getNumberOfPlayers

gameLoop:
  jsr takeTurn
  jsr findNextAlivePlayer
  lda alive_players
  sec
  sbc #1
  and alive_players
  bne gameLoop
  jsr cls
  ldx #$3F
  stx PPUADDR
  ldx #$40
  stx PPUADDR
  lda #$1A
  sta PPUDATA
  lda #>winText
  sta 1
  lda #<winText
  sta 0
  lda #$20
  sta 3
  lda #$62
  sta 2
  jsr printMsg
  
  ; position after "Player"
  lda #$20
  sta PPUADDR
  lda #$69
  sta PPUADDR
  lda cur_turn
  clc
  adc #'1'
  sta PPUDATA

waitForStartOver:
  jsr read_pads
  lda nmis
:
  cmp nmis
  beq :-
  lda #0
  sta PPUSCROLL
  sta PPUSCROLL
  lda #VBLANK_NMI|BG_1000
  sta PPUCTRL
  lda #BG_ON
  sta PPUMASK
  lda new_keys
  and #KEY_A|KEY_START
  beq waitForStartOver
  jmp startOver
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

.proc drawIntroText
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
  lda #>introText
  sta 1
  lda #<introText
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

.proc getNumberOfPlayers
  jsr drawIntroText
  
  ; set up cursor sprite
  lda #$F0
  ldx #0
:
  sta OAM,x
  inx
  bne :-
  lda #175
  sta OAM
  lda #'>'
  sta OAM+1
  lda #0
  sta OAM+2

loop:
  jsr read_pads

  lda new_keys
  and #KEY_RIGHT|KEY_SELECT
  beq notInc
  inc num_players
  lda nmis
  eor rand0
  sta rand0
  ldy #4
  jsr random
notInc:

  lda new_keys
  and #KEY_LEFT
  beq notDec
  dec num_players
  lda nmis
  eor rand0
  sta rand0
  ldy #4
  jsr random
notDec:

  ; Wrap number of players within 2-6
  lda num_players
  cmp #2
  bcs :+
  lda #6
:
  cmp #7
  bcc :+
  lda #2
:
  sta num_players

  ; Check for exit
  lda new_keys
  and #KEY_A|KEY_START
  beq notDone

  lda nmis
  eor rand0
  sta rand0
  ; Determine the initial alive players and the starting player
  ldy #8
  jsr random
  ldx num_players
  jsr roll
  sta cur_turn
  ldx num_players
  lda #0
:
  sec
  rol a
  dex
  bne :-
  sta alive_players
  rts
notDone:

  ; Move the cursor
  lda num_players
  asl a
  adc num_players
  asl a
  asl a
  asl a
  sbc #31
  sta OAM+3

  lda nmis
:
  cmp nmis
  beq :-
  lda #0
  sta PPUSCROLL
  sta PPUSCROLL
  sta OAMADDR
  lda #>OAM
  sta OAM_DMA
  lda #VBLANK_NMI|BG_1000|OBJ_1000
  sta PPUCTRL
  lda #BG_ON|OBJ_ON
  sta PPUMASK
  jmp loop
.endproc

.proc takeTurn
  jsr cls
  lda #>turnText1
  sta 1
  lda #<turnText1
  sta 0
  lda #$20
  sta 3
  lda #$62
  sta 2
  jsr printMsg
  
  ; position after "Player"
  lda #$20
  sta PPUADDR
  lda #$69
  sta PPUADDR
  lda cur_turn
  clc
  adc #'1'
  sta PPUDATA

  lda #120
  sta 0
  sta 1

spinCylLoop:
  ; cyl spinning sound
  lda 1
  clc
  adc 0
  sta 0
  bcc noTurnSound
  lda #1
  sta $400C
  lda #4
  sta $400E
  lda #$18
  sta $400F
noTurnSound:
  lda nmis
:
  cmp nmis
  beq :-
  
  ; reset background color
  ldx #$3F
  stx PPUADDR
  ldx #$40
  stx PPUADDR
  lda #$02
  sta PPUDATA
  ; and turn on the rendering
  lda #0
  sta PPUSCROLL
  sta PPUSCROLL
  lda #VBLANK_NMI|BG_1000
  sta PPUCTRL
  lda #BG_ON
  sta PPUMASK
  dec 1
  bne spinCylLoop

waitButtonLoop:
  jsr read_pads
  jsr read_trigger
  lda nmis
:
  cmp nmis
  beq :-
  lda new_keys
  and #KEY_A
  ora new_keys+2
  beq waitButtonLoop

  ldx #6
  jsr roll
  cmp #0
  beq died
  lda nmis
:
  cmp nmis
  beq :-
  lda #>turnTextSafe
  sta 1
  lda #<turnTextSafe
  sta 0
  lda #$20
  sta 3
  lda #$C2
  sta 2
  jsr printMsg
  lda #0
  sta PPUSCROLL
  sta PPUSCROLL
  lda #1
  sta $400C
  lda #4
  sta $400E
  lda #$18
  sta $400F
  
  ; wait two seconds
  ldx #120
  jmp waitXFrames
  
died:
  ; eliminate player
  ldx cur_turn
  lda le_bits,x
  eor #$FF
  and alive_players
  sta alive_players
  lda #$0F
  sta $400C
  lda #$0C
  sta $400E
  lda #$08
  sta $400F
  lda nmis
:
  cmp nmis
  beq :-
  lda #0
  sta PPUMASK
  ldx #$3F
  stx PPUADDR
  ldx #$40
  stx PPUADDR
  lda #$16
  sta PPUDATA
  lda #>turnTextDied
  sta 1
  lda #<turnTextDied
  sta 0
  lda #$20
  sta 3
  lda #$C2
  sta 2
  jsr printMsg
  ldx #180
.endproc
.proc waitXFrames
  lda nmis
:
  cmp nmis
  beq :-
  lda #0
  sta PPUSCROLL
  sta PPUSCROLL
  lda #BG_ON
  sta PPUMASK
  dex
  bne waitXFrames
  rts
.endproc

.proc findNextAlivePlayer
  ldx cur_turn
  ldy #8  ; This should never become 0
loop:
  inx
  cpx num_players
  bcc notTooMany
  ldx #0
notTooMany:
  lda alive_players
  and le_bits,x
  bne foundOne
  dey
  bne loop
foundOne:
  stx cur_turn
  rts
.endproc

.segment "RODATA"
le_bits:
  .byt $01,$02,$04,$08,$10,$20,$40,$80
introText:
  .byt "Russian Roulette 0.01",10
  .byt "Copr. 2010 Damian Yerrick",10
  .byt 10
  .byt "In this game, roulette",10
  .byt "plays YOU.  Here's how:",10
  .byt 10
  .byt "One of a revolver's six",10
  .byt "chambers holds a paint",10
  .byt "capsule.  Each player in",10
  .byt "turn spins the cylinder",10
  .byt "and pulls the trigger.",10
  .byt "(No Zapper? Press A on",10
  .byt "controller 1.) If it doesn't",10
  .byt "fire, pass it to the next",10
  .byt "player.  Get splattered",10
  .byt "and you're out!",10
  .byt 10
  .byt "How many players?",10
  .byt 10
  .byt " 2  3  4  5  6",0

turnText1:
  .byt "Player   grabs the marker",10
  .byt "and spins the cylinder.",0
turnTextSafe:
  .byt "(click)",0
turnTextDied:
  .byt "BANG!",10
  .byt "Player eliminated.",0
winText:
  .byt "Player   wins the game!",10
  .byt 10
  .byt "Press Start to play again",0

