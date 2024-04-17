#include "primlib.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_OF_POLES 3
#define NUM_OF_DISKS 4
#define RECTANGLE_POINTS 4
#define DISK_WIDTH 20
#define FLOOR_HEIGHT 50
#define DISK_HEIGHT 15
#define BAD_CHOICE -1
#define GAME_EXIT -2

int gamestate[NUM_OF_POLES][NUM_OF_DISKS];
int coords_of_disks[NUM_OF_DISKS][RECTANGLE_POINTS]; // stored (x1,y1,x2,y2)
int ref_coords_array[NUM_OF_DISKS][RECTANGLE_POINTS];
int coords_of_poles[NUM_OF_POLES]; // just x coords

void generate_poles()
{
	for (int i = 1; i <= NUM_OF_POLES; i++) {
		gfx_filledRect((gfx_screenWidth() / (NUM_OF_POLES + 1)) * i,
					   2 * gfx_screenHeight() / 3 - 1,
					   (gfx_screenWidth() / (NUM_OF_POLES + 1)) * i +
						   30 / NUM_OF_POLES,
					   gfx_screenHeight() - FLOOR_HEIGHT - 1, YELLOW);
	}
}

void upload_disks_to_gamestate()
{
	for (int k = 0; k < NUM_OF_POLES; k++) {
		for (int j = 0; j < NUM_OF_DISKS; j++) {
			gamestate[k][j] = 0;
		}
	}
	for (int i = 0; i < NUM_OF_DISKS; i++) {
		gamestate[0][i] = i + 1;
	}
}

void create_coords(int array[NUM_OF_DISKS][RECTANGLE_POINTS])
{
	for (int i = NUM_OF_DISKS - 1; i >= 0; i--) {
		array[i][0] = coords_of_poles[0] - (((i + 1) * DISK_WIDTH)/2);
		array[i][1] = (gfx_screenHeight() - FLOOR_HEIGHT) -
					  (DISK_HEIGHT * (NUM_OF_DISKS - i - 1)) - 1;
		array[i][2] = coords_of_poles[0] + (((i + 1) * DISK_WIDTH)/2);
		array[i][3] = (gfx_screenHeight() - FLOOR_HEIGHT) -
					  (DISK_HEIGHT * (NUM_OF_DISKS - i - 1)) - DISK_HEIGHT - 1;
	}
}

void draw_disks()
{
	for (int i = 0; i < NUM_OF_DISKS; i++) {
		gfx_filledRect(coords_of_disks[i][0], coords_of_disks[i][1],
					   coords_of_disks[i][2] - 1, coords_of_disks[i][3] + 1,
					   CYAN);
		gfx_rect(coords_of_disks[i][0], coords_of_disks[i][1] + 1,
				 coords_of_disks[i][2], coords_of_disks[i][3] + 1, RED);
	}
}

void draw_on_screen()
{
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
	gfx_filledRect(0, gfx_screenHeight() - 50, gfx_screenWidth(),
				   gfx_screenHeight(), BLUE);

	generate_poles();
	draw_disks();
}

int peek(int column)
{
	column = column - 1;
	if (gamestate[column][NUM_OF_DISKS - 1] == 0) {
		return 0;
	}
	else {
		for (int i = NUM_OF_DISKS - 1; i >= 0; i--) {
			if (gamestate[column][i] == 0) {
				return gamestate[column][i + 1];
			}
		}
		return 1;
	}
}

int peek_for_index(int column)
{
	column = column - 1;
	for (int i = NUM_OF_DISKS - 1; i >= 0; i--) {
		if (gamestate[column][i] == 0) {
			return i;
		}
	}
	return -1;
}

void fill_poles_coords()
{
	for (int i = 1; i <= NUM_OF_POLES; i++) {
		coords_of_poles[i - 1] =
			(((gfx_screenWidth() / (NUM_OF_POLES + 1)) * i) +
			 (((gfx_screenWidth() / (NUM_OF_POLES + 1)) * i +
			   30 / NUM_OF_POLES))) /
			2;
	}
}

void move(int disk_fr, int disk_t, int pole_fr, int pole_t)
{
	int index_from, index_to;
	index_from = peek_for_index(pole_fr);
	index_to = peek_for_index(pole_t);

	for (int loop_counter = 0; loop_counter != 3;) {
		if (loop_counter == 0) {
			coords_of_disks[disk_fr - 1][1] -= 1;
			coords_of_disks[disk_fr - 1][3] -= 1;
			if (coords_of_disks[disk_fr - 1][1] <= gfx_screenHeight() / 4) {
				loop_counter += 1;
			}
		}
		if (loop_counter == 1) {
			if (pole_fr - 1 < pole_t - 1) {
				coords_of_disks[disk_fr - 1][0] += 1;
				coords_of_disks[disk_fr - 1][2] += 1;
			}
			else {
				coords_of_disks[disk_fr - 1][0] -= 1;
				coords_of_disks[disk_fr - 1][2] -= 1;
			}
			if (((coords_of_disks[disk_fr - 1][0] +
				  coords_of_disks[disk_fr - 1][2]) /
				 2) == coords_of_poles[pole_t - 1]) {
				loop_counter += 1;
			}
		}
		if (loop_counter == 2) {
			coords_of_disks[disk_fr - 1][1] += 1;
			coords_of_disks[disk_fr - 1][3] += 1;
			if (coords_of_disks[disk_fr - 1][3] ==
				ref_coords_array[index_to][3]) {
				loop_counter += 1;
			}
		}
		draw_on_screen();
		gfx_updateScreen();
	}
	gamestate[pole_fr - 1][index_from + 1] = 0;
	gamestate[pole_t - 1][index_to] = disk_fr;
}

int get_input()
{
	int key = gfx_getkey();
	int pole = key - 48;
	if (pole >= 0 && pole <= NUM_OF_POLES) {
		if (pole == 0 && NUM_OF_POLES != 10) {
			return BAD_CHOICE;
		}
		else {
			if (pole == 0) {
				return 10;
			}
			else {
				return pole;
			}
		}
	}
	else if (pole == -21) {
		return GAME_EXIT;
	}
	else {
		return BAD_CHOICE;
	}
}

int is_win()
{
	if (gamestate[NUM_OF_POLES - 1][0] == 1) {
		printf("YOU WIN!");
		return 1;
	}
	else {
		return 0;
	}
}

void execute_game(int pole_fr, int pole_t){
	int disk_fr = peek(pole_fr);
	int disk_t = peek(pole_t);
	if ((disk_fr < disk_t || disk_t == 0) && disk_fr != 0) {
		move(disk_fr, disk_t, pole_fr, pole_t);
		}
	else {
		printf("wrong move!\n");
	}
}

int main(int argc, char* argv[])
{
	if (gfx_init()) {
		exit(3);
	}

	fill_poles_coords();
	upload_disks_to_gamestate(gamestate);
	create_coords(coords_of_disks);
	create_coords(ref_coords_array);
	char key_from, key_to;

	while (1) {
		draw_on_screen();
		gfx_updateScreen();

		key_from = get_input();
		if (key_from == GAME_EXIT) {
			return 0;
		}
		key_to = get_input();
		if (key_from == BAD_CHOICE || key_to == BAD_CHOICE) {
			printf("not a valid move\n");
		}
		else if (key_to == GAME_EXIT) {
			return 0;
		}
		else {
			execute_game(key_from, key_to);
		}
		if (is_win() == 1) {
			return 0;
		}
	}
	return 0;
}
