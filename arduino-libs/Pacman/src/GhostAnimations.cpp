#include "GhostAnimations.h"

#include "SpriteManager.h"
#include "Sprite.h"
#include "ServiceLocator.h"

#include "Config.h"

#include "gen/ghost_png.h"

namespace pacman {

GhostAnimations::GhostAnimations()
{
	SpriteManager* spriteManager = ServiceLocator<SpriteManager>::GetService();
    SpriteTexture tex = CreateSpriteTextureFromHeader(ghost_png);

	int speed = Config::ANIMATION_SPEED_GHOST; // length of each frame

    memset(m_colorAnimations, 0, sizeof(m_colorAnimations));
	for (int y = 0; y < 4; y++) {
		DirectionCollection collection;

		for (int d = 0; d < 4; d++) {

			Animation* ptr = collection.GetDirFromInt(d);

			for (int c = 0; c < 2; c++) {
				ptr->AddFrame(AnimationFrame(
					spriteManager->CreateSprite(tex, ((d * 2) + c) * 16, y * 16, 16, 16),
					speed
				));
			}
		}

		m_colorAnimations[y] = collection;
	}

	for (int i = 0; i < 4; i++) {
		Animation* ptr = m_deathCollection.GetDirFromInt(i);

		ptr->AddFrame(AnimationFrame(
				spriteManager->CreateSprite(tex, (4 + i) * 16, 4 * 16, 16, 16),
				1000
			));
	}

	WarningAnimation = new Animation();
	
	for (int i = 0; i < 2; i++) {
		WarningAnimation->AddFrame(AnimationFrame(
			spriteManager->CreateSprite(tex, (2 + i) * 16, 4 * 16, 16, 16),
			speed
		));
	}

	BlueAnimation = new Animation();
	for (int i = 0; i < 2; i++) {
		AnimationFrame next_frame = AnimationFrame(
			spriteManager->CreateSprite(tex, i * 16, 4 * 16, 16, 16),
			speed
		);

		BlueAnimation->AddFrame(next_frame);
		WarningAnimation->AddFrame(next_frame);
	}
}


GhostAnimations::~GhostAnimations()
{
	m_deathCollection.DeletePointers();

	for (int y = 0; y < 4; y++) {
        auto c = m_colorAnimations[y];
		c.DeletePointers();
	}

	delete BlueAnimation;
	delete WarningAnimation;
}

Animation * GhostAnimations::GetAnimation(Vect2 p_dir, int p_index)
{
	return m_colorAnimations[p_index].GetDirFromVect(p_dir);
}

Animation * GhostAnimations::GetDeathAnimation(Vect2 p_dir)
{
	return m_deathCollection.GetDirFromVect(p_dir);
}

}; // namespace pacman
