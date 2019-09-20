#pragma once

#include "CommonHeader.h"

struct SVertex2D
{
	SVertex2D() {};
	SVertex2D(const XMFLOAT2& _Position) : Position{ _Position } {};

	XMFLOAT2 Position{};
};

struct SIndex
{
	SIndex() {};
	SIndex(DWORD _0, DWORD _1, DWORD _2) : I0{ _0 }, I1{ _1 }, I2{ _2 } {};

	DWORD I0{}, I1{}, I2{};
};

class CObject2D
{
public:
	CObject2D(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext) : m_Device{ Device }, m_DeviceContext{ DeviceContext }  {};
	~CObject2D() {};

	void Create(const vector<SVertex2D>& vVertices, const vector<SIndex>& vIndices);

	void Draw();

private:
	ID3D11Device*			m_Device{};
	ID3D11DeviceContext*	m_DeviceContext{};

	ComPtr<ID3D11Buffer>	m_VertexBuffer{};
	ComPtr<ID3D11Buffer>	m_IndexBuffer{};

	UINT					m_VertexBufferStride{ sizeof(SVertex2D) };
	UINT					m_VertexBufferOffset{};

	vector<SVertex2D>		m_Vertices{};
	vector<SIndex>			m_Indices{};
};
