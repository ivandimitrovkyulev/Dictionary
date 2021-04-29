// Implements a dictionary's functionality
#include <stdio.h>
#include <stdbool.h>
#include <cs50.h>
#include "dictionary.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// Number of buckets in hash table
const unsigned int table_size = 26;

// Hash table
node *table[table_size];

unsigned int word_count = 0;

bool loaded = true;

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    unsigned int index = 0;
    char word[LENGTH + 1];

    FILE *file = fopen(dictionary, "r");
    if (file == NULL) {
        printf("Could not open dictionary %s\n", dictionary);
        return false; }

    for (int c = fgetc(file); c != EOF; c = fgetc(file))
    {
        if (isalpha(c) || (c == '\'' && index > 0))
        {
            word[index] = c;
            index++;

            if (index > LENGTH)
            {
                while ((c = fgetc(file)) != EOF && isalpha(c));
                index = 0;
            }
        }

        else if (isdigit(c))
        {
            while ((c = fgetc(file)) != EOF && isalnum(c));
            index = 0;
        }

        else if (index > 0)
        {
            word[index] = '\0';

            node *n = malloc(sizeof(node));
            if (n == NULL)
            {
                printf("Error -- out of memory.\n");
                return false;
            }
            strcpy(n->word, word);
            int hashed = hash(word);

            if (table[hashed] == NULL)
            {
                table[hashed] = n;
                n->next = NULL;
            }
            else
            {
                n->next = table[hashed];
                table[hashed] = n;
            }
            word_count++;
            index = 0;
        }
    }
    fclose(file);
    return true;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    int len = strlen(word);
    char lower[LENGTH + 1];
    for (int i = 0; i < len; i++)
        lower[i] = tolower(word[i]);
    lower[len] = '\0';

    int hashed = hash(lower);
    if (table[hashed] == NULL)
        return false;

    node *ptr = table[hashed];

    while (ptr != NULL)
    {
        if (strcmp(ptr->word, lower) == 0)
        {
            return true;
        }
        ptr = ptr->next;
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{

    unsigned int hashed;

    if (!isalnum(word[0]))
        hashed = 0;
    else
        hashed = toupper(word[0]) - 'A';

    return hashed % table_size;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return word_count;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    for (int i = 0; i < table_size; i++)
    {
        node *cursor = table[i];
        while (cursor != NULL)
        {
            node *tmp = cursor;
            cursor = cursor->next;
            free(tmp);
        }
    }
    loaded = false;
    return true;
}