#pragma once

class BCEntityReactor : public AcDbEntityReactor
{
public:
    BCEntityReactor() { m_objIdList.removeAll(); }
    ~BCEntityReactor() { delAllReactor(); }

public:
    bool addReactorToObject(AcDbObject *dbObj);
    bool delReactorFromObject(AcDbObject *dbObj);
    bool delAllReactor();

protected:
    virtual void openedForModify(const AcDbObject *dbObj) override;
    virtual void modified(const AcDbObject *dbObj) override;

public:
    static void printObjInfo(const AcDbObject *pObj);
    static AcDbObject *selectObject(AcDb::OpenMode openMode);

private:
    AcDbObjectIdArray m_objIdList;
    double m_measuredValue = 0.;
};

extern BCEntityReactor *g_pEntityReactor;

void addEntityReactor();
void removeEntityReactor();
void clearEntityReactor();