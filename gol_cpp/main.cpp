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

sf::RectangleShape g_rects[CELL_COUNT];
bool g_prev_state[CELL_COUNT]{};
bool g_current_state[CELL_COUNT]{};
bool g_next_state[CELL_COUNT]{};

bool g_draw_colored{true};
bool g_is_paused{false};
int g_counter{10};

bool get_random_bool()
{
  return std::rand() > RAND_MAX / 2;
}

void reset_cells()
{
  for (int i{0}; i < CELL_COUNT; ++i) {
    g_prev_state[i] = false;
    g_current_state[i] = get_random_bool();
    g_next_state[i] = false;
  }
}

void init_rects()
{
  int i{0};
  for (int y{0}; y < COLUMNS; ++y) {
    for (int x{0}; x < ROWS; ++x) {
      g_rects[i] = sf::RectangleShape();
      g_rects[i].setSize(sf::Vector2((float) CELL_SIZE, (float) CELL_SIZE));
      g_rects[i].setPosition((float) x * CELL_SIZE, (float) y * CELL_SIZE);
      ++i;
    }
  }
}

bool check_left(int i)
{
  if (i % ROWS != 0)
    return g_current_state[i - 1];
  return false;
}

bool check_right(int i)
{
  if (i % ROWS != ROWS - 1)
    return g_current_state[i + 1];
  return false;
}

int count_neighbours(int i)
{
  int neighbours{0}, t;

  neighbours += check_left(i);
  neighbours += check_right(i);

  if (i / ROWS >= 1) {// check top
    t = i - ROWS;
    neighbours += g_current_state[t];
    neighbours += check_left(t);
    neighbours += check_right(t);
  }

  if (i / ROWS < COLUMNS - 1) {// check bottom
    t = i + ROWS;
    neighbours += g_current_state[t];
    neighbours += check_left(t);
    neighbours += check_right(t);
  }

  return neighbours;
}

void update_cells()
{
  for (int i{0}; i < CELL_COUNT; ++i) {
    int neighbours{count_neighbours(i)};
    if (neighbours == 3 || (neighbours == 2 && g_current_state[i]))
      g_next_state[i] = true;
    else
      g_next_state[i] = false;
  }

  for (int i{0}; i < CELL_COUNT; ++i) {
    g_prev_state[i] = g_current_state[i];
    g_current_state[i] = g_next_state[i];
    g_next_state[i] = false;
  }
}

void redraw_cells(sf::RenderWindow *window)
{
  for (int i{0}; i < CELL_COUNT; ++i) {
    const sf::Color *cell_color{nullptr};
    if (!g_draw_colored) {
      if (g_current_state[i])
        cell_color = &COLOR_LIVE_CELL;
    }
    else {
      if (g_prev_state[i] && g_current_state[i])
        cell_color = &COLOR_LIVE_CELL;
      else if (g_current_state[i])
        cell_color = &COLOR_JUST_BORN_CELL;
      else if (g_prev_state[i])
        cell_color = &COLOR_JUST_DIED_CELL;
    }
    if (cell_color) {
      g_rects[i].setFillColor(*cell_color);
      window->draw(g_rects[i]);
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
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
    if (g_counter % 100 == 0)
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

    //    window.clear();
    window.clear(COLOR_DEAD_CELL);
    redraw_cells(&window);
    window.display();
  }

  return EXIT_SUCCESS;
}
