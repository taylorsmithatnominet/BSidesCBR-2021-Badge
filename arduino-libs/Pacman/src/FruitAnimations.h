#pragma once
#include <vector>

namespace pacman {

class Animation;
class Sprite;

class FruitAnimations
{
private:
	std::vector<Sprite*> m_sprites;
	std::vector<Animation*> m_still;
	std::vector<Animation*> m_flash;
	std::vector<Animation*> m_scores;
public:
	FruitAnimations();
	~FruitAnimations();
	Animation* GetAnimation(int p_index);
	Animation* GetFlashAnimation(int p_index);
	Animation* GetScoreAnimation(int p_index);
};

}; // namespace pacman
