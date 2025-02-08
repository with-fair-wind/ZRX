#include <BCLineDimensionPE.h>
#include <BCDimensionAssoc.h>
#include <BCDimension.h>

ACRX_NO_CONS_DEFINE_MEMBERS(BCLineDimensionPE, AcRxObject);

void BCLineDimensionPE::modified(AcDbObject *pObj, const AcDbObject *pAssocObj)
{
    if (!isUsAble())
        return;
    AcDbObjectId assocId = pObj->objectId();
    if (assocId.isNull())
        return;
    BCDimensionAssoc *pAssoc = BCDimensionAssoc::cast(pObj);
    if (!pAssoc)
        return;
    const AcDbObjectId entId = pAssocObj->objectId();
    if (entId.isNull())
        return;

    m_assocIdData = {assocId, AssocData{}};

    if (pAssoc->dimensionId() == entId)
        m_assocIdData.second._dimensionId = entId;
}

void BCLineDimensionPE::close(bool bUpdate)
{
    setUsAble(false);
    if (bUpdate)
        update(m_assocIdData.first, m_assocIdData.second);
    clear();
}

void BCLineDimensionPE::update(const AcDbObjectId &assocId, const AssocData &assocData)
{
    if (!assocData._dimensionId.isNull())
    {
        AcDbObjectPointer<BCDimensionAssoc> pAssoc(assocId, AcDb::kForRead);
        if (pAssoc)
        {
            AcDbObjectPointer<AcDbRotatedDimension> pRotatedDim(pAssoc->dimensionId(), AcDb::kForRead);
            double measuredValue = 0;
            pRotatedDim->measurement(measuredValue);
            modifyDistance(pAssoc->lineId(), measuredValue, 100);

            AcDbObjectId dictId = pRotatedDim->extensionDictionary();
            AcDbDictionary *pDict = nullptr;
            if (acdbOpenObject(pDict, dictId, AcDb::kForRead) == Acad::eOk)
            {
                AcDbDimAssoc *pDimAssoc = nullptr;
                pDict->getAt(_T("ACAD_DIMASSOC"), pDimAssoc, AcDb::kForWrite);
                pDict->close();
                if (pDimAssoc)
                {
                    Acad::ErrorStatus es = pDimAssoc->updateDimension();
                    pDimAssoc->close();
                }
            }
        }
    }
}