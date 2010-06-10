;CALIBRATE=1
.include "instr_test.inc"

instrs:
	entry $B5,"LDA z,X" ; AXY = z,XY
	entry $B4,"LDY z,X"
	entry $B6,"LDX z,Y"
	
	entry $95,"STA z,X" ; z,XY = AXY
	entry $94,"STY z,X"
	entry $96,"STX z,Y"
	
	entry $F6,"INC z,X" ; z,XY = op z,XY
	entry $D6,"DEC z,X"
	entry $16,"ASL z,X"
	entry $56,"LSR z,X"
	entry $36,"ROL z,X"
	entry $76,"ROR z,X"
	
	entry $75,"ADC z,X" ; A = A op z,XY
	entry $F5,"SBC z,X"
	entry $15,"ORA z,X"
	entry $35,"AND z,X"
	entry $55,"EOR z,X"
	
	entry $D5,"CMP z,X" ; A op z,XY
		
.ifndef OFFICIAL_ONLY
	entry $14,"DOP z,X"
	entry $34,"DOP z,X"
	entry $54,"DOP z,X"
	entry $74,"DOP z,X"
	entry $D4,"DOP z,X"
	entry $F4,"DOP z,X"

	entry $17,"SLO z,X"
	entry $37,"RLA z,X"
	entry $57,"SRE z,X"
	entry $77,"RRA z,X"
	entry $D7,"DCP z,X"
	entry $F7,"ISC z,X"
	
	entry $97,"AAX z,Y"
	entry $B7,"LAX z,Y"
.endif
instrs_size = * - instrs

operand = <$FE

instr_template:
	lda <operand
	jmp instr_done
instr_template_size = * - instr_template

.macro set_in
	lda values+1,y
	sta <(operand+1)
	
	lda values+2,y
	sta <(operand+2)
	
	set_paxyso
.endmacro

.macro check_out
	check_paxyso
	
	lda <(operand+1)
	jsr update_crc_fast
	
	lda <((operand+2)&$FF)
	jsr update_crc_fast
.endmacro

.include "instr_test_end.s"

test_values:
	lda #1
	jsr :+
	lda #2
:       sta in_x
	eor #3
	sta in_y
	test_normal
	rts

correct_checksums:
.dword $74AC0591
.dword $9C01FBA0
.dword $4938459C
.dword $4C8B3BDD
.dword $5F3FDDF5
.dword $04B0EC16
.dword $88E36EA4
.dword $8E98A309
.dword $22EDBFF1
.dword $7F75B40A
.dword $AB0397C3
.dword $B9C6DF9F
.dword $937FB78F
.dword $10D596B4
.dword $40584791
.dword $551E5A68
.dword $2C833CB8
.dword $AC63458B
.dword $0141F400
.dword $0141F400
.dword $0141F400
.dword $0141F400
.dword $0141F400
.dword $0141F400
.dword $EB55F719
.dword $40B406D1
.dword $3DDF8B9E
.dword $0CB8A74E
.dword $19D3E06D
.dword $5C02AF8C
.dword $9778A0A0
.dword $B8A8644A

