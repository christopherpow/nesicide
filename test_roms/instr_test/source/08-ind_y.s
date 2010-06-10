;CALIBRATE=1
.include "instr_test.inc"

instrs:
	entry $B1,"LDA (z),Y" ; A = (z),Y
	
	entry $91,"STA (z),Y" ; (z),Y = A
	
	entry $D1,"CMP (z),Y" ; A op (z),Y
	
	entry $11,"ORA (z),Y" ; A = A op (z),Y
	entry $F1,"SBC (z),Y"
	entry $71,"ADC (z),Y"
	entry $31,"AND (z),Y"
	entry $51,"EOR (z),Y"
.ifndef OFFICIAL_ONLY
	entry $13,"SLO (z),Y"
	entry $33,"RLA (z),Y"
	entry $53,"SRE (z),Y"
	entry $73,"RRA (z),Y"
	entry $B3,"LAX (z),Y"
	entry $D3,"DCP (z),Y"
	entry $F3,"ISC (z),Y"
.endif
instrs_size = * - instrs

address = <$FF
operand = $2FF

instr_template:
	lda (address),y
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
	
	lda #0
	jsr :+
	lda #1
:       sta in_y
	test_normal
	rts

correct_checksums:
.dword $3CBFEB4E
.dword $52B9C4AB
.dword $835017B7
.dword $6A274DB3
.dword $F7663CB1
.dword $89D7CB75
.dword $BC0F95E8
.dword $66F73D45
.dword $AC618DA5
.dword $93C2B030
.dword $BA6102C0
.dword $E761D6C6
.dword $4E13D288
.dword $D4F184FB
.dword $BC04B321
