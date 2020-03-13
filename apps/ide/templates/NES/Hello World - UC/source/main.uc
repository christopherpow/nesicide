; Conway's Game of Life for NES
;
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

import "nes.uc"
import "data.uc"

; Main globals
export fast byte vblankFlag

export sub main
	nesInit

	;write background palette
	nesSendPPU @dataPalette, $3f00, 16
	;write Name and Attribute tables
	nesWriteNAT @dataNameAttr, 0

	;turn on PPU and initialize scroll
	ppuOn
	ppuNoScroll
	
	while 1==1
	;infinite loop 
	end while

end sub

export sub frame
	; Set the vblank flag before returning
	vblankFlag += 1
end sub

export sub interrupt
end sub

