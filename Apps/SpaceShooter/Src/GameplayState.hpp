#pragma once

#include "BackgroundStars.hpp"
#include "GameState.hpp"
#include "Hud.hpp"
#include "ParticleSystems.hpp"

class GameplayState final : public GameState
{
  public:
    GameplayState();

    void start() override;

    void update(GameTime time) override;

    void draw(GraphicsDevice gfx) override;

  private:
    struct Enemy
    {
        Vec2 pos;
        i32  health      = 1;
        u32  srcRectIdx  = 0;
        int  scorePoints = 10;

        Rectf rect() const;
    };

    struct Player
    {
        Vec2        pos;
        u32         currentMoveDirection = 0;
        Maybe<Vec2> bulletPos;
        u32         health = 0;

        Rectf rect() const;
    };

    void updatePlayer(const GameTime& time);

    void updateEnemies(const GameTime& time);

    void spawnEnemies(StringView enemyRows);

    void drawPlayer(GraphicsDevice gfx);

    void drawEnemies(GraphicsDevice gfx);

    Maybe<Enemy&> checkCollisionWithEnemy(const Rectf& rect);

    void killEnemy(const Enemy& enemy);

    void hitPlayer(Vec2 pos);

    Image           _spritesheet;
    Sound           _song;
    Sound           _player_shoot_sound;
    Sound           _player_hurt_sound;
    Sound           _enemy_shoot_sound;
    List<Sound>     _enemy_kill_sounds;
    u32             _score = 0;
    Player          _player;
    List<Enemy>     _enemies;
    List<Vec2>      _enemy_bullets;
    int             _enemies_direction         = 1;
    u32             _level_enemy_count         = 0;
    float           _time_to_next_enemy_move   = 1.0f;
    float           _time_to_next_enemy_bullet = 0.0f;
    float           _enemy_sway_speed          = 1.0f;
    u32             _enemy_anim_idx            = 0;
    ParticleSystems _particle_systems;
    BackgroundStars _background_stars;
    Hud             _hud;
};
