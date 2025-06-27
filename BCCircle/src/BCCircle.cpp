#include "BCCircle.h"
#include "BCCircleCopyRule.h"

static BCCircleOverrule *s_g_bcCircleOverrule = nullptr;

static void CreateDictionary();
static Acad::ErrorStatus getObjectAndGsMarker(AcDbObjectId &objId, int &marker);
static void highlightEdge(const AcDbObjectId &objId, const int gsmarker);
static void setXData(AcDbEntity *&pObj);
static bool printXData(AcDbEntity *&pObj);
static void printXDList(resbuf *pRb);

ACRX_DXF_DEFINE_MEMBERS(BCCircle, AcDbEntity,
						AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
						AcDbProxyEntity::kAllAllowedBits, BCCIRCLE, HelloWorld)

ACRX_DXF_DEFINE_MEMBERS(
	MyObject, AcDbObject,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kAllAllowedBits, MYOBJECT, HelloWorld)

Adesk::UInt8 BCCircle::entity_version = 1;

BCCircle::BCCircle()
{
	m_pLine = new AcDbPolyline(2);
	m_pLine->addVertexAt(0, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));
	m_pLine->addVertexAt(1, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));

	m_pLine1 = new AcDbPolyline(3);
	m_pLine1->addVertexAt(0, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));
	m_pLine1->addVertexAt(1, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));
	m_pLine1->addVertexAt(2, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));
}

BCCircle::BCCircle(AcGePoint3d pos, double radius) : m_centerPos(pos), m_radius(radius)
{
	m_pLine = new AcDbPolyline(2);
	m_pLine->addVertexAt(0, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));
	m_pLine->addVertexAt(1, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));

	m_pLine1 = new AcDbPolyline(3);
	m_pLine1->addVertexAt(0, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));
	m_pLine1->addVertexAt(1, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));
	m_pLine1->addVertexAt(2, AcGePoint3d::kOrigin.convert2d(AcGePlane::kXYPlane));
}

inline Acad::ErrorStatus BCCircle::setCenter(const AcGePoint3d &pos)
{
	assertWriteEnabled();
	m_centerPos = pos;
	m_pLine->setPointAt(0, (m_centerPos + AcGeVector3d(-m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine->setPointAt(1, (m_centerPos + AcGeVector3d(m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));

	m_pLine1->setPointAt(0, (m_centerPos + AcGeVector3d(-m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine1->setPointAt(1, (m_centerPos + AcGeVector3d(0, m_radius, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine1->setPointAt(2, (m_centerPos + AcGeVector3d(m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));
	return Acad::eOk;
}

inline Acad::ErrorStatus BCCircle::setRadius(double radius)
{
	assertWriteEnabled();
	m_radius = radius;
	m_pLine->setPointAt(0, (m_centerPos + AcGeVector3d(-m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine->setPointAt(1, (m_centerPos + AcGeVector3d(m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));

	m_pLine1->setPointAt(0, (m_centerPos + AcGeVector3d(-m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine1->setPointAt(1, (m_centerPos + AcGeVector3d(0, m_radius, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine1->setPointAt(2, (m_centerPos + AcGeVector3d(m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));
	return Acad::eOk;
}

Acad::ErrorStatus BCCircle::dwgInFields(AcDbDwgFiler *filer)
{
	assertWriteEnabled();
	Acad::ErrorStatus es;
	if ((es = AcDbEntity::dwgInFields(filer)) != Acad::eOk)
		return es;

	Adesk::UInt8 sVersion;
	filer->readUInt8(&sVersion);
	filer->readPoint3d(&m_centerPos);
	filer->readDouble(&m_radius);
	UINT64 size = 0;
	filer->readUInt64(&size);
	m_childVec.resize(size, AcDbHardPointerId::kNull);
	for (auto &tmp : m_childVec)
		filer->readHardPointerId(&tmp);

	m_pLine->setPointAt(0, (m_centerPos + AcGeVector3d(-m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine->setPointAt(1, (m_centerPos + AcGeVector3d(m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));

	m_pLine1->setPointAt(0, (m_centerPos + AcGeVector3d(-m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine1->setPointAt(1, (m_centerPos + AcGeVector3d(0, m_radius, 0)).convert2d(AcGePlane::kXYPlane));
	m_pLine1->setPointAt(2, (m_centerPos + AcGeVector3d(m_radius, 0, 0)).convert2d(AcGePlane::kXYPlane));

	return filer->filerStatus();
}

Acad::ErrorStatus BCCircle::dwgOutFields(AcDbDwgFiler *filer) const
{
	assertReadEnabled();
	Acad::ErrorStatus es;
	if ((es = AcDbEntity::dwgOutFields(filer)) != Acad::eOk)
		return es;

	filer->writeUInt8(entity_version);
	filer->writePoint3d(m_centerPos);
	filer->writeDouble(m_radius);
	filer->writeUInt64(m_childVec.size());
	for (const auto &tmp : m_childVec)
		filer->writeHardPointerId(tmp);

	return filer->filerStatus();
}

void BCCircle::subList() const
{
	assertReadEnabled();
	AcDbEntity::subList();

	ACHAR szPrompt1[1024] = L"\0";
	ACHAR szPrompt2[1024] = L"\0";
	ACHAR szPrompt3[1024] = L"\0";
	acdbRToS(m_centerPos.x, -1, -1, szPrompt1);
	acdbRToS(m_centerPos.y, -1, -1, szPrompt2);
	acdbRToS(m_centerPos.z, -1, -1, szPrompt3);
	acutPrintf(L"\n圆心： %s, %s, %s", szPrompt1, szPrompt2, szPrompt3);

	acdbRToS(m_radius, -1, -1, szPrompt1);
	acutPrintf(L"\n半径： %s", szPrompt1);
}

Acad::ErrorStatus BCCircle::subTransformBy(const AcGeMatrix3d &xform)
{
	assertWriteEnabled();

	setCenter(m_centerPos.transformBy(xform));
	setRadius(m_radius * xform.scale());

	return Acad::eOk;
}

Acad::ErrorStatus BCCircle::subExplode(AcDbVoidPtrArray &entitySet) const
{
	assertReadEnabled();

	AcDbCircle *pCircle = new AcDbCircle(m_centerPos, AcGeVector3d::kZAxis, m_radius);
	entitySet.append(pCircle);
	if (m_pLine)
		entitySet.append(m_pLine);

	return Acad::eOk;
}

Acad::ErrorStatus BCCircle::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d &pickPoint, const AcGePoint3d &lastPoint, const AcGeMatrix3d &viewXform, AcGePoint3dArray &snapPoints, AcDbIntArray &geomIds) const
{
	assertReadEnabled();

	switch (osnapMode)
	{
	case AcDb::kOsModeEnd:
		break;
	case AcDb::kOsModeMid:
		break;
	case AcDb::kOsModeCen:
		snapPoints.append(m_centerPos);
		break;
	case AcDb::kOsModeNode:
		break;
	case AcDb::kOsModeQuad:
		break;
	case AcDb::kOsModeIns:
		break;
	case AcDb::kOsModePerp:
		break;
	case AcDb::kOsModeTan:
		break;
	case AcDb::kOsModeNear:
		break;
	case AcDb::kOsModeCentroid:
		break;
	default:
		break;
	}

	return Acad::eOk;
}

Acad::ErrorStatus BCCircle::subGetGripPoints(AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds) const
{
	assertReadEnabled();

	AcDbObjectId id = this->objectId();

	std::vector<AcGePoint3d> circleGrip{m_centerPos};
	circleGrip.emplace_back(m_centerPos.x, m_centerPos.y + m_radius, m_centerPos.z);
	circleGrip.emplace_back(m_centerPos.x, m_centerPos.y - m_radius, m_centerPos.z);
	circleGrip.emplace_back(m_centerPos.x + m_radius, m_centerPos.y, m_centerPos.z);
	circleGrip.emplace_back(m_centerPos.x - m_radius, m_centerPos.y, m_centerPos.z);

	for (const auto &pnt : circleGrip)
		gripPoints.append(pnt);

	return Acad::eOk;
}

Acad::ErrorStatus BCCircle::subMoveGripPointsAt(const AcDbIntArray &indices, const AcGeVector3d &offset)
{
	if (indices.length() == 0 || offset.isZeroLength())
		return Acad::eOk;

	assertWriteEnabled();

	AcDbObjectId id = this->objectId();

	bool bMove = false;
	for (int i = 0; i < indices.logicalLength(); ++i)
	{
		switch (indices[i])
		{
		case 0:
		{
			setCenter(m_centerPos += offset);
			bMove = true;
			break;
		}
		case 1:
		{
			AcGePoint3d pnt{m_centerPos.x, m_centerPos.y + m_radius, m_centerPos.z};
			setRadius(m_centerPos.distanceTo(pnt + offset));
			break;
		}
		case 2:
		{
			AcGePoint3d pnt{m_centerPos.x, m_centerPos.y - m_radius, m_centerPos.z};
			setRadius(m_centerPos.distanceTo(pnt + offset));
			break;
		}
		case 3:
		{
			AcGePoint3d pnt{m_centerPos.x + m_radius, m_centerPos.y, m_centerPos.z};
			setRadius(m_centerPos.distanceTo(pnt + offset));
			break;
		}
		case 4:
		{
			AcGePoint3d pnt{m_centerPos.x - m_radius, m_centerPos.y, m_centerPos.z};
			setRadius(m_centerPos.distanceTo(pnt + offset));
			break;
		}
		default:
			break;
		}
	}

#if 1
	if (!m_childVec.empty())
	{
		for (const auto &childId : m_childVec)
		{
			BCCircle *pChild = nullptr;
			acdbOpenObject(pChild, childId, AcDb::kForWrite);

			if (bMove && this->isModified())
				pChild->setCenter(pChild->getCenter() + offset);

			pChild->setRadius(m_radius);
			// pChild->subMoveGripPointsAt(indices, offset);
			pChild->close();
		}
	}
#else
	AcDbHardPointerId parentId, childId;
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbDictionaryPointer pNamedobj;
	pNamedobj.open(pDb->namedObjectsDictionaryId(), AcDb::kForRead);
	if (pNamedobj.openStatus() == Acad::eOk)
	{
		AcDbObjectId dictId;
		Acad::ErrorStatus es = pNamedobj->getAt(_T("CUSTOM_DICT"), dictId);
		if (es == Acad::eOk)
		{
			AcDbDictionaryPointer pDict(dictId, AcDb::kForRead);
			if (pDict.openStatus() == Acad::eOk)
			{
				AcDbDictionaryIterator *pDictIter = pDict->newIterator();

				MyObject *pObj = nullptr;
				for (; !pDictIter->done(); pDictIter->next())
				{
					es = pDictIter->getObject((AcDbObject *&)pObj, AcDb::kForRead);
					if (es == Acad::eOk)
					{
						parentId = pObj->getParentObjId();
						childId = pObj->getChildObjId();
						pObj->close();
					}
				}
				delete pDictIter;
			}
		}
	}

	AcDbObjectId curId = this->objectId();
	if (curId.isNull())
	{
		if (!parentId.isNull())
		{
			BCCircle *pChild = nullptr;
			acdbOpenObject(pChild, childId, AcDb::kForWrite);
			if (bMove)
				pChild->setCenter(pChild->getCenter() + offset);
			pChild->setRadius(m_radius);
			pChild->close();
		}
	}
	else
	{
		if (curId == parentId)
		{
			BCCircle *pChild = nullptr;
			acdbOpenObject(pChild, childId, AcDb::kForWrite);
			if (bMove)
				pChild->setCenter(pChild->getCenter() + offset);
			pChild->setRadius(m_radius);
			pChild->close();
		}
	}
#endif
	return Acad::eOk;
}

Adesk::Boolean BCCircle::subWorldDraw(AcGiWorldDraw *mode)
{
	assertReadEnabled();

	mode->subEntityTraits().setSelectionMarker(kSubentTypeCircle);
	mode->geometry().circle(m_centerPos, m_radius, AcGeVector3d::kZAxis);

	mode->subEntityTraits().setSelectionMarker(kSubentTypeDiameter);
	m_pLine->setDatabaseDefaults(database());
	m_pLine->worldDraw(mode);
	// mode->geometry().pline(*m_pLine);

	mode->subEntityTraits().setSelectionMarker(kSubentTypeDiameter2);
	m_pLine1->setDatabaseDefaults(database());
	m_pLine1->worldDraw(mode);
	// mode->geometry().pline(*m_pLine1);
	// AcGePoint3d points[2]{ m_centerPos + AcGeVector3d(-m_radius, 0, 0),  m_centerPos + AcGeVector3d(m_radius, 0, 0) };
	// mode->geometry().polyline(2, points);
	return Adesk::kTrue;
}

Acad::ErrorStatus BCCircle::subGetSubentPathsAtGsMarker(AcDb::SubentType type, Adesk::GsMarker gsMark, const AcGePoint3d &pickPoint, const AcGeMatrix3d &viewXform, int &numPaths, AcDbFullSubentPath *&subentPaths, int numInserts, AcDbObjectId *entAndInsertStack) const
{
	assertReadEnabled();

	if (type != kSubentTypeCircle && type != kSubentTypeDiameter)
		return Acad::eInvalidInput;

	AcDbFullSubentPath *pPaths = new AcDbFullSubentPath[1];
	pPaths->subentId().setIndex(type);
	pPaths->subentId().setType(type);
	pPaths->objectIds().append(objectId());

	subentPaths = pPaths;
	numPaths = 1;

	return Acad::eOk;
}

Acad::ErrorStatus BCCircle::subGetGsMarkersAtSubentPath(const AcDbFullSubentPath &subPath, AcArray<Adesk::GsMarker> &gsMarkers) const
{
	assertReadEnabled();

	gsMarkers.append(subPath.subentId().index());

	return Acad::eOk;
}

AcDbEntity *BCCircle::subSubentPtr(const AcDbFullSubentPath &id) const
{
	if (id.subentId().type() == kSubentTypeDiameter && id.subentId().index() == 1)
		return m_pLine;
	return nullptr;
}

void BCCircleJig::doIt()
{
#ifdef CenterDrag
	m_pCircle = new BCCircle();
	setDispPrompt(_T("\n请指定圆心:"));
	AcEdJig::DragStatus stat = drag();

	m_iPromptCount = 1;
	setDispPrompt(_T("\n请指定半径:"));
	stat = drag();
#else
	ads_point stPnt;
	acedInitGet(NULL, NULL);
	if (acedGetPoint(NULL, _T("\n指定圆心: "), stPnt) != RTNORM)
		return;
	acdbUcs2Wcs(stPnt, stPnt, false);
	m_centerPos = asPnt3d(stPnt);
	m_pCircle = new BCCircle(m_centerPos);

	setDispPrompt(_T("\n请指定半径:"));
	AcEdJig::DragStatus stat = drag();
#endif
	if (AcEdJig::kNormal == stat)
		AcDbObjectId entId = append();
	else
	{
		delete m_pCircle;
		m_pCircle = nullptr;
	}
}

AcEdJig::DragStatus BCCircleJig::sampler()
{
	setUserInputControls((UserInputControls)(AcEdJig::kAccept3dCoordinates | AcEdJig::kNoNegativeResponseAccepted | AcEdJig::kNoZeroResponseAccepted));
#ifdef CenterDrag
	AcEdJig::DragStatus stat;
	if (m_iPromptCount == 0)
	{
		AcGePoint3d inputPt;
		stat = acquirePoint(inputPt);
		if (stat == AcEdJig::kNormal)
		{
			if (inputPt.isEqualTo(m_centerPos))
				return AcEdJig::kNoChange;
			m_centerPos = inputPt;
		}
	}
	else if (m_iPromptCount == 1)
	{
		double inputradius = -1;
		stat = acquireDist(inputradius, m_centerPos);
		if (stat == AcEdJig::kNormal)
		{
			if (inputradius == m_radius)
				return AcEdJig::kNoChange;
			m_radius = inputradius;
		}
	}
#else
	double inputradius = -1.;
	AcEdJig::DragStatus stat = acquireDist(inputradius, m_centerPos);
	if (stat == AcEdJig::kNormal)
	{
		if (inputradius == m_radius)
			return AcEdJig::kNoChange;
		m_radius = inputradius;
	}
#endif
	return stat;
}

Adesk::Boolean BCCircleJig::update()
{
	if (m_pCircle)
#ifdef CenterDrag
		m_iPromptCount == 0 ? m_pCircle->setCenter(m_centerPos) : m_pCircle->setRadius(m_radius);
#else
		m_pCircle->setRadius(m_radius);
#endif
	return Adesk::kTrue;
}

void test_mycircle()
{
	BCCircleJig bcCircleJig;
	bcCircleJig.doIt();
}

void test_buildlink()
{
	ads_name ent;
	ads_point pt;
	AcDbObjectId parentEntId, childEntId;

	int rc = acedEntSel(_T("选择父实体"), ent, pt);
	if (rc != RTNORM)
		return;
	acdbGetObjectId(parentEntId, ent);

	rc = acedEntSel(_T("选择子实体"), ent, pt);
	if (rc != RTNORM)
		return;
	acdbGetObjectId(childEntId, ent);

	BCCircle *pParentEnt = nullptr;
	acdbOpenObject(pParentEnt, parentEntId, AcDb::kForWrite);
	pParentEnt->addChild(childEntId);
	pParentEnt->close();

#if 1
	CreateDictionary();
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbDictionaryPointer pNamedobj;
	pNamedobj.open(pDb->namedObjectsDictionaryId(), AcDb::kForRead);

	if (pNamedobj.openStatus() == Acad::eOk)
	{
		AcDbObjectId dictId;
		Acad::ErrorStatus es = pNamedobj->getAt(_T("CUSTOM_DICT"), dictId);

		if (es == Acad::eOk)
		{
			AcDbDictionaryPointer pDict(dictId, AcDb::kForWrite);

			if (pDict.openStatus() == Acad::eOk)
			{
				MyObject *pObj = new MyObject{parentEntId, childEntId};
				AcDbObjectId rId;
				pDict->setAt(_T("OBJ"), pObj, rId);
				pObj->close();
			}
		}
	}
#endif

#if 0
	BCCircle* pParentEnt = nullptr, * pChildEnt = nullptr;
	acdbOpenObject(pParentEnt, parentEntId, AcDb::kForWrite);
	acdbOpenObject(pChildEnt, childEntId, AcDb::kForWrite);

	ACHAR szXdata[32] = _T("\0");
	_tcsncpy(szXdata, _T("对象关系"), 31);
	acdbRegApp(szXdata);

	AcDbHandle handle;
	pChildEnt->getAcDbHandle(handle);
	ACHAR szHandle[256];
	handle.getIntoAsciiBuffer(szHandle);

	struct resbuf* pBuf = acutBuildList(
		AcDb::kDxfRegAppName, szXdata,
		AcDb::kDxfHandle, szHandle,
		RTNONE);

	Acad::ErrorStatus es = pParentEnt->setXData(pBuf);
	if (es == Acad::eOk)
	{
		pParentEnt->close();
		pChildEnt->close();
	}
	acutRelRb(pBuf);
#endif
}

void test_gsMarker()
{
	AcDbObjectId objId;
	int marker;
	if (getObjectAndGsMarker(objId, marker) != Acad::eOk)
		return;
	acutPrintf(_T("\nGsMarker: %d"), marker);
	highlightEdge(objId, marker);
}

static void CreateDictionary()
{
	AcDbDictionary *pNamedobj = nullptr;
	acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);

	AcDbDictionary *pDict = new AcDbDictionary;
	// if (pNamedobj->getAt(_T("CUSTOM_DICT"), (AcDbObject*&)pDict, AcDb::kForWrite) == Acad::eKeyNotFound)
	//{
	AcDbObjectId DictId;
	pNamedobj->setAt(_T("CUSTOM_DICT"), pDict, DictId);
	//}
	pNamedobj->close();
	pDict->close();
}

Acad::ErrorStatus MyObject::dwgInFields(AcDbDwgFiler *filer)
{
	assertWriteEnabled();
	Acad::ErrorStatus es = AcDbObject::dwgInFields(filer);
	if (es != Acad::eOk)
		return (es);

	filer->readHardPointerId(&m_parentId);
	filer->readHardPointerId(&m_childId);

	return filer->filerStatus();
}

Acad::ErrorStatus MyObject::dwgOutFields(AcDbDwgFiler *filer) const
{
	assertReadEnabled();
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(filer);
	if (es != Acad::eOk)
		return (es);

	filer->writeHardPointerId(m_parentId);
	filer->writeHardPointerId(m_childId);

	return filer->filerStatus();
}

static Acad::ErrorStatus getObjectAndGsMarker(AcDbObjectId &objId, int &marker)
{
	ads_name sset;
	if (acedSSGet(L"_:S", nullptr, nullptr, nullptr, sset) != RTNORM)
	{
		acutPrintf(_T("\nacedSSGet 函数调用失败"));
		return Acad::eInvalidAdsName;
	}
	struct resbuf *pRb = nullptr;
	if (acedSSNameX(&pRb, sset, 0) != RTNORM)
	{
		acedSSFree(sset);
		return Acad::eAmbiguousOutput;
	}
	acedSSFree(sset);

	struct resbuf *pTemp = pRb->rbnext->rbnext;
	ads_name ename;
	ads_name_set(pTemp->resval.rlname, ename);

	pTemp = pTemp->rbnext;
	marker = pTemp->resval.rint;
	acutRelRb(pRb);
	acdbGetObjectId(objId, ename);

	return Acad::eOk;
}

static void highlightEdge(const AcDbObjectId &objId, const int gsmarker)
{
	AcDbEntity *pEnt = nullptr;
	if (Acad::eOk != acdbOpenAcDbEntity(pEnt, objId, AcDb::kForRead))
		return;

	AcGePoint3d pickpnt;
	AcGeMatrix3d xform;
	int numIds = 0;
	AcDbFullSubentPath *subentIds = nullptr;
	// pEnt->getSubentPathsAtGsMarker(AcDb::kEdgeSubentType, gsmarker, pickpnt, xform, numIds, subentIds);
	pEnt->getSubentPathsAtGsMarker(BCCircle::kSubentTypeDiameter, gsmarker, pickpnt, xform, numIds, subentIds);

	if (numIds > 0)
	{
		// Highlight the subenttity
		pEnt->highlight(subentIds[0]);

		TCHAR szData[128] = {0}; // space for acedGetString pauses below
		acedGetString(0, _T("\nPlease press <Space> or <Enter> to continue..."), szData);

		pEnt->unhighlight(subentIds[0]);

		// Copy a subentity
		// You can override AcDbSubentityOverrule::subentPtr to customize the return value
		AcDbEntity *pSubEnt = pEnt->subentPtr(subentIds[0]);
		if (pSubEnt && pSubEnt->isKindOf(AcDbPolyline::desc()))
		{
			acutPrintf(_T("\n SubEntity done!"));
			AcDbPolyline *pPLine = AcDbPolyline::cast(pSubEnt);
			// Acad::ErrorStatus es = pPLine->setColorIndex(1);
			// es = pPLine->setConstantWidth(10);
			if (!printXData(pSubEnt))
				setXData(pSubEnt);
		}
	}

	if (subentIds)
	{
		delete[] subentIds;
		subentIds = nullptr;
	}
	pEnt->close();
}

void bc_RemoveObjOverrule()
{
	if (s_g_bcCircleOverrule)
	{
		AcRxOverrule::removeOverrule(BCCircle::desc(), s_g_bcCircleOverrule);
		AcRxOverrule::setIsOverruling(false);
		delete s_g_bcCircleOverrule;
		s_g_bcCircleOverrule = nullptr;
	}
}

void bc_AddObjOverruleCanCopy()
{
	bc_RemoveObjOverrule();
	s_g_bcCircleOverrule = new BCCircleOverrule(true);
	AcRxOverrule::addOverrule(BCCircle::desc(), s_g_bcCircleOverrule, false);
	AcRxOverrule::setIsOverruling(true);
}

void bc_AddObjOverruleCanNotCopy()
{
	bc_RemoveObjOverrule();
	s_g_bcCircleOverrule = new BCCircleOverrule(false);
	AcRxOverrule::addOverrule(BCCircle::desc(), s_g_bcCircleOverrule, false);
	AcRxOverrule::setIsOverruling(true);
}

static void setXData(AcDbEntity *&pObj)
{
	ACHAR appName[132]{}, resString[200]{};
	acedGetString(NULL, _T("\nEnter application name: "), appName);
	acedGetString(NULL, _T("\nEnter string to be added: "), resString);

	resbuf *pRb = nullptr, *pTemp = nullptr;
	pRb = pObj->xData(appName);

	if (pRb)
		for (pTemp = pRb; pTemp->rbnext != nullptr; pTemp = pTemp->rbnext)
			;
	else
	{
		acdbRegApp(appName);
		pRb = acutNewRb(AcDb::kDxfRegAppName);
		pTemp = pRb;
		pTemp->resval.rstring = (TCHAR *)malloc((_tcslen(appName) + 1) * sizeof(ACHAR));
		_tcscpy(pTemp->resval.rstring, appName);
	}
	pTemp->rbnext = acutNewRb(AcDb::kDxfXdAsciiString);
	pTemp = pTemp->rbnext;
	pTemp->resval.rstring = (TCHAR *)malloc((_tcslen(resString) + 1) * sizeof(ACHAR));
	_tcscpy(pTemp->resval.rstring, resString);

	pTemp->rbnext = acutNewRb(AcDb::kDxfXdReal);
	pTemp = pTemp->rbnext;
	pTemp->resval.rreal = 11.11;

	pTemp->rbnext = acutNewRb(AcDb::kDxfXdInteger16);
	pTemp = pTemp->rbnext;
	pTemp->resval.rint = 77;

	pObj->upgradeOpen();
	pObj->setXData(pRb);
	acutRelRb(pRb);
}

static bool printXData(AcDbEntity *&pObj)
{
	ACHAR appname[133]{};
	acedGetString(NULL, _T("\nEnter the Xdata application name: "), appname);
	resbuf *pRb = pObj->xData(appname);
	if (pRb)
	{
		printXDList(pRb);
		acutRelRb(pRb);
		return true;
	}
	else
		acutPrintf(_T("\nNo xdata for appname: %s."), appname);
	return false;
}

static void printXDList(resbuf *pRb)
{
	int rt = 0;

	while (pRb)
	{
		if (pRb->restype < 1010)
			rt = RTSTR;
		else if (pRb->restype < 1040)
			rt = RT3DPOINT;
		else if (pRb->restype < 1060)
			rt = RTREAL;
		else if (pRb->restype < 1071)
			rt = RTSHORT;
		else if (pRb->restype == 1071)
			rt = RTLONG;
		else
			rt = pRb->restype; // other

		switch (rt)
		{
		case RTSHORT:
			acutPrintf(_T("\n(%d . %d)"), pRb->restype, pRb->resval.rint);
			break;
		case RTREAL:
			acutPrintf(_T("\n(%d . %0.3f)"), pRb->restype, pRb->resval.rreal);
			break;
		case RTSTR:
			acutPrintf(_T("\n(%d . \"%s\")"), pRb->restype, pRb->resval.rstring);
			break;
		case RT3DPOINT:
			acutPrintf(_T("\n(%d %0.3f %0.3f %0.3f)"), pRb->restype, pRb->resval.rpoint[X], pRb->resval.rpoint[Y], pRb->resval.rpoint[Z]);
			break;
		case RTLONG:
			acutPrintf(_T("\n%ld"), pRb->resval.rlong);
			break;
		}
		pRb = pRb->rbnext;
	}
}

AcDbObjectId getRegApp()
{
	AcDbRegAppTable *pRegAppTable = nullptr;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pRegAppTable, AcDb::kForRead);

	AcDbRegAppTableIterator *pRegAppIter = nullptr;
	if (pRegAppTable->newIterator(pRegAppIter) == Acad::eOk)
	{
		for (pRegAppIter->start(); !pRegAppIter->done(); pRegAppIter->step())
		{
			AcDbRegAppTableRecord *pRegAppTableRec = nullptr;
			if (pRegAppIter->getRecord(pRegAppTableRec, AcDb::kForRead) == Acad::eOk)
			{
				AcString szRegAppName;
				pRegAppTableRec->getName(szRegAppName);
				pRegAppTableRec->close();

				if (szRegAppName == _T("KK"))
				{
					acutPrintf(_T("\nRegApp:%s"), szRegAppName.constPtr());
					return pRegAppTableRec->objectId();
				}
			}
		}

		delete pRegAppIter;
		pRegAppIter = nullptr;
	}
	pRegAppTable->close();
	return AcDbObjectId::kNull;
}
