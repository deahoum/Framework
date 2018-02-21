#include "ColorShaderClass.h"



ColorShaderClass::ColorShaderClass()
	: m_vertexShader(NULL),
	m_pixelShader(NULL),
	m_layout(NULL),
	m_matrixBuffer(NULL)
{
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& arg)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Init(ID3D11Device* _device, HWND _hWnd)
{
	if (!InitShader(_device, _hWnd, L"../Shader/color.vs", L"../Shader/color.ps"))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Destroy()
{
	DestroyShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* _deviceContext, int _indexCount, XMMATRIX& _worldMatrix, XMMATRIX& _viewMatrix,
	XMMATRIX& _projectionMatrix)
{
	if (!SetShaderParameters(_deviceContext, _worldMatrix, _viewMatrix, _projectionMatrix))
	{
		return false;
	}

	RenderShader(_deviceContext, _indexCount);

	return true;
}

bool ColorShaderClass::InitShader(ID3D11Device* _device, HWND _hWnd, WCHAR* _vsFilename, WCHAR* _psFilename)
{
	ID3D10Blob* errorMessage = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	if (FAILED(D3DCompileFromFile(_vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage)))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, _hWnd, _vsFilename);
		}
		else
		{
			MessageBox(_hWnd, _vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	if (FAILED(D3DCompileFromFile(_psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage)))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, _hWnd, _psFilename);
		}
		else
		{
			MessageBox(_hWnd, _psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	if (FAILED(_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
	{
		return false;
	}

	if (FAILED(_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	if (FAILED(_device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if (FAILED(_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::DestroyShader()
{
	SAFE_RELEASE(m_matrixBuffer);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* _errorMessage, HWND _hWnd, WCHAR* _shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;

	compileErrors = (char*)(_errorMessage->GetBufferPointer());

	bufferSize = _errorMessage->GetBufferSize();

	SAFE_RELEASE(_errorMessage);

	MessageBox(_hWnd, L"Error compiling shader", _shaderFilename, MB_OK);

	return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* _deviceContext, XMMATRIX& _worldMatrix, XMMATRIX& _viewMatrix,
	XMMATRIX& _projectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	_worldMatrix = XMMatrixTranspose(_worldMatrix);
	_viewMatrix = XMMatrixTranspose(_viewMatrix);
	_projectionMatrix = XMMatrixTranspose(_projectionMatrix);

	if (FAILED(_deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = _worldMatrix;
	dataPtr->view = _viewMatrix;
	dataPtr->projection = _projectionMatrix;

	_deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}