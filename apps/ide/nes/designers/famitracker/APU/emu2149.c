/****************************************************************************

  emu2149.c -- YM2149/AY-3-8910 emulator by Mitsutaka Okazaki 2001

  2001 04-28 : Version 1.00beta -- 1st Beta Release.
  2001 08-14 : Version 1.10
  2001 10-03 : Version 1.11     -- Added PSG_set_quality().
  
  References: 
    psg.vhd        -- 2000 written by Kazuhiro Tsujikawa.
    s_fme7.c       -- 1999,2000 written by Mamiya (NEZplug).
    ay8910.c       -- 1998-2001 Author unknown (MAME).
    MSX-Datapack   -- 1991 ASCII Corp.
    AY-3-8910 data sheet
    
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emu2149.h"

#if defined(_MSC_VER)
#define INLINE __forceinline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif

#define EMU2149_VOL_DEFAULT 1
#define EMU2149_VOL_YM2149 0
#define EMU2149_VOL_AY_3_8910 1

static uint32 voltbl[2][32]=
{
	{  0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x09,0x0B,0x0D,0x0F,0x12,
	   0x16,0x1A,0x1F,0x25,0x2D,0x35,0x3F,0x4C,0x5A,0x6A,0x7F,0x97,0xB4,0xD6,0xFF,0xFF },
	{  0x00,0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x05,0x05,0x07,0x07,0x0B,0x0B,0x0F,0x0F,
	   0x16,0x16,0x1F,0x1F,0x2D,0x2D,0x3F,0x3F,0x5A,0x5A,0x7F,0x7F,0xB4,0xB4,0xFF,0xFF }
} ;

#define GETA_BITS 24
static uint32 clk, rate, base_incr, HQ ;

EMU2149_API void PSG_init(uint32 c, uint32 r)
{
	clk = c ;
	rate = r ;
  PSG_set_quality(0) ;
}

EMU2149_API void PSG_set_quality(uint32 q)
{
  if(q)
    base_incr = 1<<GETA_BITS ;
  else if(rate!=0)
    base_incr = (uint32)((double)clk * (1<<GETA_BITS) / (16*rate) ) ;
  /* assert( base_incr < (1<<32) ) */

  HQ = q ;
}

EMU2149_API PSG *PSG_new()
{
	PSG *psg ;

	psg = malloc(sizeof(PSG)) ;
	if( psg == NULL ) return NULL ;

	PSG_setVolumeMode(psg,EMU2149_VOL_DEFAULT) ;

	return psg ;
}

EMU2149_API void PSG_setVolumeMode(PSG *psg, int type)
{
	switch(type)
  {
	case 1:
		psg->voltbl = voltbl[EMU2149_VOL_YM2149] ;
		break ;
	case 2:
		psg->voltbl = voltbl[EMU2149_VOL_AY_3_8910] ;
		break ;
	default:
		psg->voltbl = voltbl[EMU2149_VOL_DEFAULT] ;
		break ;
	}
}

EMU2149_API void PSG_reset(PSG *psg)
{
	int i ;

  psg->base_count = 0 ;

	for(i=0;i<3;i++){
		psg->count[i] = 0x1000 ;
		psg->freq[i] = 0 ;
		psg->edge[i] = 0 ;
		psg->volume[i] = 0 ;
    psg->mute[i] = 0 ;
	}

	for(i=0;i<16;i++) psg->reg[i] = 0 ;
  psg->adr = 0 ;

	psg->realstep = (uint32)((1<<31)/rate) ;
	psg->psgstep =  (uint32)((1<<31)/(clk/16)) ;
	psg->psgtime = 0 ;

	psg->noise_seed = 0xffff ;
	psg->noise_count = 0x40 ;
	psg->noise_freq = 0 ;

	psg->env_volume = 0 ;
	psg->env_ptr = 0 ;
	psg->env_reverse = 0 ;
	psg->env_freq = 0 ;
	psg->env_count = 0 ;
	psg->env_enable = 0 ;

	psg->out = 0 ;
}

EMU2149_API void PSG_delete(PSG *psg)
{
	free(psg) ;
}

EMU2149_API void PSG_close()
{
}

EMU2149_API uint8 PSG_readIO(PSG *psg)
{
  return (uint8)(psg->reg[psg->adr]) ;
}

EMU2149_API uint8 PSG_readReg(PSG *psg, uint32 reg) 
{

	return (uint8)(psg->reg[reg&0x1f]) ;

}

EMU2149_API void PSG_writeIO(PSG *psg, uint32 adr, uint32 val)
{
  if(adr & 1) PSG_writeReg(psg, psg->adr, val) ;
  else psg->adr = val & 0x1f ;
}

EMU2149_API void PSG_writeReg(PSG *psg, uint32 reg, uint32 val)
{

	int c ;

	if (reg>15) return ;

	psg->reg[reg] = (uint8)(val & 0xff) ;

	switch(reg)
  {
	case 0: case 2: case 4:
	case 1: case 3: case 5:
		c = reg >> 1 ;	
		psg->freq[c] = ( (psg->reg[c*2+1]&15) << 8 ) + psg->reg[c*2] ;
 		break ;

	case 6:
    psg->noise_freq = val==0 ? 1 : ((val & 31) << 1) ;
		break ;
	
	case 7:
		psg->tmask[0] =  (val&1) ; 
		psg->tmask[1] =  (val&2) ; 
		psg->tmask[2] =  (val&4) ;
		psg->nmask[0] = (val&8)  ; 
		psg->nmask[1] = (val&16) ; 
		psg->nmask[2] = (val&32) ;
		break ;

	case 8:
	case 9:
	case 10:
		psg->volume[reg-8] = val << 1 ;
    if(val&16) psg->env_enable = 1 ;
    break ;

	case 11:
	case 12:
		psg->env_freq = (psg->reg[12] << 8) + psg->reg[11] ;
		break ; 

	case 13:
		val = val&15 ;
		if(val<4) val = 9 ;	
    else if(val<8) val = 15 ;
		psg->env_count = 0x10000 - psg->env_freq ;
		psg->env_ptr = 0 ;
		psg->env_hold = (val&1)^1 ;
		psg->env_reverse = ((val>>2)&1)^1 ;
		psg->env_alt = ((val>>1)&1)^(val&1) ;
		break ;
		
	case 14:
	case 15:
	default:
		break;

	}

	return ;

}

int psg_chan_vols_l[3], psg_chan_vols_r[3];

int16 mix_l, mix_r;

void PSG_set_chan_vol(int chan, int vol_l, int vol_r)
{
	psg_chan_vols_l[chan] = vol_l;
	psg_chan_vols_r[chan] = vol_r;
}

int16 PSG_get_mix_l()
{
	return mix_l << 4 ;
}

int16 PSG_get_mix_r()
{
	return mix_r << 4 ;
}

int32 psg_volumes[3];

int32 PSG_getchanvol(int i)
{
	return psg_volumes[i];
}

INLINE static int16 calc(PSG *psg)
{

  int i,noise ;
  uint32 incr ;
  int16 mix = 0 ;
  int16 val = 0;

  mix_l = 0 ;
  mix_r = 0 ;

  psg->base_count += base_incr ;
  incr = (psg->base_count >> GETA_BITS) ;
  psg->base_count &= (1 << GETA_BITS) - 1 ;

	/* Envelope */
	if(psg->env_enable)
  {
    psg->env_count += incr ;
		if(psg->env_count&0x10000)
    {
			psg->env_ptr = (psg->env_ptr + 1)&31 ;
			if(psg->env_ptr == 0)
      {
				psg->env_enable = psg->env_hold ;
				psg->env_reverse = psg->env_reverse^psg->env_alt ;
			}
		  psg->env_count -= psg->env_freq ;
		}
	}

	/* Noise */
  psg->noise_count += incr ;
  if(psg->noise_count&0x40)
  {
		if(psg->noise_seed&1) psg->noise_seed^= 0x24000 ;
    psg->noise_seed >>= 1 ;
		psg->noise_count -= psg->noise_freq ;
	}

	noise = psg->noise_seed & 1 ;

	/* Tone */
	for(i=0;i<3;i++)
  {
    psg->count[i] += incr ;
		if(psg->count[i]&0x1000)
    {
       if(psg->freq[i]>1)
       {
         psg->edge[i] = !psg->edge[i] ;
         psg->count[i] -= psg->freq[i] ;
       }
       else
       {
         psg->edge[i] = 1 ;
       }
    }

    if(psg->mute[i]) continue ;

    if((psg->tmask[i]||psg->edge[i])&&(psg->nmask[i]||noise))
    {
		if((psg->volume[i]&32)==0) {
			val = (int16)psg->voltbl[psg->volume[i]&31];
		}
		else if(psg->env_reverse) {
			val = (int16)psg->voltbl[31-psg->env_ptr];
		}
		else {
			val = (int16)psg->voltbl[psg->env_ptr];
		}
		}
    else if(psg->tmask[i]&&psg->nmask[i])
    {
      val = (int16)psg->voltbl[psg->volume[i]&31];
    }
	psg_volumes[i] = val;
	mix += val;
	}

	return mix ;

}

EMU2149_API int16 PSG_calc(PSG *psg)
{
  if(!HQ) return calc(psg) << 4 ;

  /* Simple rate converter */
  while (psg->realstep > psg->psgtime)
  {
    psg->psgtime += psg->psgstep ;
    psg->out += calc(psg) ;
	  psg->out >>= 1 ;

//	  mix_l >>= 1 ;
//	  mix_r >>= 1 ;
  }

  psg->psgtime = psg->psgtime - psg->realstep ;
  
  return (int16)( psg->out << 4 ) ; 
}
