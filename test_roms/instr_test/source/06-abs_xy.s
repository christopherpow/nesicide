;CALIBRATE=1
.include "instr_test.inc"

instrs:
	entry $BD,"LDA a,X" ; AXY = a,XY
	entry $B9,"LDA a,Y"
	entry $BC,"LDY a,X"
	entry $BE,"LDX a,Y"
	
	entry $9D,"STA a,X" ; a,XY = A
	entry $99,"STA a,Y"
	
	entry $FE,"INC a,X" ; a,XY = op a,XY
	entry $DE,"DEC a,X"
	entry $1E,"ASL a,X"
	entry $5E,"LSR a,X"
	entry $3E,"ROL a,X"
	entry $7E,"ROR a,X"
	
	entry $7D,"ADC a,X" ; A = A op a,XY
	entry $79,"ADC a,Y"
	entry $FD,"SBC a,X"
	entry $F9,"SBC a,Y"
	entry $1D,"ORA a,X"
	entry $19,"ORA a,Y"
	entry $3D,"AND a,X"
	entry $39,"AND a,Y"
	entry $5D,"EOR a,X"
	entry $59,"EOR a,Y"
	
	entry $DD,"CMP a,X" ; A op a,XY
	entry $D9,"CMP a,Y"
.ifndef OFFICIAL_ONLY
	entry $1C,"TOP abs,X"
	entry $3C,"TOP abs,X"
	entry $5C,"TOP abs,X"
	entry $7C,"TOP abs,X"
	entry $DC,"TOP abs,X"
	entry $FC,"TOP abs,X"

	entry $1F,"SLO abs,X"
	entry $3F,"RLA abs,X"
	entry $5F,"SRE abs,X"
	entry $7F,"RRA abs,X"
	entry $9C,"SYA abs,X"
	entry $DF,"DCP abs,X"
	entry $FF,"ISC abs,X"

	entry $1B,"SLO abs,Y"
	entry $3B,"RLA abs,Y"
	entry $5B,"SRE abs,Y"
	entry $7B,"RRA abs,Y"
	entry $9E,"SXA abs,Y"
	entry $BF,"LAX abs,Y"
	entry $DB,"DCP abs,Y"
	entry $FB,"ISC abs,Y"
.endif
instrs_size = * - instrs

operand = $2FE

instr_template:
	lda operand
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
.dword $129F9395
.dword $9C01FBA0
.dword $4938459C
.dword $4C8B3BDD
.dword $318545DB
.dword $88E36EA4
.dword $8E98A309
.dword $22EDBFF1
.dword $7F75B40A
.dword $AB0397C3
.dword $B9C6DF9F
.dword $937FB78F
.dword $32A781C9
.dword $10D596B4
.dword $1F29205A
.dword $40584791
.dword $2C86F701
.dword $551E5A68
.dword $80AB400F
.dword $2C833CB8
.dword $6B37AE2B
.dword $AC63458B
.dword $A92FAF9B
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
.dword $8259541D
.dword $19D3E06D
.dword $5C02AF8C
.dword $DCB5D491
.dword $7BB98494
.dword $312E523F
.dword $99AA0039
.dword $617DE967
.dword $B8A8644A
.dword $B64E8914
.dword $58D23FB4
.dword $E8696DF2
