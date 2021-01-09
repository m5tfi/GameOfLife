def check_left(int i, list state, int rows):
    if not i % rows == 0:
        return state[i - 1]
    return False

def check_right(int i, list state, int rows):
    if not i % rows == rows - 1:
        return state[i + 1]
    return False

def count_neighbours(int i, list state, int rows, int columns):
    cdef unsigned int neighbours = check_left(i, state, rows)
    neighbours += check_right(i, state, rows)

    # check top
    if i / rows >= 1:
        t = i - rows
        neighbours += state[t]
        neighbours += check_left(t, state, rows)
        neighbours += check_right(t, state, rows)

    if (i / rows) < (columns - 1):
        b = i + rows
        neighbours += state[b]
        neighbours += check_left(b, state, rows)
        neighbours += check_right(b, state, rows)

    return neighbours

def get_next_gen(list state, int rows, int columns):
    cdef list next_gen = []
    cdef unsigned int neighbours
    cdef unsigned int will_live
    for i, c in enumerate(state):
        neighbours = count_neighbours(i, state, rows, columns)
        will_live = 0
        if c:
            if 2 <= neighbours <= 3:
                will_live = 1
        else:
            if neighbours == 3:
                will_live = 1
        next_gen.append(will_live)
    return next_gen
