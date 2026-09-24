#define _USE_MATH_DEFINES
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Константы игры (настройки мира, физики, экрана)
static const int   SCREEN_W  = 1024;   // ширина окна (пиксели)
static const int   SCREEN_H  = 480;    // высота окна
static const int   GROUND_Y  = 360;    // Y-координата земли (0 – верх)
static const float GRAVITY  = 1500.0f; // ускорение свободного падения (px/с²)
static const float JUMP_VEL = -630.0f; // начальная скорость прыжка (px/с, вверх)
static const float INITIAL_SPEED = 280.0f;  // стартовая скорость движения (px/с)
static const float MAX_SPEED  = 850.0f;  // максимальная скорость
static const float OBS_INTERVAL_INIT = 2.3f;    // начальный интервал между препятствиями (с)
static const float OBS_INTERVAL_MIN  = 1.0f;    // минимальный интервал (при макс. скорости)
static const int   SAMPLE_RATE  = 44100;   // частота звука (Гц)


// Вспомогательные функции для рисования примитивов

namespace Draw {

// Установка цвета рендера (с альфа-каналом)
inline void color(SDL_Renderer* r, Uint8 red, Uint8 g, Uint8 b, Uint8 a = 255) {
    SDL_SetRenderDrawColor(r, red, g, b, a);
}

// Залитый круг (через горизонтальные линии)
void filledCircle(SDL_Renderer* r, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; dy++) {
        int dx = (int)std::sqrt((double)(radius * radius - dy * dy));
        SDL_RenderDrawLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

// Контур круга (алгоритм Брезенхема для окружности)
void circleOutline(SDL_Renderer* r, int cx, int cy, int radius) {
    int x = radius, y = 0, err = 0;
    while (x >= y) {
        SDL_RenderDrawPoint(r, cx + x, cy - y); SDL_RenderDrawPoint(r, cx + y, cy - x);
        SDL_RenderDrawPoint(r, cx - y, cy - x); SDL_RenderDrawPoint(r, cx - x, cy - y);
        SDL_RenderDrawPoint(r, cx - x, cy + y); SDL_RenderDrawPoint(r, cx - y, cy + x);
        SDL_RenderDrawPoint(r, cx + y, cy + x); SDL_RenderDrawPoint(r, cx + x, cy + y);
        y++; err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0) { x--; err += 1 - 2 * x; }
    }
}

// Круг с заданной толщиной линии (несколько вложенных контуров)
void thickCircle(SDL_Renderer* r, int cx, int cy, int radius, int thick) {
    for (int t = 0; t < thick; t++) circleOutline(r, cx, cy, radius - t);
}

// Прямоугольник (залитый или только рамка)
void rect(SDL_Renderer* r, int x, int y, int w, int h, bool filled = true) {
    SDL_Rect rc = { x, y, w, h };
    if (filled) SDL_RenderFillRect(r, &rc);
    else        SDL_RenderDrawRect(r, &rc);
}

// Залитый треугольник (сканирующие линии)
void triangle(SDL_Renderer* r, int x0, int y0, int x1, int y1, int x2, int y2) {
    // Рисуем границы (контур)
    SDL_RenderDrawLine(r, x0, y0, x1, y1);
    SDL_RenderDrawLine(r, x1, y1, x2, y2);
    SDL_RenderDrawLine(r, x2, y2, x0, y0);
    // Заливка: для каждой строки вычисляем пересечения с рёбрами
    int minY = std::min({ y0, y1, y2 });
    int maxY = std::max({ y0, y1, y2 });
    for (int sy = minY; sy <= maxY; sy++) {
        std::vector<int> xs;
        auto edge = [&](int ax, int ay, int bx, int by) {
            if ((ay <= sy && by > sy) || (by <= sy && ay > sy))
                xs.push_back(ax + (sy - ay) * (bx - ax) / (by - ay));
        };
        edge(x0,y0,x1,y1); edge(x1,y1,x2,y2); edge(x2,y2,x0,y0);
        if (xs.size() >= 2) {
            std::sort(xs.begin(), xs.end());
            SDL_RenderDrawLine(r, xs.front(), sy, xs.back(), sy);
        }
    }
}

} // namespace Draw


// Звуковой менеджер (генерирует простые звуки программно)

class SoundManager {
public:
    SoundManager() {
        if (Mix_OpenAudio(SAMPLE_RATE, AUDIO_S16SYS, 1, 1024) < 0) {
            std::cerr << "[Sound] " << Mix_GetError() << "\n"; return;
        }
        Mix_AllocateChannels(8);
        ok_    = true;
        // Создаём звуки с помощью синтеза
        jump_  = makeTone(500.0f, 0.09f, 0.38f);   // короткий высокий тон
        land_  = makeTone(210.0f, 0.06f, 0.25f); // низкий короткий удар
        death_ = makeNoise(0.40f, 0.55f);  // шум (падение)
        score_ = makeTone(920.0f, 0.05f, 0.20f);  // звонок при наборе 100 очков
    }
    ~SoundManager() {
        freeChunk(jump_); freeChunk(land_);
        freeChunk(death_); freeChunk(score_);
        if (ok_) Mix_CloseAudio();
    }
    void playJump()  { play(jump_);  }
    void playLand()  { play(land_);  }
    void playDeath() { play(death_); }
    void playScore() { play(score_); }

private:
    bool ok_ = false;
    Mix_Chunk *jump_=nullptr, *land_=nullptr, *death_=nullptr, *score_=nullptr;

    // Создаёт Mix_Chunk из вектора сэмплов (Sint16)
    Mix_Chunk* allocChunk(const std::vector<Sint16>& s) {
        Uint32 len = (Uint32)(s.size() * sizeof(Sint16));
        Uint8* buf = (Uint8*)SDL_malloc(len);
        if (!buf) return nullptr;
        SDL_memcpy(buf, s.data(), len);
        Mix_Chunk* c = (Mix_Chunk*)SDL_malloc(sizeof(Mix_Chunk));
        if (!c) { SDL_free(buf); return nullptr; }
        c->allocated = 1; c->abuf = buf; c->alen = len; c->volume = MIX_MAX_VOLUME;
        return c;
    }

    // Генерация синусоидального тона с огибающей (экспоненциальное затухание)
    Mix_Chunk* makeTone(float freq, float dur, float amp) {
        int n = (int)(SAMPLE_RATE * dur);
        std::vector<Sint16> s(n);
        for (int i = 0; i < n; i++) {
            float t = (float)i / SAMPLE_RATE;
            float env = std::exp(-5.5f * t / dur);
            s[i] = (Sint16)(env * amp * 32767.0f * std::sin(2.f * (float)M_PI * freq * t));
        }
        return allocChunk(s);
    }

    // Генерация шума (белый шум с экспоненциальной огибающей)
    Mix_Chunk* makeNoise(float dur, float amp) {
        int n = (int)(SAMPLE_RATE * dur);
        std::vector<Sint16> s(n);
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> d(-1.f, 1.f);
        for (int i = 0; i < n; i++) {
            float env = std::exp(-4.f * (float)i / n);
            s[i] = (Sint16)(env * amp * 32767.0f * d(rng));
        }
        return allocChunk(s);
    }

    void freeChunk(Mix_Chunk* c) {
        if (!c) return;
        SDL_free(c->abuf); SDL_free(c);
    }
    void play(Mix_Chunk* c) { if (ok_ && c) Mix_PlayChannel(-1, c, 0); }
};


// Рендер текста (TTF) с автоматическим поиском шрифта

class TextRenderer {
public:
    TextRenderer() {
        if (TTF_Init() < 0) { std::cerr << "[TTF] " << TTF_GetError() << "\n"; return; }
        // Список возможных путей к системным шрифтам (кроссплатформенный)
        static const char* paths[] = {
            "font.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/usr/share/fonts/noto/NotoSans-Regular.ttf",
            "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/segoeui.ttf",
            "/Library/Fonts/Arial.ttf",
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            nullptr
        };
        // Открываем первый доступный шрифт в разных размерах
        for (int i = 0; paths[i]; ++i) {
            big_ = TTF_OpenFont(paths[i], 40);
            if (!big_) continue;
            med_   = TTF_OpenFont(paths[i], 26);
            small_ = TTF_OpenFont(paths[i], 18);
            tiny_  = TTF_OpenFont(paths[i], 14);
            ok_ = true;
            std::cout << "[TTF] Font: " << paths[i] << "\n";
            break;
        }
        if (!ok_) std::cerr << "[TTF] No font found!\n";
    }
    ~TextRenderer() {
        if (big_)   TTF_CloseFont(big_);
        if (med_)   TTF_CloseFont(med_);
        if (small_) TTF_CloseFont(small_);
        if (tiny_)  TTF_CloseFont(tiny_);
        TTF_Quit();
    }
    // Удобные обёртки: выбор размера и центрирование
    void big  (SDL_Renderer* r, const std::string& t, int x, int y, SDL_Color c, bool cx=false) { draw(r,t,x,y,c,big_,cx); }
    void med  (SDL_Renderer* r, const std::string& t, int x, int y, SDL_Color c, bool cx=false) { draw(r,t,x,y,c,med_,cx); }
    void small(SDL_Renderer* r, const std::string& t, int x, int y, SDL_Color c, bool cx=false) { draw(r,t,x,y,c,small_,cx); }
    void tiny (SDL_Renderer* r, const std::string& t, int x, int y, SDL_Color c, bool cx=false) { draw(r,t,x,y,c,tiny_,cx); }

private:
    bool ok_ = false;
    TTF_Font *big_=nullptr, *med_=nullptr, *small_=nullptr, *tiny_=nullptr;

    // Отрисовка текста (создаёт текстуру из поверхности)
    void draw(SDL_Renderer* r, const std::string& t, int x, int y,
              SDL_Color col, TTF_Font* f, bool centered) {
        if (!f || t.empty()) return;
        SDL_Surface* s = TTF_RenderUTF8_Blended(f, t.c_str(), col);
        if (!s) return;
        SDL_Texture* tx = SDL_CreateTextureFromSurface(r, s);
        if (tx) {
            SDL_Rect dst = { centered ? x - s->w/2 : x, y, s->w, s->h };
            SDL_RenderCopy(r, tx, nullptr, &dst);
            SDL_DestroyTexture(tx);
        }
        SDL_FreeSurface(s);
    }
};


// Базовый игровой объект (позиция, размер, хитбокс, виртуальные методы)

class GameObject {
public:
    float x, y, w, h;
    GameObject(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    virtual ~GameObject() = default;
    virtual void update(float) {}   // dt
    virtual void draw(SDL_Renderer* r) = 0;

    // Хитбокс чуть меньше реального спрайта (для честных коллизий)
    SDL_Rect hitbox() const {
        return { (int)(x + w*0.14f), (int)(y + h*0.08f), (int)(w*0.72f), (int)(h*0.82f) };
    }
    bool hits(const GameObject& o) const {
        SDL_Rect a = hitbox(), b = o.hitbox();
        return SDL_HasIntersection(&a, &b) == SDL_TRUE;
    }
};


// Игрок (динозавр на велосипеде) – физика, анимация, отрисовка

class Player : public GameObject {
public:
    float vy  = 0.0f;
    bool grounded = true;
    float anim = 0.0f;    // таймер для анимации (вращение педалей/колёс, мигание)
    bool dead = false;
    SoundManager* snd;

    Player(float x, float y, SoundManager* s)
        : GameObject(x, y, 74, 96), snd(s) {}

    void jump() {
        if (!grounded || dead) return;
        vy = JUMP_VEL;
        grounded = false;
        if (snd) snd->playJump();
    }

    void update(float dt) override {
        anim += dt;
        if (!grounded) {
            vy += GRAVITY * dt;
            y  += vy * dt;
            float floorY = (float)GROUND_Y - h;
            if (y >= floorY) {
                y = floorY;
                vy = 0.0f;
                grounded = true;
                if (snd) snd->playLand();   // звук приземления
            }
        }
    }

    void draw(SDL_Renderer* r) override {
        if (dead) drawDead(r);
        else      drawRiding(r);
    }

private:
    // Геометрия велосипеда (координаты относительно x,y)
    static const int WR = 18;   // радиус колеса

    int rwX() const { return (int)x + 18; } // заднее колесо X
    int rwY() const { return (int)y + (int)h - WR; }
    int fwX() const { return (int)x + 56; } // переднее колесо
    int fwY() const { return rwY(); }
    int bbX() const { return (int)x + 35; } // каретка (педали)
    int bbY() const { return rwY() - 2; }
    int stX() const { return (int)x + 26; } // седло
    int stY() const { return rwY() - 38; }
    int htX() const { return (int)x + 55; } // руль
    int htY() const { return rwY() - 32; }

    // Отрисовка колеса со спицами (угол поворота от анимации)
    void drawWheel(SDL_Renderer* r, int cx, int cy, float angle) const {
        Draw::color(r, 28, 28, 28);
        Draw::filledCircle(r, cx, cy, WR);                // шина
        Draw::color(r, 190, 190, 190);
        Draw::thickCircle(r, cx, cy, WR - 3, 3);          // обод
        Draw::color(r, 140, 140, 140);
        for (int s = 0; s < 4; s++) {                     // 4 спицы
            float a = angle + s * (float)M_PI / 2.f;
            int dx = (int)((WR - 5) * std::cos(a));
            int dy = (int)((WR - 5) * std::sin(a));
            SDL_RenderDrawLine(r, cx, cy, cx + dx, cy + dy);
        }
        Draw::color(r, 255, 205, 0);
        Draw::filledCircle(r, cx, cy, 3);                 // втулка
    }

    // Рисование рамы, седла, руля
    void drawBike(SDL_Renderer* r, float angle) const {
        drawWheel(r, rwX(), rwY(), angle);  // заднее
        drawWheel(r, fwX(), fwY(), angle);  // переднее

        Draw::color(r, 235, 75, 0);   // оранжевая рама
        // Задний треугольник: каретка -> задн. ось -> седло
        SDL_RenderDrawLine(r, bbX(), bbY(),  rwX(), rwY() - WR + 1);
        SDL_RenderDrawLine(r, stX(), stY(),  rwX(), rwY() - WR + 1);
        SDL_RenderDrawLine(r, stX(), stY(),  bbX(), bbY());
        // Передний треугольник: седло -> руль -> каретка
        SDL_RenderDrawLine(r, stX(), stY(),  htX(), htY());
        SDL_RenderDrawLine(r, htX(), htY(),  bbX(), bbY());
        // Вилка
        SDL_RenderDrawLine(r, htX(), htY(),  fwX(), fwY() - WR + 1);

        Draw::color(r, 50, 20, 5);
        Draw::rect(r, stX() - 12, stY() - 6, 24, 7); // седло

        Draw::color(r, 105, 105, 105);
        SDL_RenderDrawLine(r, htX(), htY() - 3, htX() + 5, htY() - 14);
        SDL_RenderDrawLine(r, htX() + 5, htY() - 14, htX() + 5, htY() - 7);
        Draw::color(r, 175, 140, 0);
        Draw::circleOutline(r, bbX(), bbY(), 9); // звездочка
    }

    // Шатуны и педали (анимация вращения)
    void drawCrank(SDL_Renderer* r, float angle,
                   int& p1x, int& p1y, int& p2x, int& p2y) const {
        p1x = bbX() + (int)(13 * std::cos(angle));
        p1y = bbY() + (int)(13 * std::sin(angle));
        p2x = bbX() - (int)(13 * std::cos(angle));
        p2y = bbY() - (int)(13 * std::sin(angle));

        Draw::color(r, 70, 70, 70);
        SDL_RenderDrawLine(r, bbX(), bbY(), p1x, p1y);
        SDL_RenderDrawLine(r, bbX(), bbY(), p2x, p2y);
        Draw::color(r, 205, 125, 15);
        Draw::rect(r, p1x - 5, p1y - 2, 10, 4);
        Draw::rect(r, p2x - 5, p2y - 2, 10, 4);
    }

    // Отрисовка динозавра (тело, голова, руки, ноги, шипы)
    void drawDino(SDL_Renderer* r, float t,
                  int p1x, int p1y, int p2x, int p2y) const {
        int bx = stX() - 8;    // левый край туловища
        int by = stY() - 36;   // верх туловища

        // Хвост
        Draw::color(r, 38, 145, 38);
        SDL_RenderDrawLine(r, bx,      by + 14, bx - 10, by + 22);
        SDL_RenderDrawLine(r, bx - 10, by + 22, bx - 16, by + 14);
        SDL_RenderDrawLine(r, bx - 16, by + 14, bx,      by + 14);

        // Тело
        Draw::color(r, 52, 185, 52);
        Draw::rect(r, bx, by, 20, 30);
        Draw::color(r, 160, 230, 120);
        Draw::rect(r, bx + 5, by + 7, 9, 17); // живот

        // Шея и голова
        Draw::color(r, 52, 185, 52);
        Draw::rect(r, bx + 14, by - 6, 8, 10);
        Draw::rect(r, bx + 12, by - 22, 22, 20);
        Draw::rect(r, bx + 25, by - 14, 12, 10); // морда

        // Ноздря
        Draw::color(r, 28, 115, 28);
        SDL_RenderDrawPoint(r, bx + 35, by - 9);
        SDL_RenderDrawPoint(r, bx + 35, by - 8);

        // Глаз с белком и зрачком
        Draw::color(r, 255, 255, 255);
        Draw::rect(r, bx + 14, by - 19, 9, 8);
        Draw::color(r, 15, 15, 15);
        Draw::rect(r, bx + 20, by - 17, 3, 6);

        // Моргание (каждые ~3.5 секунды на 0.13 с)
        if (std::fmod(t, 3.5f) < 0.13f) {
            Draw::color(r, 52, 185, 52);
            Draw::rect(r, bx + 14, by - 19, 9, 4);
        }

        // Зубы
        Draw::color(r, 255, 255, 255);
        for (int ti = 0; ti < 3; ti++)
            SDL_RenderDrawLine(r, bx + 27 + ti*3, by - 4, bx + 27 + ti*3, by - 1);

        // Рука к рулю
        Draw::color(r, 38, 155, 38);
        SDL_RenderDrawLine(r, bx + 18, by + 4, htX() + 4, htY() - 5);
        Draw::filledCircle(r, htX() + 4, htY() - 5, 3);

        // Ноги на педалях
        Draw::color(r, 38, 155, 38);
        SDL_RenderDrawLine(r, bx + 10, by + 28, p1x, p1y - 2);
        SDL_RenderDrawLine(r, bx + 10, by + 28, p2x, p2y - 2);
        Draw::rect(r, p1x - 3, p1y - 5, 8, 4);
        Draw::rect(r, p2x - 3, p2y - 5, 8, 4);

        // Шипы на спине
        Draw::color(r, 28, 130, 28);
        for (int s = 0; s < 4; s++) {
            int sx = bx + 2, sy = by + 3 + s * 7;
            Draw::triangle(r, sx, sy - 6, sx - 4, sy + 1, sx + 4, sy + 1);
        }
    }

    // Обычное состояние (езда)
    void drawRiding(SDL_Renderer* r) {
        float angle = anim * 9.0f;          // вращение колёс
        drawBike(r, angle);
        int p1x, p1y, p2x, p2y;
        drawCrank(r, anim * 7.0f, p1x, p1y, p2x, p2y);
        drawDino(r, anim, p1x, p1y, p2x, p2y);
    }

    // Состояние "Game Over": дино лежит на боку, сломанный велосипед, звёздочки
    void drawDead(SDL_Renderer* r) {
        float t = anim;
        int bx = (int)x + 4, by = (int)y + 10;

        // Колеса (ещё крутятся)
        Draw::color(r, 28, 28, 28);
        Draw::filledCircle(r, rwX(), rwY(), WR);
        Draw::filledCircle(r, fwX(), fwY(), WR);
        Draw::color(r, 190, 190, 190);
        Draw::thickCircle(r, rwX(), rwY(), WR - 3, 2);
        Draw::thickCircle(r, fwX(), fwY(), WR - 3, 2);

        // Искореженная рама
        Draw::color(r, 235, 75, 0);
        SDL_RenderDrawLine(r, rwX(), rwY() - WR, bbX() + 6, bbY() - 5);
        SDL_RenderDrawLine(r, fwX(), fwY() - WR, bbX() + 6, bbY() - 5);
        SDL_RenderDrawLine(r, bbX() + 6, bbY() - 5, stX(), stY() + 8);

        // Динозавр на боку
        Draw::color(r, 52, 185, 52);
        Draw::rect(r, bx, by + 22, 34, 18);
        Draw::rect(r, bx + 28, by + 12, 20, 17); // голова
        Draw::color(r, 15, 15, 15);
        SDL_RenderDrawLine(r, bx+30, by+14, bx+35, by+19);
        SDL_RenderDrawLine(r, bx+35, by+14, bx+30, by+19); // X_X
        Draw::color(r, 220, 60, 60);
        SDL_RenderDrawLine(r, bx+36, by+28, bx+36, by+34);
        SDL_RenderDrawLine(r, bx+36, by+34, bx+32, by+37); // язык

        // Вращающиеся звёздочки
        Draw::color(r, 255, 218, 0);
        float sa = t * 4.5f;
        for (int i = 0; i < 5; i++) {
            float a = sa + i * 2.f * (float)M_PI / 5.f;
            int sx = bx + 38 + (int)(20 * std::cos(a));
            int sy = by + 14 + (int)(13 * std::sin(a));
            for (int p = 0; p < 5; p++) {
                float pa = (float)p * 2.f * (float)M_PI / 5.f;
                SDL_RenderDrawPoint(r, sx + (int)(5*std::cos(pa)),
                                       sy + (int)(4*std::sin(pa)));
                SDL_RenderDrawPoint(r, sx + (int)(3*std::cos(pa)),
                                       sy + (int)(2*std::sin(pa)));
            }
        }
    }
};


// Препятствия: кактусы (малые / большие) и птицы

class Obstacle : public GameObject {
public:
    enum class Type { CactusS, CactusL, Bird };
    Type  type;
    float spd;      // текущая скорость движения (обновляется из Game::speed_)
    float t = 0.f;  // таймер для анимации крыльев

    Obstacle(float x, float y, float w, float h, Type tp, float spd)
        : GameObject(x, y, w, h), type(tp), spd(spd) {}

    void update(float dt) override { t += dt; x -= spd * dt; }
    bool offScreen() const { return x + w < -20.f; }

    void draw(SDL_Renderer* r) override {
        if (type == Type::Bird) drawBird(r);
        else                    drawCactus(r);
    }

private:
    // Кактус: несколько стволов, шипы, зелёная гамма
    void drawCactus(SDL_Renderer* r) {
        int bx = (int)x, by = (int)y;
        int cw = (int)w, ch = (int)h;
        int tw = (type == Type::CactusL) ? 13 : 10;
        int tx = bx + cw / 2 - tw / 2;

        Draw::color(r, 32, 138, 32);
        Draw::rect(r, tx, by, tw, ch);   // основной ствол

        // Левое плечо
        int la_y = by + ch / 3;
        Draw::rect(r, bx, la_y, tx - bx + 1, 8);
        Draw::rect(r, bx, la_y - 15, 9, 17);
        // Правое плечо
        int ra_y = by + ch * 2 / 5;
        Draw::rect(r, tx + tw, ra_y, cw - (tx + tw - bx), 8);
        Draw::rect(r, bx + cw - 9, ra_y - 12, 9, 14);

        // Тёмная полоска по стволу
        Draw::color(r, 20, 96, 20);
        Draw::rect(r, tx + tw/2 - 1, by + 5, 2, ch - 10);

        // Шипы
        Draw::color(r, 185, 245, 185);
        for (int i = 1; i < 4; i++) {
            int sy = by + i * ch / 4;
            SDL_RenderDrawLine(r, tx-1, sy, tx-6, sy-4);
            SDL_RenderDrawLine(r, tx+tw, sy, tx+tw+5, sy-4);
        }
        SDL_RenderDrawLine(r, bx+1, la_y-15, bx+1, la_y-19);
        SDL_RenderDrawLine(r, bx+cw-1, ra_y-12, bx+cw-1, ra_y-16);
    }

    // Птица: летит навстречу игроку, анимированные крылья
    void drawBird(SDL_Renderer* r) {
        int bx = (int)x, by = (int)y;
        float wing = std::sin(t * 11.0f);
        float bob  = std::sin(t * 4.0f) * 5.f;   // небольшое покачивание вверх-вниз
        int cy2    = by + (int)bob + (int)h / 2;
        int mx     = bx + (int)w / 2;

        Draw::color(r, 60, 60, 210);
        Draw::filledCircle(r, mx, cy2, 12);   // тело
        Draw::filledCircle(r, mx - 14, cy2 - 4, 8); // голова

        // Хохолок
        Draw::color(r, 240, 45, 45);
        Draw::triangle(r, mx-14, cy2-12, mx-18, cy2-7, mx-10, cy2-7);

        // Глаз
        Draw::color(r, 255, 255, 70);
        Draw::rect(r, mx - 19, cy2 - 7, 4, 4);
        Draw::color(r, 0, 0, 0);
        SDL_RenderDrawPoint(r, mx - 18, cy2 - 6);
        // Клюв
        Draw::color(r, 255, 170, 0);
        Draw::triangle(r, mx-22, cy2-4, mx-28, cy2-3, mx-22, cy2-1);

        // Крыло с изменяемым размахом
        int wdy = (int)(wing * 15);
        Draw::color(r, 80, 80, 235);
        SDL_RenderDrawLine(r, mx+2, cy2, mx+20, cy2 + wdy);
        SDL_RenderDrawLine(r, mx+20, cy2 + wdy, mx+8, cy2+2);
        Draw::color(r, 100, 100, 255);
        for (int i = 0; i < 11; i++)
            SDL_RenderDrawLine(r, mx+2+i, cy2, mx+2+i, cy2 + wdy - i/2);

        // Хвост
        Draw::color(r, 60, 60, 210);
        SDL_RenderDrawLine(r, mx+11, cy2,   mx+23, cy2+6);
        SDL_RenderDrawLine(r, mx+11, cy2,   mx+24, cy2-1);
        SDL_RenderDrawLine(r, mx+11, cy2,   mx+22, cy2+2);
    }
};


// Облака – декоративные, движутся медленнее фона

class Cloud : public GameObject {
    float spd;
public:
    Cloud(float x, float y, float spd)
        : GameObject(x, y, 90, 42), spd(spd) {}
    void update(float dt) override { x -= spd * dt; }
    bool offScreen() const { return x + w < -10.f; }
    void draw(SDL_Renderer* r) override {
        int bx = (int)x, by = (int)y;
        Draw::color(r, 248, 252, 255, 220);
        Draw::filledCircle(r, bx + 22, by + 28, 17);
        Draw::filledCircle(r, bx + 42, by + 22, 22);
        Draw::filledCircle(r, bx + 66, by + 27, 16);
        Draw::rect(r, bx + 22, by + 22, 44, 23);
    }
};


// Земля с бегущей текстурой и мелкими камешками

class Ground {
    float scrollX = 0.f;
public:
    void update(float dt, float spd) {
        scrollX = std::fmod(scrollX + spd * dt, 48.f);
    }
    void draw(SDL_Renderer* r) {
        Draw::color(r, 80, 56, 28);
        Draw::rect(r, 0, GROUND_Y, SCREEN_W, SCREEN_H - GROUND_Y);
        Draw::color(r, 55, 162, 55);
        Draw::rect(r, 0, GROUND_Y, SCREEN_W, 6); // травяная полоска

        // Скользящие штрихи (имитация грунтовой дороги)
        Draw::color(r, 115, 90, 48);
        for (int i = -1; i < SCREEN_W / 48 + 2; i++) {
            int gx = (int)(i * 48 - scrollX);
            SDL_RenderDrawLine(r, gx, GROUND_Y + 14, gx + 22, GROUND_Y + 14);
            SDL_RenderDrawLine(r, gx + 10, GROUND_Y + 24, gx + 28, GROUND_Y + 24);
        }
        // Камешки
        Draw::color(r, 100, 76, 46);
        for (int i = -1; i < SCREEN_W / 95 + 2; i++) {
            int gx = (int)(i * 95 + 38 - std::fmod(scrollX * 0.55f, 95.f));
            Draw::filledCircle(r, gx, GROUND_Y + 33, 5);
            Draw::filledCircle(r, gx + 48, GROUND_Y + 42, 3);
        }
    }
};


// Управление счётом и рекордами

class ScoreManager {
public:
    int score = 0;
    int hiScore = 0;
    float dist = 0.f;   // накопленная дистанция (условные единицы)
    float timer = 0.f;
    bool  milestone = false; // флаг, что набрано очередные 100 очков

    void update(float dt, float spd) {
        dist  += spd * dt / 100.f;
        timer += dt;
        if (timer >= 0.08f) {
            timer -= 0.08f;
            int old = score;
            score = (int)dist;
            if (score / 100 > old / 100) milestone = true;
        }
    }
    bool takeMilestone() { if (milestone) { milestone=false; return true; } return false; }
    void saveHigh() { if (score > hiScore) hiScore = score; }
    void reset()    { score=0; dist=0.f; timer=0.f; milestone=false; }
};


// Состояния игры

enum class State { Menu, Playing, Dead };


// Главный класс игры – объединяет все компоненты

class Game {
public:
    Game()  = default;
    ~Game() { cleanup(); }

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            std::cerr << "SDL_Init: " << SDL_GetError() << "\n"; return false;
        }
        win_ = SDL_CreateWindow(
            "Дино на Велосипеде – Великий Побег из Парка",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
        if (!win_) { std::cerr << "Window: " << SDL_GetError() << "\n"; return false; }

        ren_ = SDL_CreateRenderer(win_, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!ren_) { std::cerr << "Renderer: " << SDL_GetError() << "\n"; return false; }

        SDL_SetRenderDrawBlendMode(ren_, SDL_BLENDMODE_BLEND);

        snd_ = std::make_unique<SoundManager>();
        txt_ = std::make_unique<TextRenderer>();

        // Начальные облака для меню
        for (int i = 0; i < 5; i++)
            spawnCloud(true, (float)(i * 200));

        // Звёзды (фиксированные позиции, меняется только яркость)
        std::uniform_real_distribution<float> sx(0, (float)SCREEN_W);
        std::uniform_real_distribution<float> sy(10, GROUND_Y - 10);
        std::uniform_real_distribution<float> sb(0.3f, 1.f);
        for (int i = 0; i < 80; ++i)
            stars_.push_back({ sx(rng_), sy(rng_), sb(rng_) });

        return true;
    }

    void run() {
        Uint32 prev = SDL_GetTicks();
        while (running_) {
            SDL_Event ev;
            while (SDL_PollEvent(&ev)) handleEvent(ev);
            Uint32 now = SDL_GetTicks();
            float  dt  = std::min((now - prev) / 1000.f, 0.05f);
            prev = now;
            update(dt);
            render();
        }
    }

private:
    SDL_Window*   win_ = nullptr;
    SDL_Renderer* ren_ = nullptr;
    std::unique_ptr<SoundManager> snd_;
    std::unique_ptr<TextRenderer> txt_;

    State state_   = State::Menu;
    bool  running_ = true;

    std::unique_ptr<Player>                player_;
    std::vector<std::unique_ptr<Obstacle>> obs_;
    std::vector<std::unique_ptr<Cloud>>    clouds_;
    Ground       ground_;
    ScoreManager score_;

    float speed_       = INITIAL_SPEED;
    float gameTime_    = 0.f;     // общее время игры (секунд)
    float obsTimer_    = 0.f;     // таймер до следующего препятствия
    float obsInterval_ = OBS_INTERVAL_INIT;
    float cloudTimer_  = 0.f;
    float deathTimer_  = 0.f;
    float menuTime_    = 0.f;
    float dayPhase_    = 0.f;     // 0 = день, 1 = ночь (плавно изменяется)

    std::mt19937 rng_{ std::random_device{}() };

    struct Star { float x, y, brightness; };
    std::vector<Star> stars_;

    // Обработка ввода (с учётом состояния игры)
    void handleEvent(const SDL_Event& ev) {
        if (ev.type == SDL_QUIT) { running_ = false; return; }

        bool anyInput = (ev.type == SDL_KEYDOWN || ev.type == SDL_MOUSEBUTTONDOWN);
        bool isJump   = (ev.type == SDL_KEYDOWN &&
                        (ev.key.keysym.sym == SDLK_SPACE || ev.key.keysym.sym == SDLK_UP    || ev.key.keysym.sym == SDLK_w))   || (ev.type == SDL_MOUSEBUTTONDOWN);

        switch (state_) {
        case State::Menu:
            if (anyInput) startGame();
            break;
        case State::Playing:
            if (isJump) player_->jump();
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
                triggerDeath();
            break;
        case State::Dead:
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_r)      startGame();
                if (ev.key.keysym.sym == SDLK_ESCAPE)  running_ = false;
            }
            if (ev.type == SDL_MOUSEBUTTONDOWN) startGame();
            break;
        }
    }

    // Старт / рестарт игры
    void startGame() {
        player_ = std::make_unique<Player>(
            110.f, (float)(GROUND_Y - 96), snd_.get());
        obs_.clear();
        speed_       = INITIAL_SPEED;
        gameTime_    = 0.f;
        obsTimer_    = 0.f;
        obsInterval_ = OBS_INTERVAL_INIT;
        cloudTimer_  = 0.f;
        deathTimer_  = 0.f;
        dayPhase_    = 0.f;
        score_.reset();
        state_ = State::Playing;
    }

    void triggerDeath() {
        if (state_ != State::Playing) return;
        score_.saveHigh();
        if (snd_) snd_->playDeath();
        player_->dead = true;
        deathTimer_   = 0.f;
        state_        = State::Dead;
    }

    // Обновление всех объектов и логики (вызывается каждый кадр)
    void update(float dt) {
        // Обновление облаков (всегда, даже в меню)
        for (auto& c : clouds_) c->update(dt);
        clouds_.erase(std::remove_if(clouds_.begin(), clouds_.end(),
            [](const auto& c){ return c->offScreen(); }), clouds_.end());

        if (state_ == State::Menu) {
            menuTime_ += dt;
            cloudTimer_ += dt;
            if (cloudTimer_ >= 4.f) { cloudTimer_ = 0; spawnCloud(false, 0); }
            ground_.update(dt, INITIAL_SPEED * 0.3f);
            return;
        }

        if (state_ == State::Dead) {
            deathTimer_ += dt;
            if (player_) player_->anim += dt;
            return;
        }

        // --- Игровой процесс (State::Playing) ---
        gameTime_ += dt;

        // Ускорение по S-образной кривой (сигмоид):
        // первые ~60 секунд скорость нарастает медленно, затем быстрее до MAX_SPEED
        {
            float u        = gameTime_ / 80.f;
            float sigmoid  = 1.f / (1.f + std::exp(-(u - 0.9f) * 2.2f));
            speed_         = INITIAL_SPEED + (MAX_SPEED - INITIAL_SPEED) * sigmoid;
            float ifactor  = 1.f - sigmoid;
            obsInterval_   = OBS_INTERVAL_MIN + (OBS_INTERVAL_INIT - OBS_INTERVAL_MIN) * ifactor;
        }

        // Цикл дня/ночи (синусоида, период ~90 секунд)
        dayPhase_ = (std::sin(gameTime_ * 0.07f) + 1.f) * 0.5f;

        player_->update(dt);

        // Спавн препятствий
        obsTimer_ += dt;
        if (obsTimer_ >= obsInterval_) {
            obsTimer_ -= obsInterval_;
            spawnObstacle();
            // Иногда спавним парное препятствие (раньше интервала)
            if (gameTime_ > 25.f &&
                std::uniform_real_distribution<float>(0,1)(rng_) < 0.22f)
                obsTimer_ = -0.4f;
        }
        for (auto& o : obs_) { o->spd = speed_; o->update(dt); }
        obs_.erase(std::remove_if(obs_.begin(), obs_.end(),
            [](const auto& o){ return o->offScreen(); }), obs_.end());

        // Облака
        cloudTimer_ += dt;
        if (cloudTimer_ >= 3.f) { cloudTimer_=0; spawnCloud(false, 0); }

        ground_.update(dt, speed_);
        score_.update(dt, speed_);
        if (score_.takeMilestone() && snd_) snd_->playScore();

        // Проверка коллизий
        for (const auto& o : obs_)
            if (player_->hits(*o)) { triggerDeath(); return; }
    }

    // Генерация объектов
    void spawnObstacle() {
        bool canBird = (gameTime_ > 9.f);
        int roll = std::uniform_int_distribution<int>(0, canBird ? 9 : 6)(rng_);
        float spd = speed_;

        if (roll < 5) {                     // малый кактус
            float oh=54, ow=40;
            obs_.push_back(std::make_unique<Obstacle>(
                (float)(SCREEN_W+10), (float)(GROUND_Y - oh), ow, oh,
                Obstacle::Type::CactusS, spd));
        } else if (roll < 7) {              // большой кактус
            float oh=76, ow=54;
            obs_.push_back(std::make_unique<Obstacle>(
                (float)(SCREEN_W+10), (float)(GROUND_Y - oh), ow, oh,
                Obstacle::Type::CactusL, spd));
        } else {                            // птица (случайная высота)
            float bh=42, bw=56;
            float heights[] = {
                (float)(GROUND_Y - 85),
                (float)(GROUND_Y - 130),
                (float)(GROUND_Y - 170)
            };
            float by2 = heights[std::uniform_int_distribution<int>(0,2)(rng_)];
            obs_.push_back(std::make_unique<Obstacle>(
                (float)(SCREEN_W+10), by2, bw, bh,
                Obstacle::Type::Bird, spd));
        }
    }

    void spawnCloud(bool fixedX, float offsetX) {
        float cy  = std::uniform_real_distribution<float>(15, 145)(rng_);
        float spd = std::max(40.f, speed_ * 0.20f);
        float cx  = fixedX ? offsetX : (float)(SCREEN_W + 95);
        clouds_.push_back(std::make_unique<Cloud>(cx, cy, spd));
    }


    // Отрисовка неба (градиент день-ночь, звёзды, луна)
    void renderSky() {
        float t = dayPhase_;
        // Горизонтальный градиент (сверху вниз) с учётом фазы
        for (int sy = 0; sy < GROUND_Y; sy++) {
            float f = (float)sy / GROUND_Y;
            auto lerp = [](float a, float b, float x) {
                return (Uint8)std::max(0.f, std::min(255.f, a + (b - a) * x));
            };
            Uint8 dr = lerp(lerp(95,160,f),  lerp(8,18,f),  t);
            Uint8 dg = lerp(lerp(175,215,f), lerp(8,18,f),  t);
            Uint8 db = lerp(lerp(255,245,f), lerp(35,60,f), t);
            SDL_SetRenderDrawColor(ren_, dr, dg, db, 255);
            SDL_RenderDrawLine(ren_, 0, sy, SCREEN_W, sy);
        }

        // Звёзды (появляются при dayPhase_ > 0.3)
        if (t > 0.3f) {
            float alpha = (t - 0.3f) / 0.7f;
            for (const auto& s : stars_) {
                int br = std::min(255, (int)(255 * alpha * s.brightness));
                SDL_SetRenderDrawColor(ren_, br, br, br, 255);
                SDL_RenderDrawPoint(ren_, (int)s.x, (int)s.y);
                if (s.brightness > 0.8f) { // более яркие звёзды – увеличенные
                    SDL_RenderDrawPoint(ren_, (int)s.x+1, (int)s.y);
                    SDL_RenderDrawPoint(ren_, (int)s.x, (int)s.y+1);
                }
            }
        }

        // Луна (при night > 0.5)
        if (t > 0.5f) {
            Uint8 ma = (Uint8)(255 * (t - 0.5f) * 2.f);
            SDL_SetRenderDrawColor(ren_, 255, 252, 210, ma);
            Draw::color(ren_, 255, 252, 210, ma);
            Draw::filledCircle(ren_, SCREEN_W - 110, 65, 26);
            // Имитация серпа (затенение части круга)
            if (t < 0.85f) {
                float shade = (0.85f - t) / 0.35f;
                int sx2 = SCREEN_W - 110 + (int)(12 * shade);
                Uint8 sa = (Uint8)(180 * shade);
                Uint8 nr = (Uint8)(8 + 18 * (1-t));
                Uint8 ng = (Uint8)(8 + 18 * (1-t));
                Uint8 nb = (Uint8)(35 + 25 * (1-t));
                SDL_SetRenderDrawColor(ren_, nr, ng, nb, sa);
                Draw::filledCircle(ren_, sx2, 65, 24);
            }
        }
    }

    void renderScene() {
        renderSky();
        for (auto& c : clouds_) c->draw(ren_);
        ground_.draw(ren_);
        for (auto& o : obs_)    o->draw(ren_);
        if (player_)             player_->draw(ren_);
    }

    // Рендер в зависимости от состояния игры

    void render() {
        SDL_SetRenderDrawColor(ren_, 135, 206, 235, 255);
        SDL_RenderClear(ren_);
        switch (state_) {
        case State::Menu:    renderMenu();    break;
        case State::Playing: renderPlaying(); break;
        case State::Dead:    renderDead();    break;
        }
        SDL_RenderPresent(ren_);
    }

    // Меню (с анимированным превью, инструкциями)
    void renderMenu() {
        dayPhase_ = 0.f; // в меню всегда день
        renderSky();
        for (auto& c : clouds_) c->draw(ren_);
        ground_.draw(ren_);

        // Превью динозавра, который едет по земле
        Player preview(680.f, (float)(GROUND_Y - 96), nullptr);
        preview.anim = menuTime_;
        preview.draw(ren_);

        // Полупрозрачная панель с текстом
        int pw = 750, ph = 360, px = SCREEN_W/2 - pw/2, py = 14;
        SDL_SetRenderDrawColor(ren_, 6, 6, 26, 215);
        Draw::rect(ren_, px, py, pw, ph);
        SDL_SetRenderDrawColor(ren_, 255, 195, 45, 255);
        Draw::rect(ren_, px, py, pw, ph, false);
        SDL_SetRenderDrawColor(ren_, 255, 195, 45, 60);
        Draw::rect(ren_, px+3, py+3, pw-6, ph-6, false);

        int cx = SCREEN_W / 2;
        SDL_Color cTitle  = {255, 218, 48,  255};
        SDL_Color cSub    = {175, 255, 135, 255};
        SDL_Color cAuthor = {185, 185, 185, 255};
        SDL_Color cHead   = { 85, 255, 135, 255};
        SDL_Color cText   = {238, 238, 238, 255};
        SDL_Color cStart  = {255, 255, 65,  255};

        txt_->big  (ren_, "ДИНО НА ВЕЛОСИПЕДЕ",       cx,  22, cTitle,  true);
        txt_->med  (ren_, "Великий Побег из Парка!",   cx,  74, cSub,    true);
        txt_->small(ren_, "Автор: Рубин Антон Анатольевич  |  Группа: Н255Б", cx, 114, cAuthor, true);

        SDL_SetRenderDrawColor(ren_, 255, 195, 45, 90);
        SDL_RenderDrawLine(ren_, px+18, 140, px+pw-18, 140);

        txt_->small(ren_, "[ ЦЕЛЬ ИГРЫ ]",                                        cx, 148, cHead, true);
        txt_->small(ren_, "Помогите Дино сбежать из парка на велосипеде!",         cx, 170, cText, true);
        txt_->small(ren_, "Перепрыгивайте кактусы и птиц — уезжайте как можно дальше.", cx, 192, cText, true);
        txt_->small(ren_, "Скорость и частота препятствий постепенно возрастают.", cx, 214, cText, true);

        SDL_SetRenderDrawColor(ren_, 255, 195, 45, 90);
        SDL_RenderDrawLine(ren_, px+18, 238, px+pw-18, 238);

        txt_->small(ren_, "[ УПРАВЛЕНИЕ ]",                                         cx, 246, cHead, true);
        txt_->small(ren_, "ПРОБЕЛ  /  \xE2\x86\x91  /  W  /  \xD0\x9B\xD0\x9A\xD0\x9C  \xe2\x80\x94  \xD0\x9F\xD1\x80\xD1\x8B\xD0\xBF\xD0\xBE\xD0\xBA", cx, 268, cText, true);
        txt_->small(ren_, "ESC  \xe2\x80\x94  Завершить текущий забег",              cx, 290, cText, true);
        txt_->small(ren_, "R  /  ЛКМ  \xe2\x80\x94  Начать заново (на экране Game Over)", cx, 312, cText, true);

        // Мигающее приглашение
        if ((int)(menuTime_ * 2) % 2 == 0)
            txt_->med(ren_, ">> Нажмите любую клавишу для старта! <<",
                      cx, 343, cStart, true);
    }

    // Игровой экран: HUD, полоска скорости, подсказка
    void renderPlaying() {
        renderScene();

        // Панель счёта
        SDL_SetRenderDrawColor(ren_, 0, 0, 0, 115);
        Draw::rect(ren_, 5, 5, 230, 66);
        SDL_SetRenderDrawColor(ren_, 255, 215, 45, 145);
        Draw::rect(ren_, 5, 5, 230, 66, false);

        SDL_Color cY = {255, 215, 45, 255};
        SDL_Color cW = {225, 225, 225, 255};
        txt_->med  (ren_, "Счёт: " + std::to_string(score_.score), 13, 10, cY);
        txt_->small(ren_, "Рекорд: " + std::to_string(score_.hiScore), 13, 42, cW);

        // Индикатор скорости (цвет от зелёного к красному)
        float ratio = (speed_ - INITIAL_SPEED) / (MAX_SPEED - INITIAL_SPEED);
        int   barW  = 140;
        SDL_SetRenderDrawColor(ren_, 0, 0, 0, 115);
        Draw::rect(ren_, SCREEN_W - barW - 14, 5, barW + 10, 52);
        SDL_SetRenderDrawColor(ren_, 45, 45, 45, 195);
        Draw::rect(ren_, SCREEN_W - barW - 11, 9, barW, 22);
        Uint8 br = (Uint8)(75  + ratio * 180);
        Uint8 bg = (Uint8)(195 - ratio * 165);
        SDL_SetRenderDrawColor(ren_, br, bg, 18, 235);
        Draw::rect(ren_, SCREEN_W - barW - 11, 9, (int)(barW * ratio), 22);
        SDL_Color cG = {178, 178, 178, 255};
        txt_->tiny(ren_, "Скорость", SCREEN_W - barW - 10, 34, cG);
        txt_->tiny(ren_, std::to_string((int)speed_) + " px/s", SCREEN_W - 80, 34, cG);

        // Подсказка (первые 5 секунд)
        if (gameTime_ < 5.f) {
            float alpha = 1.f - gameTime_ / 5.f;
            SDL_SetRenderDrawColor(ren_, 0, 0, 0, (Uint8)(90 * alpha));
            Draw::rect(ren_, SCREEN_W/2 - 195, SCREEN_H - 40, 390, 30);
            SDL_Color cH = {255, 255, 110, (Uint8)(205 * alpha)};
            txt_->small(ren_, "ПРОБЕЛ / стрелка вверх / ЛКМ — Прыжок",
                        SCREEN_W/2, SCREEN_H - 34, cH, true);
        }
    }

    // Экран Game Over с затемнением и статистикой
    void renderDead() {
        renderScene();
        SDL_SetRenderDrawColor(ren_, 0, 0, 0, 160);
        Draw::rect(ren_, 0, 0, SCREEN_W, SCREEN_H);

        int pw = 540, ph = 300, px = SCREEN_W/2 - pw/2, py = SCREEN_H/2 - ph/2;
        SDL_SetRenderDrawColor(ren_, 16, 6, 40, 228);
        Draw::rect(ren_, px, py, pw, ph);
        SDL_SetRenderDrawColor(ren_, 245, 50, 50, 255);
        Draw::rect(ren_, px, py, pw, ph, false);
        SDL_SetRenderDrawColor(ren_, 245, 50, 50, 55);
        Draw::rect(ren_, px+3, py+3, pw-6, ph-6, false);

        int cx = SCREEN_W / 2;
        SDL_Color cR  = {248, 50,  50,  255};
        SDL_Color cW  = {255, 255, 255, 255};
        SDL_Color cY  = {255, 215, 45,  255};
        SDL_Color cGr = { 70, 255, 125, 255};
        SDL_Color cGy = {170, 170, 170, 255};

        txt_->big  (ren_, "ИГРА ОКОНЧЕНА",  cx, py + 18,  cR,  true);
        txt_->med  (ren_, "Дино поймали!",  cx, py + 70,  cW,  true);

        txt_->med(ren_, "Счёт:     " + std::to_string(score_.score),
                  cx, py + 116, cY,  true);
        txt_->med(ren_, "Рекорд:   " + std::to_string(score_.hiScore),
                  cx, py + 158, cGr, true);

        int meters = (int)(score_.dist * 8.5f);
        txt_->small(ren_, "Дистанция: " + std::to_string(meters) + " м",
                    cx, py + 200, cGy, true);

        SDL_SetRenderDrawColor(ren_, 245, 50, 50, 75);
        SDL_RenderDrawLine(ren_, px+20, py+228, px+pw-20, py+228);

        txt_->small(ren_, "R / ЛКМ  \xe2\x80\x94  Играть снова",  cx, py + 236, cW, true);
        txt_->small(ren_, "ESC  \xe2\x80\x94  Выход из игры",       cx, py + 262, cW, true);
    }


    // Освобождение ресурсов
    void cleanup() {
        snd_.reset();
        txt_.reset();
        if (ren_) { SDL_DestroyRenderer(ren_); ren_=nullptr; }
        if (win_) { SDL_DestroyWindow(win_);   win_=nullptr; }
        SDL_Quit();
    }
};


// Точка входа
int main(int /*argc*/, char* /*argv*/[]) {
    setlocale(LC_ALL, "C.UTF-8");
    std::srand((unsigned int)std::time(nullptr));
    Game game;
    if (!game.init()) return 1;
    game.run();
    return 0;
}