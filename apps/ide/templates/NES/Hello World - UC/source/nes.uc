; uc65 - Micro Code for 6502/NES
; Copyright (C) 2013  Norman B. Lancaster
; 
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

; NES System Library
;TODO not yet implemented
;rom librom
;ram libram

; NES Port Definitions
export port ppuControl = $2000
export port ppuMask = $2001
export port ppuStatus = $2002
export port ppuOamAddr = $2003
export port ppuOamData = $2004
export port ppuScroll = $2005
export port ppuAddress = $2006
export port ppuData = $2007
export port apuDmcFrequencey = $4010
export port oamDma = $4014
export port controller1 = $4016
export port apuFrameCounter = $4017

;; Turn rendering off
export sub ppuOff
	ppuControl = %00000000
	ppuMask &= %00000110
end sub

;; Turn on rendering
export sub ppuOn
	ppuControl = %10000000
	ppuMask = %00011110
end sub

;; Initialize to no scroll
export sub ppuNoScroll
	asm
		lda ppuControl
	end asm
	ppuScroll = 0
	ppuScroll = 0
end sub

;; Initialize the NES and PPU (but not a memory mapper).
;;
;; Call this subroutine at the start of Main to clear system RAM and disable the
;; APU and PPU. This also executes the PPU warm-up wait cycle. Don't forget to
;; turn the APU and / or PPU back on when you're ready to use them!
export sub nesInit
	; Turn off APU and PPU
	apuFrameCounter = $40
	apuDmcFrequencey = 0
	;ppuControl = 0
	;ppuMask = 0
	ppuOff
	
	; Clear RAM
	asm
		ldx #$00
		loop:
			lda #$ff	; Page $02 needs to be cleared to $FF to force OAM data
			sta	$0200,x	; off-screen. Change this page if you don't use $02 for
			lda #$00	; the OAM DMA buffer.
			sta $0000,x ; We skip the stack page because it should not need to
			sta $0300,x ; be initialized, and we'll stomp on our return address
			sta $0400,x ; if we do.
			sta $0500,x
			sta $0600,x
			sta $0700,x
			dex
			bne	loop
	end asm
	
	; Wait for the PPU to warm up
	asm
		bit ppuStatus
		loop1:
			bit ppuStatus
			bpl loop1
		loop2:
			bit ppuStatus
			bpl loop2
	end asm
end sub

;; Upload a block of data to the PPU 256 Bytes or less.
;;
;; @param src The source memory address in CPU address space
;; @param dest The destination memory address in PPU address space
;; @param length The number of bytes to copy, 0 means 256 bytes
export sub nesSendPPU address src, word dest, byte length
	; Load PPU address latch, high-byte first
	ppuAddress = dest.b
	ppuAddress = dest.a
	
	; Copy loop
	asm
		ldy #0
	loop:
		lda (nesSendPPU_src),y
		sta ppuData
		iny
		cpy nesSendPPU_length
		bne loop
	end asm
end sub

;; Set a block of data in PPU address space to a constant value
;;
;; @param value The value to write to PPU address space
;; @param dest The destination memory address in PPU address space
;; @param length The number of bytes to copy
export sub nesSetPPU byte value, word dest, word length
	ppuAddress = dest.b
	ppuAddress = dest.a
	
	; Copy loop
	while length != 0
		ppuData = value
		length -= 1
	end while
end sub

;; Upload a Name Table and Attribute table set. (1KB in size)
;;
;; @param src The source memory address in CPU address space
;; @param ntnum The nametable number (0-3) in PPU address space
export sub nesWriteNAT address src, byte ntnum
	; Load PPU address latch, high-byte first
	address ppuaddr
	if ntnum == 0
		ppuaddr = $2000	;NT0
	else if ntnum == 1
		ppuaddr = $2400	;NT1
	else if ntnum == 2
		ppuaddr = $2800	;NT2
	else if ntnum == 3
		ppuaddr = $2C00	;NT3
	end if

	; Copy loop
	nesSendPPU src, ppuaddr, 256
	ppuaddr += 256
	src += 256
	nesSendPPU src, ppuaddr, 256
	ppuaddr += 256
	src += 256
	nesSendPPU src, ppuaddr, 256
	ppuaddr += 256
	src += 256
	nesSendPPU src, ppuaddr, 256

end sub

