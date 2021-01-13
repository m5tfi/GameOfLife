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

import pygame

pyximport.install(language_level='3str')
import gol

FPS = 240

COLOR_FONT = pygame.Color("coral")


class GameOfLive:
    def __init__(self):
        pygame.init()
        pygame.display.set_mode(gol.get_screen_size())
        pygame.display.set_caption("Conway's Game of Life")
        self.clock = pygame.time.Clock()

        self.font = pygame.font.SysFont("Arial", 27)
        self.game_is_running = True
        self.game_is_paused = False
        self.draw_colored = True

        gol.reset_cells()

    def _redraw_fps_txt(self, game_surface: pygame.Surface, fps: float):
        fps_str = str(int(fps))
        fps_txt = self.font.render(fps_str, True, COLOR_FONT)
        game_surface.blit(fps_txt, (5, 5))

    def _redraw(self):
        self.clock.tick(FPS)
        game_surface = pygame.display.get_surface()

        gol.redraw_cells(game_surface, self.draw_colored)

        self._redraw_fps_txt(game_surface, self.clock.get_fps())
        pygame.display.flip()

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
                    gol.update_cells()
                if event.key == pygame.K_r:
                    gol.reset_cells()
                if event.key == pygame.K_c:
                    self.draw_colored = not self.draw_colored

    def run(self):
        while self.game_is_running:
            self._catch_events()
            if not self.game_is_paused:
                gol.update_cells()
            self._redraw()


if __name__ == "__main__":
    GameOfLive().run()
