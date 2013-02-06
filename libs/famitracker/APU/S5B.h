#ifndef _S5B_H_
#define _S5B_H_

#include "external.h"
#include "channel.h"

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
	uint8	m_iChannels;

	uint32	m_iTime;

	float	m_fVolume;

};

#endif /* _S5B_H_ */
