#include <BCDimension.h>

AcDbObjectId g_lineId;
double g_oriDistance;

void test_dim()
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
        if (acdbOpenAcDbEntity(pEnt, objId, AcDb::kForRead) == Acad::eOk)
        {
            if (pEnt->isKindOf(AcDbRotatedDimension::desc()))
            {
                AcDbRotatedDimension *pRotatedDim = AcDbRotatedDimension::cast(pEnt);
                if (pRotatedDim)
                {
                    double measuredValue = 0;
                    pRotatedDim->measurement(measuredValue);
                    acutPrintf(L"测量单位：%g\n", measuredValue);

                    ACHAR *dimText = pRotatedDim->dimensionText();
                    acutPrintf(L"文字替代：%s\n", dimText);

                    AcDbObjectId dictId = pRotatedDim->extensionDictionary();
                    AcDbDictionary *pDict = nullptr;
                    if (acdbOpenObject(pDict, dictId, AcDb::kForRead) == Acad::eOk)
                    {
                        AcDbDimAssoc *pDimAssoc = nullptr;
                        pDict->getAt(_T("ACAD_DIMASSOC"), pDimAssoc, AcDb::kForRead);
                        pDict->close();
                        if (pDimAssoc)
                        {
                            AcDbObjectIdArray geomIdArray;
                            pDimAssoc->getDimAssocGeomIds(geomIdArray);

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

                            modifyDistance(geomIdArray, measuredValue, 100);

                            std::cout << geomIdArray.logicalLength();

                            pDimAssoc->close();
                        }
                    }
                }
            }
            else
            {
                acutPrintf(_T("\n未选择有效标注"));
            }
            pEnt->close();
        }
    }
}

void modifyDistance(const AcDbObjectIdArray &objIdArr, double oriDistance, double targetDistance)
{
    AcDbEntity *pEnt = nullptr;
    Acad::ErrorStatus es;
    if (acdbOpenAcDbEntity(pEnt, objIdArr[1], AcDb::kForWrite) == Acad::eOk)
    {
        if (pEnt->isKindOf(AcDbLine::desc()))
        {
            AcDbLine *pLine = AcDbLine::cast(pEnt);

            AcGeVector3d direction = pLine->endPoint() - pLine->startPoint();
            AcGeVector3d moveDirection = direction.y >= 0 ? AcGeVector3d(direction.y, -direction.x, 0) : AcGeVector3d(-direction.y, direction.x, 0);
            moveDirection.normalize();

            AcGeMatrix3d moveMatrix = AcGeMatrix3d::translation(moveDirection * (targetDistance - oriDistance));
            pLine->transformBy(moveMatrix);

            // AcGePoint3d newStartPoint = pLine->startPoint() + moveDirection * (targetDistance - oriDistance);
            // AcGePoint3d newEndPoint = pLine->endPoint() + moveDirection * (targetDistance - oriDistance);

            // es = pLine->setStartPoint(newStartPoint);
            // es = pLine->setEndPoint(newEndPoint);

            // es = pLine->upgradeOpen();
            // es = pLine->close();
        }
        es = pEnt->close();
    }
}

void modifyDistance(const AcDbObjectId &lineId, double oriDistance, double targetDistance)
{
    AcDbEntity *pEnt = nullptr;
    Acad::ErrorStatus es;
    if (acdbOpenAcDbEntity(pEnt, lineId, AcDb::kForWrite) == Acad::eOk)
    {
        if (pEnt->isKindOf(AcDbLine::desc()))
        {
            AcDbLine *pLine = AcDbLine::cast(pEnt);

            AcGeVector3d direction = pLine->endPoint() - pLine->startPoint();
            AcGeVector3d moveDirection = direction.y >= 0 ? AcGeVector3d(direction.y, -direction.x, 0) : AcGeVector3d(-direction.y, direction.x, 0);
            moveDirection.normalize();

            AcGeMatrix3d moveMatrix = AcGeMatrix3d::translation(moveDirection * (targetDistance - oriDistance));
            pLine->transformBy(moveMatrix);

            // AcGePoint3d newStartPoint = pLine->startPoint() + moveDirection * (targetDistance - oriDistance);
            // AcGePoint3d newEndPoint = pLine->endPoint() + moveDirection * (targetDistance - oriDistance);

            // es = pLine->setStartPoint(newStartPoint);
            // es = pLine->setEndPoint(newEndPoint);

            // es = pLine->upgradeOpen();
            // es = pLine->close();
        }
        es = pEnt->close();
    }
}

void test_modify_distance()
{
    modifyDistance(g_lineId, g_oriDistance, 100);
}