#ifndef CFILTERMODEL_H
#define CFILTERMODEL_H

#include "model/csubmodel.h"

class CFilterModel : public CSubModel
{
   Q_OBJECT
private:
   friend class CProjectModel;
   CFilterModel();
};

#endif // CFILTERMODEL_H
