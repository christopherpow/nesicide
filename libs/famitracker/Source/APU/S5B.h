#ifndef S5B_H
#define S5B_H

#include "External.h"
#include "Channel.h"

class CS5B : public CExternal {
public:
	CS5B(CMixer *pMixer);
	virtual ~CS5B();
	void	Reset();
	void	Process(uint32 Time);
	void	EndFrame();
	void	Write(uint16 Address, uint8 Value);
	uint8 	Read(uint16 Address, bool &Mapped);
	void	SetSampleSpeed(uint32 SampleRate, double ClockRate, uint32 FrameRate);
	void	SetVolume(float fVol);
//	void	SetChannelVolume(int Chan, int LevelL, int LevelR);
protected:
	void	GetMixMono();
private:
	static float AMPLIFY;
private:
	uint8	m_iRegister;

	uint32	m_iTime;

	float	m_fVolume;

};

#endif /* S5B_H */
