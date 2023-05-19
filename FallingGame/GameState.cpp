#include "GameState.hpp"

#include "Game.hpp"
#include "misc.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

void CloudHandler::init_menu(CloudHandler& ch, Drawer& d)
{
    for (int i = 0; i < (int)ch.clouds.size(); ++i) {
        ch.clouds[i].construct(d, (float)((int)ch.clouds.size() - i) / (float)ch.clouds.size());
        ch.clouds[i]._x_vel += rand_uni();
    }
}

void CloudHandler::init_game(CloudHandler& ch, Drawer& d)
{
    for (int i = 0; i < (int)ch.clouds.size(); ++i) {
        ch.clouds[i].construct(d, (float)((int)ch.clouds.size() - i) / (float)ch.clouds.size());
    }
}

void CloudHandler::menu_logic(CloudHandler& ch, Game& g)
{
    for (auto& e : ch.clouds) {
        if ((e.x + e.w) < -Layer::WIDTH || (e.x - e.w) > Layer::WIDTH || (e.y - e.h) > Layer::HEIGHT) {
            e.tex = (TEX::_)(TEX::cloud_1 + rand_int(0, 4));

            e._x_vel *= -1.f;
        }
        e.x += e._x_vel * g.l.dt * (1.f - e.z);
    }
}

void CloudHandler::game_logic(CloudHandler& ch, Game& g, Camera& c)
{
    for (auto& e : ch.clouds) {
        if ((e.x + e.w) < -Layer::WIDTH || (e.x - e.w) > Layer::WIDTH || (e.y - e.h) > Layer::HEIGHT) {
            e.init(g.d);
        }
        e.y += (c.y_dif) * (1.f - e.z);
        e.x += e._x_vel * g.l.dt * (1.f - e.z);
    }
}

template <int BOUNCERS>
static void bouncers_logic(std::array<Bouncer, BOUNCERS>& bouncers, Player& p,
    float timer, float dt)
{
    const bool can_bounce_player = p._y_vel < 0.0f;

    for (auto& e : bouncers) {
        if (!e._destroyed) {
            if (can_bounce_player) {
                // TODO: Maybe remove the branches from this if?
                if (e.h.y < p._prev_y && e.h.y > p._r.y && p._r.x < e.h.x + e.h.w && p._r.x + p._r.w > e.h.x) {
                    if (!p._powerup_active) {
                        p._bounce_x_vel = std::min(1.f, -0.5f * p._y_vel) * 10.f * p._r.x_dist(e.h);
                        p._y_vel = std::clamp(-0.5f * p._y_vel, 0.75f, 1.5f);
                        e.bounced_on(p._bounce_x_vel);
                    } else {
                        e._destroyed = true;
                    }
                }
            }

            switch (e._type) {
            case Bouncer::Type::Moves:

                e._x_vel += dt * (sinf(0.8f + 2.f * std::fabs(e.h.x) + timer + 381.345f * e._init_time));

                e.h.x += dt * e._x_vel;

                if (e.h.x < -Game::G_WIDTH) {
                    e._x_vel = std::fabs(e._x_vel);
                } else if (e.h.x > Game::G_WIDTH) {
                    e._x_vel = -std::fabs(e._x_vel);
                }
                break;
            default:
                break;
            }
        } else {
        }
    }
}

struct CoinHandler {
    std::vector<Coin> _coins;

    float _next_coin_y;

    void init() { _next_coin_y = -1.5f; }

    void logic(Game& g, Camera& c, Player& p)
    {
        const float remove_bound = c.y + 2.f * g.l.HEIGHT;

        // delete coins that have gone too high
        for (int i = (int)_coins.size() - 1; i >= 0; --i) {
            if (_coins[i]._r.y > remove_bound) { // should be removed
                _coins.erase(_coins.begin() + i);
            }
        }

        handle_collisions_player_coins(p);

        float spawn_bound = c.y - g.l.HEIGHT * 2.f;

        // spawn coin
        while (spawn_bound <= _next_coin_y) {
            _coins.emplace_back(g, _next_coin_y);
            _next_coin_y -= (2.f * g.l.HEIGHT * (0.5f + 0.5f * rand_01()));
        }
    }

    void handle_collisions_player_coins(Player& p)
    {
        for (int i = (int)_coins.size() - 1; i >= 0; --i) {
            auto& e = _coins[i];
            if (p._r.intersect(e._r)) {
                // std::cout << "COIN \n";
                if (!e.picked_up) {
                    // make bird shiny
                    p._time_since_coin = 0.f;
                    ++p._coins;
                }
                // coins.erase(coins.begin() + i);
                e.got_picked_up();
            }
        }
    }
    void draw(Game& g, Camera& c, float timer)
    {
        for (auto& e : _coins) {
            e.draw(g, c, timer);
        }
    }
};

struct Feather {
    Rect _r;
    bool _picked_up;
    float _time_offset;

    void init(float x, float y, float time_offset)
    {
        _r.x = x;
        _r.y = y;
        _r.w = 0.3f;
        _r.h = 0.3f;
        _picked_up = false;
        _time_offset = time_offset;
    }

    void draw(Game& g, Camera& c, float timer)
    {
        if (!_picked_up) {
            float sin_val = std::sin((timer - _time_offset) * 2.f);
            float width = _r.w * (std::pow(std::abs(sin_val), 0.9f)) * sign(sin_val);
            g.d.draw_image(TEX::feather, _r.x + _r.w / 2, _r.y + _r.h / 2,
                width, _r.h, 0.f);

            _r.draw(g.d, { 1.f, 0.f, 0.f, 0.4f });
        } else // spin away animation
        {
            float sin_val = std::sin(timer * 20.f);
            float width = _r.w * (std::pow(std::abs(sin_val), 0.9f)) * sign(sin_val);
            float height = _r.h;
            float dec = (1.f + 50.f * _time_offset * _time_offset * _time_offset);
            width /= dec;
            height /= dec;

            g.d.draw_image(TEX::feather, _r.x + _r.w / 2, _r.y + _r.h / 2,
                width, height, 0.f);

            _r.draw(g.d, { 1.f, 0.f, 0.f, 0.4f });

            _time_offset += g.l.dt;
        }
    }
    void got_picked_up()
    {
        if (!_picked_up) {
            _picked_up = true;
            _time_offset = 0.f;
        }
    }
};

template <int N>
static void handle_feather_collisions(std::array<Feather, N>& feathers,
    Player& p)
{
    for (auto& e : feathers) {
        if (p._r.intersect(e._r)) {
            if (!e._picked_up) {
                // make bird shiny
                p.get_feather(1);
            }
            e.got_picked_up();
        }
    }
}

struct FeatherHandler {

    static constexpr int _MAX_FEATHERS = 10;

private:
    std::array<Feather, _MAX_FEATHERS> _feathers;

    float _next_y;
    int _feather_at;

public:
    void init()
    {
        for (auto& e : _feathers) {
            e.init(0.f, 1000.f, 0.f);
        }
        _next_y = 1.f;
        _feather_at = 0;
    }

    void logic(Player& p)
    {
        handle_feather_collisions<_MAX_FEATHERS>(_feathers, p);

        while (p._r.y - Game::G_HEIGHT * 2.f < _next_y) {
            _feathers[_feather_at++].init(rand_uni() * Game::G_WIDTH, _next_y, 0.f);
            _next_y -= 4.0f + 6.f * rand_01();
            if (_feather_at >= _MAX_FEATHERS) {
                _feather_at = 0;
            }
        }
    }
    void draw(Game& g, Camera& c, float timer)
    {
        for (auto& e : _feathers) {
            e.draw(g, c, timer);
        }
    }
};

struct BouncerHandler {
private:
    static constexpr int _MAX_BOUNCERS = 100;
    std::array<Bouncer, _MAX_BOUNCERS> _bouncers;

    // How it works:
    // We have a `_next_bouncer_y` that we place bouncers at when the camera is
    // sufficiently close to it. We place the bouncer at `_bouncer_index`, which
    // hopefully has already gone through the player scene, and is stranded up
    // This works as long as we don't spawn too many bouncers at once, then
    // `_MAX_BOUNCERS` will have to be increased
    float _next_bouncer_y;
    // which bouncer to move next. When it reaches `_MAX_BOUNCERS` it resets to
    // 0
    int _bouncer_index;

    float _percent_move;

    CoinHandler _coinh;
    FeatherHandler _fh;

public:
    void init(float percent_move)
    {
        _next_bouncer_y = -0.5f;
        _bouncer_index = 0;
        _percent_move = percent_move;

        for (auto& e : _bouncers) {
            e.init(0.f, 2.f * Layer::HEIGHT, Bouncer::Type::Normal, 0.f);
        }

        _coinh.init();
        _fh.init();
    }

    void logic(Game& g, Player& p, Camera& c, float _level_end, float timer,
        float dt)
    {
        // Bounce Logic
        bouncers_logic<_MAX_BOUNCERS>(_bouncers, p, timer, dt);

        // Spawn bouncers
        float spawn_bound = c.y - Layer::HEIGHT * 2.25f;
        while (spawn_bound <= _next_bouncer_y && _next_bouncer_y > _level_end - Layer::HEIGHT) {
            const auto type = (rand_01() > _percent_move)
                ? Bouncer::Type::Normal
                : Bouncer::Type::Moves;
            _bouncers[_bouncer_index++].init(Game::G_WIDTH, _next_bouncer_y,
                type, timer);
            if (_bouncer_index >= _MAX_BOUNCERS) {
                _bouncer_index = 0;
            }

            _next_bouncer_y -= (2.8f * (0.3f + 0.5f * rand_01()));
        }

        _coinh.logic(g, c, p);
        _fh.logic(p);
    }

    void draw(Game& g, Camera& c, float timer)
    {
        for (auto& e : _bouncers) {
            e.draw(g, c);
        }
        _coinh.draw(g, c, timer);
        _fh.draw(g, c, timer);
    }
};

struct TutorialBouncerHandler {
    static constexpr int _MAX_BOUNCERS = 22;
    std::array<Bouncer, _MAX_BOUNCERS> _bouncers;

    static constexpr int _MAX_FEATHERS = 5;
    std::array<Feather, _MAX_FEATHERS> _feathers;

    TutorialBouncerHandler()
    {
        // bouncers
        {
            int bi = 0;
            for (int i = 0; i < 7; ++i) {
                _bouncers[bi].init(0.1f, -5.f, Bouncer::Type::Normal, 0.f);
                _bouncers[bi].h.x = 0.15f * (float)i - 0.4f;
                ++bi;
            }

            for (int i = 0; i < 8; ++i) {
                _bouncers[bi].init(0.1f, -10.f, Bouncer::Type::Normal, 0.f);
                _bouncers[bi].h.x = -0.175f * (float)i + 0.2f;
                ++bi;
            }

            _bouncers[bi].init(0.f, -17.5f, Bouncer::Type::Normal, 0.f);
            _bouncers[bi].h.w = Game::G_WIDTH * 2.f;
            _bouncers[bi].h.x = -Game::G_WIDTH;
            ++bi;

            for (int i = 0; i < 6; ++i) {
                _bouncers[bi].init(0.15f, -20.f - (float)(2 * i),
                    Bouncer::Type::Normal, 0.f);
                _bouncers[bi].h.x = ((i % 2) == 0) ? -0.3f : 0.3f;
                ++bi;
            }
        }

        {
            int i = 0;
            for (auto& e : _feathers) {
                e.init(-0.5f + 0.2f * i, -17.3f, (float)i);
                ++i;
            }
        }
    }

    void logic(Game& g, Player& p, Camera& c, float _level_end, float timer,
        float dt)
    {
        bouncers_logic<_MAX_BOUNCERS>(_bouncers, p, timer, dt);
        handle_feather_collisions<_MAX_FEATHERS>(_feathers, p);
    };

    void draw(Game& g, Camera& c, float timer)
    {
        for (auto& e : _bouncers) {
            e.draw(g, c);
        }

        for (auto& e : _feathers) {
            e.draw(g, c, timer);
        }

        g.d.draw_text<true>("<--|-->", { 0.f, 0.f, 0.f, 1.f }, 0.f, -4.6f - c.y,
            0.012f);

        g.d.draw_text<true>("<--|-->", { 0.f, 0.f, 0.f, 1.f }, 0.f, -9.6f - c.y,
            0.012f);
    };
};

void Button::init(const char* text, float text_size, float mid_x, float mid_y,
    float w, float h)
{
    _r.init(mid_x - w / 2.f, mid_y - h / 2.f, w, h);
    _text = text;
    _text_size = text_size;
    _almost_pressed = false;
    _just_pressed = false;
    _draw_pressed = false;
}

void Button::logic(Layer& l, float cam_y, float timer, bool move)
{
    _just_pressed = false;
    _draw_pressed = false;

    if (move)
        _r.x += 0.0010f * cosf(timer);

    Pos finger = l.m_finger_pos;
    finger.y += cam_y;
    if (!_almost_pressed) {
        if (l.m_finger_just_down && _r.intersect_point(finger)) {
            _almost_pressed = true;
            _draw_pressed = true;
        }
    } else {
        static constexpr float BOUND = 0.01f;
        if (fabs(l.m_finger_move.y) > BOUND || fabs(l.m_finger_move.x) > BOUND) {
            _almost_pressed = false;
            return;
        }
        bool intersects = _r.intersect_point(finger);
        _draw_pressed = intersects;
        if (l._finger_just_up) {
            if (intersects) {
                _just_pressed = true;
            }
            _almost_pressed = false;
        }
    }
}

void Button::draw_start(Drawer& d, float cam_y)
{
    Color c = (_draw_pressed) ? (Color { 0.5f, 0.5f, 0.f, 1.f })
                              : (Color { 1.f, 1.f, 0.f, 1.f });
    d.draw_rectangle(_r.x, _r.y, _r.w, _r.h, c);
    d.draw_text<true>(_text, { 0.f, 0.f, 0.f, 1.f }, _r.x + _r.w / 2.f,
        _r.y + _r.h / 2.f - cam_y, _text_size);
}

void Button::draw_level(Drawer& d, float cam_y, LevelState ls)
{
    Color c;
    switch (ls) {
    case LevelState::Locked:
        c = Color { 0.5f, 0.5f, 0.f, 0.5f };
        break;
    case LevelState::Unlocked:
        c = (_draw_pressed) ? (Color { 0.5f, 0.5f, 0.f, 1.f })
                            : (Color { 1.f, 1.f, 0.f, 1.f });
        break;
    case LevelState::Done:
        c = (_draw_pressed) ? (Color { 0.5f, 0.5f, 0.f, 1.f })
                            : (Color { 0.35f, 0.95f, 0.1f, 1.f });
        break;
    }
    d.draw_rectangle(_r.x, _r.y, _r.w, _r.h, c);
    d.draw_text<true>(_text, { 0.f, 0.f, 0.f, 1.f }, _r.x + _r.w / 2.f,
        _r.y + _r.h / 2.f - cam_y, _text_size);
}

bool Button::just_pressed() { return _just_pressed; }

MenuState::MenuState(Game& g)
{
    MenuState& gs = *this;
    gs.timer = 0.f;

    gs.ch.init_menu(gs.ch, g.d);

    gs.c.init();

    gs._btn_start.init("Start", 0.0025f, 0.f, 0.f, 0.5f, 0.25f);
}

void MenuState::entry_point(Game& g)
{
    MenuState& gs = *this;

    if (g.ge.exit_current_session) {
        g.should_quit = true;
    }

    gs._btn_start.logic(g.l, 0.f, 0.f, false);

    g.ge.enter_game_session |= gs._btn_start.just_pressed();

    // logic
    if (g.ge.enter_game_session) {
        g.set_new_state(SessionToChangeTo::LevelSelector, -1);
    }

    {
        float move = 1.1f * g.l.dt;
        gs.c.y += move;
    }

    gs.ch.menu_logic(gs.ch, g);

    g.d.before_draw(g, 5000000.f, 0.f, gs.timer, { 1.f, 1.f, 1.f, 1.f });
    g.d.draw_sky(g, gs.c.y);
    g.d.draw_clouds(gs.ch);
    gs._btn_start.draw_start(g.d, 0.f);
    // g.d.draw_text<true>("Down-Fall", { 0.5f,0.f,0.5f,1.f }, 0.00f,
    // Layer::HEIGHT*0.5f, 0.004f);
    {
        static constexpr float size = 0.002f;
        const float w = g.d.tex_sizes[TEX::title_screen][0] * size;
        const float h = g.d.tex_sizes[TEX::title_screen][1] * size;
        const float x_offset = sinf(gs.timer) * 0.1f;
        const float rotation = sinf(gs.timer * 1.5f) * 0.6f;
        g.d.draw_image(TEX::title_screen, 0.f + x_offset, Layer::HEIGHT * 0.5f,
            w, h, rotation);
    }
    // d.draw_rectangle(-Layer::WIDTH, -Layer::HEIGHT, Layer::WIDTH*2.f,
    // Layer::HEIGHT*2.f, {1.f,1.f,1.f,1.f});
}

LevelSelectorState::LevelSelectorState(Game& g)
{
    int i = 0;

    static constexpr std::array<const char*, SaveState::TOTAL_LEVELS> names = {
        "Level 1",
        "Level 2",
        "Level 3",
        "Level 4",
        "Level 5",
        "Level 6",
        "Level 7",
        "Level 6",
        "Level 7",
    };

    for (auto& e : _btn_levels) {
        e.init(names[i], 0.00175f, 0.4f * sinf(float(i) / 1.75f), -i * 0.3f,
            0.5f, 0.2f);
        ++i;
    }
    timer = 0.f;
    _scroll_y = 0.f;
}

void LevelSelectorState::entry_point(Game& g)
{
    LevelSelectorState& gs = *this;
    // logic

    if (g.ge.exit_current_session) {
        g.set_new_state(SessionToChangeTo::Menu, -1);
    }

    /*if (g.l.m_finger_down)*/ {
        _scroll_y += g.l._finger_scroll.y;
        _scroll_y = fmin(0.f, _scroll_y);
        _scroll_y = fmax(-3.f, _scroll_y);
    }
    const float cam_y = _scroll_y;

    {
        int i = 0;
        for (auto& e : _btn_levels) {
            e.logic(g.l, cam_y, 2.f * gs.timer + 0.7f * float(i), true);
            if (e.just_pressed() && g._save_state.level_info[i].state != LevelState::Locked) {
                g.set_new_state(SessionToChangeTo::Game, i);
            }
            ++i;
        }
    }

    // printf("finger rel y: %f\n", g.l.m_finger_move.y);

    // draw

    g.d.before_draw(g, 5000000.f, cam_y, gs.timer, { 1.f, 1.f, 1.f, 1.f });
    g.d.draw_sky(g, gs.timer);
    int i = 0;
    for (auto& e : _btn_levels) {
        e.draw_level(g.d, cam_y, g._save_state.level_info[i].state);
        ++i;
    }

    g.d.draw_text<true>("Level Selector", { 0.5f, 0.f, 0.5f, 1.f }, 0.00f,
        Layer::HEIGHT * 0.5f - cam_y, 0.004f);
}

static void set_movement_events(Game& g)
{
    g.ge.player_to_right = g.l.key_down(SDL_SCANCODE_D);
    g.ge.player_to_left = g.l.key_down(SDL_SCANCODE_A);
    if (g.l.m_finger_down) {
        g.ge.player_to_left = g.l.m_finger_pos.x < 0.0f; // middle of screen
        g.ge.player_to_right = !g.ge.player_to_left;
    }

    g.ge.player_activate_special = g.l.key_down(SDL_SCANCODE_L);
}

// draw death storm at death_y
static void draw_death_storm(float death_y, Game& g)
{
    g.d.draw_image(TEX::storm, 0.f, death_y + g.l.HEIGHT, g.l.WIDTH * 2.f,
        g.l.HEIGHT * 2.f, 0.f);
    g.d.draw_rectangle(-g.l.WIDTH, death_y + 2.f * g.l.HEIGHT, 2.f * g.l.WIDTH,
        g.l.HEIGHT * 2.f, { 0.f, 0.f, 0.f, 1.f });
}

// filled is from 0.f to 1.f
static void draw_fire_bar(Game& g, Player& p, float timer)
{
    const float filled = p.powerup_filled();

    static constexpr float size = 0.005f;
    const float bar_w = g.d.tex_sizes[TEX::fire_bar][0] * size;
    const float bar_h = g.d.tex_sizes[TEX::fire_bar][1] * size;

    const float bar_right = 0.f + bar_w / 2.f;
    const float bar_top = g.G_HEIGHT;

    {
        const float frame_speed = (filled >= 0.5f) ? (4.f) : (2.f);
        TEX::_ fire_tex = (TEX::_)((int)TEX::fire_0 + (((int)(timer * frame_speed)) % 7));

        const float w = bar_w * filled;
        const float h = bar_h;

        g.d.draw_firebar(fire_tex, bar_right - w / 2.f, bar_top - h / 2.f, w, h,
            (p._powerup_active) ? 1.f : filled);
    }

    {
        g.d.draw_image(TEX::fire_bar, bar_right - bar_w / 2.f,
            bar_top - bar_h / 2.f, bar_w, bar_h, 0.f, false);
    }
}

template <typename ObstacleHandler, bool DEATH_STORM_ACTIVE>
struct GameState final : public BaseState {
public:
    Camera c;
    Player p;

    enum class State : unsigned char {
        Playing,
        Win,
        Lose
    };

    State game_state;

    int _level;

    float death_y; // y coordinate of the death barrier

    // which y coordinate the level ends at
    float _level_end;

    CloudHandler ch;
    ObstacleHandler _oh;

    float _time_when_playing_ended;
    std::array<Button, 3> _buttons;

    virtual void entry_point(Game& g) override
    {
        GameState& gs = *this;

        switch (gs.game_state) {
        case GameState::State::Playing:
            GameState::main_loop_playing(gs, g);
            break;
        case GameState::State::Win:
            GameState::main_loop_not_playing<GameState::State::Win>(gs, g);
            break;
        case GameState::State::Lose:
            GameState::main_loop_not_playing<GameState::State::Lose>(gs, g);
            break;
        }
    }

    GameState(Game& g, int level, float level_end)
    {
        GameState& gs = *this;

        gs._level = level;
        gs._level_end = level_end;

        gs.death_y = 3.f;
        gs.timer = 0.f;
        gs.p.init();
        gs.c.init();
        gs.c.set_in_game(gs.p, -1000.f);

        gs.ch.init_game(gs.ch, g.d);

        gs.game_state = GameState::State::Playing;
    }

    // draw coin counter
    static void draw_coins_counter(GameState& gs, Game& g)
    {
        char buf[10];
        snprintf(buf, 10, "Coins: %d", gs.p._coins);
        g.d.draw_text(buf, { 1.f, 1.f, 0.f, 1.f }, Game::G_WIDTH - 0.2f,
            Layer::HEIGHT - 0.2f, 0.001f);
    }

    // playing decides if win has happened or not
    static void main_loop_playing(GameState& gs, Game& g)
    {
        // events
        set_movement_events(g);

        // Exit
        if (g.ge.exit_current_session) {
            g.set_new_state(SessionToChangeTo::LevelSelector, -1);
        }

        {
            // Win
            if (gs.p._r.y + 2.f * gs.p.HEIGHT < gs.c.y - g.l.HEIGHT) {
                gs.game_state = GameState::State::Win;
                g._save_state.level_info[gs._level].state = LevelState::Done;
                if (gs._level + 1 < SaveState::TOTAL_LEVELS) {
                    if (g._save_state.level_info[gs._level + 1].state != LevelState::Done) {
                        g._save_state.level_info[gs._level + 1].state = LevelState::Unlocked;
                    }
                }

                // init things
            }
            // Lose
            else if (gs.death_y + 2.f * Layer::HEIGHT + gs.c.player_screen_top_offset / 2.f < gs.p._r.y) {
                gs.game_state = GameState::State::Lose;
            }

            // if not playing anymore after this frame
            if (gs.game_state != GameState::State::Playing) {
                gs._time_when_playing_ended = gs.timer;

                gs._buttons[0].init("Back", 0.001f, 0.f, 0.f + gs.c.y, 0.3f,
                    0.1f);
                gs._buttons[1].init("Next level", 0.001f, 0.f, -0.3f + gs.c.y,
                    0.3f, 0.1f);
                gs._buttons[2].init("Retry", 0.001f, 0.f, 0.f + gs.c.y, 0.3f,
                    0.1f);
            }

            if constexpr (DEATH_STORM_ACTIVE) {
                // increase the more far away the player is from the barrier
                gs.death_y -= 1.4f * g.l.dt * std::max(std::log(gs.timer / 100.f), std::max(1.f, 0.05f * std::pow(gs.death_y - gs.p._r.y, 2.f)));
            }

            // gs.death_y -= 1.4f * g.l.dt * std::max(std::min(1.8f,
            // std::log(gs.timer / 10.f)), std::max(1.f, 0.05f *
            // std::pow(gs.death_y - gs.p.r.y, 2.f)));

            // increase speed of death_y over time
            // gs.death_y -= 1.41f * g.l.dt * std::max(1.f, std::log(gs.timer
            // / 10.f));

            gs.p.logic(g.ge, g.l.dt);

            // set CAMERA position
            gs.c.set_in_game(gs.p, gs._level_end);

            // printf("Camera y: %f", gs.c.y);
            gs._oh.logic(g, gs.p, gs.c, gs._level_end, gs.timer, g.l.dt);
        }
        gs.ch.game_logic(gs.ch, g, gs.c);

        // Drawing
        g.d.before_draw(g, gs.death_y, gs.c.y, gs.timer, { 0.f, 0.f, 0.f, 1.f });

        g.d.draw_sky(g, gs.c.y);

        g.d.draw_clouds(gs.ch);

        const bool draw_player_in_front = gs.p.draw_in_front();

        if (!draw_player_in_front)
            gs.p.draw(g, gs.c);

        gs._oh.draw(g, gs.c, gs.timer);

        // side background
        g.d.draw_sides(gs.p);

        draw_death_storm(gs.death_y, g);

        draw_fire_bar(g, gs.p, gs.timer);

        draw_coins_counter(gs, g);

        if (draw_player_in_front)
            gs.p.draw(g, gs.c);
    }

    template <State STATE>
    static void main_loop_not_playing(GameState& gs, Game& g)
    {
        // logic
        gs.c.y_dif = 0.f;
        static constexpr float menu_h = 1.f, menu_w = 1.f;
        const float time_passed = gs.timer - gs._time_when_playing_ended;
        const float time_passed_limited = std::min(1.f, time_passed);
        const float menu_x_offset = std::pow(
            1.5f * Layer::WIDTH * time_passed_limited - 1.5f * Layer::WIDTH,
            2.f);

        gs._buttons[0]._r.x = menu_x_offset - menu_w / 2.f + 0.1f;
        gs._buttons[0].logic(g.l, gs.c.y, 0.f, false);

        gs._buttons[1]._r.x = menu_x_offset - 0.2f;
        gs._buttons[1].logic(g.l, gs.c.y, 0.f, false);

        gs._buttons[2]._r.x = menu_x_offset + menu_w / 2.f - 0.4f;
        gs._buttons[2].logic(g.l, gs.c.y, 0.f, false);

        g.ge.exit_current_session |= gs._buttons[0].just_pressed();

        if (gs._buttons[1].just_pressed()) {
            if constexpr (STATE == GameState::State::Win) {
                g.set_new_state(SessionToChangeTo::Game, gs._level + 1);
            } else if constexpr (STATE == GameState::State::Lose) {
            }
        }

        if (gs._buttons[2].just_pressed()) {
            g.set_new_state(SessionToChangeTo::Game, gs._level);
        }

        // Exit
        if (g.ge.exit_current_session) {
            g.set_new_state(SessionToChangeTo::LevelSelector, -1);
        }

        gs.ch.game_logic(gs.ch, g, gs.c);

        //////////////////////////////////

        // Drawing
        g.d.before_draw(g, gs.death_y, gs.c.y, gs.timer, { 0.f, 0.f, 0.f, 1.f });

        g.d.draw_sky(g, gs.c.y);

        g.d.draw_clouds(gs.ch);

        gs.p.draw(g, gs.c);
        gs._oh.draw(g, gs.c, gs.timer);

        // side background
        g.d.draw_sides(gs.p);

        draw_death_storm(gs.death_y, g);

        draw_coins_counter(gs, g);

        g.d.draw_rectangle(menu_x_offset - menu_w / 2.f, -menu_h / 2.f + gs.c.y,
            menu_w, menu_h, { 0.73f, 0.88f, 0.88f, 0.7f });

        gs._buttons[0].draw_start(g.d, gs.c.y);
        gs._buttons[2].draw_start(g.d, gs.c.y);
        if constexpr (STATE == GameState::State::Win) {
            gs._buttons[1].draw_level(g.d, gs.c.y, LevelState::Unlocked);
        } else if constexpr (STATE == GameState::State::Lose) {
            gs._buttons[1].draw_level(g.d, gs.c.y, LevelState::Locked);
        }

        const char* title_text;
        if constexpr (STATE == GameState::State::Win) {
            title_text = "You won!";
        }
        if constexpr (STATE == GameState::State::Lose) {
            title_text = "You lost :(";
        }

        g.d.draw_text<true>(title_text, { 0.93f, 1.f, 0.f, 1.f }, menu_x_offset,
            0.4f, 0.002f);
    }
};

[[nodiscard]] BaseState* create_new_game_session(Game& g, int level)
{
    float level_end = -6.f * level * level;
    float percent_move = 0.f;

    SDL_Log("Starting level: %d", level);

    switch (level) {
    case LEVEL::Tutorial: {
        auto gs = new GameState<TutorialBouncerHandler, false> { g, level, -30.f };
        gs->death_y = 10.f;
        return gs;
    } break;
    case LEVEL::W1_0:
        level_end = 0.f;
        break;
    case LEVEL::W1_1:
        level_end = -5.f;
        break;
    case LEVEL::W1_2:
        level_end = -25.f;
        break;
    case LEVEL::W1_3:
        level_end = 0.f;
        percent_move = 1.f;
        break;
    case LEVEL::W1_4:
        level_end = -7.f;
        percent_move = 0.5f;
        break;
    case LEVEL::W1_5:
        level_end = -49.f;
        percent_move = 0.75f;
        break;
    case LEVEL::W1_6:
        break;
    case LEVEL::W1_7:
        break;
    case LEVEL::W1_8:
        break;
    default:
        break;
    }

    auto gs = new GameState<BouncerHandler, true> { g, level, level_end };
    gs->_oh.init(percent_move);

    return gs;
}

[[nodiscard]] BaseState* create_new_session(Game& g, SessionToChangeTo session,
    int level)
{
    switch (session) {
    case SessionToChangeTo::Game:
        return create_new_game_session(g, level);
        break;
    case SessionToChangeTo::Menu:
        return new MenuState { g };
        break;
    case SessionToChangeTo::LevelSelector:
        return new LevelSelectorState { g };
        break;
    default:
        return nullptr;
    }
}
