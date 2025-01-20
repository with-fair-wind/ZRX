#include <chapter_13/persreactor.h>

ACRX_DXF_DEFINE_MEMBERS(CPersistentReactor, AcDbObject,
                        AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
                        0, CPersistentReactor, persreactor);

Acad::ErrorStatus CPersistentReactor::dwgInFields(AcDbDwgFiler *pFiler)
{
    assertWriteEnabled();
    AcDbObject::dwgInFields(pFiler);
    pFiler->readItem(&mFactor);
    pFiler->readItem((AcDbSoftPointerId *)&mld);
    return pFiler->filerStatus();
}

Acad::ErrorStatus CPersistentReactor::dwgOutFields(AcDbDwgFiler *pFiler) const
{
    assertReadEnabled();
    AcDbObject::dwgOutFields(pFiler);
    pFiler->writeItem(mFactor);
    pFiler->writeItem((AcDbSoftPointerId)mld);
    return pFiler->filerStatus();
}

void CPersistentReactor::modified(const AcDbObject *pObj)
{
    AcDbCircle *pCircle = AcDbCircle::cast(pObj);
    if (!pCircle)
    {
        const ACHAR *cstr = pObj->isA()->name();
        acutPrintf(_T("\n这是一个 %s"), cstr);
        return;
    }
    acutPrintf(_T("\n反应器被附加到对象%lx上, 并通知对象%lx"), pCircle->objectId(), mld);
    AcDbCircle *pCircle2 = nullptr;
    if (acdbOpenObject((AcDbObject *&)pCircle2, mld, AcDb::kForWrite) == Acad::eOk)
    {
        double r = pCircle->radius();
        pCircle2->setRadius(r);
        pCircle2->close();
    }
}

void assocCircles()
{
    AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
    AcDbObjectId aId, bId;
    AcDbCircle *pCircleA = new AcDbCircle;
    pCircleA->setDatabaseDefaults(pDb);
    pCircleA->setCenter(AcGePoint3d(1, 1, 0));
    pCircleA->setRadius(1);
    addToModelSpace(aId, pCircleA);
    acutPrintf(_T("\n圆A的ID是%lx, 半径为1"), pCircleA->objectId());

    AcDbCircle *pCircleB = new AcDbCircle;
    pCircleB->setDatabaseDefaults(pDb);
    pCircleB->setCenter(AcGePoint3d(3, 3, 0));
    pCircleB->setRadius(1);
    addToModelSpace(bId, pCircleB);
    acutPrintf(_T("\n圆B的ID是%lx, 半径为1"), pCircleB->objectId());

    AcDbDictionary *pNamedObj = nullptr;
    AcDbDictionary *pNameList = nullptr;
    pDb->getNamedObjectsDictionary(pNamedObj, AcDb::kForWrite);
    if (pNamedObj->getAt(_T("ASSOC_DICTIONARY"), (AcDbObject *&)pNameList, AcDb::kForWrite) == Acad::eKeyNotFound)
    {
        pNameList = new AcDbDictionary;
        AcDbObjectId dictId;
        pNamedObj->setAt(_T("ASSOC_DICTIONARY"), pNameList, dictId);
    }
    pNamedObj->close();
    CPersistentReactor *pObj = new CPersistentReactor();

    pObj->eLinkage(bId);
    AcDbObjectId objId;
    if (pNameList->getAt(_T("object_to_notify_A"), objId) == Acad::eKeyNotFound)
    {
        pNameList->setAt(_T("object_to_notify_A"), pObj, objId);
        pObj->close();
    }
    else
    {
        delete pObj;
        ads_printf(_T("\nobject_to_notify_A 已经存在"));
    }
    pCircleA->addPersistentReactor(objId);
    pCircleA->close();

    pObj = new CPersistentReactor();
    pObj->eLinkage(aId);
    if (pNameList->getAt(_T("object_to_notify_B"), objId) == Acad::eKeyNotFound)
    {
        pNameList->setAt(_T("object_to_notify_B"), pObj, objId);
        pObj->close();
    }
    else
    {
        delete pObj;
        ads_printf(_T("\nobject_to_notify_B 已经存在"));
    }
    pCircleB->addPersistentReactor(objId);
    pCircleB->close();
    pNameList->close();
}

void addToModelSpace(AcDbObjectId &objId, AcDbEntity *pEntity)
{
    AcDbBlockTable *pBlkTable = nullptr;
    AcDbBlockTableRecord *pSpaceRecord = nullptr;
    acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlkTable, AcDb::kForRead);
    pBlkTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord, AcDb::kForWrite);
    pBlkTable->close();
    pSpaceRecord->appendAcDbEntity(objId, pEntity);
    pSpaceRecord->close();
    return;
}