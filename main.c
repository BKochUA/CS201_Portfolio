#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "functions.h"

int currentScreen = 0;
const char *options[] = {"View Today's Log", "View Calendar", "Search for Food", "EXIT"};
int keyPress;
int selected = 0;
int loopvar = 1;
WINDOW *mainMenu;
int day, month, year;

//get current date
void GetCurrentDate()
{
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

}


void ScreenHandler()
{
	switch(currentScreen)
        {
            case 0: //login screen
                //this will be the login screen
                break;
            case 1: //loading screen
                break;
            case 2: //main menu
		GetCurrentDate();
		for(int i = 0; i < 4; i++)
		{
		    mvwprintw(mainMenu, 1, 1, "Welcome Brian, today's date is %d/%d/%d", month, day, year);
		    wattron(mainMenu, A_UNDERLINE);
		    mvwprintw(mainMenu, 3, 13, "Please select an option");
		    wattroff(mainMenu, A_UNDERLINE);
		    if(i == selected)
		    {
		        wattron(mainMenu, A_REVERSE);
		        mvwprintw(mainMenu, i+4, 25-((int)strlen(options[i])/2), options[i]);
		        wattroff(mainMenu, A_REVERSE);
		    }else
		    {
		        mvwprintw(mainMenu, i+4, 25-((int)strlen(options[i])/2), options[i]);
		    }
		}

		keyPress = wgetch(mainMenu);
		switch(keyPress)
		{
		    case KEY_UP:
		        if(selected != 0) selected--;
		        break;
		    case KEY_DOWN:
		        if(selected != 3) selected++;
		        break;
		    case 10:
		        if(selected == 3)
		        {
		            loopvar = 0;
		        }
		        break;
		    default:
		        break;
		}

		wrefresh(mainMenu);
                break;
            case 3: //food search
                break;
            case 4: //food diary
                break;
            case 5: //confirm exit screen
                break;
            default:
                break;
        }
}


int main() {
    currentScreen = 2;
    //read all the data
    FILE *datafile;
    char str[300];
    datafile = fopen("food_nutrient_db.csv", "r");
    int count = 0;
    while(fgets(str, sizeof(str), datafile))
    {
        if(count < 100)
        {
            createItem(str);
        }
        count++;
    }

    fclose(datafile);
    free(datafile);

    printf("done");
    initscr();
    noecho();
    cbreak();
    curs_set(0);


    int sizeY, sizeX;
    getmaxyx(stdscr, sizeY, sizeX);
    WINDOW *mainScreen = newwin(sizeY, sizeX, 0, 0);
    //printf("%d ", sizeY);
    //printf("%d", sizeX);
    box(mainScreen, 0, 0);

    mainMenu = newwin(10, 50, (sizeY/2)-5, (sizeX/2)-25);
    box(mainMenu, 0, 0);
    refresh();
    wrefresh(mainScreen);
    wrefresh(mainMenu);
    keypad(mainMenu, true);

    while(loopvar)
    {
	ScreenHandler();
    }
    wrefresh(mainMenu);

    endwin();
    return 0;
}
