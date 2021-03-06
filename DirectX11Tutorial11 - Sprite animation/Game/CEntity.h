#pragma once

#include "../Helper/CObject2D.h"
#include "../Helper/CTexture.h"

static constexpr size_t KAnimationIndexInvalid{ std::string::npos };

enum ERenderFlipOption
{
	None,
	Horizontal,
	Vertical,
	HorizontalAndVertical
};

struct SAnimationFrame
{
	SAnimationFrame() {}
	SAnimationFrame(const XMFLOAT2& _UVOffset, const XMFLOAT2& _UVSize) : UVOffset{ _UVOffset }, UVSize{ _UVSize } {}

	XMFLOAT2 UVOffset{};
	XMFLOAT2 UVSize{};
};

struct SAnimation
{
	SAnimation(const string& _Name, size_t _TickPerFrame) : Name{ _Name }, TickPerFrame{ _TickPerFrame } {}

	string Name{};
	size_t TickPerFrame{};
	size_t CurrentFrameIndex{};
	vector<SAnimationFrame> vFrames{};
};

class CEntity
{
	friend class CEntityPool;

public:
	CEntity(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext) : m_Device{ Device }, m_DeviceContext{ DeviceContext } {};
	~CEntity() {};

	void CreateRectangle(const XMFLOAT2& RectangleSize);

	void AddTexture(const string& TextureFileName);
	CTexture* GetTexture(size_t Index);

	void SetRectangleUVRange(const XMFLOAT2& OffsetInTexture, const XMFLOAT2& SizeInTexture, ERenderFlipOption Flip = ERenderFlipOption::None);
	void SetRectangleUVRangeAndSize(const XMFLOAT2& OffsetInTexture, const XMFLOAT2& SizeInTexture, ERenderFlipOption Flip = ERenderFlipOption::None);

	SAnimation* AddAnimation(const string& AnimationName, size_t TickPerFrame = 30);
	void SetAnimation(size_t AnimationIndex);
	void SetAnimation(const string& AnimationName);
	void SetAnimationFlipping(ERenderFlipOption Flip);
	void Animate();

	void Draw();

public:
	ESampler				Sampler{ ESampler::Point };
	XMFLOAT2				WorldPosition{};
	float					RotationAngle{};
	XMFLOAT2				Scalar{ 1.0f, 1.0f };

private:
	void UpdateAnimationFrame();

private:
	ID3D11Device*					m_Device{};
	ID3D11DeviceContext*			m_DeviceContext{};

	vector<CObject2D>				m_vObject2Ds{};
	vector<CTexture>				m_vTextures{};

	XMMATRIX						m_MatrixWorld{ XMMatrixIdentity() };

	vector<unique_ptr<SAnimation>>	m_vAnimations{};
	size_t							m_AnimationIndex{ KAnimationIndexInvalid };
	size_t							m_AnimationTick{};
	ERenderFlipOption				m_AnimationFlip{ ERenderFlipOption::None };
	unordered_map<string, size_t>	m_AnimationIndexMap{};
};