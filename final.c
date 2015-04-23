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
bool unload (void);

//types for trie implementation

typedef struct node 
{
    bool is_word;
    struct node* children [ALPH_SIZE];
}
node;

// global variable for the root node of the trie
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

// inserts a single word into the trie
void insert(char* word)
{
    int length = strlen(word);
    // initialize a crawler that starts at the root node
    node* crawl = &root;
    for(int i = 0; i < length; i++)
    {
        // finds the 0-25 index of the letter
        int index = word[i] - 'a';
        // if there is already a pointer for that letter, crawl to the 
        // node it points to, then continue down
        if (crawl->children[index] != NULL)
        {
            crawl = crawl->children[index];    
        }
        // if there is no pointer for that letter at this level of the trie,
        // malloc one then crawl to it
        else 
        {
            crawl->children[index] = malloc(sizeof(node));
            crawl = crawl->children[index];
        }
        // if we are at the last letter of the word, set the bool at this level
        // to true, to indicate a finished word
        if (i == length - 1)
        {
            crawl->is_word = true;
        }
    };
    return;
}

// loads an entire dictionary into the trie
bool load(const char* dictionary)
{
    // attempt to open the dictionary filepath in read mode
    FILE* dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return false;
    }
    
    // start reading words and inserting them until out of words
    char* word = NULL;
    while((fscanf(dict, "%s", word)) == 1)
    {
        insert(word);
    }
    
    return true;
}

// recursive helper function that unloads all children of a pointer to a node
void unload_recursive(node* n)
{
    for(int i = 0; i < ALPH_SIZE; i++)
    {
        if (n->children[i] != NULL)
        {
            unload_recursive(n->children[i]);
            n -> children[i] = NULL;
        }
    }
    free(n);
}

// unloads all the children of the root node, resets the root to NULL pointers
bool unload(void)
{
    for(int i = 0; i < ALPH_SIZE; i++)
    {
        unload_recursive(root.children[i]);
        root.children[i] = NULL;
    }
    
    return true;
}
