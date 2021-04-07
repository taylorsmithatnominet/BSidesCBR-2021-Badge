#include "AnimationController.h"

#include "DrawManager.h"
#include "ServiceLocator.h"

#include "Animation.h"

namespace pacman {

AnimationController::AnimationController()
{
	m_drawManagerwPtr = ServiceLocator<DrawManager>::GetService();
}


AnimationController::~AnimationController()
{
}

bool AnimationController::Update(float p_delta)
{
	m_animationPos += p_delta * 1000.0f;

	while (m_animationPos > (float)m_length)
	{
		m_animationPos -= (float)m_length;
		m_hasLooped = true;
	}

	return m_hasLooped;
}

void AnimationController::Draw(Vect2 p_pos)
{
	m_drawManagerwPtr->Draw(m_animation->GetSprite((int)m_animationPos), p_pos.x, p_pos.y);
}

void AnimationController::SetAnimation(Animation* p_animation)
{
	if (m_animation == nullptr || m_animation != p_animation) {
		m_animation = p_animation;
		m_length = m_animation->GetLength();
		m_animationPos = 0;
		m_hasLooped = false;
	}
}

void AnimationController::ResetAnimation()
{
	m_animationPos = 0;
	m_hasLooped = false;
}

}; // namespace pacman
