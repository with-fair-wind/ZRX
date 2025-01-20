#include <chapter_13/dbreactor.h>

long gEntNum = 0;
CDbReactor *gpDbr = nullptr;

void CDbReactor::objectAppended(const AcDbDatabase *dwg, const AcDbObject *pObj)
{
    printDbEvent(pObj, "objectAppended");
    acutPrintf(_T("\n数据库: %lx, "), (long)dwg);
    gEntNum++;
    acutPrintf(_T("实体数目: %d 个"), gEntNum);
}

void CDbReactor::objectModified(const AcDbDatabase *dwg, const AcDbObject *pObj)
{
    printDbEvent(pObj, "objectModified");
    acutPrintf(_T("\n数据库: %lx, "), (long)dwg);
}

void CDbReactor::objectErased(const AcDbDatabase *dwg, const AcDbObject *pObj, Adesk::Boolean pErased)
{
    if (pErased)
    {
        printDbEvent(pObj, "objectErased");
        gEntNum--;
    }
    else
    {
        printDbEvent(pObj, "object(Un)erased");
        gEntNum++;
    }
    acutPrintf(_T("\n数据库: %lx, "), (long)dwg);
    acutPrintf(_T("实体数目: %d 个"), gEntNum);
}

void printDbEvent(const AcDbObject *pObj, const char *pEvent)
{
    acutPrintf(_T("\n事件: AcDbDatabaseReactor::%s"), pEvent);
    printObj(pObj);
}

void printObj(const AcDbObject *pObj)
{
    if (!pObj)
    {
        acutPrintf(_T("NULL"));
        return;
    }
    AcDbHandle objHand;
    ACHAR handbuf[17];
    pObj->getAcDbHandle(objHand);
    objHand.getIntoAsciiBuffer(handbuf);
    acutPrintf(_T("\n类: %s, 句柄: %s, 标识符(ID): %lx, 数据库: %lx"), pObj->isA()->name(), handbuf, pObj->objectId().asOldId(), pObj->database());
}

void watchDb()
{
    if (!gpDbr)
        gpDbr = new CDbReactor();

    gEntNum = 0;
    AcDbDatabase *pCur = acdbHostApplicationServices()->workingDatabase();
    AcDbBlockTable *pBlkTable = nullptr;
    pCur->getSymbolTable(pBlkTable, AcDb::kForRead);
    AcDbBlockTableRecord *pBtr = nullptr;
    pBlkTable->getAt(ACDB_MODEL_SPACE, pBtr, AcDb::kForRead);
    pBlkTable->close();
    AcDbBlockTableRecordIterator *pBtrIter = nullptr;
    pBtr->newIterator(pBtrIter);
    for (; !pBtrIter->done(); pBtrIter->step())
    {
        AcDbEntity *pEnt = nullptr;
        pBtrIter->getEntity(pEnt, AcDb::kForRead);
        if (pEnt)
        {
            gEntNum++;
            pEnt->close();
        }
    }
    delete pBtrIter;
    pBtr->close();
    pCur->addReactor(gpDbr);
    acutPrintf(_T("\n已经将数据库反应器添加到当前数据库中, 当前数据库的模型空间中由 %d 个实体"), gEntNum);
}

void clearReactors()
{
    AcDbDatabase *pCur = acdbHostApplicationServices()->workingDatabase();
    if (pCur)
    {
        pCur->removeReactor(gpDbr);
        delete gpDbr;
        gpDbr = nullptr;
    }
    gEntNum = 0;
}