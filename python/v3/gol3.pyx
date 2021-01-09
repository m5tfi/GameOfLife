from __future__ import print_function
from random import randint

import pygame

DEF SCREEN_WIDTH = 900
DEF SCREEN_HEIGHT = 900
DEF CELL_SIZE = 5
DEF ROWS = SCREEN_WIDTH // CELL_SIZE
DEF COLUMNS = SCREEN_HEIGHT // CELL_SIZE
DEF CELL_COUNT = ROWS * COLUMNS

cdef bint g_prev_state[CELL_COUNT]
cdef bint g_current_state[CELL_COUNT]
cdef bint g_next_state[CELL_COUNT]

g_game_surface = pygame.display.get_surface()

COLOR_LIVE_CELL = pygame.Color('black')
COLOR_DEAD_CELL = pygame.Color('white')
COLOR_JUST_DIED_CELL = pygame.Color('red')
COLOR_JUST_BORN_CELL = pygame.Color('green')

def get_screen_size():
    return SCREEN_WIDTH, SCREEN_HEIGHT

def count_neighbours(int i):
    cdef unsigned int t
    cdef unsigned int neighbours = 0

    # check left
    if not i % ROWS == 0:
        neighbours += g_current_state[i - 1]
    # check right
    if not i % ROWS == ROWS - 1:
        neighbours += g_current_state[i + 1]

    # check top
    if i / ROWS >= 1:
        t = i - ROWS
        neighbours += g_current_state[t]
        # top-left
        if not t % ROWS == 0:
            neighbours += g_current_state[t - 1]
        # top-right
        if not t % ROWS == ROWS - 1:
            neighbours += g_current_state[t + 1]

    # check bottom
    if i / ROWS < COLUMNS - 1:
        t = i + ROWS
        neighbours += g_current_state[t]
        # bottom-left
        if not t % ROWS == 0:
            neighbours += g_current_state[t - 1]
        # bottom-right
        if not t % ROWS == ROWS - 1:
            neighbours += g_current_state[t + 1]

    return neighbours

def update_cells():
    cdef unsigned int neighbours
    cdef unsigned int will_live
    for i in range(CELL_COUNT):
        neighbours = count_neighbours(i)
        will_live = 0
        if g_current_state[i]:
            if 2 <= neighbours <= 3:
                will_live = 1
        else:
            if neighbours == 3:
                will_live = 1
        g_next_state[i] = will_live

    for i in range(CELL_COUNT):
        g_prev_state[i] = g_current_state[i]
        g_current_state[i] = g_next_state[i]
        g_next_state[i] = False

def redraw_cells(game_surface, bint draw_colored):
    game_surface.fill(COLOR_DEAD_CELL)
    cdef int x = 0, y = 0
    for i in range(CELL_COUNT):
        color = None
        if not draw_colored:
            if g_current_state[i]:
                color = COLOR_LIVE_CELL
        else:
            if g_prev_state[i] and g_current_state[i]:
                color = COLOR_LIVE_CELL
            elif g_current_state[i]:
                color = COLOR_JUST_BORN_CELL
            elif g_prev_state[i]:
                color = COLOR_JUST_DIED_CELL

        if color:
            x = i % ROWS
            y = (i - x) // ROWS
            pygame.draw.rect(
                game_surface, color,
                (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE)
            )

def reset_cells():
    for i in range(CELL_COUNT):
        g_prev_state[i] = False
        g_current_state[i] = randint(0, 1)
