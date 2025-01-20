#pragma once

void assocCircles();
void addToModelSpace(AcDbObjectId &, AcDbEntity *);

class CPersistentReactor : public AcDbObject
{
public:
    ACRX_DECLARE_MEMBERS(CPersistentReactor)
    CPersistentReactor() = default;
    void eLinkage(AcDbObjectId i, double f = 1.)
    {
        mld = i;
        mFactor = f;
    }

    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler) override;
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const override;
    virtual void modified(const AcDbObject *dbObj) override;

private:
    AcDbObjectId mld;
    double mFactor;
};