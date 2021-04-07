#pragma once

#include <cstdint>
#include <functional>

namespace pacman {

class DrawManager;
class SpriteManager;
class StateManager;
class InputManager;
class IGame;

class Engine
{
public:
    Engine();
    ~Engine();
    void Initialize(); // Create all manager, initialize graphics
    void Shutdown(); // Delete all managers, shutdown graphics
    void Run(); // Handle all events, calculate delta, update the GAME
    void RunOnce();
    bool IsRunning();
    void SetBeginFrame(std::function<void(void)> func) {m_begin_frame = func;}
    void SetEndFrame(std::function<void(void)> func) {m_end_frame = func;}
    void SetGetTickCountMs(std::function<uint32_t(void)> func) {m_get_tick_count_ms = func;}
    void SetDrawPixel(std::function<void(uint16_t,uint16_t,uint16_t)> func);
    void SetDrawString8x8(std::function<void(int16_t,int16_t,const char *,uint16_t)> func);
    void SetGameOver(std::function<void(uint16_t)> func);
    void KeyEvent(uint8_t p_key, bool p_key_down); // see Keyboard.h
private:
    void CalculateDeltatime(); // Calculates the time in ms between this and the last
    DrawManager* m_drawManager;
    SpriteManager* m_spriteManager;
    InputManager* m_inputManager;
    bool m_running;
    unsigned int m_lastTick;
    unsigned int m_lastDraw;
    float m_delta;

    IGame* m_game;
    std::function<uint32_t(void)> m_get_tick_count_ms;
    std::function<void(void)> m_begin_frame;
    std::function<void(void)> m_end_frame;
    std::function<void(uint16_t,uint16_t,uint16_t)> m_draw_pixel;
    std::function<void(int16_t,int16_t,const char *,uint16_t)> m_draw_string;
    std::function<void(uint16_t)> m_game_over;
};

}; // namespace pacman
