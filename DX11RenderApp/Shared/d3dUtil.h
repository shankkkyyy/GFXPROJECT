#pragma once

// Include Header file for Memory Leak Detection in Debug mode
#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif


#define KEY_W 0x57
#define KEY_S 0x53
#define KEY_A 0x41
#define KEY_D 0x44


// has d3d11.h
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include "ddsTextureloader.h"

// Include DX debug code, windows.h
#include "dxerr.h"
// Include For Validation Check
#include <cassert>
#include <string>


#include <fstream>
#include <sstream>
#include <vector>
#include <forward_list>
#include <wrl.h> // for COM
#include <Algorithm>


#include "Mathhelper.h"
#include "Lighting.h"
#include "GeometryGenerator.h"


#pragma comment (lib, "d3d11.lib")
using namespace DirectX;








#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR							                               
	#define HR(x)						                               \
	{									                               \
		HRESULT hr = x;										           \
		if (FAILED(hr))												   \
		{															   \
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);		   \
		}															   \
	}									   
	#endif // !HR

#else
	#ifndef HR
	#define HR(x) (x)
	#endif // !HR
#endif

#define ReleaseCOM(x) {if(x){x->Release(); x= 0;}}

#define SafeDelete_Array(x) {delete[] x; x= 0;}

#define SafeDelete(x) {delete x; x = 0;}





class TextHelper
{
public:
	template<typename T>
	static std::wstring ToString(const T& s)
	{
		std::wostringstream oss;
		oss << s;
		return oss.str();
	}

	template<typename T>
	static T FromString(const std::wstring& s)
	{
		T x;
		std::wistringstream iss;
		iss >> x;
		return x;
	}

};

class d3dHelper
{
public:

	static bool LoadShaderByteCode(const wchar_t* _path, std::vector<char>& _byteCode);
	static void ForwardVector(XMFLOAT3& _val);
	static void RightVector(XMFLOAT3& _val);
	static void UpVector(XMFLOAT3& _val);
};



#pragma region From CPU to GPU


struct VSCBPerFrame
{
	XMFLOAT4X4 camPosTransform;
	XMFLOAT4X4 viewProj;
};

struct PSCBPerFrame
{
	XMFLOAT3  eyePosition; 
	float     fogRange; //16
	XMFLOAT3  fogColor;
	float     fogStart; //32
	Light     light[3];    //32 + 48 * 3 = 176
	XMFLOAT4  ambientLight; //192
};

struct GSCBPerFrame
{
	XMFLOAT4   eyePosition;
	XMFLOAT4X4 viewProj;
};

struct VSCBPerObj
{
	XMFLOAT4X4 wvp;
	XMFLOAT4X4 world;
	XMFLOAT4X4 texTransform;
};

struct PSCBPerObj
{
	Material   material; // 4
	XMINT4     renderSetting; // x: whether has diffuse texture, y: isTransparent, z: tex Amount
};

struct InstanceData
{
	XMFLOAT4X4 world;
};



#pragma endregion

struct OBJVertexIndices
{
	UINT positionIndex;
	UINT uvIndex;
	UINT normalIndex;
	UINT Index = 0;

	OBJVertexIndices() {}
	OBJVertexIndices(UINT _vi, UINT _uvi, UINT _ni) :
		positionIndex(_vi), uvIndex(_uvi), normalIndex(_ni)
	{}
	bool operator== (const OBJVertexIndices& _other)
	{
		if (this == &_other)
		{
			return true;
		}
		else
		{
			if (positionIndex == _other.positionIndex
				&& uvIndex == _other.uvIndex
				&& normalIndex == _other.normalIndex)
			{
				return true;
			}
			else return false;
		}
	}
};

#pragma region  Hash Search Table


template<typename Type>
class HTable
{

	const unsigned           mBucketSize;
	unsigned                 mSize;
	unsigned(*hFunction)(const Type &v);

	std::forward_list<Type>* mEntries;

public:

	HTable(unsigned _bucketSize, unsigned(*_hFunc)(const Type &v));
	~HTable();
	HTable(const HTable& _other) = delete;
	HTable& operator = (const HTable& _other) = delete;

public:

	unsigned GetSize() const;

	// Research the agru, return the research result 
	const Type* const Find(const Type& _v);
	void Insert(const Type& _v);

};

template<typename Type>
inline HTable<Type>::HTable(unsigned _bucketSize, unsigned(*_hFunc)(const Type &v))
	:mBucketSize(_bucketSize), hFunction(_hFunc), mSize(0)
{
	mEntries = new std::forward_list<Type>[_bucketSize];
}

template<typename Type>
inline HTable<Type>::~HTable()
{
	delete[] mEntries;
}

template<typename Type>
inline unsigned HTable<Type>::GetSize() const
{
	return mSize;
}

template<typename Type>
inline const Type* const HTable<Type>::Find(const Type& _v) 
{
	unsigned bucket = hFunction(_v);

	auto iter = mEntries[bucket].begin();
	Type* bFind = nullptr;
	while (iter != mEntries[bucket].end())
	{
		if (*iter == _v)
		{
			bFind = &(*iter);
			break;
		}
		iter++;
	}
	return bFind;
}

template<typename Type>
inline void HTable<Type>::Insert(const Type & _v)
{
	unsigned bucket = hFunction(_v);
	mEntries[bucket].push_front(_v);
	mSize++;
}


#pragma endregion

