#pragma once

#include <string>

#include "../util/Vector3.h"
#include "../util/Quaternion.h"


using namespace std;

struct BoneData
{
    string name;
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;

};