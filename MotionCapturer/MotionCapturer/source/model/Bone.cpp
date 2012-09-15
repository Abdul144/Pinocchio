
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

/// �� Ű�������� �� �����͸� alpha[0,1]�� ������ �����Ͽ� ��ȯ��� ������Ʈ
void Bone::update(BoneData *curData, BoneData *nextData, float alpha)
{
    Matrix objectTransform;
    if (nextData != null)
    {   // ���� �����Ͱ� �ִ�.. �� �����͸� ����
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
		
        // ����
		position = curData->position * (1.f - alpha) + nextData->position * alpha;
		rotation.slerp(alpha, curData->rotation, nextData->rotation);
		scale = curData->scale * (1.f - alpha) + nextData->scale * alpha;

        // ��� ����
		Matrix scaleMat, rotationMat, positionMat;
		scaleMat.setScale(scale);
		rotationMat.setRotate(rotation);
		positionMat.setTranslate(position);

		objectTransform.multiply(positionMat, rotationMat);
		objectTransform.multiply(scaleMat);
    }
    else
    {   // ���� �����Ͱ� ����.. ���� ������ ����
        // ��� ����
		Matrix scaleMat, rotationMat, positionMat;
		scaleMat.setScale(curData->scale);
		rotationMat.setRotate(curData->rotation);
		positionMat.setTranslate(curData->position);

		objectTransform.multiply(positionMat, rotationMat);
		objectTransform.multiply(scaleMat);
    }

    // ��ȯ��� ����
    if (parent != null)
	{
		transform.multiply(parent->transform, transformFromParent);
		transform.multiply(objectTransform);

	}else
	{
		transform.multiply(transformFromParent, objectTransform);
	}
}
