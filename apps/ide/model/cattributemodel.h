#ifndef CATTRIBUTEMODEL_H
#define CATTRIBUTEMODEL_H

#include "model/csubmodel.h"

class CAttributeModel : public CSubModel
{
   Q_OBJECT
private:
   friend class CProjectModel;
   CAttributeModel();
};

#endif // CATTRIBUTEMODEL_H
