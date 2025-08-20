#include "Polly/Main.hpp"

#include "SpaceShooterGame.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Game::run<SpaceShooterGame>();
    return EXIT_SUCCESS;
}
