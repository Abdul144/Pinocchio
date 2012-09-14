#pragma once

#include <vector>
#include <hash_map>

#include "../util/defines.h"
#include "../util/Matrix.h"
#include "../util/Vector3.h"
#include "../util/Quaternion.h"


using namespace std;

class Animation;
class AnimationInstance;
class Bone;

class Actor
{
	// ��� ����
private:
    vector<Bone*> boneList;				///< free
    hash_map<int, Bone*> boneMap;		///< Bone::BoneType , Bone* �ؽø�.. no free
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    AnimationInstance *animationInstance;	///< �ִϸ��̼� �ν��Ͻ�.. free


	// ��� �Լ�
private:
	void addBone(int parentType, const Vector3 &localPosition, const Quaternion &localRotation, const Vector3 &localScale,
        string name, int type);

	void destroy();

public:
    Actor();
	~Actor();

    void initializeBones();
    
    void update(int deltaFrame);

    void draw();

    void setCurAnimation(Animation *animation);

	inline Bone* getBone(int index)
	{
		return boneList[index];
	}


    // ����
    GETTER_REF(Vector3, Position, position)
    GETTER_REF(Vector3, Rotation, rotation)
    GETTER_REF(Vector3, Scale, scale)
	GETTER(int, BoneCount, boneList.size())
};