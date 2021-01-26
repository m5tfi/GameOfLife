#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>

class GOL {
 private:
  static constexpr int m_SCREEN_WIDTH{900};
  static constexpr int m_SCREEN_HEIGHT{900};
  static constexpr int m_CELL_SIZE{5};
  static constexpr int m_ROWS{m_SCREEN_WIDTH / m_CELL_SIZE};
  static constexpr int m_COLUMNS{m_SCREEN_HEIGHT / m_CELL_SIZE};
  static constexpr int m_CELL_COUNT{m_ROWS * m_COLUMNS};

  sf::RectangleShape m_rects[m_CELL_COUNT];
  bool m_prev_state[m_CELL_COUNT]{};
  bool m_current_state[m_CELL_COUNT]{};
  bool m_next_state[m_CELL_COUNT]{};

  bool m_draw_colored{true};
  bool m_is_paused{false};

  static bool get_random_bool()
  {
    static auto seed{std::mt19937::result_type(std::time(nullptr))};
    // static auto seed{std::mt19937::result_type(3)};  // for testing only
    static std::mt19937 mersenne{seed};
    static std::uniform_int_distribution rnd_bool{0, 1};
    return rnd_bool(mersenne);
  }

 public:
  const sf::Color COLOR_LIVE_CELL{sf::Color::Black};
  const sf::Color COLOR_DEAD_CELL{sf::Color::White};
  const sf::Color COLOR_JUST_DIED_CELL{sf::Color::Red};
  const sf::Color COLOR_JUST_BORN_CELL{sf::Color::Green};

 public:
  explicit GOL()
  {
    int i{0};
    for (int y{0}; y < m_COLUMNS; ++y) {
      for (int x{0}; x < m_ROWS; ++x) {
        m_rects[i] = sf::RectangleShape();
        m_rects[i].setSize(sf::Vector2((float)m_CELL_SIZE, (float)m_CELL_SIZE));
        m_rects[i].setPosition((float)x * m_CELL_SIZE, (float)y * m_CELL_SIZE);
        ++i;
      }
    }

    reset_cells();
  }

  static constexpr int get_screen_width() { return m_SCREEN_WIDTH; }
  static constexpr int get_screen_height() { return m_SCREEN_HEIGHT; }
  constexpr bool is_game_paused() const { return m_is_paused; }
  constexpr bool is_game_colored() const { return m_is_paused; }
  constexpr void toggle_game_paused() { m_is_paused = !m_is_paused; }
  constexpr void toggle_draw_colored() { m_draw_colored = !m_draw_colored; }

  void reset_cells()
  {
    for (int i{0}; i < m_CELL_COUNT; ++i) {
      m_prev_state[i] = false;
      m_current_state[i] = get_random_bool();
      m_next_state[i] = false;
    }
  }

  constexpr bool check_left(const int i)
  {
    if (i % m_ROWS != 0) return m_current_state[i - 1];
    return false;
  }

  constexpr bool check_right(const int i)
  {
    if (i % m_ROWS != m_ROWS - 1) return m_current_state[i + 1];
    return false;
  }

  constexpr int count_neighbours(const int i)
  {
    int neighbours{0}, t;

    neighbours += check_left(i);
    neighbours += check_right(i);

    if (i / m_ROWS >= 1) {  // check top
      t = i - m_ROWS;
      neighbours += m_current_state[t];
      neighbours += check_left(t);
      neighbours += check_right(t);
    }

    if (i / m_ROWS < m_COLUMNS - 1) {  // check bottom
      t = i + m_ROWS;
      neighbours += m_current_state[t];
      neighbours += check_left(t);
      neighbours += check_right(t);
    }

    return neighbours;
  }

  constexpr void update_cells()
  {
    for (int i{0}; i < m_CELL_COUNT; ++i) {
      int neighbours{count_neighbours(i)};
      if (neighbours == 3 || (neighbours == 2 && m_current_state[i]))
        m_next_state[i] = true;
      else
        m_next_state[i] = false;
    }

    for (int i{0}; i < m_CELL_COUNT; ++i) {
      m_prev_state[i] = m_current_state[i];
      m_current_state[i] = m_next_state[i];
      m_next_state[i] = false;
    }
  }

  constexpr void redraw_cells(sf::RenderWindow *window)
  {
    for (int i{0}; i < m_CELL_COUNT; ++i) {
      const sf::Color *cell_color{nullptr};
      if (!m_draw_colored && m_current_state[i]) {
        cell_color = &COLOR_LIVE_CELL;
      }
      else if (m_draw_colored) {
        if (m_prev_state[i] && m_current_state[i])
          cell_color = &COLOR_LIVE_CELL;
        else if (m_current_state[i])
          cell_color = &COLOR_JUST_BORN_CELL;
        else if (m_prev_state[i])
          cell_color = &COLOR_JUST_DIED_CELL;
      }
      if (cell_color) {
        m_rects[i].setFillColor(*cell_color);
        window->draw(m_rects[i]);
      }
    }
  }
};

void catch_events(sf::RenderWindow *window, GOL *gol)
{
  static sf::Event event{};
  while (window->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window->close();
    }
    if (event.type == sf::Event::KeyReleased) {
      if (event.key.code == sf::Keyboard::Escape) {
        std::cout << "Game is closed." << std::endl;
        window->close();
      }
      if (event.key.code == sf::Keyboard::R) {
        gol->reset_cells();
        std::cout << "Reset Gol!" << std::endl;
      }
      if (event.key.code == sf::Keyboard::P) {
        gol->toggle_game_paused();
        std::cout << "Game paused: " << gol->is_game_paused() << std::endl;
      }
      if (event.key.code == sf::Keyboard::Space && gol->is_game_paused()) {
        gol->update_cells();
      }
      if (event.key.code == sf::Keyboard::C) {
        gol->toggle_draw_colored();
        std::cout << "Draw colored: " << gol->is_game_colored() << std::endl;
      }
    }
  }
}

int main()
{
  auto gol{std::make_unique<GOL>()};

  sf::RenderWindow window(
      sf::VideoMode(GOL::get_screen_width(), GOL::get_screen_height()),
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

    catch_events(&window, gol.get());

    if (!gol->is_game_paused()) gol->update_cells();

    window.clear(gol->COLOR_DEAD_CELL);
    gol->redraw_cells(&window);
    window.display();
  }

  return EXIT_SUCCESS;
}
