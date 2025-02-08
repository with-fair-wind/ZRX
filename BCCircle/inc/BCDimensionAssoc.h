#pragma once

#include <dbdimassoc.h>

class BCDimensionAssoc : public AcDbObject
{
public:
    ACRX_DECLARE_MEMBERS(BCDimensionAssoc);
    BCDimensionAssoc() = default;
    virtual ~BCDimensionAssoc() = default;

    AcDbObjectId dimensionId() const
    {
        assertReadEnabled();
        return m_dimensionId;
    }

    void setDimensionId(const AcDbObjectId &objId)
    {
        assertWriteEnabled();
        m_dimensionId = objId;
    }

    AcDbObjectId lineId() const
    {
        assertReadEnabled();
        return m_lineId;
    }

    void setLineId(const AcDbObjectId &objId)
    {
        assertWriteEnabled();
        m_lineId = objId;
    }

public:
    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler) override;
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const override;

    virtual void modifiedGraphics(const AcDbEntity *dbEnt) override;

private:
    AcDbObjectId m_dimensionId = AcDbObjectId::kNull;
    AcDbObjectId m_lineId = AcDbObjectId::kNull;
};

void test_dimension();