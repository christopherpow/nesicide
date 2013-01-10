#include "cmusicfamitrackerdata.h"
#include "famitracker/Common.h"
#include "famitracker/APU/APU.h"

#define GET_PATTERN(Frame, Channel) m_pSelectedTune->GetFramePattern(Frame, Channel)

CSoundGen* pSG = new CSoundGen();
MFCCRAP theApp(pSG);

CMusicFamiTrackerData::CMusicFamiTrackerData()
{
   pDoc = new CFamiTrackerDoc();
}

CMusicFamiTrackerData::~CMusicFamiTrackerData()
{
   delete pDoc;
}
