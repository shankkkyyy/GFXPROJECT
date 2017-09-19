#include "Terrian.h"
#include "Engine.h"
#include "Vertex.h"



Terrian::Terrian()
{
}


Terrian::~Terrian()
{
	delete mCtrlPts;
	delete mIndices;
}

void Terrian::Initialize(const TerrianInfo & _info)
{
	mInfo = _info;

	mNumCtrlPtsRows = ((mInfo.heightMapDepth - 1) / MAXTESSFACTOR) + 1;
	mNumCtrlPtsCols = ((mInfo.heightMapWidth - 1) / MAXTESSFACTOR) + 1;

	mTotalCtrlPts = mNumCtrlPtsRows * mNumCtrlPtsCols;
	mTotalQuadFaces = (mNumCtrlPtsRows - 1) * (mNumCtrlPtsCols - 1);

	float width = (mInfo.heightMapWidth - 1) * mInfo.cellSpacing;
	float depth = (mInfo.heightMapDepth - 1) * mInfo.cellSpacing;

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float patchWidth = width / (mNumCtrlPtsCols - 1);
	float patchDepth = depth / (mNumCtrlPtsRows - 1);

	float du = 1.0f / (mNumCtrlPtsCols - 1);
	float dv = 1.0f / (mNumCtrlPtsRows - 1);

	mCtrlPts = new std::vector<VertexTerrian>(mTotalCtrlPts);
	mIndices = new std::vector<UINT>(4 * mTotalQuadFaces);


	//build the vertex data for control points
	float x, z;
	UINT currentIndex;
	for (UINT iCol = 0; iCol < mNumCtrlPtsCols; iCol++)
	{
		x = -halfWidth + iCol * patchWidth;
		for (size_t iRow = 0; iRow < mNumCtrlPtsRows; iRow++)
		{
			z = halfDepth - iRow * patchDepth;
			currentIndex = (UINT)(iRow * mNumCtrlPtsCols + iCol);
			(*mCtrlPts)[currentIndex].pos = XMFLOAT3(x, 0, z);
			(*mCtrlPts)[currentIndex].uv = XMFLOAT2(iCol * du, iRow * dv);
		}
	}

	currentIndex = 0;
	for (UINT iCol = 0; iCol < mNumCtrlPtsCols - 1; iCol++)
	{
		for (size_t iRow = 0; iRow < mNumCtrlPtsRows - 1; iRow++)
		{
			(*mIndices)[currentIndex]     = (UINT)(iRow * mNumCtrlPtsCols + iCol          );
			(*mIndices)[currentIndex + 1] = (UINT)(iRow * mNumCtrlPtsCols + iCol + 1      );
			(*mIndices)[currentIndex + 2] = (UINT)((iRow + 1) * mNumCtrlPtsCols + iCol    );
			(*mIndices)[currentIndex + 3] = (UINT)((iRow + 1) * mNumCtrlPtsCols + iCol + 1);

			currentIndex += 4;
		}
	}
}

void Terrian::ReleaseResource()
{
	mQuadCtrlPtsBuffer.Reset();
	mQuadIndicesBuffer.Reset();
}

void Terrian::Draw()
{

	const Engine* const engine = Engine::GetEngine();
	ID3D11DeviceContext* devContext = engine->GetDeviceContext();
	UINT stride = sizeof(VertexTerrian), offset = 0;

	devContext->IASetVertexBuffers(0, 1, mQuadCtrlPtsBuffer.GetAddressOf(), &stride, &offset);
	devContext->IASetIndexBuffer(mQuadIndicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	devContext->IASetInputLayout(engine->GetShaders()->GetTerrianIL());
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	devContext->DrawIndexed((UINT)mIndices->size(), 0, 0);


}

const std::vector<struct VertexTerrian>* const Terrian::GetCtrlPoints() const
{
	return mCtrlPts;
}

const std::vector<UINT>* const Terrian::GetIndices() const
{
	return mIndices;
}

void Terrian::CreateGeometryBuffer()
{
	ID3D11Device* device = Engine::GetEngine()->GetDevice();

	D3D11_BUFFER_DESC vbd, ibd;
	D3D11_SUBRESOURCE_DATA initData;

	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VertexTerrian) * (UINT)(mCtrlPts->size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	initData.pSysMem = mCtrlPts->data();
	HR(device->CreateBuffer(&vbd, &initData, &mQuadCtrlPtsBuffer));

	ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.ByteWidth = sizeof(UINT) * (UINT)(mIndices->size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	initData.pSysMem = mIndices->data();

	HR(device->CreateBuffer(&ibd, &initData, &mQuadIndicesBuffer));

}
