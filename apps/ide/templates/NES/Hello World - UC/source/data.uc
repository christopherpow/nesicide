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

; Static data management

; This puts our character data into the ROM image
rom 1
binfile dataChr "../data/graphics.chr"

; Everything else is in the PRG area
rom 0

; We only need the first 32 bytes of the palette file, the rest is unused but
; YYCHR uses 256-byte palette files.
export binfile dataPalette "../data/graphics.pal" 0 16

; Name and Attribute tables
export binfile dataNameAttr "../data/graphics.nam" 0 1024
;export binfile dataNameTable "../data/graphicsname.nam" 0 960
export binfile dataAttrTable "../data/graphics.atr" 0 64

