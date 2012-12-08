#ifndef CGRAPHICSBANKMODEL_H
#define CGRAPHICSBANKMODEL_H

#include "model/csubmodel.h"

class CGraphicsBankModel : public CSubModel
{
   Q_OBJECT
private:
   friend class CProjectModel;
   CGraphicsBankModel();
};

#endif // CGRAPHICSBANKMODEL_H
