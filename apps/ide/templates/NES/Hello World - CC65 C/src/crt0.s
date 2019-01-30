; Startup code for cc65 (NES version)
; based on code by Groepaz/Hitmen <groepaz@gmx.net>, Ullrich von Bassewitz <uz@cc65.org>

    .export _exit
    .export __STARTUP__:absolute=1      ; Mark as startup
	.import initlib
	.import push0,_main,zerobss,copydata
	.import popa
	.import _oam_clear
	.import FamiToneUpdate,FamiToneSfxInit,FamiToneInit,FamiToneSampleInit

; Linker generated symbols
	.import __RAM_START__   ,__RAM_SIZE__
	.import __ROM0_START__  ,__ROM0_SIZE__
	.import __STARTUP_LOAD__,__STARTUP_RUN__,__STARTUP_SIZE__
	.import	__CODE_LOAD__   ,__CODE_RUN__   ,__CODE_SIZE__
	.import	__RODATA_LOAD__ ,__RODATA_RUN__ ,__RODATA_SIZE__

    .include "zeropage.inc"
	.include "memmap.inc"

.segment       	"STARTUP"

start:
_exit:

    sei
    ldx #$ff
    txs
    inx
    stx PPU_MASK
    stx DMC_FREQ
    stx PPU_CTRL		;no NMI

    bit PPU_STATUS
@1:
    bit PPU_STATUS
    bpl @1

    txa
@clearRAM:
    sta $000,x
    sta $100,x
    sta $200,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne @clearRAM

@clearVRAM:
	lda #$20
	sta PPU_ADDR
	lda #$00
	sta PPU_ADDR
@3:
	ldy #$40
@4:
	sta PPU_DATA
	iny
	bne @4
	inx
	bne @3

	;jsr _oam_clear

    bit PPU_STATUS
@2:
    bit PPU_STATUS
    bpl @2

    jsr	zerobss
	jsr	copydata

    lda #<(__RAM_START__+__RAM_SIZE__)
    sta	sp
    lda	#>(__RAM_START__+__RAM_SIZE__)
    sta	sp+1            ; Set argument stack ptr

	jsr	initlib

	lda #%10000000
	sta PPU_CTRL_VAR
	sta PPU_CTRL		;enable NMI

	lda <FRAMECNT1
@wait:
	cmp <FRAMECNT1
	beq @wait

	ldx #52				;blargg's code
	ldy #24
@detectNTSC:
	dex
	bne @detectNTSC
	dey
	bne @detectNTSC

	lda PPU_STATUS
	and #$80
	sta <NTSCMODE

	;lda <NTSCMODE
	;jsr FamiToneInit

	;ldx #<music_dpcm
	;ldy #>music_dpcm
	;jsr FamiToneSampleInit

	;ldx #<sounds
	;ldy #>sounds
	;jsr FamiToneSfxInit

	lda #1
	sta <RAND_SEED

	jmp _main			;no parameters


nmi:
	pha
	txa
	pha
	tya
	pha

	lda <VRAMUPDATE
	bne @upd
	jmp @skipUpd

@upd:
	lda #$00
	sta PPU_OAM_ADDR
	lda #>OAM_BUF
	sta PPU_OAM_DMA

	lda #$3f
	sta PPU_ADDR
	lda #$00
	sta PPU_ADDR

	lda PAL_BUF+0
	sta PPU_DATA
	lda PAL_BUF+1
	sta PPU_DATA
	lda PAL_BUF+2
	sta PPU_DATA
	lda PAL_BUF+3
	sta PPU_DATA

	lda PPU_DATA
	lda PAL_BUF+5
	sta PPU_DATA
	lda PAL_BUF+6
	sta PPU_DATA
	lda PAL_BUF+7
	sta PPU_DATA

	lda PPU_DATA
	lda PAL_BUF+9
	sta PPU_DATA
	lda PAL_BUF+10
	sta PPU_DATA
	lda PAL_BUF+11
	sta PPU_DATA

	lda PPU_DATA
	lda PAL_BUF+13
	sta PPU_DATA
	lda PAL_BUF+14
	sta PPU_DATA
	lda PAL_BUF+15
	sta PPU_DATA

	lda PPU_DATA
	lda PAL_BUF+17
	sta PPU_DATA
	lda PAL_BUF+18
	sta PPU_DATA
	lda PAL_BUF+19
	sta PPU_DATA

	lda PPU_DATA
	lda PAL_BUF+21
	sta PPU_DATA
	lda PAL_BUF+22
	sta PPU_DATA
	lda PAL_BUF+23
	sta PPU_DATA

	lda PPU_DATA
	lda PAL_BUF+25
	sta PPU_DATA
	lda PAL_BUF+26
	sta PPU_DATA
	lda PAL_BUF+27
	sta PPU_DATA

	lda PPU_DATA
	lda PAL_BUF+29
	sta PPU_DATA
	lda PAL_BUF+30
	sta PPU_DATA
	lda PAL_BUF+31
	sta PPU_DATA

	ldx <NAME_UPD_LEN
	beq @skipUpd
	ldy #0
@updName:
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_DATA
	dex
	bne @updName

@skipUpd:
	lda #0
	sta PPU_ADDR
	sta PPU_ADDR

	lda <SCROLL_X
	sta PPU_SCROLL
	lda <SCROLL_Y
	sta PPU_SCROLL
	lda <PPU_CTRL_VAR
	sta PPU_CTRL

	inc <FRAMECNT1
	inc <FRAMECNT2
	lda <FRAMECNT2
	cmp #6
	bne @skipNtsc
	lda #0
	sta <FRAMECNT2
@skipNtsc:

	;jsr FamiToneUpdate

	pla
	tay
	pla
	tax
	pla

irq:
    rti


.segment "VECTORS"

    .word nmi	;$fffa vblank nmi
    .word start	;$fffc reset
   	.word irq	;$fffe irq / brk
