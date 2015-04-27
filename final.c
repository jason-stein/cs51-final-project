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
#include <ctype.h>

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
void trie_insert(char* word, trie_node* root);
bool load(const char* dictionary, trie_node* root);
void unload(trie_node* n);
bool search(char* query, trie_node* root);
bool trie_test(char* query, trie_node* root);
void list_insert(char* word);
bool free_list(void);
bool check_alpha(char* word);
void find_words(int* letters, trie_node* trie, bool last_letter);
int score_word(char* word, char* list1, char* list2);
void print_possibilities (list_node* head);

/*
 * Root of linked list
 */

list_node* head = NULL;

char available[LIST_MAX_LENGTH];

int main(int argc, char* argv[])
{
    trie_node* root = calloc(1,sizeof(trie_node));
    root->stored_word = NULL;
    
    // check usage
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
        != LIST_MAX_LENGTH)
    {
        printf("Please enter a total of 25 letters.");
        return 1;
    }
    
    // and check that they're all letters
    else 
        if (!check_alpha(argv[1]) || !check_alpha(argv[1]) 
            || !check_alpha(argv[1]))
        {
            printf("Please only enter letters!\n");
            return 1;
        }
    // if input is valid, parse arguments
    else
    {
        // concatenate the command line arguments into one string
        strcat(available, argv[1]);
        strcat(available, argv[2]);
        strcat(available, argv[3]);
        // make all letters lowercase for convenience
        for(int i = 0, length = strlen(available); i < length; i++)
        {
            available[i] = tolower(available[i]);
        }
        printf("Available letters: %s\n", available); 
    }
    
    // attempt to load the dictionary
    if (!load(DICTIONARY, root))
    {
        printf("error: failed to load dictionary\n");
        return 1;
    }
    
    printf("Successfully loaded dictionary.\n");
    
    // some tests
    trie_test("mason", root);
    trie_test("butts", root);
    trie_test("agammaglobulinemias", root);
    trie_test("asdfgh", root);
    
    int letters[ALPH_SIZE] = {0};
    
    for (int i = 0; i < strlen(available); i++)
    {
        letters[available[i] - 'a']++;
    }
    
    find_words(letters, root, true);
    
    print_possibilities(head);
    
    if (!free_list())
    {
        return 1;
    }
    
    printf("Successfully freed list.");
    
    unload(root);
    
    printf("Successfully unloaded dictionary.\n");
    return 0;
 

}

// inserts a single word into the trie
void trie_insert(char* word, trie_node* root)
{
    int length = strlen(word);
    // initialize a crawler that starts at the root node
    trie_node* crawl = root;
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
bool load(const char* dictionary, trie_node* root)
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
        trie_insert(word, root);
    }
    
    return true;
}

// checks if a given query is stored in the dictionary
bool search(char* query, trie_node* root)
{
    int length = strlen(query);
    // initialize a crawler
    trie_node* crawl = root;
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
void unload(trie_node* n)
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
            unload(n->children[i]);
            n->children[i] = NULL;
        }
    }
    
    // free the node
    free(n->stored_word);
    free(n);
    
}

bool trie_test(char* query, trie_node* root)
{
    if(search(query, root))
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

// inserts a node into a linked list
void list_insert(char* word)
{
    // allocate space for the new node (
    list_node* new_node = malloc(sizeof(list_node));
    
    if (new_node == NULL)
    {
        printf("Failed to allocate memory.");
        return;
    }
    
    // allocate space to store the new node's word
    new_node->stored_word = calloc(LIST_MAX_LENGTH, sizeof(char));
    
    // store the word in the node
    strcpy(new_node->stored_word, word);
    
    // base case for empty list
    if (head == NULL)
    {
        new_node->next = NULL;
        head = new_node;
    }
    // insert into existing list
    else
    {
        new_node->next = head;
        head = new_node;
    }
}

// frees all memory used by the linked list
bool free_list(void)
{
    // initialize a list_node pointer to act as a crawler
    list_node* crawler = head;
    
    // crawl through the list, freeing all memory
    while (crawler != NULL)
    {
        list_node* temp = crawler;
        crawler = crawler->next;
        free(temp->stored_word);
        free(temp);
    }
    
    return true;
}

void find_words(int* letters, trie_node* trie, bool last_letter)
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

// Finds the score for a word:

// By Letterpress rules, taking a letter from the opponent (any letter from
// list1) is worth 2 points, because it adds one to your score and subtracts 1 
// from the opponent. Using unclaimed letters (letters in list2) is worth 1 point 
int score_word(char* word, char* list1, char* list2)
{
    int score = 0;
    // sets of 26 "buckets" for letters -- tracks the number available of each letter
    int list1_counts[ALPH_SIZE] = {0};
    int list2_counts[ALPH_SIZE] = {0};
    // Fill the "buckets" -- for each letter, increment bucket value by 1.
    for(int i = 0, length = strlen(list1); i < length; i++)
    {
        list1_counts[list1[i] - 'a']++;
    }
    for(int i = 0, length = strlen(list2); i < length; i++)
    {
        list2_counts[list2[i] - 'a']++;
    }
    // For each letter, check if it's in list1 if yes, add 2 to score and decrement that
    // letter's bucket. If not, check list2 and add 1 to score. Otherwise, the letter 
    // has no value.
    for(int i = 0, length = strlen(word); i < length; i++)
    {
        if(list1_counts[word[i] - 'a'] != 0)
        {
            score += 2;
            list1_counts[word[i] - 'a']--;
        }
        else if (list2_counts[word[i] - 'a'] != 0)
        {
            score += 1;
            list2_counts[word[i] - 'a']--;
        }
    }
    
    return score;
}

// Makes sure a string is just letters.
bool check_alpha(char* word)
{
    for(int i = 0, length = strlen(word); i < length; i++)
    {
        if(!isalpha(word[i]))
            return false;
    }
    
    return true;
}

void print_possibilities (list_node* head)
{
    list_node* crawler = head;
    while (crawler != NULL)
    {
        printf("%s\n", crawler->stored_word);
        crawler = crawler->next;
    }
}
