#include <zdbidmap.h>
#include "BCCircleCopyRule.h"
#include "BCCircle.h"

Acad::ErrorStatus BCCircleOverrule::deepClone(const AcDbObject *pSubject, AcDbObject *pOwnerObject, AcDbObject *&pClonedObject, AcDbIdMapping &idMap, Adesk::Boolean isPrimary)
{
	Acad::ErrorStatus es = Acad::eOk;
	if (isApplicable(pSubject))
	{
		es = AcDbObjectOverrule::deepClone(pSubject, pOwnerObject, pClonedObject, idMap, isPrimary);

#if 1
		if (pSubject && pSubject->isKindOf(BCCircle::desc()))
		{
			BCCircle *bccircle = BCCircle::cast(pSubject);
			if (bccircle && bccircle->hasChild())
			{
				const std::vector<AcDbHardPointerId> &childVec = bccircle->getChildVec();
				for (const auto &childId : childVec)
				{
					BCCircle *pChild = nullptr;
					acdbOpenObject(pChild, childId, AcDb::kForRead);

					AcDbObject *pCloneObj = nullptr;
					AcDbIdMapping childIdMap;
					es = pChild->deepClone(pOwnerObject, pCloneObj, childIdMap, false);
					// es = AcDbObjectOverrule::deepClone(pChild, pOwnerObject, pCloneObj, childIdMap, false);
					pChild->close();
				}
			}
		}
#endif

		if (pClonedObject && pClonedObject->isKindOf(BCCircle::desc()))
		{
			BCCircle *bccircle = BCCircle::cast(pClonedObject);
			bccircle->setColorIndex(1);

			if (bccircle->hasChild())
				bccircle->clearChild();
		}
		return es;
	}
	return AcDbObjectOverrule::deepClone(pSubject, pOwnerObject, pClonedObject, idMap, isPrimary);
}

Acad::ErrorStatus BCCircleOverrule::wblockClone(const AcDbObject *pSubject, AcRxObject *pOwnerObject, AcDbObject *&pClonedObject, AcDbIdMapping &idMap, Adesk::Boolean isPrimary)
{
	return Acad::ErrorStatus();
}

bool BCCircleOverrule::isApplicable(const AcRxObject *pOverruledSubject) const
{
	return pOverruledSubject->isKindOf(BCCircle::desc());
}
