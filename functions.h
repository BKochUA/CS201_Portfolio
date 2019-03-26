#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>

struct FoodItem
{
    unsigned int ID;
    char name[200]; //max is 199
    char mfg[80]; //max is 78
    float calories, carbs, fat, protein, servingWeight, servingSize;
    char servingUnits[130];
    bool grams;
};

struct TreeNode
{
    struct TreeNode *children[256];
    struct FoodItem *items;

    bool isLeaf;
};

typedef struct TreeNode Node;

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
struct FoodItem createItem(const char *data)
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
    return createdItem;
    //printFoodItem(&createdItem);
    //printf("_____________________________________________________\n");
}

//theoryofprogramming.com/2015/09/01/trie-tree-practise-spoj-dict/
//was used for both insert and searchWord
void insert(struct TreeNode *trieTree, struct FoodItem *item)
{
    struct TreeNode *temp = trieTree;

    while(*item->name != '\0')
    {
        if(temp->children[*item->name - 'a'] == NULL)
        {
            temp->children[*item->name - 'a'] = (struct TreeNode *) calloc(1, sizeof(struct TreeNode));
        }

        temp = temp->children[*item->name - 'a'];
        ++*item->name;
    }

    temp->items = item;
    temp->isLeaf = true;
    free(temp->children);
}

void insert2(struct TreeNode *trieTree, char *word)
{
    struct TreeNode *temp = trieTree;
    char data[300];
    strcpy(data, word);
    word = word + 9; //clears
    while(*word != '~')
    {
        //if(temp->children[*word - 'A'] == NULL)
        if(temp->children[*word] == NULL)
        {
            //temp->children[*word- 'A'] = calloc(1, sizeof(struct TreeNode));
            temp->children[*word] = calloc(1, sizeof(struct TreeNode));
        }

        //temp = temp->children[*word - 'A'];
        temp = temp->children[*word];
        ++word;
        //printf("%s", word);
    }


    //temp->items = item;
    //try gdb for this
    //*temp->items = createItem(data);
    temp->isLeaf = true;
    //free(temp->children);
}

struct TreeNode *searchWord(struct TreeNode *node, char *searchWord)
{
    //printw("searching\n");
    while(*searchWord != '\0')
    {
        //if(node->children[*searchWord - 'A'] != NULL)
        if(node->children[*searchWord] != NULL)
        {
            node = node->children[*searchWord];
            //node = node->children[*searchWord - 'A'];
            ++searchWord;
        }else
        {
            break;
        }
    }

    if(*searchWord == '\0')
    {
        return node;
    }else
    {
        return NULL;
    }
}


//www.ritambhara.in/print-all-words-in-a-trie-data-structure/
void printAllWords(struct TreeNode *root, char *wordArray, int pos, WINDOW *searchResults)
{
    if(root == NULL)
    {
        return;
    }
    if(root->isLeaf)
    {

        for(int j=0; j<pos; j++)
        {
            mvwprintw(searchResults, j+1, 1, "%s", wordArray);
        }
    }
    for(int i=0; i<256; i++)
    {
        if(root->children[i] != NULL)
        {
            wordArray[pos] = (char)i;
            printAllWords(root->children[i], wordArray, pos+1, searchResults);
        }
    }
}

void lexicographPrint(struct TreeNode *trieTree, char *word, char *prefix, WINDOW *searchResults)
{
    int i;
    bool noChild = true;

    if(trieTree->isLeaf && strlen(word) != 0)
    {
        mvwprintw(searchResults, 3, 1, "%s", word);
        char *itr;
        itr = &word[0];
        mvwprintw(searchResults, 1, 1, "%s", prefix);
        //printf("%s", prefix);
        while(*itr != word[strlen(word)-1])
        {
            mvwprintw(searchResults, 1, (int)strlen(prefix)+1, "%c", *itr);
            //printf("%c", *itr);
            //printf("before");
            itr++;
            //printf("after");
        }
        printf("\n");
    }
    //printf("2");

    char currentWord[strlen(word)+1];
    for(i = 0; i<256; ++i)
    {
        if(trieTree->children[i] != NULL)
        {
            noChild = false;
            strncpy(currentWord, word, strlen(word));
            //currentWord[strlen(word)-1] = (char)('A'+i);
            currentWord[strlen(word)-1] = (char)(i);
            fflush(stdout);
            lexicographPrint(trieTree->children[i], currentWord, prefix, searchResults);
            currentWord[strlen(word)-1] = '\0';
        }
    }
    //printf("3");

    //currentWord[strlen(word)-1] = '\0';

    //printf("4");

}

#endif
