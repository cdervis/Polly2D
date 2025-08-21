#include "SplashScreenState.hpp"

#include "Constants.hpp"
#include "MainMenuState.hpp"
#include "SpaceShooterGame.hpp"

SplashScreenState::SplashScreenState()
    : _logoTweenChain(
          Array{
              sSplashLogoEntryTweener, // First, the logo appears
              sSplashLogoExitTweener, // Then it disappears
              sSplashBackgroundTweener, // Then the background color turns from white to black
          })
    , _logo("logo-splash.png")
{
}

void SplashScreenState::start()
{
    if (sSkipSplashScreen)
    {
        goToNextState();
    }
}

void SplashScreenState::update(GameTime time)
{
    if (wasKeyJustPressed(Array{Scancode::Return, Scancode::Space}))
    {
        goToNextState();
    }

    _logoTweenChain.update(time.elapsed());

    if (_logoTweenChain.hasEnded())
    {
        goToNextState();
    }
}

void SplashScreenState::draw(Painter painter)
{
    const auto isBackgroundFading = _logoTweenChain.currentTweenerIndex() == 2;
    auto       backgroundColor     = white;

    if (isBackgroundFading)
    {
        backgroundColor = lerp(white, black, _logoTweenChain.currentTweener().percentage());
    }

    painter.setCanvas({}, backgroundColor);

    if (not isBackgroundFading)
    {
        painter.drawSprite(
            _logo,
            (painter.viewSize() - _logo.size()) / 2,
            white.withAlpha(_logoTweenChain.value()));
    }
}

bool SplashScreenState::drawsIntoLowResCanvas() const
{
    return false;
}

void SplashScreenState::goToNextState()
{
    _game->switchToState<MainMenuState>();
}
