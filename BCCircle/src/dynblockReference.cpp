#include <dynblockReference.h>

void test_dynamicBlock()
{
    ads_name ename;
    ads_point ptres;
    acedInitGet(NULL, NULL);
    int ret = acedEntSel(_T("\n—°‘Ò∂ØÃ¨øÈ≤Œ’’: "), ename, ptres);

    if (RTNORM == ret)
    {
        AcDbObjectId objId;
        acdbGetObjectId(objId, ename);

        AcDbDynBlockReference dynBlkRef(objId);
        bool isDynBlkRef = dynBlkRef.isDynamicBlock();
        acutPrintf(_T("\nisDynBlkRef: %s"), isDynBlkRef ? _T("true") : _T("false"));
        if (isDynBlkRef)
        {
            AcDbDynBlockReferencePropertyArray propertyArray;
            dynBlkRef.getBlockProperties(propertyArray);
            for (int i = 0; i < propertyArray.logicalLength(); ++i)
            {
                AcString propertyName = propertyArray[i].propertyName();
                acutPrintf(_T("\nname: %s "), propertyName);
                if (propertyName == _T("æ‡¿Î"))
                {
                    AcDb::DwgDataType type = propertyArray[i].propertyType();
                    if (type == AcDb::kDwgReal)
                    {
                        AcDbEvalVariant val = propertyArray[i].value();
                        double distance = 0.;
                        val.getValue(distance);
                        acutPrintf(_T("\ndistance: %f"), distance);
                        if (distance > 0.)
                        {
                            Acad::ErrorStatus es = propertyArray[i].setValue(AcDbEvalVariant(110.5));
                            if (es != Acad::eOk)
                                acutPrintf(_T("\n setValue fail "));
                        }
                    }
                }
            }
        }
    }
}