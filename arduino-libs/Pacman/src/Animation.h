#pragma once
#include <vector>

namespace pacman {

class Sprite;

struct AnimationFrame { // length in milliseconds
	int m_length;
	Sprite* m_spritewPtr;

	AnimationFrame();
	AnimationFrame(Sprite* p_sprite, int p_length);

	static const AnimationFrame Empty;
};

class Animation
{
private:
#define MAX_ANIMATION_FRAME_COUNT 10
	AnimationFrame m_frames[MAX_ANIMATION_FRAME_COUNT];
    size_t m_frames_count;
public:
	Animation();
	~Animation();

	void AddFrame(AnimationFrame p_frame);

	int GetLength();
	Sprite* GetSprite(int p_time);
};

}; // namespace pacman
