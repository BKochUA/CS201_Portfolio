#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include "functions.h"

FILE *diaryFile;
int currentScreen = 0;
const char *options[] = {"View Today's Log", "Search for Food", "Return to Login", "EXIT"};
const char *itemOptions[] = {"Add to Log", "Return to Search", "Return to Main Menu"};
const char *diaryOptions[] = {"Add Item", "Update Item", "Delete Item", "Return To Main Menu"};
int keyPress;
int selected = 0;
int loopvar = 1;
WINDOW *mainScreen, *mainMenu, *loginScreen, *searchBar, *searchResults, *searchResultsBox, *itemDetails, *diaryScreen;
struct FoodItem results[40];
int numResults = 0;
struct FoodItem *displayedItem;
struct RadixNode *radixTree;
int day, month, year;
char username[20];
char searchKey[75];
int sizeY, sizeX;
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

void freeMemory(struct TreeNode *node)
{
    if(node == NULL)
    {
        return;
    }
    if(node->isLeaf)
    {
        free(node->items);
        free(node);
    }
    for(int i=0; i<256; i++)
    {
        if(node->children[i] != NULL)
        {
            freeMemory(node->children[i]);
        }
    }
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
                wrefresh(mainMenu);
                ch = wgetch(loginScreen);
                switch(ch)
                {
                    case 27: //escape is pressed, exit program
                        loopvar = 0;
                        break;
                    case 10: //enter so go to next screen and load diary of username
                        if(strlen(username))
                        {
                            char *diaryFileName = malloc(30*sizeof(char));
                            strncpy(diaryFileName, "logs/", 5);
                            strncpy(diaryFileName+5, username, strlen(username));
                            strncpy(diaryFileName+5+strlen(username), ".log", 4);
                            diaryFileName[strlen(username)+9] = '\0';
                            if((diaryFile = fopen(diaryFileName, "r+")) == NULL)
                            {
                                diaryFile = fopen(diaryFileName, "w+");
                                //mvwprintw(mainScreen, 1, 1, "-%s-", diaryFileName);
                            }
                            free(diaryFileName);
                            loadUserDiary(diaryFile);
                        }else
                        {
                            mvwprintw(loginScreen, 3, 1, "Username must be at least one character long");
                        }

                        wrefresh(mainScreen);
                        currentScreen = 2;
                        wclear(loginScreen);
                        break;
                    case 127: //backspacing
                        if(strlen(username) != 0)
                        {
                            username[strlen(username)-1] = '\0';
                            wattron(loginScreen, A_UNDERLINE);
                            mvwprintw(loginScreen, 2, 1, "                    ");
                            mvwprintw(loginScreen, 2, 1, "%s", username);
                            wattroff(loginScreen, A_UNDERLINE);
                        }
                        break;
                    default:
                        if(((ch > 64 && ch < 91) || (ch > 96 && ch < 123) || ch == 32 || ch == 95) && strlen(username) < 20) //this only allows letters, spaces, and underscores through when inputting username
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
                        }
                        if(strlen(username) == 20)
                        {
                            mvwprintw(loginScreen, 3, 1, "You have reached the character limit (20)");
                        }else { mvwprintw(loginScreen, 3, 1, "                                            "); }
                        break;
                }
                wrefresh(loginScreen);
                break;
            case 1: //loading screen
                break;
            case 2: //main menu
                wclear(mainScreen);
                box(mainScreen, 0, 0);
                wrefresh(mainScreen);
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
                        if(selected == 0) //go to food diary
                        {
                            currentScreen = 5;
                            wclear(mainMenu);
                        }else if(selected == 1) //go to search screen
                        {
                            currentScreen = 3;
                            memset(searchKey, 0, strlen(searchKey));
                            wclear(mainMenu);
                        }else if(selected == 2) //return to login screen
                        {
                            currentScreen = 0; //sets current screen to loginscreen
                            memset(username, 0, strlen(username)); //clears the username
                            wclear(mainMenu);
                        }else if(selected == 3) //exits the program
                        {
                            loopvar = 0;
                        }
                        selected = 0;
                        break;
                    default:
                        break;
                }
                wrefresh(mainMenu);
                break;
            case 3: //food search
                box(mainScreen, 0, 0);
                box(searchResultsBox, 0, 0);
                box(searchBar, 0, 0);
                mvwprintw(mainScreen, 1, 1, "Enter the name of the food you're looking for:");
                wrefresh(mainScreen);
                wrefresh(searchResultsBox);
                wrefresh(searchBar);
                wrefresh(searchResults);
                keyPress = wgetch(searchBar);
                if(strlen(searchKey) == 0)
                {
                    mvwprintw(searchBar, 1, 1, "                               ");
                }
                switch(keyPress)
                {
                    case KEY_UP:
                        if(selected != 0) selected--;
                        goto SHOWRESULTS;
                        break;
                    case KEY_DOWN:
                        if(selected != numResults) selected++;
                        goto SHOWRESULTS;
                        break;
                    SHOWRESULTS:
                        //print all the results (if there are any?)
                        wclear(searchResults);
                        for(int z = 0; z < numResults; z++)
                        {
                            if(z+1 == selected)
                            {
                                wattron(searchResults, A_REVERSE);
                                mvwprintw(searchResults, z, 0, "item: %s - mfg: %s", results[z].name, results[z].mfg);
                                wattroff(searchResults, A_REVERSE);
                            }else
                            {
                                mvwprintw(searchResults, z, 0, "item: %s - mfg: %s", results[z].name, results[z].mfg);
                            }
                        }
                        //wrefresh(searchResults);
                        break;
                    case 10:
                        wrefresh(searchResults);
                        wclear(searchResults);
                        //these 2 lines are just here so I can read the error summary from valgrind
                        //loopvar = 0;
                        //endwin();

                        if(selected == 0)
                        {
                            char *printUtility = malloc(200 * sizeof(char));
                            numResults = findByPrefix(radixTree, searchKey, 0, results, printUtility);
                            for (int q = 0; q < numResults; q++) {
                                mvwprintw(searchResults, q, 0, "item: %s - mfg: %s", results[q].name, results[q].mfg);
                            }
                            free(printUtility);
                        }else
                        {
                            displayedItem = &results[selected-1];
                            selected = 0;
                            currentScreen = 4;
                            memset(searchKey, 0, strlen(searchKey));
                            wclear(searchBar);
                            wclear(searchResults);
                            wclear(searchResultsBox);
                        }
                        break;
                    case 27:
                        wclear(mainScreen);
                        wrefresh(mainScreen);
                        wclear(searchBar);
                        wclear(searchResultsBox);
                        wclear(searchResults);
                        wrefresh(searchResults);
                        wrefresh(searchBar);
                        wrefresh(searchResultsBox);
                        currentScreen = 2;
                        break;
                    case KEY_BACKSPACE:
                        if (strlen(searchKey) != 0) {
                            searchKey[strlen(searchKey) - 1] = '\0';
                            mvwprintw(searchBar, 1, 1, "                                                                                        ");
                            mvwprintw(searchBar, 1, 1, "%s", searchKey);
                            //mvwprintw(loginScreen, 2, (int)strlen(username), "%c", 0);
                        }
                        break;
                    default:
                        if (strlen(searchKey) < 75) {
                            searchKey[strlen(searchKey)] = (char) toupper(keyPress);
                            mvwprintw(searchBar, 1, 1, "%s", searchKey);
                            //mvwprintw(loginScreen, 2, (int)strlen(username), "%c", ch);
                        } else if (strlen(searchKey) == 75) {
                            //mvwprintw(searchBar, 1, 1, "You have reached the character limit (20)");
                        }
                        break;
                }
                wrefresh(searchBar);
                break;
            case 4: //item details
                wclear(mainScreen);
                box(mainScreen, 0, 0);
                wrefresh(mainScreen);
                mvwprintw(itemDetails, 0, 0, "Item Details:");
                mvwprintw(itemDetails, 1, 0, "Name:            %s", displayedItem->name);
                mvwprintw(itemDetails, 2, 0, "Manufacturer:    %s", displayedItem->mfg);
                if(displayedItem->grams)
                {
                    mvwprintw(itemDetails, 3, 0, "Serving Size:    %.2f %s (%dg)", displayedItem->servingSize, displayedItem->servingUnits, displayedItem->servingWeight);
                }else
                {
                    mvwprintw(itemDetails, 3, 0, "Serving Size:    %.2f %s (%d%ml)", displayedItem->servingSize, displayedItem->servingUnits, displayedItem->servingWeight);
                }
                mvwprintw(itemDetails, 4, 0, "Calories:        %.2f", displayedItem->calories);
                mvwprintw(itemDetails, 5, 0, "Total Fat        %.2fg", displayedItem->fat);
                mvwprintw(itemDetails, 6, 0, "Carbohydrate:    %.2fg", displayedItem->carbs);
                mvwprintw(itemDetails, 7, 0, "Protein:         %.2fg", displayedItem->protein);
                //wclear(itemDetails);
                for(int w = 0; w < 3; w++)
                {
                    if(w == selected)
                    {
                        wattron(itemDetails, A_REVERSE);
                        mvwprintw(itemDetails, w+9, 25-((int)strlen(itemOptions[w])/2), itemOptions[w]);
                        wattroff(itemDetails, A_REVERSE);
                    }else
                    {
                        mvwprintw(itemDetails, w+9, 25-((int)strlen(itemOptions[w])/2), itemOptions[w]);
                    }
                }
                keyPress = wgetch(itemDetails);
                switch(keyPress)
                {
                    case KEY_UP:
                        if(selected != 0) selected--;
                        break;
                    case KEY_DOWN:
                        if(selected < 2) selected++;
                        break;
                    case 10:
                        if(selected == 0)
                        {

                        }else if(selected == 1)
                        {
                            currentScreen = 3;
                            wclear(itemDetails);
                        }else{
                            currentScreen = 2;
                            wclear(itemDetails);
                        }
                        selected = 0;
                        break;
                    default:
                        break;
                }
                wrefresh(itemDetails);
                break;
            case 5: //food diary
                wclear(mainScreen);
                box(mainScreen, 0, 0);
                wrefresh(mainScreen);
                mvwprintw(diaryScreen, 0, 0, "Food log for %d/%d/%d", month, day, year);

                for(int c = 0; c < 4; c++) //print options
                {
                    if(c == selected)
                    {
                        wattron(diaryScreen, A_REVERSE);
                        mvwprintw(diaryScreen, c+9, 25-((int)strlen(diaryOptions[c])/2), diaryOptions[c]);
                        wattroff(diaryScreen, A_REVERSE);
                    }else
                    {
                        mvwprintw(diaryScreen, c+9, 25-((int)strlen(diaryOptions[c])/2), diaryOptions[c]);
                    }
                }
                keyPress = wgetch(diaryScreen);
                switch(keyPress)
                {
                    case KEY_UP:
                        if(selected != 0) selected--;
                        break;
                    case KEY_DOWN:
                        if(selected < 3) selected++;
                        break;
                    case 10:
                        if(selected == 0) //add item, launches search screen
                        {
                            currentScreen = 3;
                            memset(searchKey, 0, strlen(searchKey));
                            wclear(diaryScreen);
                        }else if(selected == 1) //update item
                        {

                        }else if(selected == 2) //delete item
                        {

                        }else {
                            wclear(mainScreen);
                            wrefresh(mainScreen);
                            wclear(diaryScreen);
                            currentScreen = 2;
                            selected = 0;
                        }
                        break;
                    default:
                        break;
                }
                wrefresh(diaryScreen);
                break;
            case 6: //confirm exit screen
                break;
            default:
                break;
        }
}


int main() {
    //system("/usr/bin/resize -s 10 100");
    currentScreen = 0;
    //read all the data
    FILE *datafile;
    char str[300];
    datafile = fopen("food_nutrient_db.csv", "r");
    int count = 0;
    while(fgets(str, sizeof(str), datafile))
    {
        if(count == 0)
        {
            //struct FoodItem *newItem;
            //*newItem = createItem(str);
            //insert2(trieTree, str);
            //newInsert(trieTree, str);
            radixTree = radixInsert(0, str, 0);
            //printf("inserted: %s", radixTree->key);
        }else
        {
            radixInsert(radixTree, str, 0);
        }
        count++;
    }
    printf("processed: %d\n", count);
    fclose(datafile);
    //free(datafile);

    /* //prints all of the top level nodes
    struct RadixNode *hmm = radixTree;
    for(int l=0; l<1000; l++)
    {
        if(hmm->next)
        {
            hmm = hmm->next;
            printf("%s\n", hmm->key);
        }else
        {
            printf("end of alphabet %d\n", l);
            break;
        }
    }*/

    struct RadixNode *hmm;
    hmm = find(radixTree, "LIGHT NONFAT YOGURT~Topco Associates, Inc.~53.0~10.0~0.0~2.94~170.0~g~1.0~CONTAINER\n", 0);
    if(hmm)
    {
        printf("%s\n", (hmm->key)-5);
    }else
    {
        printf("dang\n");
    }


    //set up all the screens
    initscr();
    noecho();
    cbreak();
    //raw();
    //nonl();
    curs_set(0);

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
    searchResultsBox = newwin(sizeY-6, sizeX-2, 5, 1);
    searchResults = newwin(sizeY-8, sizeX-4, 6, 2);
    refresh();
    wrefresh(searchBar);
    keypad(searchBar, true);
    wrefresh(searchResults);
    wrefresh(searchResultsBox);

    itemDetails = newwin(sizeY-2, sizeX-2, 1, 1);
    refresh();
    wrefresh(itemDetails);
    keypad(itemDetails, true);

    diaryScreen = itemDetails = newwin(sizeY-2, sizeX-2, 1, 1);
    refresh();
    wrefresh(diaryScreen);
    keypad(diaryScreen, true);

    wrefresh(mainMenu);
    wrefresh(mainScreen);

    //start GUI and handle all of the screen management
    //loopvar = 0;
    while(loopvar)
    {
	    ScreenHandler();
    }
    wrefresh(mainMenu);

    if(diaryFile != NULL)
    {
        fclose(diaryFile);
    }

    //end program
    endwin();
    return 0;
}
