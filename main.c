#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

struct FoodItem
{
    unsigned int ID;
    char name[200]; //max is 199
    char mfg[80]; //max is 78
    float calories, carbs, fat, protein, servingWeight, servingSize;
    char servingUnits[130];
    bool grams;
};

static char *tokenize(char *str, const char *delimiter)
{
    static char *src = NULL;
    char *p, *value = 0;

    if(str != NULL)
    {
        src = str;
    }
    if(src == NULL || *src == '\0')
    {
        return NULL;
    }
    value = src;
    if((p = strpbrk(src, delimiter)) != NULL)
    {
        *p = 0;
        src = ++p;
    }else { src += strlen(src); }
    return value;
}

void printFoodItem(struct FoodItem *itemToPrint)
{
    printf("%d - %s\nManufacturer: %s\n", itemToPrint->ID, itemToPrint->name, itemToPrint->mfg);
    printf("calories: %.2f g\ncarbs: %.2f g\nfat: %.2f g\nprotein: %.2f g\n", itemToPrint->calories, itemToPrint->carbs, itemToPrint->fat, itemToPrint->protein);
    printf("serving size: %.2f %s (%.2f %s)\n", itemToPrint->servingSize, itemToPrint->servingUnits, itemToPrint->servingWeight, itemToPrint->grams ? "g": "ml");
}

//product id~product name~mfg~energy~carbs~fat~protein~servingsize in grams or ml~grams or ml~servingsize~servingsize units
/* math for converting to units
 * example, energy=200, carbs=35, fat=6.25, protein=2.5
 * serving in grams or ml = 40
 * energy = 200.0 * (40.0/100.0)
 * energy
*/
void createItem(const char *data)
{
    static char tempstr[300];
    strcpy(tempstr, data);
    char *pch = tokenize(tempstr, "~");
    int count = 0;
    float tempServingWeight;
    struct FoodItem createdItem;
    while(pch != NULL)
    {
        switch(count)
        {
            case 0:
                createdItem.ID = (unsigned int)strtod(pch, NULL);
                break;
            case 1:
                strcpy(createdItem.name, pch);
                break;
            case 2:
                strcpy(createdItem.mfg, pch);
                break;
            case 3:
                createdItem.calories = (float)strtod(pch, NULL);
                break;
            case 4:
                createdItem.carbs = (float)strtod(pch, NULL);
                break;
            case 5:
                createdItem.fat = (float)strtod(pch, NULL);
                break;
            case 6:
                createdItem.protein = (float)strtod(pch, NULL);
                break;
            case 7:
                tempServingWeight = (float)strtod(pch, NULL);
                createdItem.servingWeight = tempServingWeight;
                createdItem.calories = createdItem.calories * (tempServingWeight / 100.0f);
                createdItem.carbs = createdItem.carbs * (tempServingWeight / 100.0f);
                createdItem.fat = createdItem.fat * (tempServingWeight / 100.0f);
                createdItem.protein = createdItem.protein * (tempServingWeight / 100.0f);
                break;
            case 8:
                if(strcmp(pch, "g") == 0)
                {
                    createdItem.grams = true;
                }else { createdItem.grams = false; }
                break;
            case 9:
                createdItem.servingSize = (float)strtod(pch, NULL);
                break;
            case 10:
                pch[strlen(pch)-1] = 0;
                strcpy(createdItem.servingUnits, pch);
                break;
            default:
                break;
        }
        pch = tokenize(NULL, "~");
        count++;
    }
    printFoodItem(&createdItem);
}

int main() {

    //read all the data
    FILE *datafile;
    char str[300];
    datafile = fopen("food_nutrient_db.csv", "r");
    int count = 0;
    while(fgets(str, sizeof(str), datafile))
    {
        if(count == 2) { createItem(str); }
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

    WINDOW *mainMenu = newwin(10, 50, (sizeY/2)-5, (sizeX/2)-25);
    box(mainMenu, 0, 0);
    refresh();
    wrefresh(mainScreen);
    wrefresh(mainMenu);
    keypad(mainMenu, true);


    const char *options[] = {"View Today's Log", "View Calendar", "Search for Food", "EXIT"};
    int keyPress;
    int selected = 0;
    int loopvar = 1;

    //get current date
    int day, month, year;
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

    while(loopvar)
    {
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
    }
    wrefresh(mainMenu);

    endwin();
    return 0;
}