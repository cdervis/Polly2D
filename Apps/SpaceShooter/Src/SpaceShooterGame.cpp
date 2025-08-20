#include "SpaceShooterGame.hpp"

#include "Constants.hpp"
#include "SplashScreenState.hpp"

static const auto init_args = GameInitArgs{
    .title               = "Space Shooter Demo",
    .initialWindowSize = sViewSize * sWindowSizeMultiplier,
    .enableAudio        = true,
};

SpaceShooterGame::SpaceShooterGame()
    : Game(init_args)
{
    window().setIsResizable(false);

    _mainCanvas = Image(sViewSize.x, sViewSize.y, ImageFormat::R8G8B8A8UNorm);
    _nextState  = makeUnique<SplashScreenState>();
}

void SpaceShooterGame::update(GameTime time)
{
    if (_cameraShakeOffset and _drawnCameraShakeCount > 2)
    {
        _cameraShakeOffset = none;
    }

    while (_nextState)
    {
        _currentState = std::move(_nextState);
        _nextState    = {};

        _currentState->_game = this;
        _currentState->start();
    }

    _currentState->update(time);
}

void SpaceShooterGame::draw(GraphicsDevice gfx)
{
    const auto shouldDrawIntoLowResCanvas = _currentState->drawsIntoLowResCanvas();

    if (shouldDrawIntoLowResCanvas)
    {
        gfx.setCanvas(_mainCanvas, black);
        gfx.setSampler(pointClamp);
    }

    _currentState->draw(gfx);

    if (shouldDrawIntoLowResCanvas)
    {
        gfx.setCanvas(none, none);

        gfx.drawSprite(
            Sprite{
                .image    = _mainCanvas,
                .dstRect = {_cameraShakeOffset.valueOr(Vec2()), window().sizePx()},
                .srcRect = none,
                .color    = white,
                .rotation = 0.0_rad,
                .origin   = {},
                .flip     = SpriteFlip::None,
            });
    }

    if (_cameraShakeOffset)
    {
        ++_drawnCameraShakeCount;
        _cameraShakeOffset = -(*_cameraShakeOffset);
    }
}

void SpaceShooterGame::triggerCameraShake()
{
    _cameraShakeOffset      = Vec2(4, 4);
    _drawnCameraShakeCount = 0;
}