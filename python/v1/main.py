"""
--------------------------------------------------------------------------------
Python 3.9.0
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
from random import randint

import pygame

FPS = 120
SCREEN_WIDTH = 900
SCREEN_HEIGHT = 900

CELL_SIZE = 1
ROWS = SCREEN_WIDTH // CELL_SIZE
COLUMNS = SCREEN_HEIGHT // CELL_SIZE
CELL_COUNT = ROWS * COLUMNS

JUST_DIED_CELL_COLOR = pygame.Color('red')
LIVE_CELL_COLOR = pygame.Color('black')
JUST_BORN_CELL_COLOR = pygame.Color('green')
FONT_COLOR = pygame.Color('coral')

DRAW_COLORED = True


class Cell:
    __slots__ = ['x', 'y', 'prev_state', 'is_alive', 'next_state']

    def __init__(self, x: int, y: int, is_alive: bool):
        self.x = x
        self.y = y
        self.prev_state = is_alive
        self.is_alive = is_alive
        self.next_state = False

    def redraw(self, game_surface: pygame.Surface):
        color = None
        if not DRAW_COLORED and self.is_alive:
            color = LIVE_CELL_COLOR
        elif DRAW_COLORED:
            if self.prev_state and self.is_alive:
                color = LIVE_CELL_COLOR
            elif self.is_alive:
                color = JUST_BORN_CELL_COLOR
            elif self.prev_state:
                color = JUST_DIED_CELL_COLOR

        if color:
            pygame.draw.rect(game_surface, color,
                             (self.x, self.y, CELL_SIZE, CELL_SIZE))


class GameOfLive:
    def __init__(self):
        pygame.init()
        pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption('Conway\'s Game of Life')
        self.clock = pygame.time.Clock()

        self.font = pygame.font.SysFont('Arial', 27)
        self.game_is_running = True
        self.game_is_paused = False

        self.cells = []
        for y in range(COLUMNS):
            for x in range(ROWS):
                self.cells.append(
                    Cell(x * CELL_SIZE, y * CELL_SIZE, randint(0, 1) > 0.5))

    def _check_left(self, i: int) -> bool:
        if not i % ROWS == 0:
            return self.cells[i - 1].is_alive
        return False

    def _check_right(self, i: int) -> bool:
        if not i % ROWS == ROWS - 1:
            return self.cells[i + 1].is_alive
        return False

    def _count_live_neighbours(self, i: int) -> int:
        neighbours = self._check_left(i)
        neighbours += self._check_right(i)

        # check top
        if i / ROWS >= 1:
            t = i - ROWS
            neighbours += self.cells[t].is_alive
            neighbours += self._check_left(t)
            neighbours += self._check_right(t)

        # check bottom
        if (i / ROWS) < COLUMNS - 1:
            b = i + ROWS
            neighbours += self.cells[b].is_alive
            neighbours += self._check_left(b)
            neighbours += self._check_right(b)

        return neighbours

    def _update_cells(self):
        for i, c in enumerate(self.cells):
            neighbours = self._count_live_neighbours(i)
            will_live = False
            if c.is_alive:
                if 2 <= neighbours <= 3:
                    will_live = True
            else:
                if neighbours == 3:
                    will_live = True
            c.next_state = will_live

        for c in self.cells:
            c.prev_state = c.is_alive
            c.is_alive = c.next_state

    def _redraw_fps_txt(self, game_surface: pygame.Surface, fps: float):
        fps_str = str(int(fps))
        fps_txt = self.font.render(fps_str, True, FONT_COLOR)
        game_surface.blit(fps_txt, (5, 5))

    def _redraw(self):
        self.clock.tick(FPS)
        game_surface = pygame.display.get_surface()
        game_surface.fill((220, 220, 220))

        for c in self.cells:
            c.redraw(game_surface)

        self._redraw_fps_txt(game_surface, self.clock.get_fps())
        pygame.display.flip()

    def _reset_cells(self):
        for c in self.cells:
            c.is_alive = randint(0, 1)
            c.prev_state = c.is_alive

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


if __name__ == '__main__':
    GameOfLive().run()
