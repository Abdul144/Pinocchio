#pragma once

#include <string>
#include <windows.h>

#include "../util/defines.h"
#include "Singleton.h"


#define BMP_EXPORTER		BmpExporter::getInstance()

using namespace std;


class BmpExporter : public Singleton<BmpExporter>
{
	USE_SINGLETON(BmpExporter)

	// ��� ����
private:


	// ��� �Լ�
public:
	/// ����
	void export(const string &filePath, int width, int height, byte *buffer, bool conversion32to24 = true);


};


