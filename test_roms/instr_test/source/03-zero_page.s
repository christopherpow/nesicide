;CALIBRATE=1
.include "instr_test.inc"

instrs:
	entry $A5,"LDA z" ; AXY = z
	entry $A6,"LDX z"
	entry $A4,"LDY z"
	
	entry $85,"STA z" ; z = AXY
	entry $86,"STX z"
	entry $84,"STY z"
	
	entry $E6,"INC z" ; z = op z
	entry $C6,"DEC z"
	entry $06,"ASL z"
	entry $46,"LSR z"
	entry $26,"ROL z"
	entry $66,"ROR z"
	
	entry $65,"ADC z" ; A = A op z
	entry $E5,"SBC z"
	entry $05,"ORA z"
	entry $25,"AND z"
	entry $45,"EOR z"
	
	entry $24,"BIT z" ; AXY op z
	entry $C5,"CMP z"
	entry $E4,"CPX z"
	entry $C4,"CPY z"
.ifndef OFFICIAL_ONLY
	entry $04,"DOP z"
	entry $44,"DOP z"
	entry $64,"DOP z"
	
	entry $07,"SLO z"
	entry $27,"RLA z"
	entry $47,"SRE z"
	entry $67,"RRA z"
	entry $87,"AAX z"
	entry $A7,"LAX z"
	entry $C7,"DCP z"
	entry $E7,"ISC z"
.endif
instrs_size = * - instrs

operand = <$FE

instr_template:
	lda <operand
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
.dword $A638BE16
.dword $8F55E4CB
.dword $2C23BFFD
.dword $998AF983
.dword $934EC43F
.dword $934EC43F
.dword $934EC43F
.dword $175CAF20
.dword $44B3A36F
.dword $FD0770C0
.dword $68B6E6B2
.dword $EA3A0EB9
.dword $2CEE378F
.dword $0F5E06DC
.dword $B9DAD68A
