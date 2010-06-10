;CALIBRATE=1
.include "instr_test.inc"

instrs:
	entry $AD,"LDA a" ; AXY = a
	entry $AE,"LDX a"
	entry $AC,"LDY a"
	
	entry $8D,"STA a" ; a = AXY
	entry $8E,"STX a"
	entry $8C,"STY a"
	
	entry $EE,"INC a" ; a = op a
	entry $CE,"DEC a"
	entry $0E,"ASL a"
	entry $4E,"LSR a"
	entry $2E,"ROL a"
	entry $6E,"ROR a"
	
	entry $6D,"ADC a" ; A = A op a
	entry $ED,"SBC a"
	entry $0D,"ORA a"
	entry $2D,"AND a"
	entry $4D,"EOR a"
	
	entry $CD,"CMP a" ; AXY op a
	entry $2C,"BIT a"
	entry $EC,"CPX a"
	entry $CC,"CPY a"
.ifndef OFFICIAL_ONLY
	entry $0C,"TOP abs"
	
	entry $0F,"SLO abs"
	entry $2F,"RLA abs"
	entry $4F,"SRE abs"
	entry $6F,"RRA abs"
	entry $8F,"AAX abs"
	entry $AF,"LAX abs"
	entry $CF,"DCP abs"
	entry $EF,"ISC abs"
.endif
instrs_size = * - instrs

operand = $2FE

instr_template:
	lda operand
	jmp instr_done
instr_template_size = * - instr_template

.define set_in    set_paxyso
.define check_out check_paxyso

.include "instr_test_end.s"

test_values:
	test_normal
	rts

correct_checksums:
.dword $A2A8D747
.dword $15DD7089
.dword $83F39F34
.dword $B8BDDA66
.dword $A53C93B0
.dword $CB4A9944
.dword $D013DAE1
.dword $D2BF4CD2
.dword $EC7AD495
.dword $AC514937
.dword $A0C024DC
.dword $23F4F940
.dword $B6D77403
.dword $EB3815B9
.dword $BD97B199
.dword $15E280F9
.dword $AED062D5
.dword $8F55E4CB
.dword $A638BE16
.dword $2C23BFFD
.dword $998AF983
.dword $934EC43F
.dword $175CAF20
.dword $44B3A36F
.dword $FD0770C0
.dword $68B6E6B2
.dword $EA3A0EB9
.dword $2CEE378F
.dword $0F5E06DC
.dword $B9DAD68A

