#include "PlayerAnimations.h"
#include "ServiceLocator.h"
#include "SpriteManager.h"

#include "Config.h"

#include "gen/pacman_png.h"

namespace pacman {

PlayerAnimations::PlayerAnimations()
{
	SpriteManager* spriteManager = ServiceLocator<SpriteManager>::GetService();
    SpriteTexture tex = CreateSpriteTextureFromHeader(pacman_png);

	AnimationFrame still_frame = AnimationFrame(
		spriteManager->CreateSprite(tex, 4 * 16, 0 * 16, 16, 16),
		Config::ANIMATION_SPEED_PACMAN);

	Still = new Animation();
	Still->AddFrame(still_frame);

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			int dir = (y * 2) + x;

			Animation* ptr = m_eating.GetDirFromInt(dir);

			ptr->AddFrame(still_frame);
			AnimationFrame second_frame;

			for (int i = 0; i < 2; i++) {
				Sprite* next_sprite = spriteManager->CreateSprite(tex, ((x * 2) + i) * 16, y * 16, 16, 16);
				AnimationFrame next_frame = AnimationFrame(next_sprite, Config::ANIMATION_SPEED_PACMAN);

				ptr->AddFrame(next_frame);

				if (i == 0) { // half closed
					second_frame = next_frame;
				}
				if (i == 1) { // fully open
					Animation* stop_ptr = m_stopped.GetDirFromInt(dir);
					stop_ptr->AddFrame(next_frame);
				}
			}

			ptr->AddFrame(second_frame); // add the half open mouth a second time
		}
	}

	DeathAnimation = new Animation();
	for (int y = 2; y < 4; y++) {
		for (int x = 0; x < 5; x++) {
			DeathAnimation->AddFrame(AnimationFrame(
				spriteManager->CreateSprite(tex, x * 16, y * 16, 16, 16),
				Config::ANIMATION_SPEED_PACMAN_DEATH
			));
		}
	}

	DeathAnimation->AddFrame(AnimationFrame(
		spriteManager->CreateSprite(tex, 4 * 16, 1 * 16, 16, 16),
		Config::ANIMATION_SPEED_PACMAN_DEATH
	));

	DeathAnimation->AddFrame(AnimationFrame(
		spriteManager->CreateSprite(tex, 0, 0, 1, 1),
		Config::ANIMATION_SPEED_PACMAN_DEATH
	));
}


PlayerAnimations::~PlayerAnimations()
{
	m_eating.DeletePointers();
	m_stopped.DeletePointers();
	delete DeathAnimation;
	delete Still;
}

Animation * PlayerAnimations::GetAnimation(Vect2 p_dir)
{
	return m_eating.GetDirFromVect(p_dir);
}

Animation * PlayerAnimations::GetStoppedAnimation(Vect2 p_dir)
{
	return m_stopped.GetDirFromVect(p_dir);
}

}; // namespace pacman
