/* 
 * 
 * Letterpress "Best Words" Solver
 * By Jason Stein and Mason Hale
 *
 */


/*
 * Function Libraries
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * Global Constants
 */
#define ALPH_SIZE 26
#define MAX_LENGTH 45

/*
 * Function Prototypes
 */
void insert(char* word);
bool load(const char* dictionary);
bool unload (void);
bool search(char* query);
bool test(char* query);

/*
 * ADTs for Trie and Linked List Nodes
 */
typedef struct trie_node 
{
    char* stored_word;
    struct trie_node* children [ALPH_SIZE];
}
trie_node;

/*
 * Root of the Trie
 */
trie_node root = {NULL,{NULL}};


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
    
    test("mason");
    test("butts");
    test("asdfgh");
    
    if (!unload())
    {
        printf("Successfully unloaded dictionary.\n");
        return 1;
    }
    return 0;
 

}

// inserts a single word into the trie
void insert(char* word)
{
    int length = strlen(word);
    // initialize a crawler that starts at the root node
    trie_node* crawl = &root;
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
            crawl->children[index] = calloc(1, sizeof(trie_node));
            crawl->children[index]->stored_word = calloc(MAX_LENGTH, sizeof(char));
            crawl = crawl->children[index];
        }
        // if we are at the last character of the word, copy it into the stored
        // word space of the node
        if (word[i + 1] == '\0')
        {
            strcpy((crawl->stored_word), word);
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
    char word [MAX_LENGTH];
    while((fscanf(dict, "%s", word)) == 1)
    {
        insert(word);
    }
    
    return true;
}

// checks if a given query is stored in the dictionary
bool search(char* query)
{
    int length = strlen(query);
    // initialize a crawler
    trie_node* crawl = &root;
    for (int i = 0; i < length; i++)
    {
        // gets the 0-25 index of the letter
        int index = query[i] - 'a';
        // if there is no pointer at this node for the letter, then the word 
        // isn't there
        if (crawl->children[index] == NULL)
        {
            return false;
        }
        // otherwise crawl to the next node
        else crawl = crawl->children[index];
    }
    
    // we are now at the last node, where this word should be stored,
    // so check that the stored word is indeed the query
    return (strcmp(query, crawl->stored_word) == 0);
}

// recursive helper function that unloads all children of a pointer to a node
void unload_recursive(trie_node* n)
{
    // base case
    if (n == NULL)
    {
        return;
    }
    
    for(int i = 0; i < ALPH_SIZE; i++)
    {
        // if the node has children, unload them first
        if (n->children[i] != NULL)
        {
            unload_recursive(n->children[i]);
            n -> children[i] = NULL;
        }
    }
    
    // free the node
    free(n);
}

// unloads all the children of the root node, resets the root to NULL pointers
bool unload(void)
{
    // recursively unload all the children of the root node
    for(int i = 0; i < ALPH_SIZE; i++)
    {
        unload_recursive(root.children[i]);
        root.children[i] = NULL;
    }
    
    return true;
}

// helper function that tests trie with a given query
bool test(char* query)
{
    if(search(query))
    {
        printf("%s is in the dictionary!\n", query);
        return true;
    }
    else 
    {
        printf("%s is not in the dictionary :(\n", query);
        return false;
    }
}
