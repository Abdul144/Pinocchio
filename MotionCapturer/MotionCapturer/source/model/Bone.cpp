
#include "stdafx.h"
#include "Bone.h"

#include "../animation/BoneData.h"
#include "../util/Vector3.h"
#include "../util/Quaternion.h"

Bone::Bone(Bone *parent, const string &name, BoneType type)
{
    this->parent = parent;
    this->name = name;
    this->type = type;

}

/// 두 키프레임의 본 데이터를 alpha[0,1]의 비율로 블렌딩하여 변환행렬 업데이트
void Bone::update(BoneData *curData, BoneData *nextData, float alpha)
{
    Matrix objectTransform;
    if (nextData != null)
    {   // 다음 데이터가 있다.. 두 데이터를 보간
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
		
        // 보간
		position = curData->position * (1.f - alpha) + nextData->position * alpha;
		rotation.slerp(alpha, curData->rotation, nextData->rotation);
		scale = curData->scale * (1.f - alpha) + nextData->scale * alpha;

        // 행렬 설정
		Matrix scaleMat, rotationMat, positionMat;
		scaleMat.setScale(scale);
		rotationMat.setRotate(rotation);
		positionMat.setTranslate(position);

		objectTransform.multiply(positionMat, rotationMat);
		objectTransform.multiply(scaleMat);
    }
    else
    {   // 다음 데이터가 없다.. 현재 데이터 적용
        // 행렬 설정
		Matrix scaleMat, rotationMat, positionMat;
		scaleMat.setScale(curData->scale);
		rotationMat.setRotate(curData->rotation);
		positionMat.setTranslate(curData->position);

		objectTransform.multiply(positionMat, rotationMat);
		objectTransform.multiply(scaleMat);
    }

    // 변환행렬 설정
    if (parent != null)
	{
		transform.multiply(parent->transform, transformFromParent);
		transform.multiply(objectTransform);

	}else
	{
		transform.multiply(transformFromParent, objectTransform);
	}
}
