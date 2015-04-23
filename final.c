/* 
 * 
 * Letterpress "Best Words" Solver
 * By Jason Stein and Mason Hale
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DICTIONARY "words.txt"

//maybe something like #define DICTIONARY "/.../cs51-final-project/words.txt" would work if that doesn't

#define ALPH_SIZE 26

void insert(char* word);
bool load(const char* dictionary);

//types for trie implementation

typedef struct node 
{
    bool is_word;
    struct node* children [ALPH_SIZE];
}
node;

node root = {false,{NULL}};

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
    
        /* The user will be entering three lists, described as follows:
         * 1) White spaces (unclaimed by either player)
         * 2) Light red spaces (claimed by opponent but up-for-grabs)
         * 3) Dark red spaces and blue spaces (not up-for-grabs--won't improve score if used)
         */
        
        printf("Usage: ./final UnclaimedArray OpponentPotentialArray \
         OpponentBlockedAndPlayer'sArray \n");
        return 0;
    }
    
    char* dictionary = DICTIONARY;
    
    if (!load(dictionary)) 
    {
        return 0;
    }   

}

void insert(char* word)
{
    int length = strlen(word);
    node* crawl = &root;
    for(int i = 0; i < length; i++)
    {
        int index = word[i] - 'a';
        if (crawl->children[index] != NULL)
        {
            crawl = crawl->children[index];    
        }
        else 
        {
            crawl->children[index] = malloc(sizeof(node));
            crawl = crawl->children[index];
        }
        if (i == length - 1)
        {
            crawl->is_word = true;
        }
    };
    return;
}

bool load(const char* dictionary)
{
    FILE* dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return false;
    }
    
    char* word = NULL;
    while((fscanf(dict, "%s", word)) == 1)
    {
        insert(word);
    }
    
    return true;
}


