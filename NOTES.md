# Notes

## Python

- Using `__slots__` in `Cell` class reduced the memory usage, while keeping the
  screen size constant at `900 x 900` and only changing the `CELL_SIZE`:
    - `CELL_SIZE = 5` the memory usage went from `25.7 MiB` to `23.3 MiB`.
    - `CELL_SIZE = 2` the memory usage went from `60.9 MiB` to `45.4 MiB`.
    - `CELL_SIZE = 1` the memory usage went from `191.7 MiB` to `124.4 MiB`.

- Using `Cython`
  in [v2](https://github.com/m5tfi/GameOfLife/blob/master/python/v2) increased
  the performance almost the double, compared to
  [v1](https://github.com/m5tfi/GameOfLife/blob/master/python/v1).
    - `CELL_SIZE = 5` the fps went from `~10fps` to `~25fps`. I don't think I am
      taking the full advantage of `Cython` yet.
    - `CELL_SIZE = 1` the memory usage went from `124.4 Mib` to `46.5 MiB`. I
      think that's because storing the states in list is cheaper than a list of
      objects.
- More refactoring the `Cython` code part in
  [v3](https://github.com/m5tfi/GameOfLife/blob/master/python/v3).
    - Calling a method or function is costly, after refactoring `check_left()`
      and `check_right()` to be inside the `count_neighbours()` functions, it
      increased performance greatly.
    - Communicating between Cython and Python slows Cython a lot, when the 
      communication is reduced, it improved the performance.
    - The python stub files can help for the IDE's autocomplete and linter.
    - Performance:
        - `CELL_SIZE = 5` the fps went from `~25fps` to `>120fps`.
        - `CELL_SIZE = 2` the fps went from `<1fps` to `~30fps`.
    
  