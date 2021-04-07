#include "Animation.h"

namespace pacman {

Animation::Animation()
{
    m_frames_count = 0;
}

Animation::~Animation()
{
}

void Animation::AddFrame(AnimationFrame p_frame)
{
    if ( m_frames_count >= MAX_ANIMATION_FRAME_COUNT )
    {
        return;
    }
	m_frames[m_frames_count] = p_frame;
    m_frames_count++;
}

int Animation::GetLength()
{
	int ret = 0;

	for (size_t i = 0; i < m_frames_count; i++) {
        auto f = m_frames[i];
		ret += f.m_length;
	}

	return ret;
}

Sprite * Animation::GetSprite(int p_time)
{
	int time = p_time;

	unsigned int i = 0;
	if (time > 0) {
		while (time > 0) {
			time -= m_frames[i].m_length;

			i++;
		}
		i--;
	}

	return m_frames[i].m_spritewPtr;
}


AnimationFrame::AnimationFrame()
{
}

AnimationFrame::AnimationFrame(Sprite* p_sprite, int p_length)
	: m_spritewPtr(p_sprite), m_length(p_length)
{
}

}; // namespace pacman
