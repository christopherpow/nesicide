#ifndef CMUSICFAMITRACKERDATA_H
#define CMUSICFAMITRACKERDATA_H

#include "famitracker/FamiTrackerDoc.h"
#include "famitracker/SoundGen.h"
#include "famitracker/ChannelMap.h"

class CMusicFamiTrackerData
{
public:
   CMusicFamiTrackerData();
   ~CMusicFamiTrackerData();

   CFamiTrackerDoc* GetDocument() { return pDoc; }

protected:
   CFamiTrackerDoc* pDoc;
};

#endif // CMUSICFAMITRACKERDATA_H
