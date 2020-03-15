;this demo is compiled with asm6


;iNES header

	.db "NES",$1a
	.db 2			;2 16K PRG
	.db 1			;1 8K CHR
	.db 0			;mapper 0, horizontal mirroring
	.dsb 9,0		;other header bytes



;register aliases

PPU_CTRL		equ $2000
PPU_MASK		equ $2001
PPU_STATUS		equ $2002
PPU_SCROLL		equ $2005
PPU_ADDR		equ $2006
PPU_DATA		equ $2007
PPU_FRAMECNT	equ $4017
DMC_FREQ		equ $4010
CTRL_PORT1		equ $4016

;gamepad readback bits aliases

PAD_A			equ $01
PAD_B			equ $02
PAD_SELECT		equ $04
PAD_START		equ $08
PAD_UP			equ $10
PAD_DOWN		equ $20
PAD_LEFT		equ $40
PAD_RIGHT		equ $80



;demo variables

	.enum $0080

FRAME_CNT		.dsb 1
NTSC_MODE		.dsb 1

PAD_BUF			.dsb 3
PAD_STATE		.dsb 1
PAD_STATEP		.dsb 1
PAD_STATET		.dsb 1

RLE_LOW			.dsb 1
RLE_HIGH		.dsb 1
RLE_TAG			.dsb 1
RLE_BYTE		.dsb 1

	.ende


	
;FamiTone2 settings

FT_BASE_ADR		= $0300	;page in the RAM used for FT2 variables, should be $xx00
FT_TEMP			= $00	;3 bytes in zeropage used by the library as a scratchpad
FT_DPCM_OFF		= $c000	;$c000..$ffc0, 64-byte steps
FT_SFX_STREAMS	= 4		;number of sound effects played at once, 1..4

FT_DPCM_ENABLE			;undefine to exclude all DMC code
FT_SFX_ENABLE			;undefine to exclude all sound effects code
FT_THREAD				;undefine if you are calling sound effects from the same thread as the sound update call

FT_PAL_SUPPORT			;undefine to exclude PAL support
FT_NTSC_SUPPORT			;undefine to exclude NTSC support



;demo code

    .org $8000

reset:

;init hardware

    sei
    ldx #$40
    stx PPU_FRAMECNT
    ldx #$ff
    txs
    inx
    stx PPU_MASK
    stx DMC_FREQ
	lda #%10000000
    sta PPU_CTRL

	jsr waitVBlank

    txa
	
clearRAM:

    sta $000,x
    sta $100,x
    sta $200,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne clearRAM

	jsr waitVBlank
	
	lda #$00
	sta PPU_SCROLL
	sta PPU_SCROLL

clearVRAM:

	lda #$20
	ldy #$00
	sta PPU_ADDR
	sty PPU_ADDR
	
	ldx #$08

@loop:

	sta PPU_DATA
	iny
	bne @loop
	dex
	bne @loop

	stx FRAME_CNT

detectNTSC:

	jsr waitNMI		;blargg's code
	ldx #52
	ldy #24
	
@loop:

	dex
	bne @loop
	dey
	bne @loop

	lda PPU_STATUS
	and #$80
	sta NTSC_MODE	;$00 PAL, $80 NTSC

showScreen:

	lda #$3f		;set palettes
	sta PPU_ADDR
	lda #$00
	sta PPU_ADDR
	ldx #0
	
@set_palette:

	lda screen_pal,x
	sta PPU_DATA
	inx
	cpx #16
	bne @set_palette


	ldx #<screen_rle	;unpack nametable and attributes
	ldy #>screen_rle
	lda #$20
	sta PPU_ADDR
	lda #$00
	sta PPU_ADDR
	jsr unrle

	lda #0
	sta PPU_ADDR
	sta PPU_ADDR

	jsr waitNMI

	lda #%00001110	;enable display, background only
	sta PPU_MASK

	

initLoop:

	ldx #<after_the_rain_music_data	;initialize using the first song data, as it contains the DPCM sound effect
	ldy #>after_the_rain_music_data
	lda NTSC_MODE
	jsr FamiToneInit		;init FamiTone

	ldx #<sounds			;set sound effects data location
	ldy #>sounds
	jsr FamiToneSfxInit

	jsr padInit				;init gamepad polling code

	

mainLoop:

	jsr waitNMI				;wait for new frame

	ldx #0					;delay to shift FamiTone's time into visible raster
	ldy #6
	
@delay:

	dex
	bne @delay
	dey
	bne @delay

	lda #%00001111			;show start of sound code time
	sta PPU_MASK
	sta $401e				;for VirtuaNES custom build

	jsr FamiToneUpdate		;update sound

	sta $401f				;show end of sound code time
	lda #%00001110
	sta PPU_MASK

	jsr padPoll				;poll gamepad

	ldx PAD_STATET
	txa
	and #PAD_LEFT
	beq @noLeftButton
	lda #0					;play effect 0 on channel 0
	ldx #FT_SFX_CH0
	jsr FamiToneSfxPlay
	jmp mainLoop
	
@noLeftButton:

	txa
	and #PAD_RIGHT
	beq @noRightButton
	lda #1					;play effect 1 on channel 1
	ldx #FT_SFX_CH1
	jsr FamiToneSfxPlay
	jmp mainLoop
	
@noRightButton:

	txa
	and #PAD_UP
	beq @noUpButton
	lda #2					;play effect 2 on channel 2
	ldx #FT_SFX_CH2
	jsr FamiToneSfxPlay
	jmp mainLoop
	
@noUpButton:

	txa
	and #PAD_DOWN
	beq @noDownButton
	lda #3					;play effect 3 on channel 3
	ldx #FT_SFX_CH3
	jsr FamiToneSfxPlay
	jmp mainLoop
	
@noDownButton:

	txa
	and #PAD_B
	beq @noBButton

	ldx #<after_the_rain_music_data	;play first song
	ldy #>after_the_rain_music_data
	lda NTSC_MODE
	jsr FamiToneInit
	lda #0
	jsr FamiToneMusicPlay

	jmp mainLoop
	
@noBButton:

	txa
	and #PAD_A
	beq @noAButton

	ldx #<danger_streets_music_data	;play second song
	ldy #>danger_streets_music_data
	lda NTSC_MODE
	jsr FamiToneInit
	lda #0
	jsr FamiToneMusicPlay

	jmp mainLoop
	
@noAButton:

	txa
	and #PAD_START
	beq @noStartButton
	lda #36						;play sample 36
	jsr FamiToneSamplePlay
	jmp mainLoop
	
@noStartButton:

	txa
	and #PAD_SELECT
	beq @noSelectButton
	jsr FamiToneMusicStop		;stop music
	
@noSelectButton:

	jmp mainLoop



;wait VBlank using PPU status

waitVBlank:

    bit PPU_STATUS
	
@loop:

    bit PPU_STATUS
    bpl @loop
	
	rts


	
;wait NMI and VBlank using counter in the NMI handler

waitNMI:

	lda FRAME_CNT
	
@loop:

	cmp FRAME_CNT
	beq @loop
	
	rts


	
;NMI handler

nmi:

	inc FRAME_CNT
	
irq:

    rti



;poll gamepad, with the DPCM bug handling

padInit:

	lda #0
	sta PAD_STATE
	sta PAD_STATEP
	sta PAD_STATET
	
	rts

	

padPoll:

	ldx #0
	jsr padPollPort
	jsr padPollPort
	jsr padPollPort

	lda PAD_BUF
	cmp PAD_BUF+1
	beq @done
	cmp PAD_BUF+2
	beq @done
	lda PAD_BUF+1
	
@done:

	sta PAD_STATE

	lda PAD_STATE
	eor PAD_STATEP
	and PAD_STATE
	sta PAD_STATET
	lda PAD_STATE
	sta PAD_STATEP

	rts

	

padPollPort:

	ldy #$01
	sty CTRL_PORT1
	dey
	sty CTRL_PORT1
	ldy #8
	
@poll1:

	lda CTRL_PORT1
	and #$01
	clc
	beq @poll2
	sec
	
@poll2:

	ror PAD_BUF,x
	dey
	bne @poll1

	inx
	rts



;depack RLE data from an address in X/Y to PPU_DATA

unrle:

	stx RLE_LOW
	sty RLE_HIGH
	ldy #0
	jsr rle_byte
	sta RLE_TAG
	
@unrle1:

	jsr rle_byte
	cmp RLE_TAG
	beq @unrle2
	
	sta PPU_DATA
	sta RLE_BYTE
	bne @unrle1
	
@unrle2:

	jsr rle_byte
	cmp #0
	beq @unrle4
	
	tax
	lda RLE_BYTE
	
@unrle3:

	sta PPU_DATA
	dex
	bne @unrle3
	beq @unrle1
	
@unrle4:

	rts

rle_byte:

	lda (RLE_LOW),y
	inc RLE_LOW
	bne @skip
	inc RLE_HIGH
	
@skip:

	rts



screen_rle:		.incbin "screen.rle"
screen_pal:		.incbin "screen.pal"



	.include "famitone2_asm6.asm"

	.include "danger_streets.asm"
	.include "after_the_rain.asm"

	.include "sounds.asm"

	
;samples

	.org FT_DPCM_OFF
	
	.incbin "after_the_rain.dmc"

	
	
;vectors

    .org  $fffa
    .dw nmi
    .dw reset
	.dw irq
	

;CHR

	.incbin "screen.chr"