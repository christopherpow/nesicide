#ifndef CBINARYFILEMODEL_H
#define CBINARYFILEMODEL_H

#include "model/csubmodel.h"

class CBinaryFileModel : public CSubModel
{
   Q_OBJECT
private:
   friend class CProjectModel;
   CBinaryFileModel();
};

#endif // CBINARYFILEMODEL_H
