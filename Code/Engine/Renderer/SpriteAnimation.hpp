#pragma once
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

enum SpriteAnimationMode
{
	SPRITE_ANIM_MODE_PLAY_TO_END,	// Plays from time=0 to durationSeconds, then finishes
	SPRITE_ANIM_MODE_LOOPING,		// Plays from time=0 to end then repeat (never finishes)
	SPRITE_ANIM_MODE_PINGPONG, 		// Plays forwards, backwards, forwards...
	NUM_SPRITE_ANIM_MODES
};


class SpriteAnimation
{
public:
	SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimationMode playbackMode = SPRITE_ANIM_MODE_PLAY_TO_END, unsigned int startSpriteFrameIndexInclusive = 0, unsigned int endSpriteFrameIndexInclusive = 0);

	void Update(float deltaSeconds);
	void Pause();					
	void Resume();				
	void Reset();					// Rewinds to time 0 and starts (re)playing
	bool IsFinished() const { return m_isFinished; }
	bool IsPlaying() const { return m_isPlaying; }
	bool IsTextureFlipped() const { return m_isTextureFlipped; }

	float GetDurationSeconds() const { return m_durationSeconds; }
	float GetSecondsElapsed() const { return m_elapsedSeconds; }
	float GetSecondsRemaining() const;
	float GetFractionElapsed() const;
	float GetFractionRemaining() const;
	float GetSecondsPerFrame() const;
	int GetStartIndex() const;

	void SetStartIndex(unsigned int startIndex);
	void SetEndIndex(unsigned int endIndex);
	void SetSecondsElapsed(float secondsElapsed);	    // Jump to specific time
	void SetFractionElapsed(float fractionElapsed);    // e.g. 0.33f for one-third in
	void SetTexturedFlipped(bool isFlipped) { m_isTextureFlipped = isFlipped; }

	int GetCurrentSpriteIndex() const;
	int GetCurrentFrameIndex() const;
	AABB2D GetCurrentTextureCoords() const;
	AABB2D GetTextureCoordsForSpriteIndex(unsigned int frameIndex);
	const Texture* GetTexture() const;
	const RHITexture2D* GetRHITexture() const;
	const SpriteSheet GetSpriteSheet() const;

	void SetNextFrame();
	void SetStartOfFrame(unsigned int frameIndex);
	
	void SetAnimationMode(const SpriteAnimationMode& anim);
	void SetAnimationRange(unsigned int startFrameIndex, unsigned int endFrameIndex);
	void SetDuration(float duration);
private:
	SpriteSheet m_spriteSheet;
	SpriteAnimationMode m_spriteAnimMode;

	unsigned int m_currentFrameIndex;
	unsigned int m_startFrameIndex;
	unsigned int m_endFrameIndex;

	bool m_isFinished;
	bool m_isPlaying;
	bool m_isTextureFlipped;

	float m_durationSeconds;
	float m_elapsedSeconds;
	float m_framesPerSecond;

	int m_pingpongDirection;
};
