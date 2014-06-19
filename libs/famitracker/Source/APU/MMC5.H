#ifndef MMC5_H
#define MMC5_H

#include "External.h"
#include "Channel.h"

class CMMC5 : public CExternal {
public:
	CMMC5(CMixer *pMixer);
	virtual ~CMMC5();

	void Reset();
	void Write(uint16 Address, uint8 Value);
	uint8 Read(uint16 Address, bool &Mapped);
	void EndFrame();
	void Process(uint32 Time);
	void LengthCounterUpdate();
	void EnvelopeUpdate();

private:	
	CSquare	*m_pSquare1;
	CSquare	*m_pSquare2;
	uint8	*m_pEXRAM;
	uint8	m_iMulLow;
	uint8	m_iMulHigh;
};

#endif /* MMC5_H */