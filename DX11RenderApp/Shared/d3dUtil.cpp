#include "d3dUtil.h"



bool d3dHelper::LoadShaderByteCode(const wchar_t* _path, std::vector<char>& _byteCode)
{
	std::ifstream fin;
	fin.open(_path, std::ios_base::binary);
	_byteCode.clear();
	if (fin.is_open())
	{
		fin.seekg(0, std::ios_base::end);
		unsigned byteSize = (unsigned)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		_byteCode. resize(byteSize);
		fin.read(_byteCode.data(), byteSize);
		fin.close();
		return true;
	}
	else
	{
		std::wstring msg = L"Can not open file at: " + TextHelper::ToString(_path);
		MessageBox(0, msg.c_str(), 0, 0);
		return false;
	}
}

void d3dHelper::ForwardVector(XMFLOAT3 & _val)
{
	_val.x = 0;
	_val.y = 0;
	_val.z = 1;
}

void d3dHelper::RightVector(XMFLOAT3 & _val)
{
	_val.x = 1;
	_val.y = 0;
	_val.z = 0;

}

void d3dHelper::UpVector(XMFLOAT3 & _val)
{
	_val.x = 0;
	_val.y = 1;
	_val.z = 0;

}
