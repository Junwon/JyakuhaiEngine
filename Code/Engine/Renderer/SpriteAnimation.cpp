#include "Engine/Renderer/SpriteAnimation.hpp"

#include "Engine//Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

SpriteAnimation::SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimationMode playbackMode, unsigned int startSpriteFrameIndexInclusive, unsigned int endSpriteFrameIndexInclusive) :
	m_spriteSheet(spriteSheet),
	m_durationSeconds(durationSeconds),
	m_spriteAnimMode(playbackMode),
	m_startFrameIndex(startSpriteFrameIndexInclusive),
	m_endFrameIndex(endSpriteFrameIndexInclusive),
	m_currentFrameIndex(startSpriteFrameIndexInclusive),
	m_isPlaying(true),
	m_isFinished(false),
	m_isTextureFlipped(false),
	m_elapsedSeconds(0.f),
	m_pingpongDirection(1)
{
	if (durationSeconds == 0)
	{
		m_framesPerSecond = 0;
	}
	else
	{
		m_framesPerSecond = ((endSpriteFrameIndexInclusive - startSpriteFrameIndexInclusive) + 1) / durationSeconds;
	}
}

void SpriteAnimation::Update(float deltaSeconds)
{
	if(m_isPlaying && !m_isFinished)
	{
		m_elapsedSeconds += m_pingpongDirection * deltaSeconds;

		SetNextFrame();
	}
}

void SpriteAnimation::Pause()
{
	m_isPlaying = false;
}

void SpriteAnimation::Resume()
{
	m_isPlaying = true;
}

void SpriteAnimation::Reset()
{
	m_elapsedSeconds = 0.f;
	m_currentFrameIndex = m_startFrameIndex;
	m_isFinished = false;
}

float SpriteAnimation::GetSecondsRemaining() const
{
	return m_durationSeconds - m_elapsedSeconds;
}

float SpriteAnimation::GetFractionElapsed() const
{
	return m_elapsedSeconds / m_durationSeconds;
}

float SpriteAnimation::GetFractionRemaining() const
{
	return 1 - GetFractionElapsed();
}

float SpriteAnimation::GetSecondsPerFrame() const
{
	return 1.f / m_framesPerSecond;
}

int SpriteAnimation::GetStartIndex() const
{
	return m_startFrameIndex;
}

void SpriteAnimation::SetStartIndex(unsigned int startIndex)
{
	m_startFrameIndex = startIndex;
}

void  SpriteAnimation::SetEndIndex(unsigned int endIndex)
{
	m_endFrameIndex = endIndex;
}

void SpriteAnimation::SetSecondsElapsed(float secondsElapsed)
{
	m_elapsedSeconds = secondsElapsed;
}

void SpriteAnimation::SetFractionElapsed(float fractionElapsed)
{
	if(IsValueBetween(fractionElapsed, 0.f, 1.f))
		m_elapsedSeconds = m_durationSeconds * fractionElapsed;
}

int SpriteAnimation::GetCurrentSpriteIndex() const
{
	return m_currentFrameIndex - m_startFrameIndex;
}

int SpriteAnimation::GetCurrentFrameIndex() const
{
	return m_currentFrameIndex;
}

AABB2D SpriteAnimation::GetCurrentTextureCoords() const
{
	return m_spriteSheet.GetTextureCoordsForSpriteIndex(m_currentFrameIndex);
}

AABB2D SpriteAnimation::GetTextureCoordsForSpriteIndex(unsigned int frameIndex)
{
	return m_spriteSheet.GetTextureCoordsForSpriteIndex(frameIndex);
}

const Texture* SpriteAnimation::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}

const RHITexture2D* SpriteAnimation::GetRHITexture() const
{
	return m_spriteSheet.GetRHITexture();
}

const SpriteSheet SpriteAnimation::GetSpriteSheet() const
{
	return m_spriteSheet;
}

void SpriteAnimation::SetNextFrame()
{
	switch (m_spriteAnimMode)
	{
	case SPRITE_ANIM_MODE_PLAY_TO_END:

		m_currentFrameIndex = m_startFrameIndex + (int)(m_elapsedSeconds * m_framesPerSecond);

		if (m_currentFrameIndex > m_endFrameIndex)
		{
			m_isFinished = true;
			m_currentFrameIndex = m_endFrameIndex;
		}

		break;

	case SPRITE_ANIM_MODE_LOOPING:
		if (m_currentFrameIndex > m_endFrameIndex)
		{
			m_currentFrameIndex = m_startFrameIndex;
			m_elapsedSeconds = 0;
		}
		else
		{
			m_currentFrameIndex = m_startFrameIndex + (int)(m_elapsedSeconds * m_framesPerSecond);
		}
		break;

	case SPRITE_ANIM_MODE_PINGPONG:
		if (m_currentFrameIndex > m_endFrameIndex)
		{
			m_pingpongDirection = -1;
		}
		else if (m_currentFrameIndex < m_startFrameIndex)
		{
			m_pingpongDirection = 1;
		}
		m_currentFrameIndex = m_startFrameIndex + (int)(m_elapsedSeconds * m_framesPerSecond);
		break;
	}
}

void SpriteAnimation::SetStartOfFrame(unsigned int frameIndex)
{
	if(IsValueBetween((float)frameIndex, (float)m_startFrameIndex, (float)m_endFrameIndex))
	{
		m_currentFrameIndex = frameIndex;
	}
}

void SpriteAnimation::SetAnimationMode(const SpriteAnimationMode& anim)
{
	m_spriteAnimMode = anim;
}

void SpriteAnimation::SetAnimationRange(unsigned int startFrameIndex, unsigned int endFrameIndex)
{
	m_startFrameIndex = startFrameIndex;
	m_endFrameIndex = endFrameIndex;
	m_framesPerSecond = ((m_endFrameIndex - m_startFrameIndex) + 1) / m_durationSeconds;
}

void SpriteAnimation::SetDuration(float duration)
{
	m_durationSeconds = duration;

	if (duration == 0)
	{
		m_framesPerSecond = 0;
	}
	else
	{
		m_framesPerSecond = ((m_endFrameIndex - m_startFrameIndex) + 1) / m_durationSeconds;
	}
}