
/* cchess.h
 * A powerful engine written by a powerful person.
 */
#include <stdbool.h>
#include <pthread.h>

#define ZOBRIST_EN_PASSANT 1500
#define ZOBRIST_COLOR 1600
#define ZOBRIST_WHITE_KINGSIDE_CASTLE 1601
#define ZOBRIST_WHITE_QUEENSIDE_CASTLE 1602
#define ZOBRIST_BLACK_KINGSIDE_CASTLE 1603
#define ZOBRIST_BLACK_QUEENSIDE_CASTLE 1604

enum piece
{
    BLACK_KING = -7,
    BLACK_QUEEN,
    BLACK_ROOK,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_PAWN,
    OUT_OF_BOUNDS,
    EMPTY,
    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING
};

typedef enum
{
    NONE,
    KINGSIDE_CASTLE,
    QUEENSIDE_CASTLE,
    EN_PASSANT,
    KNIGHT_PROMOTION,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION
} Flag;

typedef enum
{
    NORTH = 10,
    NORTHEAST = 11,
    EAST = 1,
    SOUTHEAST = -9,
    SOUTH = -10,
    SOUTHWEST = -11,
    WEST = -1,
    NORTHWEST = 9,
    NORTHNORTHEAST = NORTH + NORTHEAST,
    NORTHEASTEAST = NORTHEAST + EAST,
    SOUTHEASTEAST = SOUTHEAST + EAST,
    SOUTHSOUTHEAST = SOUTH + SOUTHEAST,
    SOUTHSOUTHWEST = SOUTH + SOUTHWEST,
    SOUTHWESTWEST = SOUTHWEST + WEST,
    NORTHWESTWEST = NORTHWEST + WEST,
    NORTHNORTHWEST = NORTH + NORTHWEST,
    NORTHNORTH = NORTH + NORTH,
    SOUTHSOUTH = SOUTH + SOUTH
} Direction;

/* The board is essentially a 10x10 array, along with some extra information required to play the game.
 * The array is one-dimensional for performance.
 * The pieces are defined as the following (from the piece enum):
 * -7: black king       6: white king
 * -6: black queen      5: white queen
 * -5: black rook       4: white rook
 * -4: black bishop     3: white bishop
 * -3: black knight     2: white knight
 * -2: black pawn       1: white pawn
 * -1: out-of-bounds    0: empty square
 * To convert from black pieces to white and vice-versa, simply flip the bits.
 * white_king and black_king determine the index of white's and black's kings.
 * If a pawn moved 2 squares last move, then en_passant is the position of that pawn.
 * Otherwise, en_passant is set to 0.
 * color is true if it's white move, and false otherwise.
 * There are 4 booleans for castling, each defining which castle is still allowed to be played.
 * There is also a hash number, which needs to be initialized once with hash() and
 * is then updated when applying or undoing moves. */
typedef struct
{
   int position[100];
   int white_king;
   int black_king;
   int en_passant;
   bool color;
   bool white_kingside_castle;
   bool white_queenside_castle;
   bool black_kingside_castle;
   bool black_queenside_castle;
   unsigned long long hash;
} Board;
const Board STARTING_BOARD;
const Board KIWIPETE_BOARD;
const Board EMPTY_BOARD;
const Board LIMITED_BOARD;

/* This was made for performance, so misusing this struct will result in undefined behavior.
 * piece and opponent_piece are used to store the related pieces for undoing them later.
 * flag is used for special moves and can be one of the following (from the flag enum):
 *  0: normal move
 *  1: kingside castle
 *  2: queenside castle
 *  3: en passant
 *  4: promotion to knight
 *  5: promotion to bishop
 *  6: promotion to rook
 *  7: promotion to queen
 * There is also some extra information about the previous state of the board
 * that's required in order to undo moves. */
typedef struct
{
    int from;
    int to;
    int piece;
    int opponent_piece;
    int flag;
    int prev_en_passant;
    int prev_kingside_castle;
    int prev_queenside_castle;
    int prev_opponent_castle;
} Move;

/* A bucket that contains the evaluation of a board position. */
typedef struct
{
    unsigned long long hash;
    int depth;
    int evaluation;
    Move best_move;
} PositionBucket;

/* Same as PositionBucket, but doesn't contain the information about the best move or depth. */
typedef struct
{
    unsigned long long hash;
    int evaluation;
} QuiescenceBucket;

/* The following needs to be initialized before using most of the functionality in this header.
 * hash_values must be an array of at least 1605 randomly generated long integers,
 * and must equal 0 at indices 0 through 99 and 1400 through 1500.
 * position_table must be an empty array of size position_table_size.
 * quiescence_table must be an empty array of size quiescence_table_size. */
unsigned long long *hash_values;
PositionBucket *position_table;
int position_table_size;
QuiescenceBucket *quiescence_table;
int quiescence_table_size;

/* TODO implement 50-move rule. */
/* TODO implement 3-move repetition search. */

/* Determine if two boards are equal to each other.
 * This method compares the boards themselves,
 * instead of using their hash values, in case a collision occurred. */
bool equals(Board *board1, Board *board2);

/* Deep copy a board. */
void copy(Board *destination, const Board *source);

/* Assign the initial hash value to a board.
 * This uses Zobrist Hashing, which means the hash value will be updated as the board is changed. */
void hash(Board *board);

/* Switch the board's color.
 * This also updates the board's hash value, and thus should be used instead of board->color = !board->color. */
void switch_color(Board *board);

/* Moves cannot be applied twice, and cannot be undone before applying.
 * Doing either of those will result in undefined behavior.
 * Executing any of these functions will switch the board color.
 * apply_move() and undo_move() are not recommended, since they require an additional color check.
 * All of the functions below also update the board's hash value as needed. */
void apply_move(Move *move, Board *board);
void apply_white_move(Move *move, Board *board);
void apply_black_move(Move *move, Board *board);
void undo_move(Move *move, Board *board);
void undo_white_move(Move *move, Board *board);
void undo_black_move(Move *move, Board *board);

/* These are similar to the functions above, but don't automatically switch the board color.
 * Functions starting with pre_undo can only undo functions starting with pre_apply. */
void pre_apply_move(Move *move, Board *board);
void pre_apply_white_move(Move *move, Board *board);
void pre_apply_black_move(Move *move, Board *board);
void pre_undo_move(Move *move, Board *board);
void pre_undo_white_move(Move *move, Board *board);
void pre_undo_black_move(Move *move, Board *board);

/* These are used to make a simple move or capture and undo that move.
 * They are used by the functions above to apply or undo the move.
 * These 4 functions can technically be used to move pieces, but they are absolutely not recommended,
 * since they don't check for any special conditions or moves (castling, en passant, and promotions).
 * A capture in the context of this engine doesn't mean a literal capture, but rather means
 * that it could either be a normal move OR a capture. */
void white_capture(Move* move, Board *board);
void black_capture(Move* move, Board *board);
void undo_white_capture(Move *move, Board *board);
void undo_black_capture(Move *move, Board *board);

/* Generate an array of pseudo-legal moves. Return the number of moves generated.
 * The array is assumed to be large enough to hold all of the moves (256 preferred). */
int generate_moves(Board *board, Move *moves);
int generate_white_moves(Board *board, Move *moves);
int generate_black_moves(Board *board, Move *moves);

/* Generate an array of all possible legal moves. Return the number of moves generated.
 * The array is assumed to be large enough to hold all of the moves (256 preferred). */
int generate_legal_moves(Board *board, Move *legal_moves);
int generate_white_legal_moves(Board *board, Move *legal_moves);
int generate_black_legal_moves(Board *board, Move *legal_moves);

/* Similar to the above, but instead of generating a proper array of legal moves,
 * generate an array of pseudo-legal moves and an array of indices to point out which moves are legal,
 * making this slightly faster. Return the number of legal moves. */
int find_legal_moves(Board *board, Move *moves, int *move_indexes);
int find_white_legal_moves(Board *board, Move *moves, int *move_indexes);
int find_black_legal_moves(Board *board, Move *moves, int *move_indexes);

/* Add moves in a certain direction. Return the updates j value, which is the index of the last move + 1.
 * Used by generate_moves() in order to effeciently generate pseudo-legal moves.
 * add_white_capture() and add_black_capture() only add the move if the square is occupied by an enemy piece. */
int add_white_move(Board *board, int from, Direction direction, int j, Move *moves);
int add_white_move_repeat(Board *board, int from, Direction direction, int j, Move *moves);
int add_white_capture(Board *board, int from, Direction direction, int j, Move *moves);
int add_black_move(Board *board, int from, Direction direction, int j, Move *moves);
int add_black_move_repeat(Board *board, int from, Direction direction, int j, Move *moves);
int add_black_capture(Board *board, int from, Direction direction, int j, Move *moves);

/* Add the desired move to the array of moves and return the updated j value. */
int push_move(Board *board, int from, int to, Flag flag, int j, Move *moves);

/* TODO inline and test performance. */

/* Check if one of the sides is in check. */
bool is_checked(Board *board);
bool is_white_checked(Board *board);
bool is_black_checked(Board *board);

/* Check if the king is attacked from a certain direction.
 * Used in combination with a few other checks to determine whether the king is in check. */
bool is_white_attacked(Board *board, Direction direction);
bool is_black_attacked(Board *board, Direction direction);

/* TODO add comment. */
Move *start_search(Board *board, int depth);
int search(Board *board, int alpha, int beta, int depth, Move *moves, int *move_indexes);
int quiescence_search(Board *board, int alpha, int beta);

/* The evaluation function. This evaluates different aspects of the position without any looking ahead.
 * The evaluation is very roughly in centipawns and returns
 * a positive integer if white is winning, 0 if it's a drawn position, and negative if black is winning.
 * It also generates the legal moves for the position. */
int evaluate(Board *board, Move *moves, int *move_indexes);

/* The following functions aren't necessary for the engine to work, but can be helpful in debugging it. */
/* ---------------------------------------------------------------------------------------------------- */

/* Split a string into an array of size 8. The strings are at most 100 characters long. */
char **split(char *string, char regex);

/* Load a board from a FEN string.
 * If the procedure failed, then the board's hash value will remain at 0. */
Board *load_fen(char *fen);

/* Perfomance test which counts all nodes upto a certain depth. */
typedef struct
{
    unsigned long long hash;
    int depth;
    long long result;
} PerftBucket;

typedef struct
{
    Board *board;
    int depth;
} PerftArguments;

typedef struct
{
    Board *board;
    int depth;
    int threads;
} ThreadedPerftArguments;

/* Needs to be initialized before running the perfomance test. */
PerftBucket *perft_table;
int perft_table_size;

long long threaded_perft(Board *board, int depth, int threads);
void *threaded_perft_wrapper(void *arguments);
long long perft(Board *board, int depth);
void *perft_wrapper(void *arguments);

/* Return the string representation of the move in simple algebraic notation. */
char *get_move_string(Move *move, Board *board);

/* Print the move in simple algebraic notation. */
void print_move(Move *move, Board *board);

/* Return the string representation of the move in standard algebraic notation. */
char *get_standard_move_string(Move *move, Board *board);

/* Print the move in standard algebraic notation. */
void print_standard_move(Move *move, Board *board);

/* Return the string representation of the board. */
char *get_board_string(Board *board);

/* Print the string representation of the board. */
void print_board(Board *board);

/* Return the full string representation of the board,
 * which includes the board color, castling rights, and the en passant square. */
char *get_full_board_string(Board *board);

/* Print the full string representation of the board,
 * which includes the board color, castling rights, and the en passant square. */
void print_full_board(Board *board);
