#pragma once

#include <string>

#include "../util/defines.h"
#include "../util/Matrix.h"
#include "../util/Vector3.h"


using namespace std;

class BoneData;


class Bone
{
public:
    enum BoneType
    {
        None				= -1,
        Hip_Center			= 0,
        Spine,
        Shoulder_Center,
            
        Head,
            
        Shoulder_Right,
        Elbow_Right,
        Wrist_Right,
        Hand_Right,


        Shoulder_Left,
        Elbow_Left,
        Wrist_Left,
        Hand_Left,

        Hip_Right,
        Knee_Right,
        Ankle_Right,
        Foot_Right,

        Hip_Left,
        Knee_Left,
        Ankle_Left,
        Foot_Left
    };


	// 멤버 변수
private:
    Bone *parent;				///< no free
    string name;
    BoneType type;

	Vector3 localEndPosition;	///< 현재 본의 끝 부분
    Matrix transform;
    Matrix transformFromParent;	///< 부모로 부터의 변환행렬


	// 멤버 함수
public:
    Bone(Bone *parent, const string &name, BoneType type);


    /// 두 키프레임의 본 데이터를 alpha[0,1]의 비율로 블렌딩하여 변환행렬 업데이트
    void update(BoneData *curData, BoneData *nextData, float alpha);


    // 접근
	GETTER(BoneType, BoneType, type)
	GETTER_SETTER_CONST_REF(Vector3, LocalEndPosition, localEndPosition)
    GETTER_CONST_REF(string, Name ,name)
    GETTER_CONST_REF(Matrix, Transform, transform)
    GETTER_SETTER_CONST_REF(Matrix, TransformFromParent, transformFromParent)
};

