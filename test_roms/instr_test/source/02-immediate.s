;CALIBRATE=1
.include "instr_test.inc"

instrs:
	entry $A9,"LDA #n" ; AXY = #n
	entry $A2,"LDX #n"
	entry $A0,"LDY #n"
	
	entry $69,"ADC #n" ; A = A op #n
	entry $E9,"SBC #n"
	entry $09,"ORA #n"
	entry $29,"AND #n"
	entry $49,"EOR #n"
	
	entry $C9,"CMP #n" ; AXY op #n
	entry $E0,"CPX #n"
	entry $C0,"CPY #n"
.ifndef OFFICIAL_ONLY
	entry $EB,"SBC #n"
	
	entry $80,"DOP #n"
	entry $82,"DOP #n"
	entry $89,"DOP #n"
	entry $C2,"DOP #n"
	entry $E2,"DOP #n"
	
	entry $0B,"AAC #n"
	entry $2B,"AAC #n"
	entry $4B,"ASR #n"
	entry $6B,"ARR #n"
	entry $AB,"ATX #n"
	entry $CB,"AXS #n"
.endif
instrs_size = * - instrs

operand = instr+1

instr_template:
	lda #0
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
.dword $B6D77403
.dword $EB3815B9
.dword $BD97B199
.dword $15E280F9
.dword $AED062D5
.dword $8F55E4CB
.dword $2C23BFFD
.dword $998AF983
.dword $EB3815B9
.dword $934EC43F
.dword $934EC43F
.dword $934EC43F
.dword $934EC43F
.dword $934EC43F
.dword $01E6EF9F
.dword $01E6EF9F
.dword $D8CAAA88
.dword $39479BD4
.dword $2CEE378F
.dword $11DE4052
