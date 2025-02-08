#include <BCDimensionAssoc.h>
#include <BCLineDimensionPE.h>

ACRX_DXF_DEFINE_MEMBERS(BCDimensionAssoc, AcDbObject,
                        AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
                        0, DIMENSIONASSOC, BCDimensionAssoc);

Acad::ErrorStatus BCDimensionAssoc::dwgInFields(AcDbDwgFiler *pFiler)
{
    assertWriteEnabled();
    AcDbObject::dwgInFields(pFiler);
    pFiler->readItem((AcDbSoftPointerId *)&m_dimensionId);
    pFiler->readItem((AcDbSoftPointerId *)&m_lineId);
    return pFiler->filerStatus();
}

Acad::ErrorStatus BCDimensionAssoc::dwgOutFields(AcDbDwgFiler *pFiler) const
{
    assertReadEnabled();
    AcDbObject::dwgOutFields(pFiler);
    pFiler->writeItem((AcDbSoftPointerId &)m_dimensionId);
    pFiler->writeItem((AcDbSoftPointerId &)m_lineId);
    return pFiler->filerStatus();
}

void BCDimensionAssoc::modifiedGraphics(const AcDbEntity *dbEnt)
{
    BCLineDimensionPE *pLineDimensionPE = BCLineDimensionPE::cast(isA()->queryX(BCLineDimensionPE::desc()));
    if (pLineDimensionPE)
        pLineDimensionPE->modified(this, dbEnt);
}

void test_dimension()
{
    ads_name entres;
    ads_point ptres;
    acedInitGet(NULL, NULL);
    int ret = acedEntSel(_T("\nÑ¡Ôñ±ê×¢: "), entres, ptres);

    if (RTNORM == ret)
    {
        AcDbObjectId objId;
        acdbGetObjectId(objId, entres);
        AcDbEntity *pEnt = nullptr;

        if (acdbOpenAcDbEntity(pEnt, objId, AcDb::kForWrite) == Acad::eOk)
        {
            if (pEnt->isKindOf(AcDbRotatedDimension::desc()))
            {
                AcDbRotatedDimension *pRotatedDim = AcDbRotatedDimension::cast(pEnt);
                AcDbObjectIdArray geomIdArray;

                AcDbObjectId dictId = pRotatedDim->extensionDictionary();
                AcDbDictionary *pDict = nullptr;
                if (acdbOpenObject(pDict, dictId, AcDb::kForRead) == Acad::eOk)
                {
                    AcDbDimAssoc *pDimAssoc = nullptr;
                    pDict->getAt(_T("ACAD_DIMASSOC"), pDimAssoc, AcDb::kForRead);
                    if (pDimAssoc)
                    {
                        pDimAssoc->getDimAssocGeomIds(geomIdArray);
                        pDimAssoc->close();
                    }

                    AcDbObjectId assocId = AcDbObjectId::kNull;
                    pDict->getAt(_T("BCDIMENSIONASSOC"), assocId);
                    if (assocId.isNull())
                    {
                        pDict->upgradeOpen();
                        BCDimensionAssoc *pBCDimensionAssoc = new BCDimensionAssoc();
                        pDict->setAt(_T("BCDIMENSIONASSOC"), pBCDimensionAssoc, assocId);
                        pBCDimensionAssoc->close();
                    }
                    pDict->close();

                    if (!assocId.isNull())
                    {
                        BCDimensionAssoc *pBCDimensionAssoc = nullptr;
                        if (acdbOpenObject(pBCDimensionAssoc, assocId, AcDb::kForWrite) == Acad::eOk)
                        {
                            pBCDimensionAssoc->setDimensionId(objId);
                            pBCDimensionAssoc->setLineId(geomIdArray[1]);
                            pBCDimensionAssoc->close();
                            pRotatedDim->addPersistentReactor(assocId);
                            pRotatedDim->close();
                        }
                    }
                }
            }
        }
    }
}