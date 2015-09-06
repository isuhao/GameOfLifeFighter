#include "qtgameoflifefighterwidget.h"

#include <cassert>
#include <iostream>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QKeyEvent>
#include <QDesktopWidget>

#include "qtgameoflifefighterplayerindex.h"
#include "gameoflifefightertrace.h"
#include "gameoflifefighterplayerindex.h"
#include "ui_qtgameoflifefighterwidget.h"

golf::QtGameOfLifeFighterWidget::QtGameOfLifeFighterWidget(
  const int width,
  const int height,
  QWidget *parent
)
  : QWidget(parent),
    ui(new Ui::QtGameOfLifeFighterWidget),
    m_game{},
    m_key_map{CreateInitialKeyMap()},
    m_keys{},
    m_pixmap(width,height)
{
  #ifndef NDEBUG
  Test();
  #endif
  ui->setupUi(this);
  OnTimer();
  {
    //Put the dialog in the screen center
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->setGeometry(0,0,screen.width() * 20 / 24,screen.height() * 8 / 24);
    this->move( screen.center() - this->rect().center() );
  }
  //Start a timer
  {
    QTimer * const timer{new QTimer(this)};
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(OnTimer()));
    timer->setInterval(100);
    timer->start();
  }
}

golf::QtGameOfLifeFighterWidget::~QtGameOfLifeFighterWidget()
{
  delete ui;
}

QColor golf::QtGameOfLifeFighterWidget::Blend(
  const QColor a,
  const QColor b
)
{
  return qRgb(
    (a.red() + b.red()) / 2,
    (a.green() + b.green()) / 2,
    (a.blue() + b.blue()) / 2
  );
}


void golf::QtGameOfLifeFighterWidget::Blend(
  QImage& image,
  const int x, const int y,
  const QColor color
)
{
  Blend(image,x,y,color.red(),color.green(),color.blue());
}

void golf::QtGameOfLifeFighterWidget::Blend(
  QImage& image,
  const int x, const int y,
  const int r, const int g, const int b
)
{
  const QColor here{image.pixel(x,y)};
  image.setPixel(
    x,y,
    qRgb(
      (here.red()   + r) / 2,
      (here.green() + g) / 2,
      (here.blue() + b) / 2
    )
  );
}

std::map<int,golf::Key> golf::QtGameOfLifeFighterWidget::CreateInitialKeyMap() noexcept
{
  std::map<int,Key> m;
  m.insert( { Qt::Key_1, Key::pattern_a1}  );
  m.insert( { Qt::Key_2, Key::pattern_b1}  );
  m.insert( { Qt::Key_3, Key::pattern_c1}  );

  m.insert( { Qt::Key_Q, Key::close_hangar1}  );
  m.insert( { Qt::Key_W, Key::up1}  );
  m.insert( { Qt::Key_E, Key::set_high1}  );

  m.insert( { Qt::Key_A, Key::left1}  );
  m.insert( { Qt::Key_S, Key::down1}  );
  m.insert( { Qt::Key_D, Key::right1}  );

  m.insert( { Qt::Key_Z, Key::open_hangar1}  );
  m.insert( { Qt::Key_X, Key::down1}  );
  m.insert( { Qt::Key_C, Key::set_low1}  );

  m.insert( { Qt::Key_7, Key::pattern_a2}  );
  m.insert( { Qt::Key_8, Key::pattern_b2}  );
  m.insert( { Qt::Key_9, Key::pattern_c2}  );

  m.insert( { Qt::Key_U, Key::close_hangar2}  );
  m.insert( { Qt::Key_I, Key::up2}  );
  m.insert( { Qt::Key_O, Key::set_high2}  );

  m.insert( { Qt::Key_J, Key::left2}  );
  m.insert( { Qt::Key_K, Key::down2}  );
  m.insert( { Qt::Key_L, Key::right2}  );

  m.insert( { Qt::Key_M, Key::open_hangar2}  );
  m.insert( { Qt::Key_Comma, Key::down2}  );
  m.insert( { Qt::Key_Period, Key::set_low2}  );

  return m;
}

void golf::QtGameOfLifeFighterWidget::keyPressEvent(QKeyEvent * e)
{
  const auto iter = m_key_map.find(e->key());
  if (iter == std::end(m_key_map)) return;
  m_keys.insert( (*iter).second );
}

void golf::QtGameOfLifeFighterWidget::keyReleaseEvent(QKeyEvent * e)
{
  const auto iter = m_key_map.find(e->key());
  if (iter == std::end(m_key_map)) return;
  m_keys.erase( (*iter).second );
}

void golf::QtGameOfLifeFighterWidget::OnTimer()
{
  m_game.Next();
  m_game.PressKeys(m_keys);
  const int height{m_pixmap.height()};
  const int width{m_pixmap.width()};
  QImage image(width,height,QImage::Format_RGB32);

  //Display grid
  for (int y=0; y!=height; ++y)
  {
    for (int x=0; x!=width; ++x)
    {
      const auto i = m_game.GetCell(x,y);
      image.setPixel(x,y,
        i == CellType::empty ? qRgb(0,0,0) : qRgb(255,255,255)
      );
    }
  }
  //Display hangars
  for (const Hangar& hangar: m_game.GetHangars())
  {
    const int left{hangar.GetLeft()};
    const int top{hangar.GetTop()};
    const int width{hangar.GetWidth()};
    const int height{hangar.GetHeight()};
    const auto player_index = hangar.GetPlayerIndex();
    //Darker if the Hangar is open
    const auto player_color = ToColor(player_index);
    const auto hangar_color = hangar.GetState() == HangarState::open
      ? Blend(player_color,qRgb(0,0,0))
      : player_color
    ;
    for (int y=0; y!=height; ++y)
    {
      for (int x=0; x!=width; ++x)
      {
        auto color = hangar_color;
        if (hangar.GetCell(x,y) == CellType::alive)
        {
          color = Blend(color,qRgb(255,255,255));
        }
        Blend(image,x+left,y+top,color);
      }
    }

  }

  //Display hearts
  for (const Heart& heart: m_game.GetHearts())
  {
    const int left{heart.GetLeft()};
    const int top{heart.GetTop()};
    const int width{heart.GetWidth()};
    const int height{heart.GetHeight()};
    const auto player_index = heart.GetPlayerIndex();
    //Darker if the Hangar is open
    const auto hangar_color = ToColor(player_index);
    for (int y=0; y!=height; ++y)
    {
      for (int x=0; x!=width; ++x)
      {
        Blend(image,x+left,y+top,hangar_color);
      }
    }

  }

  //Display players
  for (const auto player_index: { PlayerIndex::player1, PlayerIndex::player2 } )
  {
    const auto player = m_game.GetPlayer(player_index);
    Blend(image,player.GetX(),player.GetY(),ToColor(player_index));
  }

  m_pixmap = QPixmap::fromImage(image);
  update(); //Essential
}

void golf::QtGameOfLifeFighterWidget::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.drawPixmap(
    rect(),
    m_pixmap
  );
}

QKeyEvent CreateDel() { return QKeyEvent(QEvent::KeyPress,Qt::Key_Delete,Qt::NoModifier); }
QKeyEvent CreateK() { return QKeyEvent(QEvent::KeyPress,Qt::Key_K,Qt::NoModifier); }
QKeyEvent CreateS() { return QKeyEvent(QEvent::KeyPress,Qt::Key_S,Qt::NoModifier); }
QKeyEvent CreateSpace() { return QKeyEvent(QEvent::KeyPress,Qt::Key_Space,Qt::NoModifier); }
QKeyEvent CreateDown() { return QKeyEvent(QEvent::KeyPress,Qt::Key_Down,Qt::NoModifier); }
QKeyEvent CreateControlDown() { return QKeyEvent(QEvent::KeyPress,Qt::Key_Down,Qt::ControlModifier); }
QKeyEvent CreateControlE() { return QKeyEvent(QEvent::KeyPress,Qt::Key_E,Qt::ControlModifier); }
QKeyEvent CreateControlN() { return QKeyEvent(QEvent::KeyPress,Qt::Key_N,Qt::ControlModifier); }

void golf::QtGameOfLifeFighterWidget::Test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }

  //A key 'S' press should move player 1 down
  {
    QtGameOfLifeFighterWidget w;
    auto down = CreateS();
    const int y_before{w.m_game.GetPlayer(PlayerIndex::player1).GetY()};
    w.keyPressEvent(&down);
    w.OnTimer();
    const int y_after{w.m_game.GetPlayer(PlayerIndex::player1).GetY()};
    assert(y_after == y_before + 1);
  }
  //A key 'K' (arrows do not do anything) press should move player 2 down
  {
    QtGameOfLifeFighterWidget w;
    auto down = CreateK();
    const int y_before{w.m_game.GetPlayer(PlayerIndex::player2).GetY()};
    w.keyPressEvent(&down);
    w.OnTimer();
    const int y_after{w.m_game.GetPlayer(PlayerIndex::player2).GetY()};
    assert(y_after == y_before + 1);
  }
}
