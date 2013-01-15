#include "cmusicfamitrackerdata.h"
#include "famitracker/Common.h"
#include "famitracker/APU/APU.h"

#include "cqtmfc.h"

CMusicFamiTrackerData::CMusicFamiTrackerData()
{
   pDoc = new CFamiTrackerDoc();

   CFamiTrackerApp::GetSoundGenerator()->start();
}

CMusicFamiTrackerData::~CMusicFamiTrackerData()
{
   CFamiTrackerApp::GetSoundGenerator()->terminate();

   delete pDoc;
}
