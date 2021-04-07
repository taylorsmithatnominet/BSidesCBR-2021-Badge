#include "Engine.h"
#include "DrawManager.h"
#include "SpriteManager.h"
#include "InputManager.h"
#include "ServiceLocator.h"
#include "PacmanGame.h"

#include <ctime>
#include <random>

namespace pacman {

Engine::Engine()
    : m_drawManager(nullptr), m_spriteManager(nullptr),
    m_inputManager(nullptr),
    m_running(true), m_lastTick(0)
{

};

Engine::~Engine()
{

};

void Engine::SetDrawPixel(std::function<void(uint16_t,uint16_t,uint16_t)> func)
{
    m_draw_pixel = func;
    if ( m_drawManager )
    {
        m_drawManager->SetDrawPixel(m_draw_pixel);
    }
}

void Engine::SetDrawString8x8(std::function<void(int16_t,int16_t,const char *,uint16_t)> func)
{
    m_draw_string = func;
    if ( m_drawManager )
    {
        m_drawManager->SetDrawString8x8(m_draw_string);
    }
}

void Engine::SetGameOver(std::function<void(uint16_t)> func)
{
    m_game_over = func;
    if ( m_game )
    {
        reinterpret_cast<PacmanGame*>(m_game)->SetGameOver(m_game_over);
    }
}

void Engine::Initialize()
{
    m_lastTick = m_get_tick_count_ms();
    m_lastDraw = m_lastTick;

    m_drawManager = new DrawManager();
    m_drawManager->SetDrawPixel(m_draw_pixel);
    m_drawManager->SetDrawString8x8(m_draw_string);
    m_drawManager->Initialize();
    m_spriteManager = new SpriteManager();
    m_inputManager = new InputManager();
    m_inputManager->Initialize();

    ServiceLocator<DrawManager>::SetService(m_drawManager);
    ServiceLocator<SpriteManager>::SetService(m_spriteManager);
    ServiceLocator<InputManager>::SetService(m_inputManager);
    srand(int(time(0)));

    m_game = new PacmanGame();
    reinterpret_cast<PacmanGame*>(m_game)->SetGameOver(m_game_over);
    m_game->Enter();
};

void Engine::Shutdown()
{
    m_inputManager->Shutdown();
    delete m_inputManager;
    m_inputManager = nullptr;

    delete m_spriteManager;
    m_spriteManager = nullptr;

    m_drawManager->Shutdown();
    delete m_drawManager;
    m_drawManager = nullptr;

    m_game->Exit();
    delete m_game;
    m_game = nullptr;
};

void Engine::RunOnce()
{
    unsigned int m_tick = m_get_tick_count_ms();

    CalculateDeltatime();
    if (!m_game->Update(m_delta))
        m_running = false;

    if ( (m_tick - m_lastDraw ) > 100 )
    {
        m_lastDraw = m_tick;
        m_begin_frame();
        m_game->Draw();
        m_end_frame();
    }

    m_inputManager->Update();
}

bool Engine::IsRunning()
{
    return m_running;
}

void Engine::Run()
{
    while (m_running)
    {
        RunOnce();
    }
}

void Engine::KeyEvent(uint8_t p_key, bool p_key_down)
{
    m_inputManager->KeyEvent(p_key, p_key_down);
}

void Engine::CalculateDeltatime()
{
    m_delta = 0.001f * (m_get_tick_count_ms() - m_lastTick);
    m_lastTick = m_get_tick_count_ms();
}

}; // namespace pacman
