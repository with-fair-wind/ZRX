#pragma once

#include <iostream>
#include <dbdimassoc.h>

void test_dim();
void modifyDistance(const AcDbObjectIdArray &objIdArr, double oriDistance, double targetDistance);

void modifyDistance(const AcDbObjectId &lineId, double oriDistance, double targetDistance);