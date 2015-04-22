#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node 
{
    bool is_word;
    struct node* children [26];
};


