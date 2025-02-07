#pragma once

class CustomClass : public AcDbObject
{
public:
    ACRX_DECLARE_MEMBERS(CustomClass);
    CustomClass() : mIntval(0) {}
    CustomClass(const Adesk::Int16 &val) : mIntval(val) {}

    Adesk::Int16 getIntData() const
    {
        assertReadEnabled();
        return mIntval;
    }
    Acad::ErrorStatus setIntData(const Adesk::Int16 &val)
    {
        assertWriteEnabled();
        mIntval = val;
        return Acad::eOk;
    }
    AcDbHardOwnershipId getIdData() const
    {
        assertReadEnabled();
        return mObjId;
    }
    Acad::ErrorStatus setIdData(const AcDbHardOwnershipId &val);

    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler) override;
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const override;
    virtual Acad::ErrorStatus subErase(Adesk::Boolean erasing) override;

private:
    Adesk::Int16 mIntval;
    AcDbHardOwnershipId mObjId;
};

void createObjs();
void listTree();
void eraseTree();
void printOut(AcDbObjectId id);
