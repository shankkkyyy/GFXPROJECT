#pragma once

// Include Header file for Memory Leak Detection in Debug mode
#if defined(DEBUG) | defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


#include <d3dx11effect.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>



// Include DX debug code, and also d3d11.h, windows.h
#include "dxerr.h"
// Include For Validation Check
#include <cassert>
#include <string>


#include <fstream>
#include <vector>
#include "Mathhelper.h"

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

	static bool LoadShaderByteCode(const char* _filepath, ID3D11Device* _d3dDevice, ID3DX11Effect** _effect);
};

