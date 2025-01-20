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
    const AcDbObjectIdArray &getGeomIdArr() const { return m_geomIdArray; }

protected:
    virtual void openedForModify(const AcDbObject *dbObj) override;
    virtual void modified(const AcDbObject *dbObj) override;
    virtual void objectClosed(const AcDbObjectId objId);

public:
    static void printObjInfo(const AcDbObject *pObj);
    static AcDbObject *selectObject(AcDb::OpenMode openMode);

private:
    AcDbObjectIdArray m_geomIdArray;
    AcDbObjectIdArray m_objIdList;
    double m_measuredValue = 0.;
    double m_curDistance = 0.;
    bool m_bflag = false;
};

extern BCEntityReactor *g_pEntityReactor;

void addEntityReactor();
void removeEntityReactor();
void clearEntityReactor();