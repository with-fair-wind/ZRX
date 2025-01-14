#pragma once

#include <dbobjectoverrule.h>

class BCCircleOverrule : public AcDbObjectOverrule
{
public:
	BCCircleOverrule(bool canCopy) : m_bCanCopy{canCopy} {}
	virtual Acad::ErrorStatus deepClone(const AcDbObject *pSubject,
										AcDbObject *pOwnerObject,
										AcDbObject *&pClonedObject,
										AcDbIdMapping &idMap,
										Adesk::Boolean isPrimary = true) override;

	virtual Acad::ErrorStatus wblockClone(const AcDbObject *pSubject,
										  AcRxObject *pOwnerObject,
										  AcDbObject *&pClonedObject,
										  AcDbIdMapping &idMap,
										  Adesk::Boolean isPrimary = true) override;

	bool isApplicable(const AcRxObject *pOverruledSubject) const;

private:
	bool m_bCanCopy = true;
};