#ifndef CIMAGECONVERTERS_H
#define CIMAGECONVERTERS_H

#include <QImage>

class CImageConverters
{
public:
    CImageConverters();

    static QByteArray fromIndexed8(QImage imgIn);
};

#endif // CIMAGECONVERTERS_H
