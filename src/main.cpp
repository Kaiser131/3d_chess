#include "ChessGame.h"

#include <cstdlib>

int main() {
  ChessGame game;
  if (!game.init()) return EXIT_FAILURE;
  game.run();
  game.shutdown();
  return EXIT_SUCCESS;
}
