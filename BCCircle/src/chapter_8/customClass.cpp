#include <chapter_8/customClass.h>

ACRX_DXF_DEFINE_MEMBERS(CustomClass, AcDbObject, AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 0, CustomClass, CUSTOMOBJECT);

Acad::ErrorStatus CustomClass::setIdData(const AcDbHardOwnershipId &ownedId)
{
    if (ownedId.asOldId() == 0L)
        return Acad::eInvalidInput;

    assertWriteEnabled();
    mObjId = ownedId;
    AcDbObject *pObj = nullptr;
    AcTransaction *pTrans = actrTransactionManager->startTransaction();
    pTrans->getObject(pObj, ownedId, AcDb::kForWrite);
    pObj->setOwnerId(objectId());
    actrTransactionManager->endTransaction();
    return Acad::eOk;
}

Acad::ErrorStatus CustomClass::dwgInFields(AcDbDwgFiler *pFiler)
{
    assertWriteEnabled();
    AcDbObject::dwgInFields(pFiler);
    if (pFiler->filerType() == AcDb::kWblockCloneFiler)
    {
        AcDbHardOwnershipId id;
        pFiler->readItem(&id);
    }
    pFiler->readItem(&mIntval);
    pFiler->readItem(&mObjId);
    return pFiler->filerStatus();
}

Acad::ErrorStatus CustomClass::dwgOutFields(AcDbDwgFiler *pFiler) const
{
    assertReadEnabled();
    AcDbObject::dwgOutFields(pFiler);
    if (pFiler->filerType() == AcDb::kWblockCloneFiler)
        pFiler->writeHardOwnershipId((AcDbHardOwnershipId)ownerId());
    pFiler->writeItem(mIntval);
    pFiler->writeItem(mObjId);
    return pFiler->filerStatus();
}

Acad::ErrorStatus CustomClass::subErase(Adesk::Boolean pErasing)
{
    AcDbObjectId pObjId;
    Acad::ErrorStatus es = AcDbObject::subErase(pErasing);
    if (es != Acad::eOk)
        return es;
    pObjId = this->getIdData();
    if (pObjId.asOldId() == 0L)
        return Acad::eInvalidInput;
    CustomClass *pCustObj = nullptr;
    es = acdbOpenObject(pCustObj, pObjId, AcDb::kForWrite);
    if (es != Acad::eOk)
        return es;
    pCustObj->erase(pErasing);
    pCustObj->close();
    return Acad::eOk;
}

void createObjs()
{
    AcDbObjectId objIdA, objIdB, objIdC;
    AcDbDictionary *pNamedObj = nullptr;
    AcDbDictionary *pDict = nullptr;
    AcDbDatabase *pCurDwg = acdbHostApplicationServices()->workingDatabase();

    CustomClass *pObjC = new CustomClass(3);
    pCurDwg->addAcDbObject(objIdC, pObjC);
    pObjC->close();
    CustomClass *pObjB = new CustomClass(2);
    pCurDwg->addAcDbObject(objIdB, pObjB);
    pObjB->setIdData(objIdC);
    pObjB->close();
    CustomClass *pObjA = new CustomClass(1);
    pCurDwg->getNamedObjectsDictionary(pNamedObj, AcDb::kForWrite);
    if (pNamedObj->getAt(_T("NEW_DICTIONARY"), (AcDbObject *&)(pDict), AcDb::kForWrite) == Acad::eKeyNotFound)
    {
        pDict = new AcDbDictionary;
        AcDbObjectId DictId;
        pNamedObj->setAt(_T("NEW_DICTIONARY"), pDict, DictId);
    }
    else
    {
        acutPrintf(_T("\nDictionary already exists"));
        pNamedObj->close();
        return;
    }
    pNamedObj->close();
    pDict->setAt(_T("OBJA"), pObjA, objIdA);
    pDict->close();
    pObjA->setIdData(objIdB);
    pObjA->close();
    return;
}

void listTree()
{
    AcDbDictionary *pNamedObj = nullptr;
    AcDbDictionary *pDict = nullptr;
    acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNamedObj, AcDb::kForWrite);
    if (pNamedObj->getAt(_T("NEW_DICTIONARY"), (AcDbObject *&)(pDict), AcDb::kForRead) == Acad::eKeyNotFound)
    {
        acutPrintf(_T("\nDictionary not found"));
        return;
    }
    if (!pDict->numEntries())
    {
        acutPrintf(_T("\nDictionary is empty"));
        pDict->close();
        return;
    }
    AcDbDictionaryIterator *pDictItr = pDict->newIterator();
    for (; !pDictItr->done(); pDictItr->next())
        printOut(pDictItr->objectId());
    delete pDictItr;
    pDict->close();
    acutPrintf(_T("\nprint info sunccess"));
    return;
}

void eraseTree()
{
    AcDbDictionary *pNamedObj = nullptr;
    AcDbDictionary *pDict = nullptr;
    acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNamedObj, AcDb::kForWrite);
    if (pNamedObj->getAt(_T("NEW_DICTIONARY"), (AcDbObject *&)(pDict), AcDb::kForWrite) != Acad::eOk)
    {
        acutPrintf(_T("\nDictionary not found"));
        pNamedObj->close();
        return;
    }
    pNamedObj->close();
    AcDbDictionaryIterator *pDictItr = pDict->newIterator();
    for (; !pDictItr->done(); pDictItr->next())
    {
        CustomClass *pCustObj = nullptr;
        if (acdbOpenObject(pCustObj, pDictItr->objectId(), AcDb::kForWrite) != Acad::eOk)
        {
            pDict->close();
            return;
        }
        pCustObj->erase();
        pCustObj->close();
    }
    delete pDictItr;
    pDict->close();
    acutPrintf(_T("\nErase success"));
    return;
}

void printOut(AcDbObjectId id)
{
    if (id.asOldId() == 0L)
    {
        acutPrintf(_T("\nInvalid Object ID"));
        return;
    }
    CustomClass *pCustObj = nullptr;
    acdbOpenObject((AcDbObject *&)pCustObj, id, AcDb::kForRead);
    acutPrintf(_T("\nIntdata: %d, ObjId: %ld, backPointer: %ld, OwnedObj: %ld"),
               pCustObj->getIntData(),
               pCustObj->objectId().asOldId(),
               pCustObj->ownerId().asOldId(),
               pCustObj->getIdData().asOldId());

    if (pCustObj->getIdData().asOldId() != 0L)
        printOut(pCustObj->getIdData());
    pCustObj->close();
}