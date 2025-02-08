#include <BCEntityReactor.h>
#include <BCDimension.h>

BCEntityReactor *g_pEntityReactor = nullptr;

bool BCEntityReactor::addReactorToObject(AcDbObject *dbObj)
{
    if (!dbObj)
        return false;

    ASSERT(dbObj->isWriteEnabled() == Adesk::kTrue);
    if (m_objIdList.contains(dbObj->objectId()))
        return false;
    else
    {
        dbObj->addReactor(this);
        m_objIdList.append(dbObj->objectId());
        return true;
    }
}

bool BCEntityReactor::delReactorFromObject(AcDbObject *dbObj)
{
    if (!dbObj)
        return false;

    int nIndex = -1;
    if (m_objIdList.find(dbObj->objectId(), nIndex))
    {
        dbObj->removeReactor(this);
        m_objIdList.removeAt(nIndex);
        return true;
    }
    return false;
}

bool BCEntityReactor::delAllReactor()
{
    AcDbObject *pObj = nullptr;
    for (int i = 0; i < m_objIdList.length(); i++)
    {
        if (acdbOpenObject(pObj, m_objIdList[i], AcDb::kForWrite, true) == Acad::eOk)
        {
            pObj->removeReactor(this);
            pObj->close();
        }
        else
            return false;
    }
    m_objIdList.removeAll();
    return true;
}
void BCEntityReactor::openedForModify(const AcDbObject *dbObj)
{
    if (!dbObj->isKindOf(AcDbRotatedDimension::desc()))
    {
        acutPrintf(_T("\nObject is not a valid RotatedDimension Entity!"));
        return;
    }

    AcDbRotatedDimension *pRotatedDim = AcDbRotatedDimension::cast(dbObj);
    pRotatedDim->measurement(m_measuredValue);
    CString strLayer;
    strLayer.Format(_T("\nCurrent Entity:%s .The layerName:%s ."), pRotatedDim->isA()->name(), pRotatedDim->layer());
    acutPrintf(strLayer);
}

void BCEntityReactor::modified(const AcDbObject *dbObj)
{
    AcTransaction *pTrans = actrTransactionManager->startTransaction();
    if (!dbObj->isKindOf(AcDbRotatedDimension::desc()))
    {
        acutPrintf(_T("\nObject is not a valid RotatedDimension Entity!"));
        return;
    }

    AcDbRotatedDimension *pRotatedDim = AcDbRotatedDimension::cast(dbObj);

    ACHAR *dimText = pRotatedDim->dimensionText();
    acutPrintf(L"\n测量单位：%g\n", m_measuredValue);
    acutPrintf(L"\n文字替代：%s\n", dimText);
    m_curDistance = std::wcstod(dimText, nullptr);
    if (m_curDistance != m_measuredValue && m_curDistance != 0.)
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
                pDimAssoc->getDimAssocGeomIds(m_geomIdArray);
                pDimAssoc->close();
                for (int i = 0; i < m_geomIdArray.logicalLength(); ++i)
                {
                    AcDbEntity *pGeom = nullptr;
                    if (acdbOpenAcDbEntity(pGeom, m_geomIdArray[i], AcDb::kForWrite) == Acad::eOk)
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

                m_bflag = true;

                CString strLayer;
                strLayer.Format(_T("\nEntity:%s has been modified.The layerName:%s.\n"), pRotatedDim->isA()->name(), pRotatedDim->layer());
                acutPrintf(strLayer);

                modifyDistance(m_geomIdArray, m_measuredValue, m_curDistance);
            }
        }
    }
    actrTransactionManager->endTransaction();
}

void BCEntityReactor::objectClosed(const AcDbObjectId objId)
{
    // if (m_bflag)
    //{
    //     m_bflag = false;
    //     acutPrintf(_T("\nobjectClosed"));

    //    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    //    //AcDbObject* dbObj = nullptr;
    //    //if ((acdbOpenObject(dbObj, objId, AcDb::kForWrite)) != Acad::eOk)
    //    //{
    //    //    if (!dbObj->isKindOf(AcDbRotatedDimension::desc()))
    //    //    {
    //    //        acutPrintf(_T("\nObject is not a valid RotatedDimension Entity!"));
    //    //        return;
    //    //    }
    //    //    AcDbRotatedDimension* pRotatedDim = AcDbRotatedDimension::cast(dbObj);
    //    //    pRotatedDim->setDimensionText(_T(""));
    //    //    pRotatedDim->close();
    //    //}
    //    modifyDistance(m_geomIdArray, m_measuredValue, m_curDistance);
    //    actrTransactionManager->endTransaction();
    //}
}

void BCEntityReactor::printObjInfo(const AcDbObject *pObj)
{
    if (!pObj)
    {
        acutPrintf(_T("\nObject is NULL!"));
        return;
    }

    AcDbHandle objHand;
    ACHAR handBuff[17];
    pObj->getAcDbHandle(objHand);
    objHand.getIntoAsciiBuffer(handBuff);

    acutPrintf(_T("\n(className==%s, handle==%s, id==%lx, database==%lx)"), pObj->isA()->name(), handBuff, pObj->objectId().asOldId(), pObj->database());
}

AcDbObject *BCEntityReactor::selectObject(AcDb::OpenMode openMode)
{
    ads_name en;
    ads_point pt;

    if (acedEntSel(_T("\nSelect an entity:"), en, pt) != RTNORM)
        return nullptr;

    AcDbObjectId eId;
    Acad::ErrorStatus retStat;
    retStat = acdbGetObjectId(eId, en);
    if (retStat != Acad::eOk)
    {
        acutPrintf(_T("\nacdbGetObjectId failed!"));
        return nullptr;
    }

    AcDbObject *obj = nullptr;
    if ((retStat = acdbOpenObject(obj, eId, openMode)) != Acad::eOk)
    {
        acutPrintf(_T("\nOpen Failed!"));
        return nullptr;
    }

    return obj;
}

void addEntityReactor()
{
    AcDbObject *pEnt = BCEntityReactor::selectObject(AcDb::kForWrite);

    if (!g_pEntityReactor)
        g_pEntityReactor = new BCEntityReactor;

    g_pEntityReactor->addReactorToObject(pEnt);
    pEnt->close();
}

void removeEntityReactor()
{
    AcDbObject *pEnt = BCEntityReactor::selectObject(AcDb::kForWrite);
    if (g_pEntityReactor)
        g_pEntityReactor->delReactorFromObject(pEnt);
    pEnt->close();
}

void clearEntityReactor()
{
    if (g_pEntityReactor)
    {
        delete g_pEntityReactor;
        g_pEntityReactor = nullptr;
    }
}