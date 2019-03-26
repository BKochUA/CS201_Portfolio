#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include "functions.h"

int currentScreen = 0;
const char *options[] = {"View Today's Log", "Search for Food", "Return to Login", "EXIT"};
int keyPress;
int selected = 0;
int loopvar = 1;
WINDOW *mainScreen, *mainMenu, *loginScreen, *searchBar, *searchResults;
struct TreeNode *trieTree;
int day, month, year;
char username[20];
char searchKey[75];
int ch;

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
                box(loginScreen, 0, 0);
                mvwprintw(loginScreen, 1, 1, "Please enter your username");
                wmove(loginScreen, 2, 1);
                if(strlen(username) == 0)
                {
                    wattron(loginScreen, A_UNDERLINE);
                    mvwprintw(loginScreen, 2, 1, "                    ");
                    wattroff(loginScreen, A_UNDERLINE);
                }
                wrefresh(loginScreen);
                ch = getch();
                if(ch == 27) //escape is pressed, exit program
                {
                    loopvar = 0;
                }else if(ch == 10) //enter is pressed, go to next screen
                {
                    currentScreen = 2;
                    wclear(loginScreen);
                }else if((ch > 64 && ch < 91) || (ch > 96 && ch < 123) || ch == 127 || ch == 32 || ch == 95) //this only allows letters, spaces, underscores, and backspaces through when inputting username
                {
                    if(ch == 127 && strlen(username) != 0) //handles backspacing
                    {
                        username[strlen(username)-1] = '\0';
                        wattron(loginScreen, A_UNDERLINE);
                        mvwprintw(loginScreen, 2, 1, "                    ");
                        mvwprintw(loginScreen, 2, 1, "%s", username);
                        wattroff(loginScreen, A_UNDERLINE);
                        //mvwprintw(loginScreen, 2, (int)strlen(username), "%c", 0);
                    }else if(strlen(username) < 20 && ch != 127) //puts characters into username string and on screen
                    {
                        if(ch == 32)
                        {
                            username[strlen(username)] = (char)95;
                        }else {
                            username[strlen(username)] = (char) ch;
                        }
                        wattron(loginScreen, A_UNDERLINE);
                        mvwprintw(loginScreen, 2, 1, "%s", username);
                        wattroff(loginScreen, A_UNDERLINE);
                        //mvwprintw(loginScreen, 2, (int)strlen(username), "%c", ch);
                    }
                    if(strlen(username) == 20)
                    {
                        mvwprintw(loginScreen, 3, 1, "You have reached the character limit (20)");
                    }else { mvwprintw(loginScreen, 3, 1, "                                         "); }

                }
                wrefresh(loginScreen);
                break;
            case 1: //loading screen
                break;
            case 2: //main menu
                box(mainMenu, 0, 0);
                GetCurrentDate();
                for(int i = 0; i < 4; i++)
                {
                    mvwprintw(mainMenu, 1, 1, "Welcome %s, today's date is %d/%d/%d", username, month, day, year);
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
                        if(selected == 0)
                        {

                        }else if(selected == 1)
                        {
                            currentScreen = 3;
                            wclear(mainMenu);
                        }else if(selected == 2)
                        {
                            currentScreen = 0; //sets current screen to loginscreen
                            memset(username, 0, strlen(username)); //clears the username
                            wclear(mainMenu);
                        }else if(selected == 3)
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
                box(mainMenu, 0, 0);
                box(searchResults, 0, 0);
                box(searchBar, 0, 0);
                mvwprintw(mainScreen, 1, 1, "Enter the name of the food you're looking for:");
                wrefresh(mainScreen);
                wrefresh(searchBar);
                wrefresh(searchResults);
                ch = getchar();
                if(strlen(searchKey) == 0)
                {
                    mvwprintw(searchBar, 1, 1, "                    ");
                }
                //mvwprintw(searchResults, 1, 1, "%d", ch);
                //wrefresh(searchResults);
                if(ch == 13)
                {
                    //mvwprintw(searchResults, 1, 1, "pressed enter");
                    wrefresh(searchResults);
                    //these 2 lines are just here so I can read the error summary from valgrind
                    //loopvar = 0;
                    //endwin();

                    struct TreeNode *temp = searchWord(trieTree, searchKey);
                    if(temp != NULL)
                    {
                        bool childrenThere = false;

                        for(int j = 0; j < 256; ++j)
                        {
                            if(temp->children[j] != NULL)
                            {
                                childrenThere = true;
                                break;
                            }
                        }

                        if(childrenThere)
                        {
                            char printUtil[199];
                            //char *printUtil;

                            printAllWords(temp, printUtil, 0, searchResults);

                            //lexicographPrint(temp, printUtil, searchKey, searchResults);
                        }else { mvwprintw(searchResults, 1, 1, "no match                        "); }
                    }else { mvwprintw(searchResults, 1, 1, "no match                        "); }
                }else if(ch == 27)
                {
                    //mvwprintw(searchResults, 1, 1, "pressed enter");
                    wrefresh(searchResults);
                    loopvar = 0;
                }else
                {
                    if(ch == 127 && strlen(searchKey) != 0) //handles backspacing
                    {
                        searchKey[strlen(searchKey)-1] = '\0';

                        mvwprintw(searchBar, 1, 1, "                                                                                        ");
                        mvwprintw(searchBar, 1, 1, "%s", searchKey);
                        //mvwprintw(loginScreen, 2, (int)strlen(username), "%c", 0);
                    }else if(strlen(searchKey) < 75 && ch != 127) //puts characters into username string and on screen
                    {
                        searchKey[strlen(searchKey)] = (char)toupper(ch);
                        mvwprintw(searchBar, 1, 1, "%s", searchKey);
                        //mvwprintw(loginScreen, 2, (int)strlen(username), "%c", ch);
                    }
                    if(strlen(searchKey) == 75)
                    {
                        //mvwprintw(searchBar, 1, 1, "You have reached the character limit (20)");
                    }//else { //mvwprintw(searchBar, 1, 1, "                                         "); }

                }
                wrefresh(searchBar);
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
    currentScreen = 0;
    //read all the data
    FILE *datafile;
    char str[300];
    datafile = fopen("food_nutrient_db.csv", "r");
    int count = 0;
    //strcpy(username, "Brian");

    trieTree = (struct TreeNode *) calloc(1, sizeof(struct TreeNode));
    while(fgets(str, sizeof(str), datafile))
    {
        if(count < 100)
        {
            //struct FoodItem *newItem;
            //*newItem = createItem(str);
            insert2(trieTree, str);
        }
        count++;
    }

    fclose(datafile);
    free(datafile);

    initscr();
    //resizeterm(24, 80);
    noecho();
    cbreak();
    curs_set(0);


    int sizeY, sizeX;
    getmaxyx(stdscr, sizeY, sizeX);
    mainScreen = newwin(sizeY, sizeX, 0, 0);
    //printf("%d ", sizeY);
    //printf("%d", sizeX);
    box(mainScreen, 0, 0);

    mainMenu = newwin(10, 50, (sizeY/2)-5, (sizeX/2)-25);
    refresh();
    wrefresh(mainScreen);
    wrefresh(mainMenu);
    keypad(mainMenu, true);

    loginScreen = newwin(10,50, (sizeY/2)-5, (sizeX/2)-25);
    box(loginScreen, 0, 0);
    refresh();
    wrefresh(loginScreen);


    searchBar = newwin(3, sizeX-2, 2, 1);
    searchResults = newwin(sizeY-6, sizeX-2, 5, 1);
    refresh();
    wrefresh(searchBar);
    wrefresh(searchResults);
    wrefresh(mainScreen);



    while(loopvar)
    {
	    ScreenHandler();
    }
    wrefresh(mainMenu);


    endwin();
    free(trieTree);
    return 0;
}
