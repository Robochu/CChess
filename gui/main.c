
/* main.c
 * A small GUI program to test CChess.
 * It's a complete mess and not designed to be actually used.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "cchess.h"

#define RANDOM_SEED 1001534112
#define WINDOW_WIDTH 512
#define SQUARE_WIDTH (WINDOW_WIDTH / 8)
#define BLOCK_WIDTH (SQUARE_WIDTH * 2)
#define HALF_WIDTH (SQUARE_WIDTH / 2)
#define QUARTER_WIDTH (HALF_WIDTH / 2)

void divide(Board *board, int depth)
{
    long long result = 0;
    long long total = 0;
    Move *moves = calloc(256, sizeof(*moves));
    int j = generate_moves(board, moves);
    for (int i = 0; i < j; i++)
    {
        pre_apply_move(&moves[i], board);
        if (!is_checked(board))
        {
            switch_color(board);
            result = perft(board, depth - 1);
            switch_color(board);
        }
        pre_undo_move(&moves[i], board);
        if (result)
        {
            printf("%s: %I64d\n", get_move_string(&moves[i], board), result);
        }
        total += result;
        result = 0;
    }
    printf("Total: %I64d\n", total);
}

SDL_Texture *load_texture(SDL_Renderer *renderer, char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        fprintf(stderr, "Unable to load %s: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture)
    {
        fprintf(stderr, "Unable to create a texture from %s: %s\n", path, SDL_GetError());
        return NULL;
    }

    return texture;
}

int main(int argc, char *argv[])
{
    /* Initialize CChess. */
    Board *board = malloc(sizeof(*board));
    copy(board, &STARTING_BOARD);

    /* Generate 1605 random long integers used for hashing and generate the initial hash value for the board. */
    srand(RANDOM_SEED);
    hash_values = calloc(1605, sizeof(*hash_values));
    for (int i = 0; i < 1605; i++)
    {
        if ((i >= 100 && i < 1400) || i > 1500)
        {
            hash_values[i] = ((uint64_t) rand() << 48) ^ ((uint64_t) rand() << 35) ^ ((uint64_t) rand() << 22) ^
                             ((uint64_t) rand() << 9) ^ ((uint64_t) rand() >> 4);
        }
    }
    hash(board);
    printf("%I64u\n", board->hash);

    /* TODO replace sizeof(Type) with sizeof(object) everywhere. */
    /* Perft tests. */
    perft_table_size = 100000000;
    perft_table = calloc(perft_table_size, sizeof(*perft_table));
    for (int i = 1; i < 8; i++)
    {
        clock_t start = clock();
        long long result = threaded_perft(board, i, 12);
        printf("Perft(%d) = %I64d\n", i, result);
        clock_t end = clock();
        double time = ((double) end - start) / CLOCKS_PER_SEC;
        printf("Took %f seconds to compute. %I64d nodes per second.\n", time, (long long) (result / time));
    }
    free(perft_table);
    printf("%I64u\n", board->hash);

    position_table_size = 10000000;
    position_table = calloc(position_table_size, sizeof(*position_table));
    quiescence_table_size = 10000000;
    quiescence_table = calloc(quiescence_table_size, sizeof(*quiescence_table));

    /* Division tests.
    divide(board, 2); */

    /* Initialize SDL. */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
        printf("Warning: linear texture filtering disabled!");
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "Error initializing SDL_image: %s\n", IMG_GetError());
        return 2;
    }

    SDL_Window *window = SDL_CreateWindow("CChess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_WIDTH, 0);
    if (!window)
    {
        fprintf(stderr, "Error creating a window: %s\n", SDL_GetError());
        return 3;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        fprintf(stderr, "Error creating a renderer: %s\n", SDL_GetError());
        return 4;
    }
    SDL_Texture *piece_texture = load_texture(renderer, "chess.png");
    if (!piece_texture)
    {
        fprintf(stderr, "Error loading the piece texture!");
        return 5;
    }

    /* Prepare everything else. */
    Move *moves = calloc(1024, sizeof(Move));
    int move_index = -1;
    Move *move = calloc(1, sizeof(Move));
    move->piece = 0;
    bool debugging = true;

    /* Run the main loop. */
    bool quit = false;
    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        move->from = 81 - event.button.y / SQUARE_WIDTH * 10 + event.button.x / SQUARE_WIDTH;
                        move->piece = board->position[move->from];
                        board->position[move->from] = 0;
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    move->to = 81 - event.button.y / SQUARE_WIDTH * 10 + event.button.x / SQUARE_WIDTH;
                    board->position[move->from] = move->piece;
                    bool legal = false;
                    Move *legal_moves = malloc(256 * sizeof(*legal_moves));
                    int j = generate_legal_moves(board, legal_moves);
                    for (int i = 0; i < j; i++)
                    {
                        if (move->from == legal_moves[i].from && move->to == legal_moves[i].to)
                        {
                            *move = legal_moves[i];
                            legal = true;
                        }
                    }
                    if (event.button.button == SDL_BUTTON_LEFT && move->piece)
                    {
                        if (legal)
                        {
                            apply_move(move, board);
                            moves[++move_index] = *move;
                            for (int i = move_index + 1; i < 1024; i++)
                            {
                                moves[i] = (Move) {0};
                            }
                        }
                        *move = (Move) {0};
                    }
                    free(legal_moves);
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_BACKQUOTE)
                    {
                        debugging = !debugging;
                    }
                    else if (event.key.keysym.sym == SDLK_LEFT)
                    {
                        if (move_index >= 0)
                        {
                            undo_move(&moves[move_index--], board);
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_RIGHT)
                    {
                        if (moves[move_index + 1].from)
                        {
                            apply_move(&moves[++move_index], board);
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_SPACE)
                    {
                        Move *computer_move = start_search(board, 4);
                        apply_move(computer_move, board);
                        moves[++move_index] = *computer_move;
                        for (int i = move_index + 1; i < 1024; i++)
                        {
                            moves[i] = (Move) {0};
                        }
                        free(computer_move);
                    }
                    break;

                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 245, 245, 220, 255);
        SDL_RenderClear(renderer);

        /* Draw the board. */
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                SDL_Rect rect;
                rect.x = x * BLOCK_WIDTH + SQUARE_WIDTH;
                rect.y = y * BLOCK_WIDTH;
                rect.w = SQUARE_WIDTH;
                rect.h = SQUARE_WIDTH;
                SDL_RenderFillRect(renderer, &rect);
                rect.x -= SQUARE_WIDTH;
                rect.y += SQUARE_WIDTH;
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        /* Draw the pieces. Inefficient, but works. */
        SDL_Rect from;
        from.w = 128;
        from.h = 128;
        SDL_Rect to;
        to.w = SQUARE_WIDTH;
        to.h = SQUARE_WIDTH;
        for (int x = 1; x < 9; x++)
        {
            for (int y = 1; y < 9; y++)
            {
                int piece = board->position[y * 10 + x];
                if (piece)
                {
                    if (piece > 0)
                    {
                        from.y = 128;
                    }
                    else
                    {
                        from.y = 0;
                        piece = ~piece;
                    }
                    from.x = (piece - 1) * 128;

                    to.x = (x - 1) * SQUARE_WIDTH;
                    to.y = (8 - y) * SQUARE_WIDTH;

                    SDL_RenderCopy(renderer, piece_texture, &from, &to);
                }
            }
        }

        /* Draw debugging information. */
        if (debugging)
        {
            SDL_Rect king;
            king.w = HALF_WIDTH;
            king.h = HALF_WIDTH;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
            if (is_white_checked(board))
            {
                king.x = (board->white_king % 10 - 1) * SQUARE_WIDTH + QUARTER_WIDTH;
                king.y = (8 - board->white_king / 10) * SQUARE_WIDTH + QUARTER_WIDTH;
                SDL_RenderFillRect(renderer, &king);
            }
            if (is_black_checked(board))
            {
                king.x = (board->black_king % 10 - 1) * SQUARE_WIDTH + QUARTER_WIDTH;
                king.y = (8 - board->black_king / 10) * SQUARE_WIDTH + QUARTER_WIDTH;
                SDL_RenderFillRect(renderer, &king);
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128);
            Move *legal_moves = calloc(256, sizeof(Move));
            int j = generate_legal_moves(board, legal_moves);
            for (int i = 0; i < j; i++)
            {
                int x1 = (legal_moves[i].from % 10 - 1) * SQUARE_WIDTH + HALF_WIDTH;
                int y1 = (8 - legal_moves[i].from / 10) * SQUARE_WIDTH + HALF_WIDTH;
                int x2 = (legal_moves[i].to % 10 - 1) * SQUARE_WIDTH + HALF_WIDTH;
                int y2 = (8 - legal_moves[i].to / 10) * SQUARE_WIDTH + HALF_WIDTH;
                SDL_RenderDrawLine(renderer, x1 + 5, y1 + 10, x2, y2);
            }
            free(legal_moves);
        }

        /* Draw the moving piece. */
        if (move->piece)
        {
            if (move->piece > 0)
            {
                from.y = 128;
                from.x = (move->piece - 1) * 128;
            }
            else
            {
                from.y = 0;
                from.x = (~move->piece - 1) * 128;
            }
            SDL_GetMouseState(&to.x, &to.y);
            to.x -= HALF_WIDTH;
            to.y -= HALF_WIDTH;
            SDL_RenderCopy(renderer, piece_texture, &from, &to);
        }

        SDL_RenderPresent(renderer);
    }

    /* Clean up. */
    SDL_DestroyTexture(piece_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    free(board);
    free(moves);
    free(move);
    piece_texture = NULL;
    renderer = NULL;
    window = NULL;
    board = NULL;

    IMG_Quit();
    SDL_Quit();
    return 0;
}
