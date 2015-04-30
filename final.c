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
#include <assert.h>

/*
 * Global Constants
 */
#define ALPH_SIZE 26
#define DICT_MAX_LENGTH 45
#define LIST_MAX_LENGTH 25
#define DICTIONARY "words.txt"
#define RESULTS 25

/*
 * ADTs for Trie, Linked List, and Queue
 */
typedef struct trie_node 
{
    char* substring;
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

typedef struct queue
{
    list_node* front;
    list_node* rear;
}queue;

// trie function prototypes 
void trie_insert(char* word, trie_node* root);
bool load(const char* dictionary, trie_node* root);
void unload(trie_node* n);
bool search(char* query, trie_node* root);
bool trie_test(char* query, trie_node* root);

// radix tree function prototypes
int only_child(trie_node* node);
trie_node* radix_collapse(trie_node* node);

// list function prototypes 
list_node* list_insert(char* word, list_node* head);
bool free_list(list_node* head);

// queue function prototypes
queue enqueue (queue q, list_node* node);
queue dequeue (queue q);

// user input function prototype
bool check_alpha(char* word);

// general functionality prototypes
list_node* find_words(int* letters, trie_node* trie, list_node* head);
int score_word(char* word, char* list1, char* list2);
queue find_finalists (list_node* head, queue q, char* string1, char* string2);
void print_finalists (list_node* front);


char available[LIST_MAX_LENGTH];

int main(int argc, char* argv[])
{
    
    // trie root 
    trie_node* root = calloc(1,sizeof(trie_node));
    root->stored_word = NULL;
    
    // all possible words linked list head
    list_node* head = NULL;

    // queue for finalists (top 25) linked list nodes
    queue q = {NULL, NULL};
    
    // check usage
    if (argc != 4)
    {
    
        /* The user will be entering three strings, described as follows:
         * 1) White space letters (unclaimed by either player)
         * 2) Light red space letters (claimed by opponent but up-for-grabs)
         * 3) Dark red space and blue space letters (not up-for-grabs--won't improve score if used)
         */
        
        printf("Usage: ./final OpponentPotentialString "\
        "UnclaimedString OpponentBlockedAndPlayer'sString \n");
        return 1;
    }
    
    // make sure the user enters the right number of letters
    if (strlen(argv[1]) + strlen(argv[2]) + strlen(argv[3]) 
        != LIST_MAX_LENGTH)
    {
        printf("Please enter a total of 25 letters.\n");
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
    assert(trie_test("mason", root));
    assert(trie_test("butts", root));
    assert(trie_test("agammaglobulinemias", root));
    assert(!trie_test("asdfgh", root));
    
    int letters[ALPH_SIZE] = {0};
    
    for (int i = 0; i < strlen(available); i++)
    {
        letters[available[i] - 'a']++;
    }
    
    head = find_words(letters, root, head);
    
    q = find_finalists(head, q, argv[1], argv[2]);
    
    
    if (!free_list(head) /*|| !free_list(q.front)*/)
    {
        return 1;
    }
    
    printf("Successfully freed lists.\n");
    
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
    // initialize a crawler
    trie_node* crawl = root;
    // the offset represents how many characters from stored substring we 
    // have crawled. in a trie it will always be 0, but in the radix tree
    // it will increase every time we hit a substring
    int offset = 0;
    for (int i = 0; query[i + offset] != '\0'; i++)
    {
        if(crawl->substring != NULL)
        {
            printf("Checking against substring: %s\n", crawl->substring);
            int sub_length = strlen(crawl->substring);
            // crawl through the substring, checking each letter of the query
            // at the point we're at against the substring
            for(int j = 0; j < sub_length; j++)
            {
                if (query[i + j + offset] != crawl->substring[j])
                {
                    return false;
                }
            }
            // if the subsection of the query lines up with the stored
            // substring, increase the offset and continue down the tree
            offset += sub_length;
        }
        // gets the 0-25 index of the letter
        int index = query[i + offset] - 'a';
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

// simple boolean test function to make sure the trie is working properly
bool trie_test(char* query, trie_node* root)
{
    return(search(query, root));
}

// inserts a node into a linked list
list_node* list_insert(char* word, list_node* head)
{
    // allocate space for the new node (
    list_node* new_node = malloc(sizeof(list_node));
    
    if (new_node == NULL)
    {
        printf("Failed to allocate memory.");
        return NULL;
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
    
    return head;
}

// frees all memory used by the linked list
bool free_list(list_node* head)
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

list_node* find_words(int* letters, trie_node* trie, list_node* head)
{

    // iterates through all of the user's possible letters and all of the 
    // current trie node's pointers simultaneously 
    for (int i = 0; i < ALPH_SIZE; i++)
    {
       // if the user has the requisite letters and there are trie pointers to 
       // traverse associated with those letters, the trie is traversed with
       // the letter used at that pointer removed (and then replaced)
        if (letters[i] > 0 && trie->children[i] != NULL)
        {
            letters[i]--;
            head = find_words(letters, trie->children[i], head);
            letters[i]++; 
        }  
        
    } 
    
    // there is a word to store at this level of the trie, so store it
    if (trie->stored_word != NULL)
        head = list_insert(trie->stored_word, head);
      
    return head;

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

queue find_finalists (list_node* head, queue q, char* string1, char* string2)
{
    // int max = 0;
    // list_node* max_node = head;
    list_node* crawler = head;
    
    // continues to increment until hitting 25, at which point nodes' scores
    // are taken into account before inserting into list
    int count = 0;
    
    // number defining the lowest value for something to get on the list
    int min_required = 0;
    
    while (crawler != NULL)
    {
        crawler->score = score_word(crawler->stored_word, string1, string2);
        
        if (count < RESULTS)
        {
            q = enqueue(q, crawler);
            
            min_required = q.rear->score;
            
            count++;
        } 
        else if (crawler->score > min_required)
        {
            //print_finalists(q.front);
            q = enqueue(q, crawler);
            q = dequeue(q);
            min_required = q.rear->score;
        }
        
        //printf("%s - %d\n", crawler->stored_word, crawler->score);
        crawler = crawler->next;
    }

    
    print_finalists(q.front);
    return q;
}

queue enqueue (queue q, list_node* node)
{
    list_node* temp = calloc(1, sizeof(list_node));
    temp->score = node->score;
    temp->stored_word = node->stored_word;
    
    list_node* crawler1 = q.front;
    list_node* crawler2 = q.front;
    // crawler2->next = crawler1;
    
    if (q.front == NULL)
    {
        q.front = temp;
        q.rear = temp;
        //q.rear->next = q.front;
        return q;
    }
    
    if (node->score >= q.front->score)
    {
        temp->next = q.front;
        q.front = temp;
        return q;
    }
    // loop below has an issue...
    while (crawler1 != NULL)
    {
        if (node->score >= crawler1->score)
        {
            temp->next = crawler1;
            crawler2->next = temp;
            return q;
        }
        
        crawler2 = crawler1;
        crawler1 = crawler1->next;
    }
    
    crawler2->next = temp;
    q.rear = temp;
    
    return q;
}

queue dequeue (queue q)
{
    list_node* crawler = q.front;
    
    while (crawler != NULL)
    {
        if (crawler->next == q.rear)
        {
            crawler->next = NULL;
            
            free(q.rear);
            
            q.rear = crawler;
            return q;
        }
        
        crawler = crawler->next;
    }
    
    return q;
}

void print_finalists (list_node* front)
{
    list_node* crawler = front;
    int number = 1;
    
    while (crawler != NULL)
    {
        printf("%d. %s - %d\n", number, crawler->stored_word, crawler->score);
        crawler = crawler->next;
        number++;
    }
}

int only_child(trie_node* node)
{
    int count = 0;
    int index = 0;
    for(int i = 0; i < ALPH_SIZE; i++)
    {
        if (node->children[i] != NULL)
        {
            count++;
            index = i;
        }
    } 
    
    if (count == 0)
    {
        return -1;
    }
    if (count > 1)
    { 
        return -2;
    }
    return index;
}

trie_node* radix_collapse(trie_node* node)
{
    // try to find the index of the node's only child
    int child_index = only_child(node);
    // if the node has no children, simply return the node
    if (child_index == -1)
    {
        return node;
    }
    // if the node has more than one child, try to collapse all of them
    if (child_index == -2)
    {
        for(int i = 0; i < ALPH_SIZE; i++)
        {
            trie_node* temp = node->children[i];
            if (temp != NULL)
            {
                node->children[i] = radix_collapse(temp);
            }
        }
        return node;
    }
    // get the pointer to the only child node
    trie_node* child_node = node->children[child_index];
    // don't collapse onto a node with a stored word (or things will get messy)
    if (strlen(node->stored_word) != 0)
    {
        node->children[child_index] = radix_collapse(child_node);
        return node;
    }
    // this is where we actually start collapsing
    // update the node's substring to include the letter that points to the 
    // only child
    char child_letter = child_index + 'a';
    if (node->substring == NULL)
    {
        node->substring = calloc(DICT_MAX_LENGTH,sizeof(char));
        node->substring[0] = child_letter;
    }
    else
    {
        int length = strlen(node->substring);
        node->substring[length] = child_letter;
        node->substring[length + 1] = '\0';
    }
    // replace the node's children with its child's children
    for (int i = 0; i < ALPH_SIZE; i++)
    {
        node->children[i] = child_node->children[i];
    }
    // if the child node stored a word, now store it at the parent
    if (child_node->stored_word != NULL)
    {
        strcpy(node->stored_word,child_node->stored_word);
    }
    // free the child node we just eliminated
    free(child_node->stored_word);
    free(child_node);
    // try to collapse this node further
    radix_collapse(node);
    return node;
}
