#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <panel.h>
#include <time.h>
#define ENTER 10
#define no_questions 28

typedef struct questions {
	char the_q[160];
	char answer;
	char variants[4][40];
} questions;

WINDOW *create_newwin(int height, int width, int starty, int startx) {
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);

	return local_win;
}

void read_q(questions **arr_q, FILE *in) {
	int i, j;
	char carry;

	for (i = 0; i < no_questions; ++i) {

		fgets((*arr_q)[i].the_q, 160, in);

		fscanf(in, "%c", &(*arr_q)[i].answer);

		for (j = 0; j < 4; ++j) {
			fscanf(in, "%c", &carry);
			fgets((*arr_q)[i].variants[j], 40, in);
		}
	}
}

int main() {

	FILE *in = fopen("q_file.in", "rt");

	questions *arr_q;
	arr_q = malloc(no_questions * sizeof(questions));
	read_q(&arr_q, in);

	WINDOW *win_menu, *game_win;
	PANEL *panel_menu, *panel_game;
	MENU *my_menu;
	ITEM **my_items_menu;

	char menu_choices[3][20] = {"New Game", "Resume Game", "Exit"};
	char laber_wr[20];

	int row, col, i, viz[28] = {0}, nr = 0, x, score = 0, miss, game_on_going = 0;
	int hide = 0, ok = 1, ok_game = 0, button = 1, idx = 0;

	initscr();			
	keypad(stdscr, TRUE);

	noecho();
	cbreak();
	srand(time(0));

	my_items_menu = (ITEM **)calloc(4, sizeof(ITEM *));
	for (i = 0; i < 3; ++i) {
		my_items_menu[i] = new_item(menu_choices[i], " ");
	}
	my_menu = new_menu((ITEM **)my_items_menu);

	getmaxyx(stdscr, row, col);
	win_menu = create_newwin(row, col, 0, 0);
	game_win = create_newwin(row, col, 0, 0);

	set_menu_win(my_menu, win_menu);
	set_menu_sub(my_menu, derwin(win_menu, 0, 0, row / 2 - 1, col / 2 - 6));

	post_menu(my_menu);
	panel_menu = new_panel(win_menu);

	panel_game = new_panel(game_win);
	hide_panel(panel_game);

	update_panels();
	doupdate();

	while (ok) {
		button = getch();

		switch (button) {
			case KEY_DOWN:
				++idx;
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				--idx;
				break;
		}
		if (idx > 2)
			idx = 2;
		if (idx < 0)
			idx = 0;

		if (button == ENTER) {
			switch (idx) {
				case 0:
					ok_game = 1;
					hide_panel(panel_menu);
					show_panel(panel_game);

					update_panels();
					doupdate();

						nr = 0;
						miss = 0;
						game_on_going = 1;
						score = 0;

						for (i = 0; i < no_questions; ++i)
							viz[i] = 0;

						while (game_on_going) {
							x = rand() % no_questions;
							if (viz[x] == 0) {
								++nr;
								mvwprintw(game_win, 1, 3, arr_q[x].the_q);
								mvwprintw(game_win, 3, 3, arr_q[x].variants[0]);
								mvwprintw(game_win, 3, 35, arr_q[x].variants[1]);
								mvwprintw(game_win, 4, 3, arr_q[x].variants[2]);
								mvwprintw(game_win, 4, 35, arr_q[x].variants[3]);
								mvwprintw(game_win, row - 3, 3, "Press a, b, c or d to select your answer.");
								mvwprintw(game_win, row - 2, 3, "Press q to get to the main menu.");
								sprintf(laber_wr, "%d Points", score);
								mvwprintw(game_win, 9, 10, laber_wr);
								sprintf(laber_wr, "%d Mistakes", miss);
								mvwprintw(game_win, 10, 10, laber_wr);

								
								viz[x] = 1;
								wrefresh(game_win);
								button = -1;

								while(button != 'a' && button != 'b' && button != 'c' &&
								button != 'd' && button != 'q') {

									button = getch();

									if ( button == arr_q[x].answer)
										++score;
									else if (button != 'q' && ( button == 'a'|| button == 'b'||
										button == 'c'|| button == 'd' ))
										++miss;

									if ( button == 'q' ) {
										hide_panel(panel_game);
										show_panel(panel_menu);

										ok_game = 0;
										game_on_going = 0;

										update_panels();
										doupdate();
									}
								}
								
								wclear(game_win);
								box(game_win, 0 , 0);
								if (miss == 3 || nr == no_questions) {
									
									sprintf(laber_wr, "Your score is: %d", score);
									if ( miss == 3 )
										mvwprintw(game_win, 1, 3, "Game Over !!");
									else
										mvwprintw(game_win, 1, 3, "No more questions !!");
									mvwprintw(game_win, row / 2, 3, laber_wr);
									mvwprintw(game_win, row - 2, 3, "Press any key to return to the menu.");
									wrefresh(game_win);
									button = getch();
									wclear(game_win);
									box(game_win, 0 , 0);
									nr = 0;
									miss = 0;
									score = 0;

									hide_panel(panel_game);
									show_panel(panel_menu);
									ok_game = 0;
									game_on_going = 0;
									update_panels();
									doupdate();
								}
							}

						}
					break;
				case 1:
					if (nr >= 1) {
						ok_game = 1;
						game_on_going = 1;

						hide_panel(panel_menu);
						show_panel(panel_game);

						update_panels();
						doupdate();

						wclear(game_win);
						box(game_win, 0 , 0);
						wrefresh(game_win);

						mvwprintw(game_win, 1, 3, arr_q[x].the_q);
						mvwprintw(game_win, 3, 3, arr_q[x].variants[0]);
						mvwprintw(game_win, 3, 35, arr_q[x].variants[1]);
						mvwprintw(game_win, 4, 3, arr_q[x].variants[2]);
						mvwprintw(game_win, 4, 35, arr_q[x].variants[3]);
						mvwprintw(game_win, row - 3, 3, "Press a, b, c or d to select your answer.");
						mvwprintw(game_win, row - 2, 3, "Press q to get to the main menu.");
						sprintf(laber_wr, "%d Points", score);
						mvwprintw(game_win, 9, 10, laber_wr);
						sprintf(laber_wr, "%d Mistakes", miss);
						mvwprintw(game_win, 10, 10, laber_wr);

						wrefresh(game_win);
						button = -1;

						while(button != 'a' && button != 'b' && button != 'c' &&
						button != 'd' && button != 'q') {

							button = getch();

							if ( button == arr_q[x].answer)
								++score;
							else if (button != 'q' && ( button == 'a'|| button == 'b'||
								button == 'c'|| button == 'd' ))
								++miss;

							if ( button == 'q' ) {
								hide_panel(panel_game);
								show_panel(panel_menu);

								ok_game = 0;
								game_on_going = 0;

								update_panels();
								doupdate();
							}
						}

						while (game_on_going) {
							x = rand() % no_questions;
							if (viz[x] == 0) {

								++nr;
								mvwprintw(game_win, 1, 3, arr_q[x].the_q);
								mvwprintw(game_win, 3, 3, arr_q[x].variants[0]);
								mvwprintw(game_win, 3, 35, arr_q[x].variants[1]);
								mvwprintw(game_win, 4, 3, arr_q[x].variants[2]);
								mvwprintw(game_win, 4, 35, arr_q[x].variants[3]);
								mvwprintw(game_win, row - 3, 3, "Press a, b, c or d to select your answer.");
								mvwprintw(game_win, row - 2, 3, "Press q to get to the main menu.");
								sprintf(laber_wr, "%d Points", score);
								mvwprintw(game_win, 9, 10, laber_wr);
								sprintf(laber_wr, "%d Mistakes", miss);
								mvwprintw(game_win, 10, 10, laber_wr);

								
								viz[x] = 1;
								wrefresh(game_win);
								button = -1;

								while(button != 'a' && button != 'b' && button != 'c' &&
								button != 'd' && button != 'q') {

									button = getch();

									if ( button == arr_q[x].answer)
										++score;
									else if (button != 'q' && ( button == 'a'|| button == 'b'||
										button == 'c'|| button == 'd' ))
										++miss;

									if ( button == 'q' ) {
										hide_panel(panel_game);
										show_panel(panel_menu);

										ok_game = 0;
										game_on_going = 0;

										update_panels();
										doupdate();
									}
								}
								
								wclear(game_win);
								box(game_win, 0 , 0);
								if (miss == 3 || nr == no_questions) {
									
									sprintf(laber_wr, "Your score is: %d", score);
									if ( miss == 3 )
										mvwprintw(game_win, 1, 3, "Game Over !!");
									else
										mvwprintw(game_win, 1, 3, "No more questions !!");
									mvwprintw(game_win, row / 2, 3, laber_wr);
									mvwprintw(game_win, row - 2, 3, "Press any key to return to the menu.");
									wrefresh(game_win);
									button = getch();
									wclear(game_win);
									box(game_win, 0 , 0);
									nr = 0;
									miss = 0;

									hide_panel(panel_game);
									show_panel(panel_menu);

									game_on_going = 0;
									update_panels();
									doupdate();
								}
								
							}
						}

					}
					break;
				case 2:
					ok = 0;
					break;
			}
		}

		wrefresh(win_menu);
		update_panels();
		doupdate();
	}
	clear();

	fclose(in);
	wrefresh(game_win);
	wrefresh(win_menu);
	
	del_panel(panel_menu);
	del_panel(panel_game);
	delwin(game_win);
	delwin(win_menu);
	unpost_menu(my_menu);
    free_menu(my_menu);
    free(arr_q);
    for(i = 0; i < 4; ++i){
        free_item(my_items_menu[i]);
    }
    free(my_items_menu);

	endwin();
	return 0;
}