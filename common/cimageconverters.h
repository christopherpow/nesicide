#ifndef CIMAGECONVERTERS_H
#define CIMAGECONVERTERS_H

#include <QImage>
#include <QByteArray>

class CImageConverters
{
public:
    CImageConverters();

    static QByteArray fromIndexed8(QImage imgIn);
    static QImage     toIndexed8(QByteArray chrIn);
};

#endif // CIMAGECONVERTERS_H
