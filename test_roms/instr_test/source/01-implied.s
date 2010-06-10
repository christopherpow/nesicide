;CALIBRATE=1
.include "instr_test.inc"
instrs:
	entry $2A,"ROL A" ; A = op A
	entry $0A,"ASL A"
	entry $6A,"ROR A"
	entry $4A,"LSR A"
	
	entry $8A,"TXA" ; AXY = AXY
	entry $98,"TYA"
	entry $AA,"TAX"
	entry $A8,"TAY"
	
	entry $E8,"INX" ; XY = op XY
	entry $C8,"INY"
	entry $CA,"DEX"
	entry $88,"DEY"
	
	entry $38,"SEC" ; flags = op flags
	entry $18,"CLC"
	entry $F8,"SED"
	entry $D8,"CLD"
	entry $78,"SEI"
	entry $58,"CLI"
	entry $B8,"CLV"
	
	entry $EA,"NOP"
	
.ifndef OFFICIAL_ONLY
	entry $1A,"NOP"
	entry $3A,"NOP"
	entry $5A,"NOP"
	entry $7A,"NOP"
	entry $DA,"NOP"
	entry $FA,"NOP"
.endif
instrs_size = * - instrs

instr_template:
	nop
	jmp instr_done
instr_template_size = * - instr_template

operand = in_a

.define set_in    set_paxyso
.define check_out check_paxyso

.include "instr_test_end.s"

test_values:
	test_normal
	rts

correct_checksums:
.dword $4ED61941
.dword $69A5CDF1
.dword $6FA2BE11
.dword $AE058528
.dword $59F51A4E
.dword $705E94BB
.dword $2CEE378F
.dword $BAC0D832
.dword $B06E4B99
.dword $9FB24D63
.dword $B430167D
.dword $71F2E9FD
.dword $D9241413
.dword $B6DEB45D
.dword $73B048B6
.dword $C869DCAE
.dword $6618DE93
.dword $9BF72C72
.dword $3CCB5D58
.dword $AA7D8339
.dword $AA7D8339
.dword $AA7D8339
.dword $AA7D8339
.dword $AA7D8339
.dword $AA7D8339
.dword $AA7D8339
