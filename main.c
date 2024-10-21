#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"
#include <time.h>

#define SCREEN_WIDTH    1200 
#define SCREEN_HEIGHT   700
#define CANVAS_WIDTH    600
#define CANVAS_HEIGHT   600
#define BLOC_SIZE       20
#define MOV_OFFSET      20

typedef struct
{
	int pos_x;
	int pos_y;
} Snake;

typedef struct
{
	Snake *snake;
	int count;
} SnakeArray;

typedef struct
{
	size_t pos_x;
	size_t pos_y;
} Apple;

typedef struct {
	size_t n_inputs;
	size_t last_input;
	bool print_cell;
} PlayerLog;

void draw_board(SnakeArray *snake, size_t *dir, Apple *apple)
{
	int count = snake->count - 1;
	while (count > 0)
	{
		snake->snake[count].pos_x = snake->snake[count - 1].pos_x;
		snake->snake[count].pos_y = snake->snake[count - 1].pos_y;
		--count;
	}
	switch (*(dir))
	{
		case 0:
			snake->snake[0].pos_x -= MOV_OFFSET;
			break;
		case 1:
			snake->snake[0].pos_x += MOV_OFFSET;
			break;
		case 2:
			snake->snake[0].pos_y += MOV_OFFSET;
			break;
		case 3:
			snake->snake[0].pos_y -= MOV_OFFSET;
			break;
		default:
			break;
		}
	for (size_t i = 0; i < snake->count; ++i)
	{
		DrawRectangle(snake->snake[i].pos_x, snake->snake[i].pos_y, BLOC_SIZE, BLOC_SIZE, BLACK);
	}
	DrawRectangle(apple->pos_x, apple->pos_y, BLOC_SIZE, BLOC_SIZE, GREEN);
	DrawLine(0, CANVAS_HEIGHT, CANVAS_WIDTH, CANVAS_HEIGHT, BLACK);
	DrawLine(CANVAS_WIDTH, 0, CANVAS_WIDTH, CANVAS_HEIGHT, BLACK);
}

void debug(PlayerLog *player_log)
{
	if(player_log->print_cell) {
		for (int i = 0; i < CANVAS_WIDTH; i += 20)
		{
			DrawLine(i, 0, i, CANVAS_HEIGHT, BLACK);
		}
		for (int i = 0; i < CANVAS_HEIGHT; i += 20)
		{
			DrawLine(0, i, CANVAS_WIDTH, i, BLACK);
		}
	}
	DrawText(TextFormat("Number of inputs: %d", player_log->n_inputs), 20, SCREEN_HEIGHT - 40, 20, BLACK);
}

void show_infos(int score)
{
	DrawText(TextFormat("Score: %d", score), 20, SCREEN_HEIGHT - 60, 20, BLACK);
}

bool check_death(SnakeArray *snake)
{
	if ((snake->snake[0].pos_x > CANVAS_WIDTH - MOV_OFFSET) || (snake->snake[0].pos_x < 0))
		return true;
	else if ((snake->snake[0].pos_y > CANVAS_HEIGHT - MOV_OFFSET) || (snake->snake[0].pos_y < 0))
		return true;
	
	if(snake->count > 5) {
		for (int i = snake->count; i > 3; --i) {
			if(snake->snake[0].pos_x == snake->snake[i].pos_x && snake->snake[0].pos_y == snake->snake[i].pos_y)
				return true;
		}
	}
	return false;
}

void update_snake(SnakeArray *snake)
{
	++snake->count;
	snake->snake = (Snake *)realloc(snake->snake, snake->count * sizeof(Snake));
	snake->snake[snake->count - 1].pos_x = snake->snake[snake->count - 2].pos_x;
    snake->snake[snake->count - 1].pos_y = snake->snake[snake->count - 2].pos_y;
}

bool eat(SnakeArray *snake, Apple *apple)
{
	if (snake->snake[0].pos_x == apple->pos_x && snake->snake[0].pos_y == apple->pos_y)
		return true;
	return false;
}

void generate_apple(Apple *apple)
{
	int x = rand() % CANVAS_WIDTH;
	int y = rand() % CANVAS_HEIGHT;

	x = x - (x % 20);
	y = y - (y % 20);
	apple->pos_x = x;
	apple->pos_y = y;
}

void init_game(SnakeArray *snake, PlayerLog *player_log, size_t *dir, bool *is_dead, int *score)
{
	snake->count = 1;
	snake->snake = (Snake *)malloc(sizeof(Snake));
	
	player_log->n_inputs = 0;
	player_log->last_input = 0;
	player_log->print_cell = false;

	*score = 0;
	*is_dead = false;
	*dir = 1;
	
	snake->snake[0].pos_x = (CANVAS_WIDTH / 2);
	snake->snake[0].pos_y = (CANVAS_HEIGHT / 2) - MOV_OFFSET;
}

int main(void)
{
	srand(time(NULL));
	// Init array of snake
	SnakeArray snake = {0};
	Apple apple = {0};
	generate_apple(&apple);
	PlayerLog player_log = {0};

	size_t dir;
	bool is_dead;
	int score;

	init_game(&snake, &player_log, &dir, &is_dead, &score);

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");

	while (!WindowShouldClose())
	{
		BeginDrawing();
			ClearBackground(RAYWHITE);
			if (IsKeyPressed(KEY_R)) {
				generate_apple(&apple);
				init_game(&snake, &player_log, &dir, &is_dead, &score);
			}
			if (dir != -1)
			{
				if (IsKeyPressed(KEY_LEFT) && dir != 1 && dir != 0) {
					dir = 0;
					++player_log.n_inputs;
					player_log.last_input = 0;
				}
				if (IsKeyPressed(KEY_RIGHT) && dir != 0 && dir != 1) {
					dir = 1;
					++player_log.n_inputs;
					player_log.last_input = 1;
				}
				if (IsKeyPressed(KEY_DOWN) && dir != 3 && dir != 2) {
					dir = 2;
					++player_log.n_inputs;
					player_log.last_input = 2;
				}
				if (IsKeyPressed(KEY_UP) && dir != 2 && dir != 3) {
					dir = 3;
					++player_log.n_inputs;
					player_log.last_input = 3;
				}
			}
			else {
				DrawText("Game over", (CANVAS_WIDTH / 2) - 42, (CANVAS_HEIGHT / 2) - 24, 24, BLACK);
				snake.count = 1;
			}
			draw_board(&snake, &dir, &apple);
			if(check_death(&snake)) dir = -1;
			if (eat(&snake, &apple))
			{
				generate_apple(&apple);
				update_snake(&snake);
				++score;
			}
			debug(&player_log);
			show_infos(score);
			DrawText("SOON...", SCREEN_WIDTH / 1.35, SCREEN_HEIGHT / 2.1,24, BLACK);
			WaitTime(0.05);
		EndDrawing();
	}
	free(snake.snake);
	return 0;
}
