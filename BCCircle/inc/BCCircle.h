#pragma once

#include <dbobjptr.h>
#include <dbents.h>
#include <vector>

#define CenterDrag

class BCCircle : public AcDbEntity
{
public:
	enum SubentType
	{
		kSubentTypeCircle = 100,
		kSubentTypeDiameter,
		kSubentTypeDiameter2
	};

	ACRX_DECLARE_MEMBERS(BCCircle);

	BCCircle();
	BCCircle(AcGePoint3d pos, double radius = 0.0);

	virtual ~BCCircle()
	{
		delete m_pLine;
		m_pLine = nullptr;
		delete m_pLine1;
		m_pLine1 = nullptr;
	};
	// BCCircle(const BCCircle&) = delete;
	// BCCircle& operator=(const BCCircle&) = delete;

	Acad::ErrorStatus setCenter(const AcGePoint3d &pos);
	AcGePoint3d getCenter() const
	{
		assertReadEnabled();
		return m_centerPos;
	}
	Acad::ErrorStatus setRadius(double radius);
	double getRadius() const
	{
		assertReadEnabled();
		return m_radius;
	}

	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *filer) override;
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *filer) const override;

	virtual void subList() const override;
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d &xform) override;
	virtual Acad::ErrorStatus subExplode(AcDbVoidPtrArray &entitySet) const override;

	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d &pickPoint,
		const AcGePoint3d &lastPoint,
		const AcGeMatrix3d &viewXform,
		AcGePoint3dArray &snapPoints,
		AcDbIntArray &geomIds) const override;

	virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray &gripPoints,
											   AcDbIntArray &osnapModes,
											   AcDbIntArray &geomIds) const override;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray &indices,
												  const AcGeVector3d &offset) override;

	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw *mode) override;

	virtual Acad::ErrorStatus subGetSubentPathsAtGsMarker(AcDb::SubentType type, Adesk::GsMarker gsMark, const AcGePoint3d &pickPoint, const AcGeMatrix3d &viewXform, int &numPaths, AcDbFullSubentPath *&subentPaths, int numInserts = 0, AcDbObjectId *entAndInsertStack = nullptr) const override;
	virtual Acad::ErrorStatus subGetGsMarkersAtSubentPath(const AcDbFullSubentPath &subPath, AcArray<Adesk::GsMarker> &gsMarkers) const override;
	virtual AcDbEntity *subSubentPtr(const AcDbFullSubentPath &id) const override;

public:
	void addChild(const AcDbHardPointerId &childId)
	{
		assertWriteEnabled();
		m_childVec.emplace_back(childId);
	}
	const std::vector<AcDbHardPointerId> &getChildVec() const
	{
		assertReadEnabled();
		return m_childVec;
	}
	bool hasChild() const { return !m_childVec.empty(); }
	void clearChild() { m_childVec.clear(); }

private:
	AcGePoint3d m_centerPos = AcGePoint3d::kOrigin;
	double m_radius = 0.0;
	std::vector<AcDbHardPointerId> m_childVec;

	AcDbPolyline *m_pLine = nullptr;

	AcDbPolyline *m_pLine1 = nullptr;

	static Adesk::UInt8 entity_version;
};

class BCCircleJig : public AcEdJig
{
public:
	BCCircleJig() = default;
	virtual ~BCCircleJig() = default;
	void doIt();

protected:
	virtual AcEdJig::DragStatus sampler() override;
	virtual Adesk::Boolean update() override;
	virtual AcDbEntity *entity() const override { return m_pCircle; };

private:
	BCCircle *m_pCircle = nullptr;
#ifdef CenterDrag
	int m_iPromptCount = 0;
#endif
	AcGePoint3d m_centerPos = AcGePoint3d::kOrigin;
	double m_radius = 0.0;
};

class MyObject : public AcDbObject
{
public:
	ACRX_DECLARE_MEMBERS(MyObject);

	MyObject() = default;
	MyObject(AcDbObjectId parentId, AcDbObjectId childId) : m_parentId{parentId}, m_childId{childId} {}
	~MyObject() = default;

	AcDbHardPointerId getParentObjId() const
	{
		assertReadEnabled();
		return m_parentId;
	}
	void setParentObjId(const AcDbObjectId &parentId)
	{
		assertWriteEnabled();
		m_parentId = parentId;
	}
	AcDbHardPointerId getChildObjId() const
	{
		assertReadEnabled();
		return m_childId;
	}
	void setChildObjId(const AcDbObjectId &childId)
	{
		assertWriteEnabled();
		m_childId = childId;
	}

	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *filer) override;
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *filer) const override;

private:
	AcDbHardPointerId m_parentId;
	AcDbHardPointerId m_childId;
};

void test_mycircle();
void test_buildlink();
void test_gsMarker();
AcDbObjectId getRegApp();

void bc_RemoveObjOverrule();
void bc_AddObjOverruleCanCopy();
void bc_AddObjOverruleCanNotCopy();