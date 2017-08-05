#include "Object.h"



Object::Object()
{
}


Object::~Object()
{
}

void Object::LoadObjFile(const char * _path, Mesh & _out)
{

	FILE* file;
	fopen_s(&file, _path, "r");
	if (file == nullptr)
	{
		std::wstring sdf = L"Can not open file at: " + TextHelper::ToString(_path);
		MessageBox(nullptr, sdf.c_str(), 0, 0);
		return;
	}
	else
	{

#pragma region Go throught the file, and get the size of geometry data to read, and create temp memory

		UINT ip = 0;    // count of position
		UINT iuv = 0; // count of uv
		UINT in = 0; // count of normal
		UINT ii = 0; // count of indices

		while (true)
		{
			char lineHeader[128];
			int read = fscanf_s(file, "%s", lineHeader, 128);
			if (read == EOF)
			{
				break;
			}
			else if (strcmp(lineHeader, "v") == 0)
			{
				ip++;
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				iuv++;
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				in++;
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				ii++;
			}

		}

		ii = ii * 3;

		XMFLOAT3* pos = new XMFLOAT3[ip];
		XMFLOAT3* nor = new XMFLOAT3[in];
		XMFLOAT2* uv = new XMFLOAT2[iuv];

#pragma endregion

#pragma region Go throught the file again and read the data

		UINT outMeshIndices = 0;
		_out.indices.clear();
		_out.indices.resize(ii);

		// works as index from now
		ip = 0;
		in = 0;
		iuv = 0;

		rewind(file);
	

		unsigned(*hFunc)(const OBJVertexIndices&) = [](const OBJVertexIndices& a) {return (a.normalIndex + a.positionIndex + a.uvIndex) % 1000; };

		HTable<OBJVertexIndices> uniqueTable(1000, hFunc);

		std::vector<OBJVertexIndices> uniqueVertexIndices;

		OBJVertexIndices tempVertecIndices[3];

		while (true)
		{
			char lineHeader[128];
			int read = fscanf_s(file, "%s", lineHeader, 128);
			if (read == EOF)
			{
				break;
			}
			else if (strcmp(lineHeader, "v") == 0)
			{
				fscanf_s(file, "%f %f %fn", &(pos[ip].x), &(pos[ip].y), &(pos[ip].z));
				ip++;
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				fscanf_s(file, "%f %fn", &(uv[iuv].x), &(uv[iuv].y));
				iuv++;
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				fscanf_s(file, "%f %f %fn", &(nor[in].x), &(nor[in].y), &(nor[in].z));
				in++;
			}
			else if (strcmp(lineHeader, "f") == 0)
			{

				fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%dn",
					&(tempVertecIndices[0].positionIndex), &(tempVertecIndices[0].uvIndex), &(tempVertecIndices[0].normalIndex),
					&(tempVertecIndices[1].positionIndex), &(tempVertecIndices[1].uvIndex), &(tempVertecIndices[1].normalIndex),
					&(tempVertecIndices[2].positionIndex), &(tempVertecIndices[2].uvIndex), &(tempVertecIndices[2].normalIndex));

				const OBJVertexIndices* dupicate;
#pragma region Check if it is unique vertex, and then add to the unique vertex list, and assemble the indices array

				for (UINT it = 0; it < 3; it++)
				{
					dupicate = uniqueTable.Find(tempVertecIndices[it]);
					if (dupicate != nullptr)
					{
						// it is not unique, put the duplicate index in our indices
						_out.indices[outMeshIndices] = dupicate->Index;
						outMeshIndices++;
					}
					else
					{
						// it is unique
						_out.indices[outMeshIndices] = uniqueVertexIndices.size();
						tempVertecIndices[it].Index = uniqueVertexIndices.size();
						uniqueVertexIndices.push_back(tempVertecIndices[it]);
						uniqueTable.Insert(tempVertecIndices[it]);
						outMeshIndices++;
					}
					//for (UINT iv = 0; iv < uniqueVertexIndics.size(); iv++)
					//{
					//	// if not unique, put this index in the indices
					//	if (tempVertecIndics[it] == uniqueVertexIndics[iv])
					//	{
					//		_out.indices[outMeshIndices] = iv;
					//		outMeshIndices++;
					//		isUnique = false;
					//		break;
					//	}
					//}
					//if (isUnique)
					//{
					//	_out.indices[outMeshIndices] = uniqueVertexIndices.size();
					//	outMeshIndices++;
					//	uniqueVertexIndices.push_back(tempVertecIndics[it]);
					//}
				}
#pragma endregion

			}
		}
#pragma endregion



#pragma region Build Our Vertex According to Unique vertex list

		_out.vertices.clear();
		_out.vertices.resize(uniqueVertexIndices.size());

		for (UINT i = 0; i < _out.vertices.size(); i++)
		{
			_out.vertices[i].pos = pos[(uniqueVertexIndices[i].positionIndex - 1)];
			_out.vertices[i].nor = nor[(uniqueVertexIndices[i].normalIndex - 1)];
			_out.vertices[i].uv = uv[(uniqueVertexIndices[i].uvIndex - 1)];
		}
#pragma endregion


		delete[] pos;
		delete[] nor;
		delete[] uv;
		return;
	}
}

