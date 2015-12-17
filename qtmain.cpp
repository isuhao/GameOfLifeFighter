#include <QApplication>
#include <cassert>
#include "qtgameoflifefighterwidget.h"
#include "gameoflifefightertrace.h"

#include <SFML/Audio/Music.hpp>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  START_TRACE();

  sf::Music music;
  const bool can_open{music.openFromFile("../GameOfLifeFighter/Resources/Music/GameOfDeath.ogg")};
  assert(can_open);
  music.play();

  golf::QtGameOfLifeFighterWidget w;
  w.show();
  return a.exec();
}
