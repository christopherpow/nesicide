PPU_CTRL	=$2000
PPU_MASK	=$2001
PPU_STATUS	=$2002
PPU_OAM_ADDR=$2003
PPU_OAM_DATA=$2004
PPU_SCROLL	=$2005
PPU_ADDR	=$2006
PPU_DATA	=$2007
PPU_OAM_DMA	=$4014
PPU_FRAMECNT=$4017
DMC_FREQ	=$4010
CTRL_PORT1	=$4016


OAM_BUF		=$0200
PAL_BUF		=$01b0


FRAMECNT1	=$00
FRAMECNT2	=$01
NTSCMODE	=$02
VRAMUPDATE	=$03
PAD_STATE	=$04
PAD_STATEP	=$05
PAD_STATET	=$06
FT_TEMP		=$07	;7 bytes in zeropage
SCROLL_X	=$0e
SCROLL_Y	=$0f
PPU_CTRL_VAR=$10
RAND_SEED	=$11
NAME_UPD_ADR=$12	;word
NAME_UPD_LEN=$14
TEMP		=$15

PAD_BUF		=TEMP

PTR			=TEMP	;word
LEN			=TEMP+2	;word
NEXTSPR		=TEMP+4
SCRX		=TEMP+5
SCRY		=TEMP+6
SRC			=TEMP+7
DST			=TEMP+9

RLE_LOW		=TEMP
RLE_HIGH	=TEMP+1
RLE_TAG		=TEMP+2
RLE_BYTE	=TEMP+3


FT_BASE_ADR		=$0100	;page in RAM, should be $xx00
FT_DPCM_OFF		=$ff00	;$c000 or higher, 64-byte steps
FT_SFX_STREAMS	=4		;number of sound effects played at once, can be 4 or less
FT_DPCM_PTR		=(FT_DPCM_OFF&$3fff)>>6

.define FT_DPCM_ENABLE 1			;undefine to exclude all the DMC code
.define FT_SFX_ENABLE  1			;undefine to exclude all the sound effects code
.define FT_THREAD      1			;undefine if you call sound effects in the same thread as sound update