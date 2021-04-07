#include "WinAnimationState.h"

#include "AnimationController.h"
#include "ServiceLocator.h"
#include "SpriteManager.h"
#include "PlayerAnimations.h"
#include "Level.h"

#include "Config.h"

//#include "gen/level_win_png.h"

namespace pacman {

WinAnimationState::WinAnimationState(GameStateData * p_data)
	: IGameState(p_data)
{
    /*
	m_controllerLevel = new AnimationController();
	m_controllerPlayer = new AnimationController();

	SpriteManager* spriteManager = ServiceLocator<SpriteManager>::GetService();
	m_animationswPtr = ServiceLocator<PlayerAnimations>::GetService();

	m_controllerPlayer->SetAnimation(m_animationswPtr->Still);

	SpriteTexture tex = CreateSpriteTextureFromHeader(level_win_png);
	m_animation = new Animation();
	
	for (int x = 0; x < 2; x++) {
		m_animation->AddFrame(AnimationFrame(
			spriteManager->CreateSprite(tex, x * 224, 0, 224, 248),
			Config::WIN_FLASH_SPEED
		));
	}

	m_controllerLevel->SetAnimation(m_animation);
    */
}

WinAnimationState::~WinAnimationState()
{
    /*
	delete m_controllerLevel;
	m_controllerLevel = nullptr;

	delete m_controllerPlayer;
	m_controllerPlayer = nullptr;

	delete m_animation;
	m_animation = nullptr;
    */
}

bool WinAnimationState::Update(float p_delta)
{
	//m_controllerLevel->Update(p_delta);
	//m_controllerPlayer->Update(p_delta);

	return m_pause.Update(p_delta);
}

void WinAnimationState::Draw()
{
    m_datawPtr->m_level->DrawLevel(0, 24, 0xffff);
    /*
	m_controllerLevel->Draw(Vect2(0, 24));
	m_controllerPlayer->Draw(m_datawPtr->m_player->GetScreenLocation());

	m_datawPtr->DrawHUD();
    */
}

void WinAnimationState::Enter()
{
	m_pause.SetPause(Config::WIN_ANIMATION_LENGTH);
}

void WinAnimationState::Exit()
{
}

}; // namespace pacman
