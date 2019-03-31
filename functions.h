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
                strcpy(createdItem->name, pch);
                break;
            case 1:
                strcpy(createdItem->mfg, pch);
                break;
            case 2:
                createdItem->calories = (float)strtod(pch, NULL);
                break;
            case 3:
                createdItem->carbs = (float)strtod(pch, NULL);
                break;
            case 4:
                createdItem->fat = (float)strtod(pch, NULL);
                break;
            case 5:
                createdItem->protein = (float)strtod(pch, NULL);
                break;
            case 6:
                tempServingWeight = (float)strtod(pch, NULL);
                createdItem->servingWeight = tempServingWeight;
                createdItem->calories = createdItem->calories * (tempServingWeight / 100.0f);
                createdItem->carbs = createdItem->carbs * (tempServingWeight / 100.0f);
                createdItem->fat = createdItem->fat * (tempServingWeight / 100.0f);
                createdItem->protein = createdItem->protein * (tempServingWeight / 100.0f);
                break;
            case 7:
                if(strcmp(pch, "g") == 0)
                {
                    createdItem->grams = true;
                }else { createdItem->grams = false; }
                break;
            case 8:
                createdItem->servingSize = (float)strtod(pch, NULL);
                break;
            case 9:
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

int findByPrefix(struct RadixNode *root, char* searchKey, int n, struct FoodItem *results, char* originalKey)
{
    if( !n ) //if n is 0, assign to length of searchkey
    {
        n = (int)strlen(searchKey);
        originalKey = searchKey;
    }
    if( !root )
    {
        return 0;
    }
    int k = prefix(searchKey,n,root->key,root->len);
    //printf("\nk: %d   n: %d\n", k, n);
    if(k==0)
    {
        //printf("no match: %s\n", root->key);
        return findByPrefix(root->next, searchKey, n, results, originalKey);
    }
    if(k == n)
    {
        //printf("key: %s %d, link: %s, link's next: %s\n", root->key, root->len, root->link->key, root->link->next->key);
        int count = 0;
        while(root->next && root->link)
        {
            struct RadixNode *temp;
            if(count == 0)
            {
                temp = root->link;
            } else
            {
                temp = root;
            }

            char *word = malloc(199*sizeof(char));
            strncpy(word, originalKey, strlen(originalKey));
            int stringLength = (int)strlen(originalKey);
            if(count != 0)
            {
                strncpy(word+stringLength, root->key, (size_t)root->len);
                stringLength = stringLength + root->len;
            }
            while(root->link)
            {
                root = root->link;
                strncpy(word+stringLength, root->key, (size_t)root->len);
                stringLength = stringLength + root->len;
            }
            word[stringLength] = '\0';
            struct FoodItem *newItem = createItem(word);
            results[count] = *newItem;
            count++;
            root = temp->next;
            free(word);
        }
        //printf("perfect Match: %s\n", root->key);
        return count;
    }
    if(k == root->len)
    {
        printf("match of length: %s\n", root->key);
        return findByPrefix(root->link, searchKey+k, n-k, results, originalKey);
    }
    /*
    if(k > 0 && k < n)
    {
        printf("look what I found!\n");
        printf("%s\n", root->key);
        root = root->link;
        int count = 0;
        while(root->next)
        {
            if(prefix())
            struct FoodItem *newItem = createItem(root->key);
            results[count] = *newItem;
            printf("%s\n", root->key);
            root = root->next;
            count++;
        }
        return root;
    }*/
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

void getAllItems2(struct TreeNode *root, struct FoodItem *results, char *wordUtil, int pos)
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
            getAllItems2(root->children[i], results, wordUtil, pos+1);
        }
    }
}

struct DiaryEntry
{
    short year, month, day;
    struct RadixNode *items;
    int *servings;
};

void loadUserDiary(FILE *diary)
{
    int count = 0;
    char str[300];
    struct DiaryEntry *newEntry;
    while(fgets(str, sizeof(str), diary))
    {
        if(str[0] == '~') //line contains date information
        {
            count = 0;
            short year = (str[1]-48)*1000 + (str[2]-48)*100 + (str[3]-48)*10 + (str[4]-48);
            short month = (str[5]-48)*10 + (str[6]-48);
            short day = (str[7]-48)*10 + (str[8]-48);
            char *nl;
            nl = strchr(str, '\n');
            unsigned long numItems = strtoul(str+9, &nl, 0);
            newEntry = malloc(sizeof(struct DiaryEntry) + numItems*(sizeof(struct RadixNode*)+sizeof(int*)));
            free(newEntry);
        }else
        {
            //tutorialspoint.com/c_standard_library/c_function_strtok.htm
            char *token;
            token = strtok(str, "~");
            while(token != NULL)
            {
                token = strtok(NULL, "~");
            }
            //printf("%s", str);
        }
        count++;
    }
}

#endif
