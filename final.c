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

typedef struct node 
{
    bool is_word;
    struct node* children [26];
}
node;

node root = {false,{NULL}};

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

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("Usage: ./final UnclaimedArray OpponentPotentialArray \
         OpponentBlockedAndPlayerArray \n");
        return 1;
    }
    
    

}
