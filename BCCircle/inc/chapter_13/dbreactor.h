#pragma once

void printDbEvent(const AcDbObject *, const char *eventStr);
void printObj(const AcDbObject *pObj);
void watchDb();
void clearReactors();

class CDbReactor : public AcDbDatabaseReactor
{
public:
    virtual void objectAppended(const AcDbDatabase *dwg, const AcDbObject *dbObj) override;
    virtual void objectModified(const AcDbDatabase *dwg, const AcDbObject *dbObj) override;
    virtual void objectErased(const AcDbDatabase *dwg, const AcDbObject *dbObj, Adesk::Boolean pErased) override;
};