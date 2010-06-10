;CALIBRATE=1
.include "instr_test.inc"

instrs:
	entry $A1,"LDA (z,X)" ; A = (z,X)
	
	entry $81,"STA (z,X)" ; (z,X) = A
	
	entry $C1,"CMP (z,X)" ; A op (z,X)
	
	entry $61,"ADC (z,X)" ; A = A op (z,X)
	entry $E1,"SBC (z,X)"
	entry $01,"ORA (z,X)"
	entry $21,"AND (z,X)"
	entry $41,"EOR (z,X)"
.ifndef OFFICIAL_ONLY
	entry $03,"SLO (z,X)"
	entry $23,"RLA (z,X)"
	entry $43,"SRE (z,X)"
	entry $63,"RRA (z,X)"
	entry $83,"AAX (z,X)"
	entry $A3,"LAX (z,X)"
	entry $C3,"DCP (z,X)"
	entry $E3,"ISC (z,X)"
.endif
instrs_size = * - instrs

address = <$FF
operand = $2FF

instr_template:
	lda (address,x)
	jmp instr_done
instr_template_size = * - instr_template

.macro set_in
	lda values+1,y
	sta operand+1
	
	lda values+2,y
	sta operand+2
	
	set_paxyso
.endmacro

.macro check_out
	check_paxyso
	
	lda operand+1
	jsr update_crc_fast
	
	lda operand+2
	jsr update_crc_fast
	
	lda address
	jsr update_crc_fast
.endmacro

.include "instr_test_end.s"

test_values:
	lda #<operand
	sta address
	lda #>operand
	sta <(address+1)
	
	lda #<(operand+1)
	sta <(address+2)
	lda #>(operand+1)
	sta <(address+3)
	
	; Be sure X doesn't have values other than
	; 0 or 2
	lda #0
	jsr :+
	lda #2
:       sta in_x
	lda #$A5
	sta address+1
	sta address+2
	sta address+3
	test_normal
	rts

correct_checksums:
.dword $38EDC104
.dword $56EBEEE1
.dword $87023DFD
.dword $8D85E13F
.dword $F33416FB
.dword $6E7567F9
.dword $B85DBFA2
.dword $62A5170F
.dword $A833A7EF
.dword $97909A7A
.dword $BE33288A
.dword $E333FC8C
.dword $AB6CE261
.dword $2DDE531C
.dword $D0A3AEB1
.dword $B856996B
