#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    int sizeY, sizeX;
    getmaxyx(stdscr, sizeY, sizeX);
    WINDOW *mainScreen = newwin(sizeY, sizeX, 0, 0);
    printf("%d ", sizeY);
    printf("%d", sizeX);
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

    while(loopvar)
    {
        for(int i = 0; i < 4; i++)
        {
            mvwprintw(mainMenu, 1, 1, "Welcome Brian, today is date");
            mvwprintw(mainMenu, 3, 1, "Please select an option");
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
