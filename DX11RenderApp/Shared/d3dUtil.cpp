#include "d3dUtil.h"

void d3dHelper::LoadShaderByteCode()
{
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
