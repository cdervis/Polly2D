#include "GameplayState.hpp"

#include "Constants.hpp"
#include "SpaceShooter.hpp"

constexpr auto sPlayerSize = Vec2(8.0f);

constexpr auto sPlayerBulletSrcRects = Array{
    Rectangle(0, 8, 8, 8),
    Rectangle(8, 8, 8, 8),
    Rectangle(16, 8, 8, 8),
};

constexpr auto sEnemySize = Vec2(8.0f);

constexpr auto sPlayerSpriteRects = Array{
    Rectangle(0, 0, 8, 8),
    Rectangle(8, 0, 8, 8),
    Rectangle(16, 0, 8, 8),
};

constexpr auto sEnemyRects = Array{
    Array{Rectangle(24, 0, 8, 8), Rectangle(24, 8, 8, 8)},
    Array{Rectangle(32, 0, 8, 8), Rectangle(32, 8, 8, 8)},
    Array{Rectangle(40, 0, 8, 8), Rectangle(40, 8, 8, 8)},
    Array{Rectangle(48, 0, 8, 8), Rectangle(48, 8, 8, 8)},
    Array{Rectangle(56, 0, 8, 8), Rectangle(56, 8, 8, 8)},
};

constexpr auto sEnemyBulletRect = Rectangle(64, 32, 5, 5);

// TODO: more levels
constexpr auto sLevel1Enemies = R"(
000000000
000---000
000333000
1-1111-11
222222222
444444444
)"_sv;

GameplayState::GameplayState()
    : _hud(_score, _player.health)
{
}

void GameplayState::start()
{
    logDebug("Starting gameplay state");

    _spritesheet        = Image("spritesheet.png");
    _song               = Sound("8bit-retro-funk-david-renda.mp3");
    _player_shoot_sound = Sound(SfxrSoundPreset::Laser, 12839);
    _player_hurt_sound  = Sound(SfxrSoundPreset::Hurt, 87224);
    _enemy_shoot_sound  = Sound(SfxrSoundPreset::Laser, 28190);

    _enemy_kill_sounds = {
        Sound(SfxrSoundPreset::Explosion, 213809),
        Sound(SfxrSoundPreset::Explosion, 989238),
        Sound(SfxrSoundPreset::Explosion, 168232),
        Sound(SfxrSoundPreset::Explosion, 46820),
        Sound(SfxrSoundPreset::Explosion, 33190),
    };

    _player.pos    = Vec2(sViewSize.x / 2, sViewSize.y - 8);
    _player.health = 3;
    spawnEnemies(sLevel1Enemies);

    _time_to_next_enemy_bullet = Random::nextFloat(sEnemyTimeToShootInterval);

    _game->audio().playInBackground(_song, 0.4f);
}

void GameplayState::update(GameTime time)
{
    _background_stars.update(time.elapsed());
    _particle_systems.update(time.elapsed());
    updatePlayer(time);
    updateEnemies(time);
}

void GameplayState::draw(Painter painter)
{
    _background_stars.draw(painter);
    drawPlayer(painter);
    drawEnemies(painter);
    _particle_systems.draw(painter);
    _hud.draw(painter);
}

Rectangle GameplayState::Enemy::rect() const
{
    return Rectangle(pos, sEnemyRects[srcRectIdx][0].size());
}

Rectangle GameplayState::Player::rect() const
{
    return Rectangle(pos, Vec2(8, 8));
}

void GameplayState::updatePlayer(const GameTime& time)
{
    constexpr auto playerMoveSpeed = 36.0f;

    _player.currentMoveDirection = 0;

    if (isKeyDown({Scancode::Left, Scancode::A}))
    {
        _player.pos.x -= time.elapsed() * playerMoveSpeed;
        _player.currentMoveDirection = 1;
    }

    if (isKeyDown({Scancode::Right, Scancode::D}))
    {
        _player.pos.x += time.elapsed() * playerMoveSpeed;
        _player.currentMoveDirection = 2;
    }

    if (isKeyDown(Scancode::Space) && !_player.bulletPos)
    {
        _player.bulletPos = _player.pos;
        _particle_systems.playerShot.triggerAt(_player.pos - Vec2(0, 6));
        _game->audio().playOnce(_player_shoot_sound);
    }

    // Update the bullet flight.
    if (_player.bulletPos)
    {
        auto newBulletPos = *_player.bulletPos;
        newBulletPos.y -= time.elapsed() * 110;

        const auto bulletRect = Rectangle(newBulletPos.x, newBulletPos.y, 4, 4);

        if (newBulletPos.y <= 0.0f)
        {
            // Bullet went out of the viewing area.
            _player.bulletPos = none;
        }
        else if (const auto enemy = checkCollisionWithEnemy(bulletRect))
        {
            // We hit the enemy. Damage it, remove it, and add some effects for player feedback.
            killEnemy(*enemy);

            _particle_systems.bulletImpact.triggerAt(newBulletPos);
            _game->audio().playOnce(*randomItem(_enemy_kill_sounds));
            _game->triggerCameraShake();

            // Setting the position to none "despawns" the bullet.
            _player.bulletPos = none;
        }
        else
        {
            // Bullet is still flying, so update its position.
            _player.bulletPos = newBulletPos;
        }
    }
}

void GameplayState::updateEnemies(const GameTime& time)
{
    const auto ddd = lerp(1.0f, 4.0f, 1.0f - float(double(_enemies.size()) / double(_level_enemy_count)));

    _time_to_next_enemy_move -= time.elapsed() * _enemy_sway_speed * ddd;

    if (_time_to_next_enemy_move <= 0.0f)
    {
        constexpr auto leftBorder    = sEnemySize.x;
        constexpr auto rightBorder   = sViewSize.x - sEnemySize.x;
        constexpr auto enemyAdvanceY = sEnemySize.y;

        for (auto& enemy : _enemies)
        {
            enemy.pos.x += float(_enemies_direction);

            const auto shouldAdvance =
                (_enemies_direction == 1 && enemy.pos.x >= rightBorder) || enemy.pos.x <= leftBorder;

            if (shouldAdvance)
            {
                for (auto& e : _enemies)
                {
                    e.pos.y += enemyAdvanceY;
                }

                _enemies_direction *= -1;
                break;
            }
        }

        _enemy_anim_idx          = (_enemy_anim_idx + 1) % 2;
        _time_to_next_enemy_move = 1.0f;
    }

    _time_to_next_enemy_bullet -= time.elapsed();

    if (_time_to_next_enemy_bullet <= 0.0f)
    {
        constexpr auto howManyShotsAtOnce = 4;

        for (int i = 0; i < howManyShotsAtOnce; ++i)
        {
            const auto enemy = randomItem(_enemies);

            // Time's up!
            _enemy_bullets.add(enemy->pos);
        }

        _game->audio().playOnce(_enemy_shoot_sound);

        // Determine the next time to shoot.
        _time_to_next_enemy_bullet = Random::nextFloat(sEnemyTimeToShootInterval);
    }

    // Update enemy bullet flights.
    for (auto& bullet : _enemy_bullets)
    {
        constexpr auto enemyBulletSpeed = 80.0f;
        bullet.y += time.elapsed() * enemyBulletSpeed;
    }

    // See if any bullets have to be removed now.
    _enemy_bullets.removeAllWhere(
        [this, playerRect = _player.rect()](Vec2 bulletPos)
        {
            if (bulletPos.y > sViewSize.y)
            {
                // Bullet flew out of the viewing area.
                return true;
            }

            const auto bulletRect = Rectangle(bulletPos, sEnemyBulletRect.size());

            if (bulletRect.intersects(playerRect))
            {
                // The bullet has hit the player.
                hitPlayer(bulletPos);
                return true;
            }

            // Bullet is still allowed to fly.
            return false;
        });
}

void GameplayState::spawnEnemies(StringView enemyRows)
{
    assume(_enemies.isEmpty());

    constexpr auto startY = 32.0f;

    const auto rows = Polly::splitString<List<String, 8>>(enemyRows, "\n");

    for (auto y = startY; const auto& row : rows)
    {
        constexpr auto gap      = 4.0f;
        const auto     rowWidth = (sEnemySize.x + gap) * float(row.size());
        const auto     offsetX  = (sEnemySize.x / 2) + ((sViewSize.x - rowWidth) / 2) + (gap / 2);

        for (auto x = 0.0f; const auto ch : row)
        {
            if (ch != '-')
            {
                const auto srcRectIdx = *StringView(&ch, 1).toUInt();

                _enemies.add(
                    Enemy{
                        .pos        = round(Vec2(x + offsetX, y)),
                        .srcRectIdx = srcRectIdx,
                    });
            }

            x += sEnemySize.x + gap;
        }

        y += sEnemySize.y + gap;
    }

    _level_enemy_count = _enemies.size();
}

void GameplayState::drawPlayer(Painter painter)
{
    painter.drawSprite(
        Sprite{
            .image   = _spritesheet,
            .dstRect = _player.rect(),
            .srcRect = sPlayerSpriteRects[_player.currentMoveDirection],
            .origin  = Vec2(8, 8) / 2,
        });

    if (_player.bulletPos)
    {
        const auto bulletSrcRect = *randomItem(sPlayerBulletSrcRects);

        painter.drawSprite(
            Sprite{
                .image   = _spritesheet,
                .dstRect = Rectangle(*_player.bulletPos, Vec2(8, 8)),
                .srcRect = bulletSrcRect,
                .origin  = Vec2(8, 8) / 2,
            });
    }
}

void GameplayState::drawEnemies(Painter painter)
{
    for (const auto& e : _enemies)
    {
        const auto srcRect = sEnemyRects[e.srcRectIdx][_enemy_anim_idx];

        painter.drawSprite(
            Sprite{
                .image   = _spritesheet,
                .dstRect = {e.pos, srcRect.size()},
                .srcRect = srcRect,
                .origin  = srcRect.size() / 2,
            });
    }

    for (const auto& bullet : _enemy_bullets)
    {
        painter.drawSprite(
            Sprite{
                .image   = _spritesheet,
                .dstRect = Rectangle(bullet, sEnemyBulletRect.size()),
                .srcRect = sEnemyBulletRect,
                .origin  = sEnemyBulletRect.size() / 2,
            });
    }
}

Maybe<GameplayState::Enemy&> GameplayState::checkCollisionWithEnemy(const Rectangle& rect)
{
    return findWhere(_enemies, [&rect](const auto& e) { return e.rect().intersects(rect); });
}

void GameplayState::killEnemy(const Enemy& enemy)
{
    _enemies.removeFirstWhere([&](const auto& e) { return &e == &enemy; });

    _score += enemy.scorePoints;
}

void GameplayState::hitPlayer(Vec2 pos)
{
    // TODO
}