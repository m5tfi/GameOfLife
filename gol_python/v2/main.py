"""
--------------------------------------------------------------------------------
Python 3.9.0
Cython 0.29.21
pygame 2.0.1
--------------------------------------------------------------------------------
Controls:
        c -> toggle state colors:
                red = just died
                green = new born
                black = no change
        r -> resets the game.
        p -> toggle game pause.
    space -> get the next generation when the game is paused, otherwise,
             it's done automatically.
--------------------------------------------------------------------------------
Reference:
    https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
    1. Any live cell with fewer than two live neighbours dies, as if by
       underpopulation.
    2. Any live cell with two or three live neighbours lives on to the next
       generation.
    3. Any live cell with more than three live neighbours dies, as if by
       overpopulation.
    4. Any dead cell with exactly three live neighbours becomes a live cell,
       as if by reproduction.
--------------------------------------------------------------------------------
"""
import pyximport
from random import randint

import pygame

pyximport.install(language_level='3str')
import gol2 as gol

FPS = 120
SCREEN_WIDTH = 900
SCREEN_HEIGHT = 900

CELL_SIZE = 5
ROWS = SCREEN_WIDTH // CELL_SIZE
COLUMNS = SCREEN_HEIGHT // CELL_SIZE
CELL_COUNT = ROWS * COLUMNS

JUST_DIED_CELL_COLOR = pygame.Color("red")
LIVE_CELL_COLOR = pygame.Color("black")
JUST_BORN_CELL_COLOR = pygame.Color("green")
FONT_COLOR = pygame.Color("coral")

DRAW_COLORED = True


class GameOfLive:
    def __init__(self):
        pygame.init()
        pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Conway's Game of Life")
        self.clock = pygame.time.Clock()

        self.font = pygame.font.SysFont("Arial", 27)
        self.game_is_running = True
        self.game_is_paused = False

        self.prev_state = [0] * ROWS * COLUMNS
        self.current_state = [0] * ROWS * COLUMNS

        self._reset_cells()

    def _update_cells(self):
        for i in range(CELL_COUNT):
            self.prev_state[i] = self.current_state[i]

        self.current_state = gol.get_next_gen(self.current_state, ROWS, COLUMNS)

    def _redraw_cells(self, game_surface: pygame.Surface):
        for i in range(CELL_COUNT):
            color = None
            if not DRAW_COLORED:
                if self.current_state[i]:
                    color = LIVE_CELL_COLOR
            else:
                if self.prev_state[i] and self.current_state[i]:
                    color = LIVE_CELL_COLOR
                elif self.current_state[i]:
                    color = JUST_BORN_CELL_COLOR
                elif self.prev_state[i]:
                    color = JUST_DIED_CELL_COLOR

            if color:
                x = i % ROWS
                y = (i - x) // ROWS
                pygame.draw.rect(
                    game_surface,
                    color,
                    (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE),
                )

    def _redraw_fps_txt(self, game_surface: pygame.Surface, fps: float):
        fps_str = str(int(fps))
        fps_txt = self.font.render(fps_str, True, FONT_COLOR)
        game_surface.blit(fps_txt, (5, 5))

    def _redraw(self):
        self.clock.tick(FPS)
        game_surface = pygame.display.get_surface()
        game_surface.fill((220, 220, 220))

        self._redraw_cells(game_surface)

        self._redraw_fps_txt(game_surface, self.clock.get_fps())
        pygame.display.flip()

    def _reset_cells(self):
        for i in range(CELL_COUNT):
            self.prev_state[i] = randint(0, 1)
            self.current_state[i] = self.prev_state[i]

    def _catch_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.game_is_running = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    self.game_is_running = False
                if event.key == pygame.K_p:
                    self.game_is_paused = not self.game_is_paused
                if event.key == pygame.K_SPACE and self.game_is_paused:
                    self._update_cells()
                if event.key == pygame.K_r:
                    self._reset_cells()
                if event.key == pygame.K_c:
                    global DRAW_COLORED
                    DRAW_COLORED = not DRAW_COLORED

    def run(self):
        while self.game_is_running:
            self._catch_events()
            if not self.game_is_paused:
                self._update_cells()
            self._redraw()


if __name__ == "__main__":
    GameOfLive().run()
