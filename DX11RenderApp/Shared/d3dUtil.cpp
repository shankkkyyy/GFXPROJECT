#include "d3dUtil.h"



bool d3dHelper::LoadShaderByteCode(const char * _filepath, ID3D11Device* _d3dDevice, ID3DX11Effect** _effect)
{
	std::ifstream fin;
	fin.open(_filepath, std::ios::binary);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios_base::end);
		int size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		std::vector<char> compiledShader(size);
		fin.read(compiledShader.data(), size);
		fin.close();

		HR(D3DX11CreateEffectFromMemory(compiledShader.data(), size, 0, _d3dDevice, _effect));
		return true;
	}
	else
	{
		return false;
	}

}
