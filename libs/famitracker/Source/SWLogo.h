#pragma once

class CSWLogo : public CSampleWinState
{
public:
	
	void Activate();
	void Deactivate();
	void Draw(CDC *pDC, bool bMessage);
	void SetSampleRate(int SampleRate);
	void SetSampleData(int *pSamples, unsigned int iCount);
};
