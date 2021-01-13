#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

inline constexpr int SCREEN_WIDTH{900};
inline constexpr int SCREEN_HEIGHT{900};
inline constexpr int CELL_SIZE{5};
inline constexpr int ROWS{SCREEN_WIDTH / CELL_SIZE};
inline constexpr int COLUMNS{SCREEN_HEIGHT / CELL_SIZE};
inline constexpr int CELL_COUNT{ROWS * COLUMNS};

inline const sf::Color COLOR_LIVE_CELL{sf::Color::Black};
inline const sf::Color COLOR_DEAD_CELL{sf::Color::White};
inline const sf::Color COLOR_JUST_DIED_CELL{sf::Color::Red};
inline const sf::Color COLOR_JUST_BORN_CELL{sf::Color::Green};

sf::RectangleShape g_rects[ROWS][COLUMNS];
bool g_prev_state[ROWS][COLUMNS]{};
bool g_current_state[ROWS][COLUMNS]{};
bool g_next_state[ROWS][COLUMNS]{};

bool g_draw_colored{true};
bool g_is_paused{false};

bool get_random_bool()
{
  return std::rand() > RAND_MAX / 2;
}

void reset_cells()
{
  for (int y{0}; y < COLUMNS; ++y) {
    for (int x{0}; x < ROWS; ++x) {
      g_prev_state[x][y] = false;
      g_current_state[x][y] = get_random_bool();
      g_next_state[x][y] = false;
    }
  }
}

void init_rects()
{
  for (int y{0}; y < COLUMNS; ++y) {
    for (int x{0}; x < ROWS; ++x) {
      g_rects[x][y] = sf::RectangleShape();
      g_rects[x][y].setSize(sf::Vector2((float) CELL_SIZE, (float) CELL_SIZE));
      g_rects[x][y].setPosition((float) x * CELL_SIZE, (float) y * CELL_SIZE);
    }
  }
}

bool check_left(int x, int y)
{
  if (x != 0)
    return g_current_state[x - 1][y];
  return false;
}

bool check_right(int x, int y)
{
  if (x < ROWS)
    return g_current_state[x + 1][y];
  return false;
}

int count_neighbours(int x, int y)
{
  int neighbours{0}, t;

  neighbours += check_left(x, y);
  neighbours += check_right(x, y);

  if (y > 0) {// check top
    neighbours += g_current_state[x][y - 1];
    neighbours += check_left(x, y - 1);
    neighbours += check_right(x, y - 1);
  }

  if (y < COLUMNS) {// check bottom
    neighbours += g_current_state[x][y + 1];
    neighbours += check_left(x, y + 1);
    neighbours += check_right(x, y + 1);
  }

  return neighbours;
}

void update_cells()
{
  for (int y{0}; y < COLUMNS; ++y) {
    for (int x{0}; x < ROWS; ++x) {
      int neighbours{count_neighbours(x, y)};
      if (neighbours == 3 || (neighbours == 2 && g_current_state[x][y]))
        g_next_state[x][y] = true;
      else
        g_next_state[x][y] = false;
    }
  }

  for (int y{0}; y < COLUMNS; ++y) {
    for (int x{0}; x < ROWS; ++x) {
      g_prev_state[x][y] = g_current_state[x][y];
      g_current_state[x][y] = g_next_state[x][y];
      g_next_state[x][y] = false;
    }
  }
}

void redraw_cells(sf::RenderWindow *window)
{
  for (int y{0}; y < COLUMNS; ++y) {
    for (int x{0}; x < ROWS; ++x) {
      const sf::Color *cell_color{nullptr};
      if (!g_draw_colored) {
        if (g_current_state[x][y])
          cell_color = &COLOR_LIVE_CELL;
      }
      else {
        if (g_prev_state[x][y] && g_current_state[x][y])
          cell_color = &COLOR_LIVE_CELL;
        else if (g_current_state[x][y])
          cell_color = &COLOR_JUST_BORN_CELL;
        else if (g_prev_state[x][y])
          cell_color = &COLOR_JUST_DIED_CELL;
      }
      if (cell_color) {
        g_rects[x][y].setFillColor(*cell_color);
        window->draw(g_rects[x][y]);
      }
    }
  }
}

void catch_events(sf::RenderWindow *window)
{
  static sf::Event event{};
  while (window->pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      window->close();
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
    std::cout << "Game is closed." << std::endl;
    window->close();
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
    reset_cells();
    std::cout << "Reset Gol!" << std::endl;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
    g_is_paused = !g_is_paused;
    std::cout << "Game is paused: " << g_is_paused << std::endl;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && g_is_paused) {
    update_cells();
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
    g_draw_colored = !g_draw_colored;
    std::cout << "Draw colored: " << g_draw_colored << std::endl;
  }
}

int main()
{
  // Setting random seed based on current time
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  reset_cells();

  init_rects();

  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
                          "Game of life");

  sf::Clock clock{sf::Clock()};
  sf::Time last_time{clock.getElapsedTime()};
  sf::Time current_time{};
  float fps;

  while (window.isOpen()) {
    current_time = clock.getElapsedTime();
    fps = 1.f / (current_time.asSeconds() - last_time.asSeconds());
    last_time = current_time;
    window.setTitle("GameOfLife, fps:" + std::to_string(fps));

    catch_events(&window);

    if (!g_is_paused)
      update_cells();

    window.clear(COLOR_DEAD_CELL);
    redraw_cells(&window);
    window.display();
  }

  return EXIT_SUCCESS;
}
