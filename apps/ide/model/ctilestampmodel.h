#ifndef CTILESTAMPMODEL_H
#define CTILESTAMPMODEL_H

#include "model/csubmodel.h"

class CTileStampModel : public CSubModel
{
   Q_OBJECT
private:
   friend class CProjectModel;
   CTileStampModel();
};

#endif // CTILESTAMPMODEL_H
