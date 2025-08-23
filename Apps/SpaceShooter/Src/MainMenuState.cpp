#include "MainMenuState.hpp"

#include "GameplayState.hpp"
#include "SpaceShooter.hpp"

void MainMenuState::start()
{
    _game->switchToState<GameplayState>();
}

void MainMenuState::update(GameTime time)
{
}

void MainMenuState::draw(Painter painter)
{
}