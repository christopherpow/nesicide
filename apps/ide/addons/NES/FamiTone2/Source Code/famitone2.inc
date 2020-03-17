;this CA65 file automatically reserves RAM for FamiTone2 variables and includes the code

.segment "ZEROPAGE"
FT_TEMP:		.res FT_TEMP_SIZE

.segment "FAMITONE"
FT_BASE_ADR:	.res FT_BASE_SIZE

.segment "CODE"
.include "famitone2.s"
