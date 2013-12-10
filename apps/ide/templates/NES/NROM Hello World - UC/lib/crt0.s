; Micro Code 65 Common Run-Time for ca65 Assembler
; NROM target
.import frame, main, interrupt

.SEGMENT "HEAD"
	.byte $4E, $45, $53, $1A	; "NES" header
	.byte $02					; PRG-ROM / 16k
	.byte $01					; CHR-ROM / 8k
	.byte $00					; Mapper 0 (NROM), no SRAM
	.byte $00					; Mapper 0 (NROM)
	.byte $00					; No mapper variant
	.byte $00					; No extended ROM pages
	.byte $00					; No Battery-backed SRAM
	.byte $00					; No non-battery-backed SRAM
	.byte $00					; NTSC video system
	.byte $00					; No Vs. System hardware
	.byte $00, $00				; Researved

.SEGMENT "VECT"
	.word frame, main, interrupt

