# Features
- 10x10 board representation for quick out-of-bounds checks implemented with a one-dimensional array for performance
- Bug-free pseudo-legal move generation which implements [Zobrist hashing](https://www.chessprogramming.org/Zobrist_Hashing) for a fast transpositional table
- Multi-threaded [perft function](https://www.chessprogramming.org/Perft) which achieves over 2,000,000,000 nodes per second on an Intel Core i7-8700K
- Basic search function with [alpha-beta pruning](https://www.chessprogramming.org/Alpha-Beta) and full [quiescence search](https://www.chessprogramming.org/Quiescence_Search)
- Evaluation function with [piece-square tables](https://www.chessprogramming.org/Piece-Square_Tables) and [mobility](https://www.chessprogramming.org/Mobility) evaluation
- Simple GUI program for debugging and playing against CChess
- Small code size with the entire engine fitting into 2 files

# Building
A prebuilt version for 64-bit Windows can be found [here](https://drive.google.com/file/d/1SBJqhRCkqKmAC0Je1vTg0PrPl5ksx__K/view?usp=sharing).

To build CChess for other platforms, SDL 2.0 or above and SDL_image 2.0 or above are required. Some code modification may be necessary in order to avoid some warnings/errors. Optimization flag (-O3) is highly recommended if building with gcc.

# Downsides
- Perhaps the biggest issue with CChess is the board representation. Even just changing it to a 12x12 board representation would get rid of any unnecessary out-of-bounds checks and slightly speed up move generation. The best choice would be to switch to bitboards and magic bitboards for move generation, but it would require rewriting the majority of the engine.
- The search function is still in heavy development and there are a ton of optimizations yet to be implemented. The evaluation function can also be improved a lot as it doesn't consider many aspects of the position yet.
