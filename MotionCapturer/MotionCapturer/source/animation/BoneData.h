#pragma once

#include <string>

#include "../util/Vector3.h"
#include "../util/Quaternion.h"


using namespace std;

struct BoneData
{
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;

	BoneData() : scale(1.f, 1.f, 1.f)
	{
	}
};