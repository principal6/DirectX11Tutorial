#include "CEntity.h"

void CEntity::CreateRectangle(const XMFLOAT2& RectangleSize)
{
	if (m_vObject2Ds.size()) return;

	m_vObject2Ds.emplace_back(m_Device, m_DeviceContext);

	float half_width{ RectangleSize.x / 2 };
	float half_height{ RectangleSize.y / 2 };

	vector<SVertex2D> vertices
	{
		SVertex2D(XMFLOAT2(-half_width, +half_height), XMFLOAT2(0.0f, 0.0f)),
		SVertex2D(XMFLOAT2(+half_width, +half_height), XMFLOAT2(1.0f, 0.0f)),
		SVertex2D(XMFLOAT2(-half_width, -half_height), XMFLOAT2(0.0f, 1.0f)),
		SVertex2D(XMFLOAT2(+half_width, -half_height), XMFLOAT2(1.0f, 1.0f)),
	};

	vector<SIndex> indices
	{
		SIndex(0, 1, 2),
		SIndex(1, 3, 2),
	};

	m_vObject2Ds.back().CreateDynamic(vertices, indices);
}

void CEntity::AddTexture(const string& TextureFileName)
{
	m_vTextures.emplace_back(m_Device, m_DeviceContext);

	m_vTextures.back().CreateTexture(TextureFileName);
}

CTexture* CEntity::GetTexture(size_t Index)
{
	return &m_vTextures[Index];
}


void CEntity::SetRectangleUVRange(const XMFLOAT2& OffsetInTexture, const XMFLOAT2& SizeInTexture, ERenderFlipOption Flip)
{
	auto texture_size{ m_vTextures.front().GetTextureSize() };
	XMFLOAT2 OffsetUV{}, SizeUV{};
	OffsetUV.x = OffsetInTexture.x / texture_size.x;
	OffsetUV.y = OffsetInTexture.y / texture_size.y;
	SizeUV.x = SizeInTexture.x / texture_size.x;
	SizeUV.y = SizeInTexture.y / texture_size.y;

	m_vObject2Ds.front().m_Vertices[3].TexCoord = m_vObject2Ds.front().m_Vertices[2].TexCoord =
		m_vObject2Ds.front().m_Vertices[1].TexCoord = m_vObject2Ds.front().m_Vertices[0].TexCoord = OffsetUV;

	m_vObject2Ds.front().m_Vertices[1].TexCoord.x += SizeUV.x;
	m_vObject2Ds.front().m_Vertices[2].TexCoord.y += SizeUV.y;
	m_vObject2Ds.front().m_Vertices[3].TexCoord.x += SizeUV.x;
	m_vObject2Ds.front().m_Vertices[3].TexCoord.y += SizeUV.y;

	if (Flip == ERenderFlipOption::Horizontal || Flip == ERenderFlipOption::HorizontalAndVertical)
	{
		std::swap(m_vObject2Ds.front().m_Vertices[0].TexCoord.x, m_vObject2Ds.front().m_Vertices[1].TexCoord.x);
		std::swap(m_vObject2Ds.front().m_Vertices[2].TexCoord.x, m_vObject2Ds.front().m_Vertices[3].TexCoord.x);
	}

	if (Flip == ERenderFlipOption::Vertical || Flip == ERenderFlipOption::HorizontalAndVertical)
	{
		std::swap(m_vObject2Ds.front().m_Vertices[0].TexCoord.y, m_vObject2Ds.front().m_Vertices[2].TexCoord.y);
		std::swap(m_vObject2Ds.front().m_Vertices[1].TexCoord.y, m_vObject2Ds.front().m_Vertices[3].TexCoord.y);
	}

	m_vObject2Ds.front().UpdateVertexBuffer();
}

void CEntity::SetRectangleUVRangeAndSize(const XMFLOAT2& OffsetInTexture, const XMFLOAT2& SizeInTexture, ERenderFlipOption Flip)
{
	m_vObject2Ds.front().m_Vertices[0].Position.x = -SizeInTexture.x / 2;
	m_vObject2Ds.front().m_Vertices[0].Position.y = +SizeInTexture.y / 2;
	m_vObject2Ds.front().m_Vertices[1].Position.x = +SizeInTexture.x / 2;
	m_vObject2Ds.front().m_Vertices[1].Position.y = +SizeInTexture.y / 2;
	m_vObject2Ds.front().m_Vertices[2].Position.x = -SizeInTexture.x / 2;
	m_vObject2Ds.front().m_Vertices[2].Position.y = -SizeInTexture.y / 2;
	m_vObject2Ds.front().m_Vertices[3].Position.x = +SizeInTexture.x / 2;
	m_vObject2Ds.front().m_Vertices[3].Position.y = -SizeInTexture.y / 2;

	SetRectangleUVRange(OffsetInTexture, SizeInTexture, Flip);
}

SAnimation* CEntity::AddAnimation(const string& AnimationName, size_t TickPerFrame)
{
	m_AnimationIndexMap[AnimationName] = m_vAnimations.size();

	m_vAnimations.emplace_back(make_unique<SAnimation>(AnimationName, TickPerFrame));

	return m_vAnimations.back().get();
}

void CEntity::SetAnimation(size_t AnimationIndex)
{
	if (m_AnimationIndex != AnimationIndex)
	{
		m_AnimationIndex = AnimationIndex;
		m_vAnimations[m_AnimationIndex]->CurrentFrameIndex = 0;
		m_AnimationTick = 0;

		UpdateAnimationFrame();
	}
}

void CEntity::SetAnimation(const string& AnimationName)
{
	SetAnimation(m_AnimationIndexMap[AnimationName]);
}

void CEntity::SetAnimationFlipping(ERenderFlipOption Flip)
{
	m_AnimationFlip = Flip;

	UpdateAnimationFrame();
}

void CEntity::Animate()
{
	++m_AnimationTick;

	if (m_AnimationTick >= m_vAnimations[m_AnimationIndex]->TickPerFrame)
	{
		++m_vAnimations[m_AnimationIndex]->CurrentFrameIndex;
		if (m_vAnimations[m_AnimationIndex]->CurrentFrameIndex >= m_vAnimations[m_AnimationIndex]->vFrames.size())
		{
			m_vAnimations[m_AnimationIndex]->CurrentFrameIndex = 0;
		}

		UpdateAnimationFrame();

		m_AnimationTick = 0;
	}
}

void CEntity::UpdateAnimationFrame()
{
	auto& current_frame_index{ m_vAnimations[m_AnimationIndex]->CurrentFrameIndex };
	auto& current_frame{ m_vAnimations[m_AnimationIndex]->vFrames[current_frame_index] };
	SetRectangleUVRange(current_frame.UVOffset, current_frame.UVSize, m_AnimationFlip);
}

void CEntity::Draw()
{
	if (m_vTextures.size())
	{
		m_vTextures.back().Use();
	}
	
	if (m_vObject2Ds.size())
	{
		m_vObject2Ds.back().Draw();
	}
}