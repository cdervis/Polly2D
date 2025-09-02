#include "SpaceShooter.hpp"

#include "Constants.hpp"
#include "SplashScreenState.hpp"

static const auto initArgs = GameInitArgs{
    .title             = "Space Shooter Demo",
    .initialWindowSize = sViewSize * sWindowSizeMultiplier,
    .enableAudio       = true,
};

SpaceShooter::SpaceShooter()
    : Game(initArgs)
{
    window().setIsResizable(false);
    window().setClearColor(black);

    _mainCanvas = Image(sViewSize.x, sViewSize.y, ImageFormat::R8G8B8A8UNorm);
    _nextState  = makeUnique<SplashScreenState>();
}

void SpaceShooter::update(GameTime time)
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

void SpaceShooter::draw(Painter painter)
{
    const auto shouldDrawIntoLowResCanvas = _currentState->drawsIntoLowResCanvas();

    if (shouldDrawIntoLowResCanvas)
    {
        painter.setCanvas(_mainCanvas, black);
        painter.setSampler(pointClamp);
    }

    _currentState->draw(painter);

    if (shouldDrawIntoLowResCanvas)
    {
        painter.setCanvas(none, none);

        painter.drawSprite(
            Sprite{
                .image    = _mainCanvas,
                .dstRect  = {_cameraShakeOffset.valueOr(Vec2()), window().sizePx()},
                .srcRect  = none,
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

void SpaceShooter::triggerCameraShake()
{
    _cameraShakeOffset     = Vec2(4, 4);
    _drawnCameraShakeCount = 0;
}