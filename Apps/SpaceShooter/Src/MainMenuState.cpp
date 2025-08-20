#include "MainMenuState.hpp"

#include "GameplayState.hpp"
#include "SpaceShooterGame.hpp"

void MainMenuState::start()
{
    _game->switchToState<GameplayState>();
}

void MainMenuState::update(GameTime time)
{
}

void MainMenuState::draw(GraphicsDevice gfx)
{
}