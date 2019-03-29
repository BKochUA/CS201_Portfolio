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
struct FoodItem *createItem(const char *data)
{
    static char tempstr[300];
    strcpy(tempstr, data);
    char *pch = tokenize(tempstr, "~");
    int count = 0;
    float tempServingWeight;
    struct FoodItem *createdItem = calloc(1, sizeof(struct FoodItem));
    while(pch != NULL)
    {
        switch(count)
        {
            case 0:
                createdItem->ID = (unsigned int)strtod(pch, NULL);
                break;
            case 1:
                strcpy(createdItem->name, pch);
                break;
            case 2:
                strcpy(createdItem->mfg, pch);
                break;
            case 3:
                createdItem->calories = (float)strtod(pch, NULL);
                break;
            case 4:
                createdItem->carbs = (float)strtod(pch, NULL);
                break;
            case 5:
                createdItem->fat = (float)strtod(pch, NULL);
                break;
            case 6:
                createdItem->protein = (float)strtod(pch, NULL);
                break;
            case 7:
                tempServingWeight = (float)strtod(pch, NULL);
                createdItem->servingWeight = tempServingWeight;
                createdItem->calories = createdItem->calories * (tempServingWeight / 100.0f);
                createdItem->carbs = createdItem->carbs * (tempServingWeight / 100.0f);
                createdItem->fat = createdItem->fat * (tempServingWeight / 100.0f);
                createdItem->protein = createdItem->protein * (tempServingWeight / 100.0f);
                break;
            case 8:
                if(strcmp(pch, "g") == 0)
                {
                    createdItem->grams = true;
                }else { createdItem->grams = false; }
                break;
            case 9:
                createdItem->servingSize = (float)strtod(pch, NULL);
                break;
            case 10:
                pch[strlen(pch)-1] = 0;
                strcpy(createdItem->servingUnits, pch);
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

//Fast and Space Efficient Trie Searches - Phil Bagwell
/*
struct AMTNode
{
    unsigned int NodeCnt:16, Chr:16; //16 bits each
    #ifdef AMTSMALL
    union{
        struct AMTNode *IndexBaseA;
        int Value;
    };
    #else
        struct AMTNode *IndexBaseA;
        int Value;
    #endif

};

struct AMTNode *AMTNext(struct AMTNode *pNode, unsigned short chr)
{
    struct AMTNode *pList;
    int L,H,M,NCnt;
    if(!(NCnt=pNode->NodeCnt)) return NULL;
    pList = pNode->IndexBaseA;
    if(NCnt <= 32)
    {
        do{
            if(chr == pList->Chr) return pList;
            pList++;
        }while(--NCnt);
        return NULL;
    }

    unsigned short Idx, Map;
    Idx = chr>>5; //get top bits
    chr&=0x1F; //clear top bits in chr
    Map=((AMTBMap *)pList)[Idx].BitMap;
}
 */





//kukuruku.co/post/radix-trees
struct RadixNode // a structure to represent tree nodes
{
    //char* key;
    int len;
    struct RadixNode* link;
    struct RadixNode* next;
    char key[];
};

struct RadixNode *newRadixNode(char *x, int n)
{
    struct RadixNode *RN = malloc(sizeof(struct RadixNode) + n*sizeof(char));
    RN->len = n;
    RN->link = 0;
    RN->next = 0;
    //memset(RN->key, 0, n*sizeof(char));
    //RN->key[n] = '\0';
    strncpy(RN->key, x, n);
    return RN;
}

int prefix(const char *x, int n, const char *key, int m) // length of the biggest common prefix of x and key strings
{
    for( int k=0; k<n; k++ ) {
        if (k == m || x[k] != key[k])
        {
            return k;
        }
    }
    return n;
}

struct RadixNode* find(struct RadixNode* t, char* x, int n) // x key search in t tree, call with n = 0
{
    //printf("%s", t->key);
    if( !n ) n = (int)strlen(x)+1;
    if( !t ) return 0;
    int k = prefix(x,n,t->key,t->len);
    if( k==0 ) return find(t->next,x,n); // let’s look for the child’s node
    if( k==n ) return t;
    if( k==t->len ) return find(t->link,x+k,n-k); // let’s look for at the child’s node
    return 0;
}

struct RadixNode* findByPrefix(struct RadixNode *root, char* searchKey, int n)
{
    if( !n ) //if n is 0, assign to length of searchkey
    {
        n = (int)strlen(searchKey);
    }
    if( !root )
    {
        return 0;
    }
    int k = prefix(searchKey,n,root->key,root->len);
    if(k==0)
    {
        printf("no match: %s\n", root->key);
        return findByPrefix(root->next, searchKey, n);
    }
    if(k > 0 && k < n)
    {
        printf("almost match?: %s\n", root->key);
        return findByPrefix(root->link, searchKey+1, n);
    }
    if(k==n)
    {
        printf("full match: %s\n", root->key);
        return root;
    }
    return 0;
}

/*
 if(k==n)
    {
        bool stillSearching = true;
        while(stillSearching)
        {
            printf("\nfound: %s\n", root->key);
            stillSearching = false;
        }
        return;
    }
*/
void split(struct RadixNode* t, int k) // dividing t node according to k key symbol
{
    struct RadixNode* p = newRadixNode(t->key+k, t->len-k);
    p->link = t->link;
    t->link = p;
    char a[k];
    //char* a = new char[k];
    strncpy(a,t->key,k);
    //memset(t->key, 0, strlen(t->key));
    //free(t->key);
    strncpy(t->key, a, k);
    //t->key = a;
    t->len = k;
}

struct RadixNode* radixInsert(struct RadixNode *t, char* x, int n) // inserting x key in t tree, call with n = 0;
{
    int k = 0;
    if( !n )
    {
        if(strlen(x) > 8) x = x + 9; //clears ID
        n = (int)strlen(x)+1;
    }
    if( t == 0 )
    {
        return newRadixNode(x, n);
    }
    //printf("%d", t->len);
    k = prefix(x,n,t->key,t->len);

    if( k==0 ) t->next = radixInsert(t->next,x,n);
    else if( k<n )
    {
        if( k<t->len ) // cut or not to cut?
            split(t,k);
        t->link = radixInsert(t->link,x+k,n-k);
    }
    return t;
}





//theoryofprogramming.com/2015/09/01/trie-tree-practise-spoj-dict/
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
    //temp->items = calloc(1, sizeof(struct FoodItem));
    temp->items[0] = *createItem(data);
    //*temp->items = createItem(data);
    temp->isLeaf = true;
    //free(temp->children);
}

void newInsert(struct TreeNode *root, char *word)
{
    printf("%s" ,word);
    char data[300];
    strcpy(data, word);
    word = word + 9; //clears
    for(int i=0; word[i] != '~'; i++)
    {
        if(root->children[word[i]] == NULL)
        {
            root->children[word[i]] = calloc(1, sizeof(struct TreeNode));
        }
        root = root->children[word[i]];
    }
    root->isLeaf = true;
    //root->items = calloc(1, sizeof(struct FoodItem));
    root->items = createItem(data);
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


void getAllItems(struct TreeNode *root, struct FoodItem *results, char *wordUtil, int pos)
{
    if(root == NULL)
    {
        return;
    }
    if(root->isLeaf)
    {
        for(int j=0; j<pos; j++)
        {
            results[j] = *root->items;
            //strncpy(results[j], wordUtil, strlen(wordUtil));
        }
    }
    for(int i=0; i<256; i++)
    {
        if(root->children[i] != NULL)
        {
            wordUtil[pos] = (char)i;
            getAllItems(root->children[i], results, wordUtil, pos+1);
        }
    }
}

void printWord(char *str, int n, WINDOW *searchResults)
{
    for(int j=0; j<n; j++)
    {
        mvwprintw(searchResults, j+1, 1, "%s", str);
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
        printWord(wordArray, pos, searchResults);
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

#endif
