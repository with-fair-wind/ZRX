#include <BCPersistentReactor.h>
#include <BCDimension.h>

ACRX_DXF_DEFINE_MEMBERS(DimensionPersistentReactor, AcDbObject,
                        AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
                        0, DIMENSIONPERSISTENTREACTOR, PersistentReactor);

Acad::ErrorStatus DimensionPersistentReactor::dwgInFields(AcDbDwgFiler *pFiler)
{
    assertWriteEnabled();
    AcDbObject::dwgInFields(pFiler);
    pFiler->readItem((AcDbSoftPointerId *)&m_objId);
    return pFiler->filerStatus();
}

Acad::ErrorStatus DimensionPersistentReactor::dwgOutFields(AcDbDwgFiler *pFiler) const
{
    assertReadEnabled();
    AcDbObject::dwgOutFields(pFiler);
    pFiler->writeItem((AcDbSoftPointerId &)m_objId);
    return pFiler->filerStatus();
}

void DimensionPersistentReactor::modified(const AcDbObject *dbObj)
{
    assertReadEnabled();
    if (dbObj->isKindOf(AcDbRotatedDimension::desc()))
    {
        AcDbRotatedDimension *pRotatedDim = AcDbRotatedDimension::cast(dbObj);
        if (!pRotatedDim)
            return;

        double measuredValue = 0;
        pRotatedDim->measurement(measuredValue);
        ACHAR *dimText = pRotatedDim->dimensionText();
        acutPrintf(L"\ndim modify 测量单位：%g\n", measuredValue);
        acutPrintf(L"\ndim modify 文字替代：%s\n", dimText);
        double curDistance = std::wcstod(dimText, nullptr);

        AcDbObjectIdArray geomIdArray;

        if (curDistance != measuredValue && curDistance != 0.)
        {
            AcDbObjectId dictId = pRotatedDim->extensionDictionary();
            AcDbDictionary *pDict = nullptr;
            if (acdbOpenObject(pDict, dictId, AcDb::kForRead) == Acad::eOk)
            {
                AcDbDimAssoc *pDimAssoc = nullptr;
                pDict->getAt(_T("ACAD_DIMASSOC"), pDimAssoc, AcDb::kForRead);
                pDict->close();
                if (pDimAssoc)
                {
                    pDimAssoc->getDimAssocGeomIds(geomIdArray);
                    pDimAssoc->close();
                    for (int i = 0; i < geomIdArray.logicalLength(); ++i)
                    {
                        AcDbEntity *pGeom = nullptr;
                        if (acdbOpenAcDbEntity(pGeom, geomIdArray[i], AcDb::kForWrite) == Acad::eOk)
                        {
                            if (pGeom->isKindOf(AcDbLine::desc()))
                            {
                                AcDbLine *pLine = AcDbLine::cast(pGeom);
                                if (pLine)
                                {
                                    AcDbHandle handle;
                                    pLine->getAcDbHandle(handle);

                                    ACHAR szHandle[256];
                                    handle.getIntoAsciiBuffer(szHandle);

                                    acutPrintf(_T("\n实体句柄: %s "), szHandle);
                                    pLine->close();
                                }
                            }
                            pGeom->close();
                        }
                    }

                    CString strLayer;
                    strLayer.Format(_T("\nEntity:%s has been modified.The layerName:%s.\n"), pRotatedDim->isA()->name(), pRotatedDim->layer());
                    acutPrintf(strLayer);
                    modifyDistance(geomIdArray, measuredValue, curDistance);
                    // bool writeAble = true;
                    // Acad::ErrorStatus es = pRotatedDim->upgradeFromNotify(writeAble);
                }
            }
        }
    }
    else if (dbObj->isKindOf(AcDbLine::desc()))
    {
        AcDbLine *pLine = AcDbLine::cast(dbObj);
        if (!pLine)
            return;

        AcDbRotatedDimension *pRotatedDim = nullptr;
        acdbOpenAcDbEntity((AcDbEntity *&)pRotatedDim, m_objId, ZcDb::kForWrite);
        if (!pRotatedDim)
            return;

        double measuredValue = 0;
        pRotatedDim->measurement(measuredValue);
        ACHAR *dimText = pRotatedDim->dimensionText();
        acutPrintf(L"\nline modify 测量单位：%g\n", measuredValue);
        acutPrintf(L"\nline modify 文字替代：%s\n", dimText);

        pRotatedDim->setDimensionText(_T(""));
        pRotatedDim->close();
    }
}

void cmd_PersistentReactor()
{
    ads_name entres;
    ads_point ptres;
    acedInitGet(NULL, NULL);
    int ret = acedEntSel(_T("\n选择标注: "), entres, ptres);

    if (RTNORM == ret)
    {
        AcDbObjectId objId;
        acdbGetObjectId(objId, entres);
        AcDbEntity *pEnt = nullptr;
        AcDbRotatedDimension *pRotatedDim = nullptr;

        AcDbObjectIdArray geomIdArray;

        if (acdbOpenAcDbEntity(pEnt, objId, AcDb::kForWrite) == Acad::eOk)
        {
            if (pEnt->isKindOf(AcDbRotatedDimension::desc()))
            {
                pRotatedDim = AcDbRotatedDimension::cast(pEnt);

                double measuredValue = 0;
                pRotatedDim->measurement(measuredValue);
                ACHAR *dimText = pRotatedDim->dimensionText();
                // acutPrintf(L"\n测量单位：%g\n", measuredValue);
                // acutPrintf(L"\n文字替代：%s\n", dimText);
                double curDistance = std::wcstod(dimText, nullptr);

                AcDbObjectId dictId = pRotatedDim->extensionDictionary();
                AcDbDictionary *pDict = nullptr;
                if (acdbOpenObject(pDict, dictId, AcDb::kForRead) == Acad::eOk)
                {
                    AcDbDimAssoc *pDimAssoc = nullptr;
                    pDict->getAt(_T("ACAD_DIMASSOC"), pDimAssoc, AcDb::kForRead);
                    pDict->close();
                    if (pDimAssoc)
                    {
                        pDimAssoc->getDimAssocGeomIds(geomIdArray);
                        pDimAssoc->close();
                        for (int i = 0; i < geomIdArray.logicalLength(); ++i)
                        {
                            AcDbEntity *pGeom = nullptr;
                            if (acdbOpenAcDbEntity(pGeom, geomIdArray[i], AcDb::kForWrite) == Acad::eOk)
                            {
                                if (pGeom->isKindOf(AcDbLine::desc()))
                                {
                                    AcDbLine *pLine = AcDbLine::cast(pGeom);
                                    if (pLine)
                                    {
                                        AcDbHandle handle;
                                        pLine->getAcDbHandle(handle);

                                        ACHAR szHandle[256];
                                        handle.getIntoAsciiBuffer(szHandle);

                                        // acutPrintf(_T("\n实体句柄: %s "), szHandle);
                                        pLine->close();
                                    }
                                }
                                pGeom->close();
                            }
                        }
                        // modifyDistance(geomIdArray, measuredValue, curDistance);
                    }
                }
            }
        }

        AcDbObjectId reactorId;
        DimensionPersistentReactor *pReactor = new DimensionPersistentReactor(geomIdArray[1]);

        AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
        AcDbDictionary *pDict = nullptr;
        AcDbDictionary *pNamedList = nullptr;
        pDb->getNamedObjectsDictionary(pDict, ZcDb::kForWrite);
        if (pDict)
        {
            if (pDict->getAt(_T("Dimension_PersistentReactor_Dict"), (AcDbObject *&)pNamedList, ZcDb::kForWrite) == Zcad::eKeyNotFound)
            {
                pNamedList = new AcDbDictionary();
                AcDbObjectId listId;
                pDict->setAt(_T("Dimension_PersistentReactor_Dict"), pNamedList, listId);
            }
            pDict->close();

            if (Acad::eKeyNotFound == pNamedList->getAt(_T("DimensionPeriReactor"), reactorId))
            {
                pNamedList->setAt(_T("DimensionPeriReactor"), pReactor, reactorId);
                pReactor->close();
            }
            else
                delete pReactor;
        }
        pRotatedDim->addPersistentReactor(reactorId);
        pRotatedDim->close();

        pReactor = new DimensionPersistentReactor(objId);
        if (Acad::eKeyNotFound == pNamedList->getAt(_T("LinePeriReactor"), reactorId))
        {
            pNamedList->setAt(_T("LinePeriReactor"), pReactor, reactorId);
            pReactor->close();
        }
        else
            delete pReactor;

        AcDbEntity *pEnt_1 = nullptr;
        if (acdbOpenAcDbEntity(pEnt_1, geomIdArray[1], AcDb::kForWrite) == Acad::eOk)
        {
            if (pEnt_1->isKindOf(AcDbLine::desc()))
            {
                AcDbLine *pLine = AcDbLine::cast(pEnt_1);
                if (pLine)
                {
                    pLine->addPersistentReactor(reactorId);
                    pLine->close();
                }
            }
        }

        pNamedList->close();
    }
}