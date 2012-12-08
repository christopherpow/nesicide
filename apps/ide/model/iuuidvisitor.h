#ifndef IUUIDVISITOR_H
#define IUUIDVISITOR_H

#include <QUuid>

class CUuid;
class CAttributeUuid;
class CBinaryFileUuid;
class CFilterUuid;
class CGraphicsBankUuid;
class CSourceFileUuid;
class CTileStampUuid;

// ------------------------------------------------------------------------
// This class can be used to obtain type information from a uuid.
// ------------------------------------------------------------------------
class IUuidVisitor
{
public:
   ~IUuidVisitor() { }

   virtual void visit(CUuid&)=0;
   virtual void visit(CAttributeUuid&)=0;
   virtual void visit(CBinaryFileUuid&)=0;
   virtual void visit(CFilterUuid&)=0;
   virtual void visit(CGraphicsBankUuid&)=0;
   virtual void visit(CSourceFileUuid&)=0;
   virtual void visit(CTileStampUuid&)=0;
};

// ------------------------------------------------------------------------
// A Uuid that allows visiting. In that sense it carries type
// information. See ProjectModel.visit* for details.
// ------------------------------------------------------------------------
class CUuid
{
public:
   CUuid(QUuid uuid) : uuid(uuid) { }
   QUuid uuid;

   virtual void accept(IUuidVisitor& visitor) { visitor.visit(*this); }
};

class CAttributeUuid : public CUuid
{
public:
   CAttributeUuid(QUuid uuid) : CUuid(uuid) { }
   virtual void accept(IUuidVisitor& visitor) { visitor.visit(*this); }
};

class CBinaryFileUuid : public CUuid
{
public:
   CBinaryFileUuid(QUuid uuid) : CUuid(uuid) { }
   virtual void accept(IUuidVisitor& visitor) { visitor.visit(*this); }
};

class CFilterUuid : public CUuid
{
public:
   CFilterUuid(QUuid uuid) : CUuid(uuid) { }
   virtual void accept(IUuidVisitor& visitor) { visitor.visit(*this); }
};

class CGraphicsBankUuid : public CUuid
{
public:
   CGraphicsBankUuid(QUuid uuid) : CUuid(uuid) { }
   virtual void accept(IUuidVisitor& visitor) { visitor.visit(*this); }
};

class CSourceFileUuid : public CUuid
{
public:
   CSourceFileUuid(QUuid uuid) : CUuid(uuid) { }
   virtual void accept(IUuidVisitor& visitor) { visitor.visit(*this); }
};

class CTileStampUuid : public CUuid
{
public:
   CTileStampUuid(QUuid uuid) : CUuid(uuid) { }
   virtual void accept(IUuidVisitor& visitor) { visitor.visit(*this); }
};


#endif // IUUIDVISITOR_H
