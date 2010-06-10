;CALIBRATE=1
.include "instr_test.inc"

instrs:
	entry $90,"BCC r" ; PC = PC op flags
	entry $50,"BVC r"
	entry $D0,"BNE r"
	entry $30,"BMI r"
	entry $10,"BPL r"
	entry $F0,"BEQ r"
	entry $B0,"BCS r"
	entry $70,"BVS r"
instrs_size = * - instrs

zp_byte operand

instr_template:
	bne :+
	sta operand
:       jmp instr_done
instr_template_size = * - instr_template

values2:
	.byte 0,$FF,$01,$02,$04,$08,$10,$20,$40,$80
values2_size = * - values2

.macro set_in
	sta in_p
	set_paxyso
.endmacro

.define check_out check_paxyso

.include "instr_test_end.s"

test_values:
	test_normal
	rts

correct_checksums:
.dword $8F012689
.dword $BDC22BFD
.dword $10579B0A
.dword $678BDA3B
.dword $CF6A1316
.dword $B8B65227
.dword $27E0EFA4
.dword $1523E2D0
