; Micro Code 65 Common Run-Time for ca65 Assembler
.import frame, main, interrupt

.SEGMENT "VECT"
	.word frame, main, interrupt

