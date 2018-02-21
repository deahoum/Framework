#include "ModelClass.h"



ModelClass::ModelClass()
	: m_vertexBuffer(NULL),
	m_indexBuffer(NULL),
	m_Texture(NULL)
{
}

ModelClass::ModelClass(const ModelClass& arg)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Init(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext, char* _textureFilename)
{
	if (!InitBuffer(_device))
	{
		return false;
	}

	// Load the texture for this model
	if (!LoadTexture(_device, _deviceContext, _textureFilename))
	{
		return false;
	}

	return true;
}

void ModelClass::Destroy()
{
	ReleaseTexture();

	DestroyBuffers();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* _devicecontext)
{
	RenderBuffers(_devicecontext);
	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitBuffer(ID3D11Device* _device)
{
	//VertexType* vertices;
	//TexVertexType* vertices;
	TNVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	m_vertexCount = 3;

	m_indexCount = 3;

	//vertices = new VertexType[m_vertexCount];
	vertices = new TNVertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
#ifdef PIXEL_COLOR_BUFFER
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
#endif 

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
#ifdef PIXEL_COLOR_BUFFER
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
#endif
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
#ifdef PIXEL_COLOR_BUFFER
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
#endif

	indices[0] = 0;  
	indices[1] = 1;  
	indices[2] = 2;  

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(TNVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(_device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	return true;
}

void ModelClass::DestroyBuffers()
{
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//stride = sizeof(VertexType);
	stride = sizeof(TNVertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext, char* _filename)
{
	m_Texture = new TextureClass;

	if (!m_Texture)
	{
		return false;
	}

	if (!m_Texture->Init(_device, _deviceContext, _filename))
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Destroy();
		SAFE_DELETE(m_Texture);
	}

	return;
}