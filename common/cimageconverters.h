#ifndef CIMAGECONVERTERS_H
#define CIMAGECONVERTERS_H

#include <QImage>
#include <QByteArray>

#include <stdint.h> // for standard base types...

class CImageConverters
{
public:
    // Create a byte stream from an image of any dimension.
    static QByteArray fromIndexed8(QImage imgIn);

    // Create a x/y dimensioned CHR image from a byte stream.
    // Default x/y dimensions create a CHR bank image.
    static QImage     toIndexed8(QByteArray chrIn,int xSize=256,int ySize=128);

    // Create a x/y dimensioned CHR image from a byte stream using the given color table.
    // Default x/y dimensions create a CHR bank image.
    static QImage     toIndexed8(QByteArray chrIn,QList<int> colorTable,int xSize=256,int ySize=128);

    // Create a x/y dimensioned CHR image from a byte stream using the given attribute map and color table.
    // Default x/y dimensions create a CHR bank image.
    static QImage     toIndexed8(QByteArray chrIn,QByteArray attrIn,QList<uint8_t> colorTable,int xSize=256,int ySize=128);
};

#endif // CIMAGECONVERTERS_H
