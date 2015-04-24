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


//maybe something like #define DICTIONARY "/.../cs51-final-project/words.txt" would work if that doesn't

#define ALPH_SIZE 26

void insert(char* word);
bool load(const char* dictionary);
bool unload (void);

//types for trie implementation

typedef struct node 
{
    char* stored_word;
    struct node* children [ALPH_SIZE];
}
node;

// global variable for the root node of the trie
node root = {NULL,{NULL}};

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
    
        /* The user will be entering three lists, described as follows:
         * 1) White spaces (unclaimed by either player)
         * 2) Light red spaces (claimed by opponent but up-for-grabs)
         * 3) Dark red spaces and blue spaces (not up-for-grabs--won't improve score if used)
         */
        
        printf("Usage: ./final Dictionary UnclaimedArray OpponentPotentialArray OpponentBlockedAndPlayer'sArray \n");
        return 0;
    }
    
    char* dictionary = argv[1];
    
    if (!load(dictionary))
    {
        printf("error: failed to load dictionary\n");
        return 1;
    }
    
    printf("Successfully loaded dictionary.\n");
    
    if (!unload())
    {
        printf("error: failed to unload dictionary\n");
        return 1;
    }
    return 0;
 

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
            crawl->stored_word = word;
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
    if (n == NULL)
    {
        return;
    }
    
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
