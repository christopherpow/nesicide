/* emu2149.h */
#ifndef _EMU2149_H_
#define _EMU2149_H_

#ifndef _MAIN_H_
typedef unsigned int uint32 ;
typedef int	int32 ;
typedef signed short int16 ;
typedef unsigned short uint16 ;
typedef signed char int8 ;
typedef unsigned char uint8 ;
#endif

#ifdef EMU2149_DLL_EXPORTS
  #define EMU2149_API __declspec(dllexport)
#elif  EMU2149_DLL_IMPORTS
  #define EMU2149_API __declspec(dllimport)
#else
  #define EMU2149_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

	/* Volume Table */
	uint32 *voltbl ;

	uint8 reg[0x20] ;
	int32 out ;

	uint32 count[3] ;
	uint32 volume[3] ;
	uint32 freq[3] ;
	uint32 edge[3] ;
	uint32 tmask[3] ;
	uint32 nmask[3] ;
  uint32 mute[3] ;

  uint32 base_count ;

	uint32 env_volume ;
	uint32 env_ptr ;
	uint32 env_enable ;
	uint32 env_reverse ;
	uint32 env_hold ;
	uint32 env_alt ;
	uint32 env_freq ;
	uint32 env_count ;

	uint32 noise_seed ;
	uint32 noise_count ;
	uint32 noise_freq ;

	/* rate converter */
	uint32 realstep ;
	uint32 psgtime ;
	uint32 psgstep ;

  /* I/O Ctrl */
  uint32 adr ;

} PSG ;
	
EMU2149_API void PSG_init(uint32 clk, uint32 rate) ;
EMU2149_API void PSG_set_quality(uint32 q) ;
EMU2149_API void PSG_close(void) ;
EMU2149_API PSG *PSG_new(void) ;
EMU2149_API void PSG_reset(PSG *) ;
EMU2149_API void PSG_delete(PSG *) ;
EMU2149_API void PSG_writeReg(PSG *, uint32 reg, uint32 val) ;
EMU2149_API void PSG_writeIO(PSG *psg, uint32 adr, uint32 val) ;
EMU2149_API uint8 PSG_readReg(PSG *psg, uint32 reg) ;
EMU2149_API uint8 PSG_readIO(PSG *psg) ;
EMU2149_API int16 PSG_calc(PSG *) ;
EMU2149_API void PSG_setVolumeMode(PSG *psg, int type) ;

void PSG_set_chan_vol(int chan, int vol_l, int vol_r);
int16 PSG_get_mix_l();
int16 PSG_get_mix_r();

int32 PSG_getchanvol(int i);

#ifdef __cplusplus
}
#endif

#endif
