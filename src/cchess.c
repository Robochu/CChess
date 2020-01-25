
/* cchess.c
 * A powerful engine written by a powerless person.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cchess.h"

/* Define the starting board as well as a few others. No hashes are included. */
const Board STARTING_BOARD = {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                               -1,  4,  2,  3,  5,  6,  3,  2,  4, -1,
                               -1,  1,  1,  1,  1,  1,  1,  1,  1, -1,
                               -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                               -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                               -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                               -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                               -1, -2, -2, -2, -2, -2, -2, -2, -2, -1,
                               -1, -5, -3, -4, -6, -7, -4, -3, -5, -1,
                               -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                              15, 85, 0, true, true, true, true, true, 0};

const Board KIWIPETE_BOARD = {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                               -1,  4,  0,  0,  0,  6,  0,  0,  4, -1,
                               -1,  1,  1,  1,  3,  3,  1,  1,  1, -1,
                               -1,  0,  0,  2,  0,  0,  5,  0, -2, -1,
                               -1,  0, -2,  0,  0,  1,  0,  0,  0, -1,
                               -1,  0,  0,  0,  1,  2,  0,  0,  0, -1,
                               -1, -4, -3,  0,  0, -2, -3, -2,  0, -1,
                               -1, -2,  0, -2, -2, -6, -2, -4,  0, -1,
                               -1, -5,  0,  0,  0, -7,  0,  0, -5, -1,
                               -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                              15, 85, 0, true, true, true, true, true, 0};

const Board LIMITED_BOARD = {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                              -1,  4,  0,  0,  5,  0,  4,  6,  0, -1,
                              -1,  1, -2,  0,  1,  0,  0,  1,  1, -1,
                              -1, -6,  0,  0,  0,  0,  2,  0,  0, -1,
                              -1,  3,  3,  1,  0,  1,  0,  0,  0, -1,
                              -1, -3,  1,  0,  0,  0,  0,  0,  0, -1,
                              -1,  0, -4,  0,  0,  0, -3, -4,  2, -1,
                              -1,  1, -2, -2, -2,  0, -2, -2, -2, -1,
                              -1, -5,  0,  0,  0, -7,  0,  0, -5, -1,
                              -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                             17, 85, 0, true, false, false, true, true, 0};

const Board EMPTY_BOARD = {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                            -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                            -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                            -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                            -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                            -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                            -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                            -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                            -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
                            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                           0, 0, 0, false, false, false, false, false, 0};

bool equals(Board *board1, Board *board2)
{
    for (int i = 11; i < 89; i++)
    {
        if (board1->position[i] != board2->position[i])
        {
            return false;
        }
    }

    return board1->en_passant == board2->en_passant &&
           board1->color == board2->color &&
           board1->white_kingside_castle == board2->white_kingside_castle &&
           board1->white_queenside_castle == board2->white_queenside_castle &&
           board1->black_kingside_castle == board2->black_kingside_castle &&
           board1->black_queenside_castle == board2->black_queenside_castle;
}

void copy(Board *destination, const Board *source)
{
    memcpy(destination->position, source->position, sizeof(source->position));
    destination->white_king = source->white_king;
    destination->black_king = source->black_king;
    destination->en_passant = source->en_passant;
    destination->color = source->color;
    destination->white_kingside_castle = source->white_kingside_castle;
    destination->white_queenside_castle = source->white_queenside_castle;
    destination->black_kingside_castle = source->black_kingside_castle;
    destination->black_queenside_castle = source->black_queenside_castle;
    destination->hash = source->hash;
}

void hash(Board *board)
{
    for (int i = 11; i < 89; i++)
    {
        if (board->position[i] < OUT_OF_BOUNDS)
        {
            board->hash ^= hash_values[(board->position[i] + 14) * 100 + i];
        }
        else if (board->position[i] > EMPTY)
        {
            board->hash ^= hash_values[board->position[i] * 100 + i];
        }
        else if (board->position[i] == OUT_OF_BOUNDS)
        {
            i++;
        }
    }

    if (board->en_passant)
    {
        board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
    }
    if (board->color)
    {
        board->hash ^= hash_values[ZOBRIST_COLOR];
    }
    if (board->white_kingside_castle)
    {
        board->hash ^= hash_values[ZOBRIST_WHITE_KINGSIDE_CASTLE];
    }
    if (board->white_queenside_castle)
    {
        board->hash ^= hash_values[ZOBRIST_WHITE_QUEENSIDE_CASTLE];
    }
    if (board->black_kingside_castle)
    {
        board->hash ^= hash_values[ZOBRIST_BLACK_KINGSIDE_CASTLE];
    }
    if (board->black_queenside_castle)
    {
        board->hash ^= hash_values[ZOBRIST_BLACK_QUEENSIDE_CASTLE];
    }
}

void switch_color(Board *board)
{
    board->color = !board->color;
    board->hash ^= hash_values[ZOBRIST_COLOR];
}

void apply_move(Move *move, Board *board)
{
    pre_apply_move(move, board);
    switch_color(board);
}

void apply_white_move(Move *move, Board *board)
{
    pre_apply_white_move(move, board);
    switch_color(board);
}

void apply_black_move(Move *move, Board *board)
{
    pre_apply_black_move(move, board);
    switch_color(board);
}

void undo_move(Move *move, Board *board)
{
    /* Since pre_apply_move() doesn't switch the board color, but apply_move() does,
     * switching it again is required before calling pre_undo_move(). */
    switch_color(board);
    pre_undo_move(move, board);
}

void undo_white_move(Move *move, Board *board)
{
    /* Since pre_apply_white_move() doesn't switch the board color, but apply_white_move() does,
     * switching it again is required before calling pre_undo_white_move(). */
    switch_color(board);
    pre_undo_white_move(move, board);
}

void undo_black_move(Move *move, Board *board)
{
    /* Since pre_apply_black_move() doesn't switch the board color, but apply_black_move() does,
     * switching it again is required before calling pre_undo_black_move(). */
    switch_color(board);
    pre_undo_black_move(move, board);
}

void pre_apply_move(Move *move, Board *board)
{
    if (board->color)
    {
        pre_apply_white_move(move, board);
    }
    else
    {
        pre_apply_black_move(move, board);
    }
}

void pre_apply_white_move(Move *move, Board *board)
{
    white_capture(move, board);
    if (board->position[move->to] == WHITE_KING)
    {
        board->white_king = move->to;
        if (board->white_kingside_castle)
        {
            move->prev_kingside_castle = board->white_kingside_castle;
            board->white_kingside_castle = false;
            board->hash ^= hash_values[ZOBRIST_WHITE_KINGSIDE_CASTLE];
        }
        if (board->white_queenside_castle)
        {
            move->prev_queenside_castle = board->white_queenside_castle;
            board->white_queenside_castle = false;
            board->hash ^= hash_values[ZOBRIST_WHITE_QUEENSIDE_CASTLE];
        }

        if (move->flag == KINGSIDE_CASTLE)
        {
            board->position[18] = EMPTY;
            board->position[16] = WHITE_ROOK;
            board->hash ^= hash_values[418];
            board->hash ^= hash_values[416];
        }
        else if (move->flag == QUEENSIDE_CASTLE)
        {
            board->position[11] = EMPTY;
            board->position[14] = WHITE_ROOK;
            board->hash ^= hash_values[411];
            board->hash ^= hash_values[414];
        }
    }
    else if (board->position[move->to] == WHITE_PAWN)
    {
        if (move->to - move->from == 20)
        {
            board->en_passant = move->to;
            board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
        }
        else if (move->flag == EN_PASSANT)
        {
            board->position[move->to + SOUTH] = EMPTY;
            board->hash ^= hash_values[1200 + move->to + SOUTH];
        }
        else if (move->flag > EN_PASSANT)
        {
            board->position[move->to] = move->flag - 2;
            board->hash ^= hash_values[100 + move->to];
            board->hash ^= hash_values[(move->flag - 2) * 100 + move->to];
        }
    }
    else if (move->from == 11 && board->white_queenside_castle)
    {
        move->prev_queenside_castle = board->white_queenside_castle;
        board->white_queenside_castle = false;
        board->hash ^= hash_values[ZOBRIST_WHITE_QUEENSIDE_CASTLE];
    }
    else if (move->from == 18 && board->white_kingside_castle)
    {
        move->prev_kingside_castle = board->white_kingside_castle;
        board->white_kingside_castle = false;
        board->hash ^= hash_values[ZOBRIST_WHITE_KINGSIDE_CASTLE];
    }

    if (move->to == 81 && board->black_queenside_castle)
    {
        move->prev_opponent_castle = board->black_queenside_castle;
        board->black_queenside_castle = false;
        board->hash ^= hash_values[ZOBRIST_BLACK_QUEENSIDE_CASTLE];
    }
    else if (move->to == 88 && board->black_kingside_castle)
    {
        move->prev_opponent_castle = board->black_kingside_castle;
        board->black_kingside_castle = false;
        board->hash ^= hash_values[ZOBRIST_BLACK_KINGSIDE_CASTLE];
    }
}

void pre_apply_black_move(Move *move, Board *board)
{
    black_capture(move, board);
    if (board->position[move->to] == BLACK_KING)
    {
        board->black_king = move->to;
        if (board->black_kingside_castle)
        {
            move->prev_kingside_castle = board->black_kingside_castle;
            board->black_kingside_castle = false;
            board->hash ^= hash_values[ZOBRIST_BLACK_KINGSIDE_CASTLE];
        }
        if (board->black_queenside_castle)
        {
            move->prev_queenside_castle = board->black_queenside_castle;
            board->black_queenside_castle = false;
            board->hash ^= hash_values[ZOBRIST_BLACK_QUEENSIDE_CASTLE];
        }

        if (move->flag == KINGSIDE_CASTLE)
        {
            board->position[88] = EMPTY;
            board->position[86] = BLACK_ROOK;
            board->hash ^= hash_values[988];
            board->hash ^= hash_values[986];
        }
        else if (move->flag == QUEENSIDE_CASTLE)
        {
            board->position[81] = EMPTY;
            board->position[84] = BLACK_ROOK;
            board->hash ^= hash_values[981];
            board->hash ^= hash_values[984];
        }
    }
    else if (board->position[move->to] == BLACK_PAWN)
    {
        if (move->from - move->to == 20)
        {
            board->en_passant = move->to;
            board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
        }
        else if (move->flag == EN_PASSANT)
        {
            board->position[move->to + NORTH] = EMPTY;
            board->hash ^= hash_values[100 + move->to + NORTH];
        }
        else if (move->flag > EN_PASSANT)
        {
            board->position[move->to] = 1 - move->flag;
            board->hash ^= hash_values[1200 + move->to];
            board->hash ^= hash_values[(15 - move->flag) * 100 + move->to];
        }
    }
    else if (move->from == 81 && board->black_queenside_castle)
    {
        move->prev_queenside_castle = board->black_queenside_castle;
        board->black_queenside_castle = false;
        board->hash ^= hash_values[ZOBRIST_BLACK_QUEENSIDE_CASTLE];
    }
    else if (move->from == 88 && board->black_kingside_castle)
    {
        move->prev_kingside_castle = board->black_kingside_castle;
        board->black_kingside_castle = false;
        board->hash ^= hash_values[ZOBRIST_BLACK_KINGSIDE_CASTLE];
    }

    if (move->to == 11 && board->white_queenside_castle)
    {
        move->prev_opponent_castle = board->white_queenside_castle;
        board->white_queenside_castle = false;
        board->hash ^= hash_values[ZOBRIST_WHITE_QUEENSIDE_CASTLE];
    }
    else if (move->to == 18 && board->white_kingside_castle)
    {
        move->prev_opponent_castle = board->white_kingside_castle;
        board->white_kingside_castle = false;
        board->hash ^= hash_values[ZOBRIST_WHITE_KINGSIDE_CASTLE];
    }
}

void pre_undo_move(Move *move, Board *board)
{
    if (board->color)
    {
        pre_undo_white_move(move, board);
    }
    else
    {
        pre_undo_black_move(move, board);
    }
}

void pre_undo_white_move(Move *move, Board *board)
{
    undo_white_capture(move, board);
    if (board->position[move->from] == WHITE_KING)
    {
        board->white_king = move->from;

        if (move->prev_kingside_castle)
        {
            board->white_kingside_castle = move->prev_kingside_castle;
            board->hash ^= hash_values[ZOBRIST_WHITE_KINGSIDE_CASTLE];
        }
        if (move->prev_queenside_castle)
        {
            board->white_queenside_castle = move->prev_queenside_castle;
            board->hash ^= hash_values[ZOBRIST_WHITE_QUEENSIDE_CASTLE];
        }

        if (move->flag == KINGSIDE_CASTLE)
        {
            board->position[16] = EMPTY;
            board->position[18] = WHITE_ROOK;
            board->hash ^= hash_values[416];
            board->hash ^= hash_values[418];
        }
        else if (move->flag == QUEENSIDE_CASTLE)
        {
            board->position[14] = EMPTY;
            board->position[11] = WHITE_ROOK;
            board->hash ^= hash_values[414];
            board->hash ^= hash_values[411];
        }
    }
    else if (move->flag == EN_PASSANT)
    {
        board->position[move->to + SOUTH] = BLACK_PAWN;
        board->hash ^= hash_values[1200 + move->to + SOUTH];
    }
    else if (move->flag > EN_PASSANT)
    {
        board->hash ^= hash_values[100 + move->to];
        board->hash ^= hash_values[(move->flag - 2) * 100 + move->to];
    }
    else if (move->from == 11 && move->prev_queenside_castle)
    {
        board->white_queenside_castle = move->prev_queenside_castle;
        board->hash ^= hash_values[ZOBRIST_WHITE_QUEENSIDE_CASTLE];
    }
    else if (move->from == 18 && move->prev_kingside_castle)
    {
        board->white_kingside_castle = move->prev_kingside_castle;
        board->hash ^= hash_values[ZOBRIST_WHITE_KINGSIDE_CASTLE];
    }

    if (move->to == 81 && move->prev_opponent_castle)
    {
        board->black_queenside_castle = move->prev_opponent_castle;
        board->hash ^= hash_values[ZOBRIST_BLACK_QUEENSIDE_CASTLE];
    }
    else if (move->to == 88 && move->prev_opponent_castle)
    {
        board->black_kingside_castle = move->prev_opponent_castle;
        board->hash ^= hash_values[ZOBRIST_BLACK_KINGSIDE_CASTLE];
    }
}

void pre_undo_black_move(Move *move, Board *board)
{
    undo_black_capture(move, board);
    if (board->position[move->from] == BLACK_KING)
    {
        board->black_king = move->from;
        if (move->prev_kingside_castle)
        {
            board->black_kingside_castle = move->prev_kingside_castle;
            board->hash ^= hash_values[ZOBRIST_BLACK_KINGSIDE_CASTLE];
        }
        if (move->prev_queenside_castle)
        {
            board->black_queenside_castle = move->prev_queenside_castle;
            board->hash ^= hash_values[ZOBRIST_BLACK_QUEENSIDE_CASTLE];
        }

        if (move->flag == KINGSIDE_CASTLE)
        {
            board->position[86] = EMPTY;
            board->position[88] = BLACK_ROOK;
            board->hash ^= hash_values[986];
            board->hash ^= hash_values[988];
        }
        else if (move->flag == QUEENSIDE_CASTLE)
        {
            board->position[84] = EMPTY;
            board->position[81] = BLACK_ROOK;
            board->hash ^= hash_values[984];
            board->hash ^= hash_values[981];
        }
    }
    else if (move->flag == EN_PASSANT)
    {
        board->position[move->to + NORTH] = WHITE_PAWN;
        board->hash ^= hash_values[100 + move->to + NORTH];
    }
    else if (move->flag > EN_PASSANT)
    {
        board->hash ^= hash_values[1200 + move->to];
        board->hash ^= hash_values[(15 - move->flag) * 100 + move->to];
    }
    else if (move->from == 81 && move->prev_queenside_castle)
    {
        board->black_queenside_castle = move->prev_queenside_castle;
        board->hash ^= hash_values[ZOBRIST_BLACK_QUEENSIDE_CASTLE];
    }
    else if (move->from == 88 && move->prev_kingside_castle)
    {
        board->black_kingside_castle = move->prev_kingside_castle;
        board->hash ^= hash_values[ZOBRIST_BLACK_KINGSIDE_CASTLE];
    }

    if (move->to == 11 && move->prev_opponent_castle)
    {
        board->white_queenside_castle = move->prev_opponent_castle;
        board->hash ^= hash_values[ZOBRIST_WHITE_QUEENSIDE_CASTLE];
    }
    else if (move->to == 18 && move->prev_opponent_castle)
    {
        board->white_kingside_castle = move->prev_opponent_castle;
        board->hash ^= hash_values[ZOBRIST_WHITE_KINGSIDE_CASTLE];
    }
}

void white_capture(Move *move, Board *board)
{
    /* Make the capture. */
    board->position[move->to] = move->piece;
    board->position[move->from] = EMPTY;
    board->hash ^= hash_values[(move->opponent_piece + 14) * 100 + move->to];
    board->hash ^= hash_values[move->piece * 100 + move->from];
    board->hash ^= hash_values[move->piece * 100 + move->to];

    /* Reset en_passant. */
    board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
    board->en_passant = 0;
}

void black_capture(Move *move, Board *board)
{
    /* Make the capture. */
    move->opponent_piece = board->position[move->to];
    board->position[move->to] = move->piece;
    board->position[move->from] = EMPTY;
    board->hash ^= hash_values[move->opponent_piece * 100 + move->to];
    board->hash ^= hash_values[(move->piece + 14) * 100 + move->from];
    board->hash ^= hash_values[(move->piece + 14) * 100 + move->to];

    /* Reset en_passant. */
    board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
    board->en_passant = 0;
}

void undo_white_capture(Move *move, Board *board)
{
    /* The inverse of white_capture(). */
    board->position[move->from] = move->piece;
    board->position[move->to] = move->opponent_piece;
    board->hash ^= hash_values[(move->opponent_piece + 14) * 100 + move->to];
    board->hash ^= hash_values[move->piece * 100 + move->from];
    board->hash ^= hash_values[move->piece * 100 + move->to];

    /* Restore en_passant. */
    board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
    board->en_passant = move->prev_en_passant;
    board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
}

void undo_black_capture(Move *move, Board *board)
{
    /* The inverse of black_capture(). */
    board->position[move->from] = move->piece;
    board->position[move->to] = move->opponent_piece;
    board->hash ^= hash_values[move->opponent_piece * 100 + move->to];
    board->hash ^= hash_values[(move->piece + 14) * 100 + move->from];
    board->hash ^= hash_values[(move->piece + 14) * 100 + move->to];

    /* Restore en_passant. */
    board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
    board->en_passant = move->prev_en_passant;
    board->hash ^= hash_values[ZOBRIST_EN_PASSANT + board->en_passant];
}

int generate_moves(Board *board, Move *moves)
{
    if (board->color)
    {
        return generate_white_moves(board, moves);
    }
    else
    {
        return generate_black_moves(board, moves);
    }
}

int generate_white_moves(Board *board, Move *moves)
{
    /* Go through the board from bottom up, and add moves as necessary.
     * There's not much more to this, as this is simply implementing the chess rules. */
    int j = 0;
    for (int i = 11; i < 89; i++)
    {
        switch (board->position[i])
        {
            case WHITE_PAWN:
                if (i > 70)
                {
                    if (!board->position[i + NORTH])
                    {
                        j = push_move(board, i, i + NORTH, KNIGHT_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTH, BISHOP_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTH, ROOK_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTH, QUEEN_PROMOTION, j, moves);
                    }
                    if (board->position[i + NORTHEAST] < OUT_OF_BOUNDS)
                    {
                        j = push_move(board, i, i + NORTHEAST, KNIGHT_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTHEAST, BISHOP_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTHEAST, ROOK_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTHEAST, QUEEN_PROMOTION, j, moves);
                    }
                    if (board->position[i + NORTHWEST] < OUT_OF_BOUNDS)
                    {
                        j = push_move(board, i, i + NORTHWEST, KNIGHT_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTHWEST, BISHOP_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTHWEST, ROOK_PROMOTION, j, moves);
                        j = push_move(board, i, i + NORTHWEST, QUEEN_PROMOTION, j, moves);
                    }
                }
                else
                {
                    if (!board->position[i + NORTH])
                    {
                        j = push_move(board, i, i + NORTH, NONE, j, moves);
                        if (i < 29 && !board->position[i + NORTHNORTH])
                        {
                            j = push_move(board, i, i + NORTHNORTH, NONE, j, moves);
                        }
                    }
                    j = add_white_capture(board, i, NORTHEAST, j, moves);
                    j = add_white_capture(board, i, NORTHWEST, j, moves);
                    if (i > 50)
                    {
                        if (i + WEST == board->en_passant)
                        {
                            j = push_move(board, i, i + NORTHWEST, EN_PASSANT, j, moves);
                        }
                        else if (i + EAST == board->en_passant)
                        {
                            j = push_move(board, i, i + NORTHEAST, EN_PASSANT, j, moves);
                        }
                    }
                }
                break;
            case WHITE_KNIGHT:
                if (i > 20)
                {
                    j = add_white_move(board, i, SOUTHEASTEAST, j, moves);
                    j = add_white_move(board, i, SOUTHSOUTHEAST, j, moves);
                    j = add_white_move(board, i, SOUTHSOUTHWEST, j, moves);
                    j = add_white_move(board, i, SOUTHWESTWEST, j, moves);
                }
                if (i < 79)
                {
                    j = add_white_move(board, i, NORTHWESTWEST, j, moves);
                    j = add_white_move(board, i, NORTHNORTHWEST, j, moves);
                    j = add_white_move(board, i, NORTHNORTHEAST, j, moves);
                    j = add_white_move(board, i, NORTHEASTEAST, j, moves);
                }
                break;
            case WHITE_BISHOP:
                j = add_white_move_repeat(board, i, NORTHEAST, j, moves);
                j = add_white_move_repeat(board, i, SOUTHEAST, j, moves);
                j = add_white_move_repeat(board, i, SOUTHWEST, j, moves);
                j = add_white_move_repeat(board, i, NORTHWEST, j, moves);
                break;
            case WHITE_ROOK:
                j = add_white_move_repeat(board, i, NORTH, j, moves);
                j = add_white_move_repeat(board, i, EAST, j, moves);
                j = add_white_move_repeat(board, i, SOUTH, j, moves);
                j = add_white_move_repeat(board, i, WEST, j, moves);
                break;
            case WHITE_QUEEN:
                j = add_white_move_repeat(board, i, NORTH, j, moves);
                j = add_white_move_repeat(board, i, NORTHEAST, j, moves);
                j = add_white_move_repeat(board, i, EAST, j, moves);
                j = add_white_move_repeat(board, i, SOUTHEAST, j, moves);
                j = add_white_move_repeat(board, i, SOUTH, j, moves);
                j = add_white_move_repeat(board, i, SOUTHWEST, j, moves);
                j = add_white_move_repeat(board, i, WEST, j, moves);
                j = add_white_move_repeat(board, i, NORTHWEST, j, moves);
                break;
            case OUT_OF_BOUNDS:
                i++;
                break;
        }
    }

    /* Since the position of the king is already known, this can be done outside of the loop. */
    j = add_white_move(board, board->white_king, NORTH, j, moves);
    j = add_white_move(board, board->white_king, NORTHEAST, j, moves);
    j = add_white_move(board, board->white_king, EAST, j, moves);
    j = add_white_move(board, board->white_king, SOUTHEAST, j, moves);
    j = add_white_move(board, board->white_king, SOUTH, j, moves);
    j = add_white_move(board, board->white_king, SOUTHWEST, j, moves);
    j = add_white_move(board, board->white_king, WEST, j, moves);
    j = add_white_move(board, board->white_king, NORTHWEST, j, moves);

    if (board->white_kingside_castle && !board->position[16] && !board->position[17])
    {
        /* TODO Reorganize this slightly, no need for two check checks. */
        if (!is_white_checked(board))
        {
            /* No need to actually move the king across the board. */
            board->white_king = 16;
            if (!is_white_checked(board))
            {
                j = push_move(board, 15, 17, KINGSIDE_CASTLE, j, moves);
            }
            board->white_king = 15;
        }
    }

    if (board->white_queenside_castle && !board->position[14] && !board->position[13] && !board->position[12])
    {
        if (!is_white_checked(board))
        {
            board->white_king = 14;
            if (!is_white_checked(board))
            {
                j = push_move(board, 15, 13, QUEENSIDE_CASTLE, j, moves);
            }
            board->white_king = 15;
        }
    }

    return j;
}

int generate_black_moves(Board *board, Move *moves)
{
    /* The inverse of generate_white_moves(). */
    int j = 0;
    for (int i = 88; i > 10; i--)
    {
        switch (board->position[i])
        {
            case BLACK_PAWN:
                if (i < 29)
                {
                    if (!board->position[i + SOUTH])
                    {
                        j = push_move(board, i, i + SOUTH, KNIGHT_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTH, BISHOP_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTH, ROOK_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTH, QUEEN_PROMOTION, j, moves);
                    }
                    if (board->position[i + SOUTHEAST] > EMPTY)
                    {
                        j = push_move(board, i, i + SOUTHEAST, KNIGHT_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTHEAST, BISHOP_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTHEAST, ROOK_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTHEAST, QUEEN_PROMOTION, j, moves);
                    }
                    if (board->position[i + SOUTHWEST] > EMPTY)
                    {
                        j = push_move(board, i, i + SOUTHWEST, KNIGHT_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTHWEST, BISHOP_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTHWEST, ROOK_PROMOTION, j, moves);
                        j = push_move(board, i, i + SOUTHWEST, QUEEN_PROMOTION, j, moves);
                    }
                }
                else
                {
                    if (!board->position[i + SOUTH])
                    {
                        j = push_move(board, i, i + SOUTH, NONE, j, moves);
                        if (i > 70 && !board->position[i + SOUTHSOUTH])
                        {
                            j = push_move(board, i, i + SOUTHSOUTH, NONE, j, moves);
                        }
                    }
                    j = add_black_capture(board, i, SOUTHEAST, j, moves);
                    j = add_black_capture(board, i, SOUTHWEST, j, moves);
                    if (i < 49)
                    {
                        if (i + WEST == board->en_passant)
                        {
                            j = push_move(board, i, i + SOUTHWEST, EN_PASSANT, j, moves);
                        }
                        else if (i + EAST == board->en_passant)
                        {
                            j = push_move(board, i, i + SOUTHEAST, EN_PASSANT, j, moves);
                        }
                    }
                }
                break;
            case BLACK_KNIGHT:
                if (i > 20)
                {
                    j = add_black_move(board, i, SOUTHEASTEAST, j, moves);
                    j = add_black_move(board, i, SOUTHSOUTHEAST, j, moves);
                    j = add_black_move(board, i, SOUTHSOUTHWEST, j, moves);
                    j = add_black_move(board, i, SOUTHWESTWEST, j, moves);
                }
                if (i < 79)
                {
                    j = add_black_move(board, i, NORTHWESTWEST, j, moves);
                    j = add_black_move(board, i, NORTHNORTHWEST, j, moves);
                    j = add_black_move(board, i, NORTHNORTHEAST, j, moves);
                    j = add_black_move(board, i, NORTHEASTEAST, j, moves);
                }
                break;
            case BLACK_BISHOP:
                j = add_black_move_repeat(board, i, NORTHEAST, j, moves);
                j = add_black_move_repeat(board, i, SOUTHEAST, j, moves);
                j = add_black_move_repeat(board, i, SOUTHWEST, j, moves);
                j = add_black_move_repeat(board, i, NORTHWEST, j, moves);
                break;
            case BLACK_ROOK:
                j = add_black_move_repeat(board, i, NORTH, j, moves);
                j = add_black_move_repeat(board, i, EAST, j, moves);
                j = add_black_move_repeat(board, i, SOUTH, j, moves);
                j = add_black_move_repeat(board, i, WEST, j, moves);
                break;
            case BLACK_QUEEN:
                j = add_black_move_repeat(board, i, NORTH, j, moves);
                j = add_black_move_repeat(board, i, NORTHEAST, j, moves);
                j = add_black_move_repeat(board, i, EAST, j, moves);
                j = add_black_move_repeat(board, i, SOUTHEAST, j, moves);
                j = add_black_move_repeat(board, i, SOUTH, j, moves);
                j = add_black_move_repeat(board, i, SOUTHWEST, j, moves);
                j = add_black_move_repeat(board, i, WEST, j, moves);
                j = add_black_move_repeat(board, i, NORTHWEST, j, moves);
                break;
            case OUT_OF_BOUNDS:
                i--;
                break;
        }
    }

    j = add_black_move(board, board->black_king, NORTH, j, moves);
    j = add_black_move(board, board->black_king, NORTHEAST, j, moves);
    j = add_black_move(board, board->black_king, EAST, j, moves);
    j = add_black_move(board, board->black_king, SOUTHEAST, j, moves);
    j = add_black_move(board, board->black_king, SOUTH, j, moves);
    j = add_black_move(board, board->black_king, SOUTHWEST, j, moves);
    j = add_black_move(board, board->black_king, WEST, j, moves);
    j = add_black_move(board, board->black_king, NORTHWEST, j, moves);

    if (board->black_kingside_castle && !board->position[86] && !board->position[87])
    {
        if (!is_black_checked(board))
        {
            board->black_king = 86;
            if (!is_black_checked(board))
            {
                j = push_move(board, 85, 87, KINGSIDE_CASTLE, j, moves);
            }
            board->black_king = 85;
        }
    }

    if (board->black_queenside_castle && !board->position[84] && !board->position[83] && !board->position[82])
    {
        if (!is_black_checked(board))
        {
            board->black_king = 84;
            if (!is_black_checked(board))
            {
                j = push_move(board, 85, 83, QUEENSIDE_CASTLE, j, moves);
            }
            board->black_king = 85;
        }
    }

    return j;
}

int generate_legal_moves(Board *board, Move *legal_moves)
{
    if (board->color)
    {
        return generate_white_legal_moves(board, legal_moves);
    }
    else
    {
        return generate_black_legal_moves(board, legal_moves);
    }
}

int generate_white_legal_moves(Board *board, Move *legal_moves)
{
    /* A move is legal if it's pseudo-legal and doesn't leave the king in check. */
    Move *moves = malloc(256 * sizeof(*moves));
    int j = generate_white_moves(board, moves);
    int k = 0;
    for (int i = 0; i < j; i++)
    {
        pre_apply_white_move(&moves[i], board);
        if (!is_white_checked(board))
        {
            legal_moves[k++] = moves[i];
        }
        pre_undo_white_move(&moves[i], board);
    }
    free(moves);
    return k;
}

int generate_black_legal_moves(Board *board, Move *legal_moves)
{
    /* Inverse of generate_white_legal_moves(). */
    Move *moves = malloc(256 * sizeof(*moves));
    int j = generate_black_moves(board, moves);
    int k = 0;
    for (int i = 0; i < j; i++)
    {
        pre_apply_black_move(&moves[i], board);
        if (!is_black_checked(board))
        {
            legal_moves[k++] = moves[i];
        }
        pre_undo_black_move(&moves[i], board);
    }
    free(moves);
    return k;
}

int find_legal_moves(Board *board, Move *moves, int *move_indexes)
{
    if (board->color)
    {
        return find_white_legal_moves(board, moves, move_indexes);
    }
    else
    {
        return find_black_legal_moves(board, moves, move_indexes);
    }    
}

int find_white_legal_moves(Board *board, Move *moves, int *move_indexes)
{
    int j = generate_white_moves(board, moves);
    int k = 0;
    for (int i = 0; i < j; i++)
    {
        pre_apply_white_move(&moves[i], board);
        if (!is_white_checked(board))
        {
            move_indexes[k++] = i;
        }
        pre_undo_white_move(&moves[i], board);
    }
    move_indexes[k] = -1;
    return k;
}

int find_black_legal_moves(Board *board, Move *moves, int *move_indexes)
{
    int j = generate_black_moves(board, moves);
    int k = 0;
    for (int i = 0; i < j; i++)
    {
        pre_apply_black_move(&moves[i], board);
        if (!is_black_checked(board))
        {
            move_indexes[k++] = i;
        }
        pre_undo_black_move(&moves[i], board);
    }
    move_indexes[k] = -1;
    return k;
}

int add_white_move(Board *board, int from, Direction direction, int j, Move *moves)
{
    /* Check that the square is either empty or occupied by a black piece and add the move. */
    if (board->position[from + direction] < OUT_OF_BOUNDS || !board->position[from + direction])
    {
        j = push_move(board, from, from + direction, NONE, j, moves);
    }
    return j;
}

int add_white_move_repeat(Board *board, int from, Direction direction, int j, Move *moves)
{
    /* Same as add_white_move(), but continues checking squares in the specified direction. */
    int i = from;
    do
    {
        i += direction;
        if (board->position[i] < OUT_OF_BOUNDS || !board->position[i])
        {
            j = push_move(board, from, i, NONE, j, moves);
        }
    } while (!board->position[i]);
    return j;
}

int add_white_capture(Board *board, int from, Direction direction, int j, Move *moves)
{
    /* Same as add_white_move(), but only add the move if the square is occupied by the enemy piece. */
    if (board->position[from + direction] < OUT_OF_BOUNDS)
    {
        j = push_move(board, from, from + direction, NONE, j, moves);
    }
    return j;
}

int add_black_move(Board *board, int from, Direction direction, int j, Move *moves)
{
    /* Inverse of add_white_move(). */
    if (board->position[from + direction] > OUT_OF_BOUNDS)
    {
        j = push_move(board, from, from + direction, NONE, j, moves);
    }
    return j;
}

int add_black_move_repeat(Board *board, int from, Direction direction, int j, Move *moves)
{
    /* Inverse of add_white_move_repeat(). */
    int i = from;
    do
    {
        i += direction;
        if (board->position[i] > OUT_OF_BOUNDS)
        {
            j = push_move(board, from, i, NONE, j, moves);
        }
    } while (!board->position[i]);
    return j;
}

int add_black_capture(Board *board, int from, Direction direction, int j, Move *moves)
{
    /* Inverse of add_white_capture(). */
    if (board->position[from + direction] > EMPTY)
    {
        j = push_move(board, from, from + direction, NONE, j, moves);
    }
    return j;
}

int push_move(Board *board, int from, int to, Flag flag, int j, Move *moves)
{
    moves[j].from = from;
    moves[j].to = to;
    moves[j].piece = board->position[from];
    moves[j].opponent_piece = board->position[to];
    moves[j].flag = flag;
    moves[j].prev_en_passant = board->en_passant;
    moves[j].prev_kingside_castle = false;
    moves[j].prev_queenside_castle = false;
    moves[j].prev_opponent_castle = false;
    return ++j;
}

bool is_checked(Board *board)
{
    if (board->color)
    {
        return is_white_checked(board);
    }
    else
    {
        return is_black_checked(board);
    }
}

bool is_white_checked(Board *board)
{
    /* TODO check checking could be slightly reorganized and optimized. */

    /* Simply go in each direction and see if something is attacking the king.
     * Additional checks are required for pawns and knights.
     * The order of the statements is based on how likely that statement is to be true. */
    return is_white_attacked(board, NORTH) || is_white_attacked(board, NORTHWEST) ||
           is_white_attacked(board, NORTHEAST) ||
           (board->white_king < 69 && board->position[board->white_king + NORTHNORTHWEST] == BLACK_KNIGHT) ||
           (board->white_king < 68 && board->position[board->white_king + NORTHNORTHEAST] == BLACK_KNIGHT) ||
           (board->white_king < 79 && board->position[board->white_king + NORTHWESTWEST] == BLACK_KNIGHT) ||
           (board->white_king < 77 && board->position[board->white_king + NORTHEASTEAST] == BLACK_KNIGHT) ||
           board->position[board->white_king + NORTHWEST] == BLACK_PAWN ||
           board->position[board->white_king + NORTHEAST] == BLACK_PAWN ||
           is_white_attacked(board, WEST) ||
           is_white_attacked(board, EAST) || is_white_attacked(board, SOUTHWEST) ||
           is_white_attacked(board, SOUTHEAST) || is_white_attacked(board, SOUTH) ||
           (board->white_king > 22 && board->position[board->white_king + SOUTHWESTWEST] == BLACK_KNIGHT) ||
           (board->white_king > 20 && board->position[board->white_king + SOUTHEASTEAST] == BLACK_KNIGHT) ||
           (board->white_king > 31 && board->position[board->white_king + SOUTHSOUTHWEST] == BLACK_KNIGHT) ||
           (board->white_king > 30 && board->position[board->white_king + SOUTHSOUTHEAST] == BLACK_KNIGHT) ||
           board->position[board->white_king + NORTH] == BLACK_KING ||
           board->position[board->white_king + NORTHEAST] == BLACK_KING ||
           board->position[board->white_king + EAST] == BLACK_KING ||
           board->position[board->white_king + SOUTHEAST] == BLACK_KING ||
           board->position[board->white_king + SOUTH] == BLACK_KING ||
           board->position[board->white_king + SOUTHWEST] == BLACK_KING ||
           board->position[board->white_king + WEST] == BLACK_KING ||
           board->position[board->white_king + NORTHWEST] == BLACK_KING;
}

bool is_black_checked(Board *board)
{
    /* Exactly the same as is_white_checked(), but for black. */
    return is_black_attacked(board, SOUTH) || is_black_attacked(board, SOUTHWEST) ||
           is_black_attacked(board, SOUTHEAST) ||
           (board->black_king > 31 && board->position[board->black_king + SOUTHSOUTHWEST] == WHITE_KNIGHT) ||
           (board->black_king > 30 && board->position[board->black_king + SOUTHSOUTHEAST] == WHITE_KNIGHT) ||
           (board->black_king > 22 && board->position[board->black_king + SOUTHWESTWEST] == WHITE_KNIGHT) ||
           (board->black_king > 20 && board->position[board->black_king + SOUTHEASTEAST] == WHITE_KNIGHT) ||
           board->position[board->black_king + SOUTHWEST] == WHITE_PAWN ||
           board->position[board->black_king + SOUTHEAST] == WHITE_PAWN ||
           is_black_attacked(board, WEST) ||
           is_black_attacked(board, EAST) || is_black_attacked(board, NORTHWEST) ||
           is_black_attacked(board, NORTHEAST) || is_black_attacked(board, NORTH) ||
           (board->black_king < 79 && board->position[board->black_king + NORTHWESTWEST] == WHITE_KNIGHT) ||
           (board->black_king < 77 && board->position[board->black_king + NORTHEASTEAST] == WHITE_KNIGHT) ||
           (board->black_king < 69 && board->position[board->black_king + NORTHNORTHWEST] == WHITE_KNIGHT) ||
           (board->black_king < 68 && board->position[board->black_king + NORTHNORTHEAST] == WHITE_KNIGHT) ||
           board->position[board->black_king + SOUTH] == WHITE_KING ||
           board->position[board->black_king + SOUTHEAST] == WHITE_KING ||
           board->position[board->black_king + EAST] == WHITE_KING ||
           board->position[board->black_king + NORTHEAST] == WHITE_KING ||
           board->position[board->black_king + NORTH] == WHITE_KING ||
           board->position[board->black_king + NORTHWEST] == WHITE_KING ||
           board->position[board->black_king + WEST] == WHITE_KING ||
           board->position[board->black_king + SOUTHWEST] == WHITE_KING;
}

bool is_white_attacked(Board *board, Direction direction)
{
    /* Iterate over in one direction and see if the king is attacked by a queen, rook, or bishop. */
    int i = board->white_king;
    do
    {
        i += direction;
        if (board->position[i] == BLACK_QUEEN)
        {
            return true;
        }
        if (direction == NORTH || direction == EAST || direction == SOUTH || direction == WEST)
        {
            if (board->position[i] == BLACK_ROOK)
            {
                return true;
            }
        }
        else
        {
            if (board->position[i] == BLACK_BISHOP)
            {
                return true;
            }
        }
    } while (!board->position[i]);
    return false;
}

bool is_black_attacked(Board *board, Direction direction)
{
    /* Exactly the same as is_white_attacked(), but for the black king. */
    int i = board->black_king;
    do
    {
        i += direction;
        if (board->position[i] == WHITE_QUEEN)
        {
            return true;
        }
        if (direction == NORTH || direction == EAST || direction == SOUTH || direction == WEST)
        {
            if (board->position[i] == WHITE_ROOK)
            {
                return true;
            }
        }
        else
        {
            if (board->position[i] == WHITE_BISHOP)
            {
                return true;
            }
        }
    } while (!board->position[i]);
    return false;
}

#include <time.h>
long long nodes = 0;
long long qnodes = 0;
Move *start_search(Board *board, int depth)
{
    /* TODO make it similar to actual search. */
    clock_t start = clock();
    Move *moves = malloc(256 * sizeof(*moves));
    Move *best_move = malloc(sizeof(*best_move));
    int best_score = -100000;
    int j = generate_legal_moves(board, moves);
    for (int i = 0; i < j; i++)
    {
        pre_apply_move(&moves[i], board);
        if (!is_checked(board))
        {
            switch_color(board);
            Move *future_moves = malloc(256 * sizeof(*future_moves));
            int *future_move_indexes = malloc(256 * sizeof(*future_move_indexes));
            find_legal_moves(board, future_moves, future_move_indexes);
            int score = -search(board, -10000, 10000, depth - 1, future_moves, future_move_indexes);
            free(future_moves);
            free(future_move_indexes);
            switch_color(board);
            if (score > best_score)
            {
                best_score = score;
                *best_move = moves[i];
            }
        }
        pre_undo_move(&moves[i], board);
    }
    clock_t end = clock();
    double time = ((double) end - start) / CLOCKS_PER_SEC;
    printf("Evaluation of %d. %I64d nodes counted. %I64d qnodes counted. Took %f seconds to compute. %I64d nodes per second.\n", board->color ? best_score : -best_score, nodes, qnodes, time, (long long) ((nodes + qnodes) / time));
    nodes = 0;
    qnodes = 0;
    memset(position_table, 0, position_table_size * sizeof(*position_table));
    free(moves);
    return best_move;
}

int search(Board *board, int alpha, int beta, int depth, Move *moves, int *move_indexes)
{
    PositionBucket *bucket = &position_table[board->hash % position_table_size];
    if (bucket->hash == board->hash && bucket->depth >= depth)
    {
        alpha = bucket->evaluation;
    }
    else
    {
        /* Replace all buckets. */
        bucket->hash = board->hash;
        bucket->depth = depth;
    
        if (depth == 0)
        {
            nodes++;
            bucket->evaluation = quiescence_search(board, alpha, beta);
            return bucket->evaluation;
        }

        /* Generate legal moves and make sure the move with the best evaluation is searched first. */
        Move **future_moves = malloc(256 * sizeof(*future_moves));
        int **future_move_indexes = malloc(256 * sizeof(*future_move_indexes));
        int best_score = -10000;
        int best_move = 0;
        for (int i = 0; move_indexes[i] >= 0; i++)
        {
            apply_move(&moves[move_indexes[i]], board);
            future_moves[i] = malloc(256 * sizeof(*future_moves[i]));
            future_move_indexes[i] = malloc(256 * sizeof(*future_move_indexes[i]));
            int score = evaluate(board, future_moves[i], future_move_indexes[i]);
            undo_move(&moves[move_indexes[i]], board);

            if (score > best_score)
            {
                best_score = score;
                best_move = i;
            }
        }
        int temp_index = move_indexes[0];
        move_indexes[0] = move_indexes[best_move];
        move_indexes[best_move] = temp_index;
        int *temp_move_indexes = future_move_indexes[0];
        future_move_indexes[0] = future_move_indexes[best_move];
        future_move_indexes[best_move] = temp_move_indexes;
        Move *temp_moves = future_moves[0];
        future_moves[0] = future_moves[best_move];
        future_moves[best_move] = temp_moves;

        best_score = -10000;
        for (int i = 0; move_indexes[i] >= 0; i++)
        {
            apply_move(&moves[move_indexes[i]], board);
            int score = -search(board, -beta, -alpha, depth - 1, future_moves[i], future_move_indexes[i]);
            undo_move(&moves[move_indexes[i]], board);

            if (score >= beta)
            {
                bucket->evaluation = score;
                for (int j = i; move_indexes[j] >= 0; j++)
                {
                    free(future_moves[j]);
                    free(future_move_indexes[j]);
                }
                free(future_moves);
                free(future_move_indexes);
                return score;
            }
            else if (score > best_score)
            {
                best_score = score;
                if (score > alpha)
                {
                    alpha = score;
                }
            }

            free(future_moves[i]);
            free(future_move_indexes[i]);
        }

        bucket->evaluation = alpha;
        free(future_moves);
        free(future_move_indexes);
    }

    return alpha;
}

int quiescence_search(Board *board, int alpha, int beta)
{
    qnodes++;
    QuiescenceBucket *bucket = &quiescence_table[board->hash % quiescence_table_size];
    if (bucket->hash == board->hash)
    {
        alpha = bucket->evaluation;
    }
    else
    {
        /* Replace all buckets. */
        bucket->hash = board->hash;

        /* Search all captures in order to stabilize the search. */
        Move *moves = malloc(256 * sizeof(*moves));
        int *move_indexes = malloc(256 * sizeof(*move_indexes));
        int stand_pat = evaluate(board, moves, move_indexes);
        if (stand_pat >= beta)
        {
            free(moves);
            free(move_indexes);
            return beta;
        }
        else if (alpha < stand_pat)
        {
            alpha = stand_pat;
        }

        int best_score = -10000;
        for (int i = 0; move_indexes[i] >= 0; i++)
        {
            if (moves[move_indexes[i]].opponent_piece)
            {
                apply_move(&moves[move_indexes[i]], board);
                int score = -quiescence_search(board, -beta, -alpha);
                undo_move(&moves[move_indexes[i]], board);

                if (score >= beta)
                {
                    free(moves);
                    free(move_indexes);
                    bucket->evaluation = score;
                    return score;
                }
                else if (score > best_score)
                {
                    best_score = score;
                    if (score > alpha)
                    {
                        alpha = score;
                    }
                }
            }
        }

        bucket->evaluation = alpha;
        free(moves);
        free(move_indexes);
    }

    return alpha;
}

int evaluate(Board *board, Move *moves, int *move_indexes)
{
    static const int WHITE_PAWN_EVALUATIONS[100] = {0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0, 100, 100, 100, 100, 100, 100, 100, 100, 0,
                                                    0, 115, 110, 115, 120, 120, 110, 110, 115, 0,
                                                    0, 110, 110, 125, 140, 140, 120, 110, 110, 0,
                                                    0, 115, 110, 135, 150, 150, 130, 110, 115, 0,
                                                    0, 135, 130, 150, 160, 160, 150, 130, 135, 0,
                                                    0, 180, 180, 180, 180, 180, 180, 180, 180, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0};
    
    static const int WHITE_KNIGHT_EVALUATIONS[100] = {0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                      0, 220, 270, 245, 250, 250, 245, 270, 220, 0,
                                                      0, 260, 270, 280, 290, 290, 280, 270, 260, 0,
                                                      0, 250, 285, 300, 280, 280, 300, 285, 250, 0,
                                                      0, 245, 290, 300, 300, 300, 300, 290, 245, 0,
                                                      0, 240, 300, 305, 310, 310, 305, 300, 240, 0,
                                                      0, 290, 310, 315, 330, 330, 315, 310, 290, 0,
                                                      0, 280, 320, 330, 340, 340, 330, 320, 280, 0,
                                                      0, 240, 300, 300, 300, 300, 300, 300, 240, 0,
                                                      0,   0,   0,   0,   0,   0,   0,   0,   0, 0};

    static const int WHITE_BISHOP_EVALUATIONS[100] = {0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                      0, 310, 285, 290, 270, 270, 290, 285, 310, 0,
                                                      0, 310, 320, 300, 310, 310, 300, 320, 310, 0,
                                                      0, 300, 320, 320, 315, 315, 320, 320, 300, 0,
                                                      0, 325, 315, 325, 325, 325, 325, 315, 325, 0,
                                                      0, 320, 330, 330, 325, 325, 330, 330, 320, 0,
                                                      0, 330, 330, 330, 330, 330, 330, 330, 330, 0,
                                                      0, 310, 330, 320, 320, 320, 320, 330, 310, 0,
                                                      0, 300, 300, 300, 300, 300, 300, 300, 300, 0,
                                                      0,   0,   0,   0,   0,   0,   0,   0,   0, 0};
    
    static const int WHITE_ROOK_EVALUATIONS[100] = {0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0, 490, 500, 520, 530, 520, 520, 500, 490, 0,
                                                    0, 480, 500, 510, 540, 540, 510, 500, 480, 0,
                                                    0, 500, 500, 500, 500, 500, 500, 500, 500, 0,
                                                    0, 500, 500, 500, 500, 500, 500, 500, 500, 0,
                                                    0, 500, 500, 500, 500, 500, 500, 500, 500, 0,
                                                    0, 500, 500, 500, 500, 500, 500, 500, 500, 0,
                                                    0, 500, 500, 500, 500, 500, 500, 500, 500, 0,
                                                    0, 500, 500, 500, 500, 500, 500, 500, 500, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0};
    
    static const int WHITE_QUEEN_EVALUATIONS[100] = {0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                     0, 880, 890, 895, 900, 900, 890, 890, 880, 0,
                                                     0, 900, 900, 910, 910, 910, 900, 900, 900, 0,
                                                     0, 900, 900, 900, 900, 900, 900, 900, 900, 0,
                                                     0, 900, 900, 900, 900, 900, 900, 900, 900, 0,
                                                     0, 900, 900, 900, 900, 900, 900, 900, 900, 0,
                                                     0, 900, 900, 910, 910, 910, 910, 900, 900, 0,
                                                     0, 900, 920, 910, 910, 910, 910, 920, 900, 0,
                                                     0, 900, 900, 900, 900, 900, 900, 900, 900, 0,
                                                     0,   0,   0,   0,   0,   0,   0,   0,   0, 0};
    
    static const int WHITE_KING_EVALUATIONS[100] = {0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0,  40,  50,  40, -10,   0,  20,  50,  40, 0,
                                                    0,  20,  10, -10, -30, -30, -10,  10,  20, 0,
                                                    0, -20, -30, -30, -30, -30, -30, -30, -20, 0,
                                                    0, -30, -30, -30, -30, -30, -30, -30, -30, 0,
                                                    0, -30, -30, -30, -30, -30, -30, -30, -30, 0,
                                                    0, -30, -30, -30, -30, -30, -30, -30, -30, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0};
    
    static const int BLACK_PAWN_EVALUATIONS[100] = {0,    0,    0,    0,    0,    0,    0,    0,    0, 0,
                                                    0,    0,    0,    0,    0,    0,    0,    0,    0, 0,
                                                    0, -180, -180, -180, -180, -180, -180, -180, -180, 0,
                                                    0, -135, -130, -150, -160, -160, -150, -130, -135, 0,
                                                    0, -115, -110, -135, -150, -150, -130, -110, -115, 0,
                                                    0, -110, -110, -125, -140, -140, -120, -110, -110, 0,
                                                    0, -115, -110, -115, -120, -120, -110, -110, -115, 0,
                                                    0, -100, -100, -100, -100, -100, -100, -100, -100, 0,
                                                    0,    0,    0,    0,    0,    0,    0,    0,    0, 0,
                                                    0,    0,    0,    0,    0,    0,    0,    0,    0, 0};
    
    static const int BLACK_KNIGHT_EVALUATIONS[100] = {0,    0,    0,    0,    0,    0,    0,    0,    0, 0,
                                                      0, -240, -300, -300, -300, -300, -300, -300, -240, 0,
                                                      0, -280, -320, -330, -340, -340, -330, -320, -280, 0,
                                                      0, -290, -310, -315, -330, -330, -315, -310, -290, 0,
                                                      0, -240, -300, -305, -310, -310, -305, -300, -240, 0,
                                                      0, -245, -290, -300, -300, -300, -300, -290, -245, 0,
                                                      0, -250, -285, -300, -280, -280, -300, -285, -250, 0,
                                                      0, -260, -270, -280, -290, -290, -280, -270, -260, 0,
                                                      0, -220, -270, -245, -250, -250, -245, -270, -220, 0,
                                                      0,    0,    0,    0,    0,    0,    0,    0,    0, 0};
    
    static const int BLACK_BISHOP_EVALUATIONS[100] = {0,    0,    0,    0,    0,    0,    0,    0,    0, 0,
                                                      0, -300, -300, -300, -300, -300, -300, -300, -300, 0,
                                                      0, -310, -330, -320, -320, -320, -320, -330, -310, 0,
                                                      0, -330, -330, -330, -330, -330, -330, -330, -330, 0,
                                                      0, -320, -330, -330, -325, -325, -330, -330, -320, 0,
                                                      0, -325, -315, -325, -325, -325, -325, -315, -325, 0,
                                                      0, -300, -320, -320, -315, -315, -320, -320, -300, 0,
                                                      0, -310, -320, -300, -310, -310, -300, -320, -310, 0,
                                                      0, -310, -285, -290, -270, -270, -290, -285, -310, 0,
                                                      0,    0,    0,    0,    0,    0,    0,    0,    0, 0};
    
    static const int BLACK_ROOK_EVALUATIONS[100] = {0,    0,    0,    0,    0,    0,    0,    0,    0, 0,
                                                    0, -500, -500, -500, -500, -500, -500, -500, -500, 0,
                                                    0, -500, -500, -500, -500, -500, -500, -500, -500, 0,
                                                    0, -500, -500, -500, -500, -500, -500, -500, -500, 0,
                                                    0, -500, -500, -500, -500, -500, -500, -500, -500, 0,
                                                    0, -500, -500, -500, -500, -500, -500, -500, -500, 0,
                                                    0, -500, -500, -500, -500, -500, -500, -500, -500, 0,
                                                    0, -480, -500, -510, -540, -540, -510, -500, -480, 0,
                                                    0, -490, -500, -520, -530, -520, -520, -500, -490, 0,
                                                    0,    0,    0,    0,    0,    0,    0,    0,    0, 0};
    
    static const int BLACK_QUEEN_EVALUATIONS[100] = {0,    0,    0,    0,    0,    0,    0,    0,    0, 0,
                                                     0, -900, -900, -900, -900, -900, -900, -900, -900, 0,
                                                     0, -900, -920, -910, -910, -910, -910, -920, -900, 0,
                                                     0, -900, -900, -910, -910, -910, -910, -900, -900, 0,
                                                     0, -900, -900, -900, -900, -900, -900, -900, -900, 0,
                                                     0, -900, -900, -900, -900, -900, -900, -900, -900, 0,
                                                     0, -900, -900, -900, -900, -900, -900, -900, -900, 0,
                                                     0, -900, -900, -910, -910, -910, -900, -900, -900, 0,
                                                     0, -880, -890, -895, -900, -900, -890, -890, -880, 0,
                                                     0,    0,    0,    0,    0,    0,    0,    0,    0, 0};
    
    static const int BLACK_KING_EVALUATIONS[100] = {0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
                                                    0,  30,  30,  30,  30,  30,  30,  30,  30, 0,
                                                    0,  30,  30,  30,  30,  30,  30,  30,  30, 0,
                                                    0,  30,  30,  30,  30,  30,  30,  30,  30, 0,
                                                    0,  20,  30,  30,  30,  30,  30,  30,  20, 0,
                                                    0, -20, -10,  10,  30,  30,  10, -10, -20, 0,
                                                    0, -40, -50, -40,  10,   0, -20, -50, -40, 0,
                                                    0,   0,   0,   0,   0,   0,   0,   0,   0, 0};

    /* Go through each piece and evaluate it based on its position. */
    int evaluation = 0;
    for (int i = 11; i < 89; i++)
    {
        switch (board->position[i])
        {
            case WHITE_PAWN:
                evaluation += WHITE_PAWN_EVALUATIONS[i];
                break;
            case WHITE_KNIGHT:
                evaluation += WHITE_KNIGHT_EVALUATIONS[i];
                break;
            case WHITE_BISHOP:
                evaluation += WHITE_BISHOP_EVALUATIONS[i];
                break;
            case WHITE_ROOK:
                evaluation += WHITE_ROOK_EVALUATIONS[i];
                break;
            case WHITE_QUEEN:
                evaluation += WHITE_QUEEN_EVALUATIONS[i];
                break;
            case BLACK_PAWN:
                evaluation += BLACK_PAWN_EVALUATIONS[i];
                break;
            case BLACK_KNIGHT:
                evaluation += BLACK_KNIGHT_EVALUATIONS[i];
                break;
            case BLACK_BISHOP:
                evaluation += BLACK_BISHOP_EVALUATIONS[i];
                break;
            case BLACK_ROOK:
                evaluation += BLACK_ROOK_EVALUATIONS[i];
                break;
            case BLACK_QUEEN:
                evaluation += BLACK_QUEEN_EVALUATIONS[i];
                break;
            case OUT_OF_BOUNDS:
                i++;
                break;
        }
    }

    evaluation += WHITE_KING_EVALUATIONS[board->white_king];
    evaluation += BLACK_KING_EVALUATIONS[board->black_king];

    /* Generate legal moves and evaluate mobility.
     * Checks are automatically discouraged since there are going to be fewer moves. */
    if (board->color)
    {
        /* First count the opponent moves in order to avoid using two arrays. */
        evaluation -= find_black_legal_moves(board, moves, move_indexes);
        evaluation += find_white_legal_moves(board, moves, move_indexes);
    }
    else
    {
        evaluation += find_white_legal_moves(board, moves, move_indexes);
        evaluation -= find_black_legal_moves(board, moves, move_indexes);
        evaluation = -evaluation;
    }

    return evaluation;
}

char **split(char *string, char regex)
{
    char **result = malloc(8 * sizeof(*result));
    int result_index = 0;
    char current[100] = {0};
    int current_index = 0;
    /* If anything gets to out-of-bounds, simply return what's already available. */
    for (int i = 0; string[i] && result_index < 8 && current_index < 100; i++)
    {
        if (string[i] == regex)
        {
            result[result_index] = calloc(100, sizeof(*result[result_index]));
            memcpy(result[result_index++], current, sizeof(current));
            memset(current, 0, sizeof(current));
            current_index = 0;
        }
        else
        {
            current[current_index++] = string[i];
        }
    }
    
    /* Don't forget to add the last string to the result. */
    if (result_index < 8 && current_index < 100)
    {
        result[result_index] = calloc(100, sizeof(*result[result_index]));
        memcpy(result[result_index++], current, sizeof(current));

        /* Make sure to initialize the rest of the strings, even though they are empty. */
        while (result_index < 8)
        {
            result[result_index] = calloc(100, sizeof(*result[result_index]));
            result_index++;
        }
    }
    return result;
}

Board *load_fen(char *fen)
{
    Board *board = malloc(sizeof(*board));
    memcpy(board, &EMPTY_BOARD, sizeof(EMPTY_BOARD));
    char **strings = split(fen, ' ');

    /* Load the pieces onto the board. */
    char **ranks = split(strings[0], '/');
    int j = 81;
    for (int i = 0; i < 8; i++)
    {
        int k = 0;
        int l = 0;
        while (l < 8)
        {
            if (ranks[i][k] == 'P')
            {
                board->position[j + l++] = WHITE_PAWN;
            }
            else if (ranks[i][k] == 'N')
            {
                board->position[j + l++] = WHITE_KNIGHT;
            }
            else if (ranks[i][k] == 'B')
            {
                board->position[j + l++] = WHITE_BISHOP;
            }
            else if (ranks[i][k] == 'R')
            {
                board->position[j + l++] = WHITE_ROOK;
            }
            else if (ranks[i][k] == 'Q')
            {
                board->position[j + l++] = WHITE_QUEEN;
            }
            else if (ranks[i][k] == 'K')
            {
                board->position[j + l] = WHITE_KING;
                board->white_king = j + l++;
            }
            else if (ranks[i][k] == 'p')
            {
                board->position[j + l++] = BLACK_PAWN;
            }
            else if (ranks[i][k] == 'n')
            {
                board->position[j + l++] = BLACK_KNIGHT;
            }
            else if (ranks[i][k] == 'b')
            {
                board->position[j + l++] = BLACK_BISHOP;
            }
            else if (ranks[i][k] == 'r')
            {
                board->position[j + l++] = BLACK_ROOK;
            }
            else if (ranks[i][k] == 'q')
            {
                board->position[j + l++] = BLACK_QUEEN;
            }
            else if (ranks[i][k] == 'k')
            {
                board->position[j + l] = BLACK_KING;
                board->black_king = j + l++;
            }
            else if (ranks[i][k] >= '1' && ranks[i][k] <= '8')
            {
                l += ranks[i][k] - '0';
            }
            else
            {
                /* The string ended without actually finishing the rank,
                 * or an invalid character popped up. */
                return board;
            }
            k++;
        }

        if (ranks[i][k])
        {
            /* Too many pieces on a single rank. */
            return board;
        }

        j -= 10;
        k = 0;
        l = 0;
    }

    /* Load the move color. */
    if (!strcmp(strings[1], "w"))
    {
        board->color = true;
    }
    else if (strcmp(strings[1], "b"))
    {
        /* Invalid color. */
        return board;
    }
    
    /* Load castling rights. */
    if (strcmp(strings[2], "-"))
    {
        if (!strings[2][0])
        {
            /* Missing castling rights. */
            return board;
        }

        for (int i = 0; strings[2][i]; i++)
        {
            switch (strings[2][i])
            {
                case 'K':
                    if (board->white_kingside_castle)
                    {
                        /* Duplicate flag. */
                        return board;
                    }
                    board->white_kingside_castle = true;
                    break;
                case 'Q':
                    if (board->white_queenside_castle)
                    {
                        return board;
                    }
                    board->white_queenside_castle = true;
                    break;
                case 'k':
                    if (board->black_kingside_castle)
                    {
                        return board;
                    }
                    board->black_kingside_castle = true;
                    break;
                case 'q':
                    if (board->black_queenside_castle)
                    {
                        return board;
                    }
                    board->black_queenside_castle = true;
                    break;
                default:
                    /* Invalid character. */
                    return board;
            }
        }
    }

    /* Load en passant. */
    if (strings[3][0] >= 'a' && strings[3][0] <= 'h')
    {
        if (strings[3][1] == '3')
        {
            board->en_passant = 41 + strings[3][0] - 'a';
        }
        else if (strings[3][1] == '6')
        {
            board->en_passant = 51 + strings[3][0] - 'a';
        }
        else
        {
            /* Invalid rank. */
            return board;
        }
        
        if (strings[3][2])
        {
            /* Too many characters. */
            return board;
        }
    }
    else if (strcmp(strings[3], "-"))
    {
        /* Invalid file and not a dash. */
        return board;
    }

    for (int i = 0; i < 8; i++)
    {
        free(strings[i]);
        free(ranks[i]);
    }
    free(strings);
    free(ranks);

    hash(board);
    return board;
}

long long threaded_perft(Board *board, int depth, int threads)
{
    if (depth == 0)
    {
        return 1;
    }

    if (threads == 0)
    {
        return 0;
    }

    Move *legal_moves = malloc(256 * sizeof(*legal_moves));
    int j = generate_legal_moves(board, legal_moves);
    /* There are two possible scenarios.
     * If there are less moves than there are threads, then the threads will have to be started a depth down.
     * Otherwise, the threads will be started here. */
    int completed = 0;
    long long result = 0;
    if (j == 1)
    {
        apply_move(&legal_moves[0], board);
        result += threaded_perft(board, depth - 1, threads);
        undo_move(&legal_moves[0], board);
    }
    else if (j < threads)
    {
        /* Divide the work in halves or thirds depending on the number of moves left.
         * Very similar to below, but call threaded_perft() instead of perft(). */
        while (completed < j)
        {
            int current_threads = j - completed == 3 ? 3 : 2;
            int used_threads = 0;
            pthread_t thread_ids[current_threads];
            for (int i = 0; i < current_threads; i++)
            {
                apply_move(&legal_moves[completed + i], board);
                ThreadedPerftArguments *threaded_perft_arguments = malloc(sizeof(*threaded_perft_arguments));
                threaded_perft_arguments->board = malloc(sizeof(*threaded_perft_arguments->board));
                copy(threaded_perft_arguments->board, board);
                threaded_perft_arguments->depth = depth - 1;
                threaded_perft_arguments->threads = i == current_threads - 1 ? threads - used_threads : threads / current_threads;
                used_threads += threads / current_threads;
                pthread_create(&thread_ids[i], NULL, threaded_perft_wrapper, (void *) threaded_perft_arguments);
                undo_move(&legal_moves[completed + i], board);
            }

            for (int i = 0; i < current_threads; i++)
            {
                void *add = malloc(sizeof(long long));
                pthread_join(thread_ids[i], &add);
                result += *((long long *) add);
                free(add);
            }

            completed += current_threads;
            used_threads = 0;
        }
    }
    else
    {
        while (completed < j)
        {
            /* If there are plenty of moves still to go, then start threads in multiples.
             * Otherwise, start a thread for each of the remaining moves. */
            int current_threads = completed + threads * 2 <= j ? threads : j - completed;
            pthread_t thread_ids[current_threads];
            for (int i = 0; i < current_threads; i++)
            {
                apply_move(&legal_moves[completed + i], board);
                PerftArguments *perft_arguments = malloc(sizeof(*perft_arguments));
                perft_arguments->board = malloc(sizeof(*perft_arguments->board));
                copy(perft_arguments->board, board);
                perft_arguments->depth = depth - 1;
                pthread_create(&thread_ids[i], NULL, perft_wrapper, (void *) perft_arguments);
                undo_move(&legal_moves[completed + i], board);
            }

            for (int i = 0; i < current_threads; i++)
            {
                void *add = malloc(sizeof(long long));
                pthread_join(thread_ids[i], &add);
                result += *((long long *) add);
                free(add);
            }

            completed += current_threads;
        }
    }

    free(legal_moves);
    return result;
}

void *threaded_perft_wrapper(void *arguments)
{
    ThreadedPerftArguments *threaded_perft_arguments = (ThreadedPerftArguments *) arguments;
    long long *result = malloc(sizeof(*result));
    *result = threaded_perft(threaded_perft_arguments->board, threaded_perft_arguments->depth, threaded_perft_arguments->threads);
    free(threaded_perft_arguments->board);
    free(threaded_perft_arguments);
    return result;
}

long long perft(Board *board, int depth)
{
    if (depth == 0)
    {
        return 1;
    }

    long long nodes = 0;
    PerftBucket *bucket = &perft_table[board->hash % perft_table_size];
    if (bucket->hash == board->hash && bucket->depth == depth)
    {
        /* Unfortunately, due to how Zobrist Hashing works,
         * there is about 1 in 4 billion chance that there is a collision,
         * which means that large perft results are unlikely to be accurate.
         * For accurate results, board->hash should be changed to a 128-bit integer. */
        nodes = bucket->result;
    }
    else
    {
        Move *moves = malloc(256 * sizeof(*moves));
        int j = generate_moves(board, moves);
        for (int i = 0; i < j; i++)
        {
            pre_apply_move(&moves[i], board);
            if (!is_checked(board))
            {
                switch_color(board);
                nodes += perft(board, depth - 1);
                switch_color(board);
            }
            pre_undo_move(&moves[i], board);
        }
        free(moves);
        
        /* Replace all buckets.
         * There are race conditions that can occur here,
         * but a large enough table should mitigate this issue. */
        bucket->hash = board->hash;
        bucket->depth = depth;
        bucket->result = nodes;
    }

    return nodes;
}

void *perft_wrapper(void *arguments)
{
    PerftArguments *perft_arguments = (PerftArguments *) arguments;
    long long *result = malloc(sizeof(*result));
    *result = perft(perft_arguments->board, perft_arguments->depth);
    free(perft_arguments->board);
    free(perft_arguments);
    return result;
}

char *get_move_string(Move *move, Board *board)
{
    /* TODO remove dependency on board. */

    /* This will return a string like 'Nc3' for piece moves and a string like 'e4' for pawn moves.
     * No special checks are made for special moves, so instead of 'O-O', this will return 'Kg1'. */
    static const char KNIGHT = 'N';
    static const char BISHOP = 'B';
    static const char ROOK = 'R';
    static const char QUEEN = 'Q';
    static const char KING = 'K';

    char place[3] = "";
    char letter = 'a' + move->to % 10 - 1;
    char digit = '1' + move->to / 10 - 1;
    strncat_s(place, sizeof(place), &letter, 1);
    strncat_s(place, sizeof(place), &digit, 1);
    char *final = calloc(4, sizeof(char));
    switch (board->position[move->from] ? board->position[move->from] : board->position[move->to])
    {
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            strncat_s(final, sizeof(final), &KNIGHT, 1);
            break;
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            strncat_s(final, sizeof(final), &BISHOP, 1);
            break;
        case WHITE_ROOK:
        case BLACK_ROOK:
            strncat_s(final, sizeof(final), &ROOK, 1);
            break;
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            strncat_s(final, sizeof(final), &QUEEN, 1);
            break;
        case WHITE_KING:
        case BLACK_KING:
            strncat_s(final, sizeof(final), &KING, 1);
            break;
    }
    strcat_s(final, sizeof(final), place);
    return final;
}

void print_move(Move *move, Board *board)
{
    puts(get_move_string(move, board));
}

char *get_standard_move_string(Move *move, Board *board)
{
    /* TODO implement. */
    return "";
}

void print_standard_move(Move *move, Board *board)
{
    puts(get_standard_move_string(move, board));
}

char *get_board_string(Board *board)
{
    char *result = calloc(72, sizeof(*result));
    int result_index = 0;
    for (int i = 81; i > 10; i++)
    {
        switch (board->position[i])
        {
            case WHITE_PAWN:
                result[result_index++] = 'P';
                break;
            case WHITE_KNIGHT:
                result[result_index++] = 'N';
                break;
            case WHITE_BISHOP:
                result[result_index++] = 'B';
                break;
            case WHITE_ROOK:
                result[result_index++] = 'R';
                break;
            case WHITE_QUEEN:
                result[result_index++] = 'Q';
                break;
            case WHITE_KING:
                result[result_index++] = 'K';
                break;
            case BLACK_PAWN:
                result[result_index++] = 'p';
                break;
            case BLACK_KNIGHT:
                result[result_index++] = 'n';
                break;
            case BLACK_BISHOP:
                result[result_index++] = 'b';
                break;
            case BLACK_ROOK:
                result[result_index++] = 'r';
                break;
            case BLACK_QUEEN:
                result[result_index++] = 'q';
                break;
            case BLACK_KING:
                result[result_index++] = 'k';
                break;
            case EMPTY:
                result[result_index++] = '.';
                break;
            case OUT_OF_BOUNDS:
                result[result_index++] = '\n';
                i -= 19;
                break;
        }
    }
    result[71] = '\0';
    return result;
}

void print_board(Board *board)
{
    puts(get_board_string(board));
}

char *get_full_board_string(Board *board)
{
    char *result = calloc(80, sizeof(*result));
    char *board_string = get_board_string(board);
    memcpy(result, board_string, 72 * sizeof(*board_string));
    free(board_string);
    result[71] = '\n';
    result[72] = board->color ? 'w' : 'b';
    result[73] = board->white_kingside_castle ? 'K' : '-';
    result[74] = board->white_queenside_castle ? 'Q' : '-';
    result[75] = board->black_kingside_castle ? 'k' : '-';
    result[76] = board->black_queenside_castle ? 'q' : '-';
    if (board->en_passant)
    {
        result[77] = board->en_passant % 10 - 1 + 'a';
        result[78] = board->en_passant > 50 ? '6' : '3';
    }
    else
    {
        result[77] = '-';
        result[78] = '-';
    }
    return result;
}

void print_full_board(Board *board)
{
    puts(get_full_board_string(board));
}
