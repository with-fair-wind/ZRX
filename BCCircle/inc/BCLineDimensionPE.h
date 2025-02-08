#pragma once

class BCLineDimensionPE : public AcRxObject
{
    struct AssocData
    {
        AcDbObjectId _lineId;
        AcDbObjectId _dimensionId;
    };

public:
    ACRX_DECLARE_MEMBERS(BCLineDimensionPE);
    BCLineDimensionPE() = default;
    ~BCLineDimensionPE() = default;
    void modified(AcDbObject *pObj, const AcDbObject *pAssocObj);
    bool isUsAble() const { return m_isUsAble; }
    void setUsAble(bool isUsAble) { m_isUsAble = isUsAble; }
    void open() { setUsAble(true); }
    void close(bool bUpdate);

private:
    void clear() { m_assocIdData = {}; }
    void update(const AcDbObjectId &assocId, const AssocData &assocData);

private:
    bool m_isUsAble = true;
    std::pair<AcDbObjectId, AssocData> m_assocIdData;
};