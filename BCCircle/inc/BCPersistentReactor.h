#pragma once

#include <dbdimassoc.h>

class DimensionPersistentReactor : public AcDbObject
{
public:
    ACRX_DECLARE_MEMBERS(DimensionPersistentReactor);
    DimensionPersistentReactor() = default;
    DimensionPersistentReactor(AcDbObjectId objId) : m_objId{objId} {}
    virtual ~DimensionPersistentReactor() = default;

public:
    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler) override;
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const override;

    virtual void modified(const AcDbObject *dbObj) override;

private:
    AcDbObjectId m_objId;
};

void cmd_PersistentReactor();