
#include "stdafx.h"
#include "Actor.h"

#include <GL/glew.h>

#include "../animation/AnimationInstance.h"
#include "Bone.h"


Actor::Actor() : animationInstance(null)
{
	animationInstance = new AnimationInstance;

	scale.set(0.01f, 0.01f, 0.01f);

    // 본리스트 초기화
    initializeBones();
}

Actor::~Actor()
{
	destroy();
}

void Actor::destroy()
{
	for (uint i=0; i<boneList.size(); ++i)
	{
		DELETE(boneList[i])
	}
	boneList.clear();
	boneMap.clear();
}

void Actor::addBone(int parentType, const Vector3 &localPosition, const Quaternion &localRotation, const Vector3 &localScale,
        string name, int type)
{
    // 본을 생성
    Bone *parentBone = null;
	if (boneMap.find(parentType) != boneMap.end())
        parentBone = boneMap[parentType];
	Bone *bone = new Bone(parentBone, name, (Bone::BoneType)type);

	// 상대 위치 설정
	bone->setLocalPosition(localPosition);

    // 본의 부모상대 변환행렬 설정
	Matrix transMat, rotMat, scaleMat, localTransform;
	transMat.setTranslate(localPosition);
	rotMat.setRotate(localRotation);
	scaleMat.setScale(localScale);
	localTransform.multiply(transMat, rotMat);
	localTransform.multiply(scaleMat);
	bone->setTransformFromParent(localTransform);

    // 리스트와 맵에 추가
	boneList.push_back(bone);
	boneMap[bone->getBoneType()] = bone;
}

void Actor::initializeBones()
{
    // 초기화
    const float baseSize = 10.f;
    destroy();

    /// 본 추가
    addBone(Bone::None, Vector3(0.f,0.f,0.f), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Hip_Center", Bone::Hip_Center);
    addBone(Bone::Hip_Center, Vector3(0.f, 20.f, 0.f), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Spine", Bone::Spine);
    addBone(Bone::Spine, Vector3(0.f, 50.f, 0.f), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Shoulder_Center", Bone::Shoulder_Center);

    addBone(Bone::Shoulder_Center, Vector3(0, 25, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Head", Bone::Head);

    addBone(Bone::Shoulder_Center, Vector3(-30, -10, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Shoulder_Right", Bone::Shoulder_Right);
    addBone(Bone::Shoulder_Right, Vector3(-50, 0, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Elbow_Right", Bone::Elbow_Right);
    addBone(Bone::Elbow_Right, Vector3(-40, 0, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Wrist_Right", Bone::Wrist_Right);
    addBone(Bone::Wrist_Right, Vector3(-20, 0, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Hand_Right", Bone::Hand_Right);

    addBone(Bone::Shoulder_Center, Vector3(30, -10, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Shoulder_Left", Bone::Shoulder_Left);
    addBone(Bone::Shoulder_Left, Vector3(50, 0, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Elbow_Left", Bone::Elbow_Left);
    addBone(Bone::Elbow_Left, Vector3(40 , 0, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Wrist_Left", Bone::Wrist_Left);
    addBone(Bone::Wrist_Left, Vector3(20, 0, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Hand_Left", Bone::Hand_Left);

    addBone(Bone::Hip_Center, Vector3(-20, -10, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Hip_Right", Bone::Hip_Right);
    addBone(Bone::Hip_Right, Vector3(0, -90, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Knee_Right", Bone::Knee_Right);
    addBone(Bone::Knee_Right, Vector3(0, -70, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Ankle_Right", Bone::Ankle_Right);
    addBone(Bone::Ankle_Right, Vector3(0, -10, -20), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Foot_Right", Bone::Foot_Right);

    addBone(Bone::Hip_Center, Vector3(20, -10, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Hip_Left", Bone::Hip_Left);
    addBone(Bone::Hip_Left, Vector3(0, -90, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Knee_Left", Bone::Knee_Left);
    addBone(Bone::Knee_Left, Vector3(0, -70, 0), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Ankle_Left", Bone::Ankle_Left);
    addBone(Bone::Ankle_Left, Vector3(0, -10, -20), Quaternion(), Vector3(1.f, 1.f, 1.f),
        "Foot_Left", Bone::Foot_Left);

}

    
void Actor::update(int deltaFrame)
{
    // 애니메이션 업데이트
    animationInstance->update(boneList, deltaFrame);
}

void Actor::draw()
{
	glPushMatrix();

	// 변환행렬 설정
	glTranslatef(position.getX(), position.getY(), position.getZ());
	glRotatef(rotation.getY(), 0.f, 1.f, 0.f);
	glRotatef(rotation.getX(), 1.f, 0.f, 0.f);
	glRotatef(rotation.getZ(), 0.f, 0.f, 1.f);
	glScalef(scale.getX(), scale.getY(), scale.getZ());
	
	glPointSize(10.f);
	glColor4ub(255, 255, 0, 255);
	
	for (uint i=0; i<boneList.size(); ++i)
	{
		glPushMatrix();

		glMultMatrixf(boneList[i]->getTransform().getPointer());
			
		glBegin(GL_POINTS);
			glVertex3f(0,0,0);
		glEnd();
			
		glPopMatrix();
	}

	glPopMatrix();
}

void Actor::setCurAnimation(Animation *animation)
{
	animationInstance->setAnimation(animation);
}
