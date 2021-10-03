//TODO
//Highligh current branch
//Quit app without selecting brach
//Fix terminal after closing

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h> 

#define WIDTH 100
#define HEIGHT 50 

int startx = 0;
int starty = 0;

char **choices;
int n_choices;
void print_menu(WINDOW *menu_win, int highlight);
FILE *fp;
char stout[1024];
char cmd[1024];
DIR *d;
struct dirent *dir;

typedef struct BRANCES {
	char** branches;
	int n_branches;
} BRANCHES;

void no_git() {
	printf("Git not installed");
	exit(1);
}

void no_repo() {
	printf("Not found Git repository in the current directory");
	exit(1);
}
BRANCHES* get_branches()
{
	//Check git
	sprintf(cmd, "which git");
	fp = popen(cmd, "r");
	if (fp == NULL) {
    		printf("Failed to check git binary\n" );
    		exit(1);
  	}
	if (fgets(stout, sizeof(stout), fp) != NULL) {
		pclose(fp);
		if (stout[0] != '/') no_git();
	}
	else {
		pclose(fp);
		no_git();
	}
	// Get branches
	BRANCHES* res = (BRANCHES*)malloc(sizeof(BRANCHES*));
	d = opendir("./.git/refs/heads");
	int br = 0;
	if (d) {
		choices = (char**)malloc(1 * sizeof(char*));
    		while ((dir = readdir(d)) != NULL) {
			if (br > 0) choices = (char**)realloc(choices, (br+1)*sizeof(char*));
			if (dir->d_name[0] != '.') {
				choices[br] = (char*)malloc(sizeof(dir->d_name));
				strncpy(choices[br], dir->d_name, sizeof(dir->d_name));
				br++;
			}
    		}
    		closedir(d);
  	}
	else no_repo();

	res->branches = choices;
	res->n_branches = br;
	return res;
}

void switch_branch(int choice) {
	if (choices[choice - 1][0] != '*') {
	  sprintf(cmd, "git checkout %s", choices[choice - 1]);
	  fp = popen(cmd, "r");
	  if (fp == NULL) {
		  printf("Error switching branch");
		  exit(1);
	  }
	  pclose(fp);
	}
}

int main()
{	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;

	initscr();
	clear();
	noecho();
	cbreak();	//Line buffering disabled
	startx = 0;
	starty = 0;

	BRANCHES* branches = get_branches();
	choices = branches->branches;
	n_choices = branches->n_branches;

	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	refresh();
	print_menu(menu_win, highlight);
	while(1)
	{	c = wgetch(menu_win);
		switch(c)
		{	case KEY_UP:
				if(highlight == 1)
					highlight = n_choices;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == n_choices)
					highlight = 1;
				else 
					++highlight;
				break;
			case 10:
				choice = highlight;
				break;
			default:
				refresh();
				break;
		}
		print_menu(menu_win, highlight);
		if(choice != 0)	
			break;
	}	
	clrtoeol();
	refresh();
	endwin();

	switch_branch(choice);

	free(branches->branches);
	free(branches);
	system("stty sane");

	return 0;
}


void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;	

	x = 2;
	y = 2;
	for(i = 0; i < n_choices; ++i)
	{	if(highlight == i + 1) // Highlight current branch
		{	wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}
