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
#define DICT_MAX_LENGTH 45
#define LIST_MAX_LENGTH 25
#define DICTIONARY "words.txt"



/*
 * ADTs for Trie and Linked List Nodes
 */
typedef struct trie_node 
{
    char* stored_word;
    struct trie_node* children [ALPH_SIZE];
}
trie_node;

typedef struct list_node
{
    char* stored_word;
    int score;
    struct list_node* next;
}
list_node;

/*
 * Function Prototypes
 */
void trie_insert(char* word);
bool load(const char* dictionary);
bool unload (void);
bool search(char* query);
bool trie_test(char* query);
void list_insert(char* word);
bool free_list(void);
void find_words(char* letters, trie_node* trie, bool last_letter);

/*
 * Roots of the trie and linked list
 */
trie_node root = {NULL,{NULL}};

list_node* head = NULL;

char available[LIST_MAX_LENGTH];


int main(int argc, char* argv[])
{
    // parse user input
    if (argc != 4)
    {
    
        /* The user will be entering three strings, described as follows:
         * 1) White space letters (unclaimed by either player)
         * 2) Light red space letters (claimed by opponent but up-for-grabs)
         * 3) Dark red space and blue space letters (not up-for-grabs--won't improve score if used)
         */
        
        printf("Usage: ./final UnclaimedString "\
        "OpponentPotentialString OpponentBlockedAndPlayer'sString \n");
        return 1;
    }
    
    
    // make sure the user enters the right number of letters
    if (strlen(argv[1]) + strlen(argv[2]) + strlen(argv[3]) 
        != LIST_MAX_LENGTH * sizeof(char))
    {
        printf("Please enter a total of 25 letters.");
        return 1;
    }
    
    else
    {
      // concatenate the command line arguments into one string
      strcat(available, argv[1]);
      strcat(available, argv[2]);
      strcat(available, argv[3]);
      printf("Available letters: %s\n", available); 
    }
    
    if (!load(DICTIONARY))
    {
        printf("error: failed to load dictionary\n");
        return 1;
    }
    
    printf("Successfully loaded dictionary.\n");
    
    trie_test("mason");
    trie_test("butts");
    trie_test("asdfgh");
    
    int letters[ALPH_SIZE] = {0};
    
    for (int i = 0; i < strlen(available); i++)
    {
        letters[available[i] - 'a']++;
    }
    
    //find_words(letters, root, true);
    
    if (!free_list())
    {
        return 1;
    }
    
    printf("Successfully freed list.");
    
    if (!unload())
    {
        return 1;
    }
    
    printf("Successfully unloaded dictionary.\n");
    return 0;
 

}

// inserts a single word into the trie
void trie_insert(char* word)
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
            crawl->children[index]->stored_word = 
              calloc(DICT_MAX_LENGTH, sizeof(char));
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
    char word [DICT_MAX_LENGTH];
    while((fscanf(dict, "%s", word)) == 1)
    {
        trie_insert(word);
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
bool trie_test(char* query)
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

void list_insert(char* word)
{
    list_node* new_node = malloc(sizeof(list_node));
    
    if (new_node == NULL)
    {
        printf("Failed to allocate memory.");
        return;
    }
    
    new_node->stored_word = calloc(LIST_MAX_LENGTH, sizeof(char));
    
    strcpy(new_node->stored_word, word);
    
    if (head == NULL)
    {
        new_node->next = NULL;
        head = new_node;
    }
    else
    {
        new_node->next = head;
        head = new_node;
    }
}

bool free_list(void)
{
    list_node* crawler = head;
    
    while (crawler != NULL)
    {
        list_node* temp = crawler;
        crawler = crawler->next;
        free(temp->stored_word);
        free(temp);
    }
    
    return true;
}

void find_words(char* letters, trie_node* trie, bool last_letter)
{
    for (int i = 0; i < ALPH_SIZE; i++)
    {
        if (letters[i] > 0 && trie->children[i] != NULL)
        {
            last_letter = false;
            letters[i]--;
            find_words(letters, trie->children[i], true);
            letters[i]++; 
        }
    }
    
    if (last_letter == true)
        list_insert(trie->stored_word);
}
