/*
===========================================================
Derived Anagram Finder
Author: Navya Narasimhappa Venkateshappa
===========================================================

Problem:
A derived anagram is a word formed by taking all letters of a
base word, adding exactly ONE extra character, and rearranging.

Example:
sail -> nails -> aliens

Goal:
Given:
  1. A dictionary file (up to 1 million words)
  2. A starting word

Find:
  - The longest derived anagram chain
  - Print ALL chains with maximum length

-----------------------------------------------------------
Algorithm Overview
-----------------------------------------------------------

1. Read dictionary and store words in a hash table.
2. For each word:
   - Store original word
   - Store sorted version (canonical form)
3. Use memoized DFS to compute longest chain:
   - For each word:
       Try inserting each printable ASCII character (33–126)
       into the sorted word.
       If new sorted word exists in dictionary:
           recursively compute its chain length.
4. Since word length strictly increases, graph is a DAG.
   No cycle detection required.
5. Use dynamic programming (memoization) to ensure each word
   is processed only once.
6. After computing maximum length, print all chains.

Time Complexity:
    O(N × 94 × L)
Space Complexity:
    O(N × L)

Works efficiently for up to 1 million words.
===========================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD 256
#define HASH_SIZE 2000003   // Large prime (~2M) for hash table

/* Structure representing each dictionary word */
typedef struct Word {
    char original[MAX_WORD];   // original word
    char sorted[MAX_WORD];     // sorted canonical form
    int length;                // word length
    int dp;                    // memoized longest chain length
    struct Word *next;         // for hash table chaining
} Word;

/* Global hash table */
Word *hash_table[HASH_SIZE];

/* ---------------------------------------------------------
   Hash function (DJB2)
--------------------------------------------------------- */
unsigned long hash(const char *str) {
    unsigned long h = 5381;
    int c;
    while ((c = *str++))
        h = ((h << 5) + h) + c;   // h * 33 + c
    return h % HASH_SIZE;
}

/* ---------------------------------------------------------
   Sort characters of a string (simple insertion sort)
--------------------------------------------------------- */
void sort_string(char *dest, const char *src) {
    strcpy(dest, src);
    int n = strlen(dest);

    for (int i = 1; i < n; i++) {
        char key = dest[i];
        int j = i - 1;
        while (j >= 0 && dest[j] > key) {
            dest[j + 1] = dest[j];
            j--;
        }
        dest[j + 1] = key;
    }
}

/* ---------------------------------------------------------
   Insert word into hash table
--------------------------------------------------------- */
void insert_word(const char *word) {
    Word *w = malloc(sizeof(Word));
    if (!w) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    strcpy(w->original, word);
    sort_string(w->sorted, word);
    w->length = strlen(word);
    w->dp = -1;   // not computed yet

    unsigned long h = hash(w->sorted);
    w->next = hash_table[h];
    hash_table[h] = w;
}

/* ---------------------------------------------------------
   Find word by sorted canonical form
--------------------------------------------------------- */
Word* find_word(const char *sorted) {
    unsigned long h = hash(sorted);
    Word *w = hash_table[h];

    while (w) {
        if (strcmp(w->sorted, sorted) == 0)
            return w;
        w = w->next;
    }
    return NULL;
}

/* ---------------------------------------------------------
   Memoized DFS to compute longest chain from word
--------------------------------------------------------- */
int longest_chain(Word *w) {
    if (w->dp != -1)
        return w->dp;

    int max_len = 1;   // at least the word itself
    char new_sorted[MAX_WORD];

    // Try adding each printable ASCII character
    for (int c = 33; c <= 126; c++) {

        int i = 0, j = 0;
        int n = w->length;

        // Insert character c into sorted position
        while (i < n && w->sorted[i] < c)
            new_sorted[j++] = w->sorted[i++];

        new_sorted[j++] = c;

        while (i < n)
            new_sorted[j++] = w->sorted[i++];

        new_sorted[j] = '\0';

        Word *next_word = find_word(new_sorted);

        if (next_word) {
            int len = 1 + longest_chain(next_word);
            if (len > max_len)
                max_len = len;
        }
    }

    w->dp = max_len;
    return max_len;
}

/* ---------------------------------------------------------
   Recursively print all maximum-length chains
--------------------------------------------------------- */
void print_chains(Word *w, int max_len,
                  char chain[][MAX_WORD], int depth) {

    strcpy(chain[depth], w->original);

    if (depth + 1 == max_len) {
        // Print chain
        for (int i = 0; i <= depth; i++) {
            printf("%s", chain[i]);
            if (i < depth)
                printf("->");
        }
        printf("\n");
        return;
    }

    char new_sorted[MAX_WORD];

    for (int c = 33; c <= 126; c++) {

        int i = 0, j = 0;
        int n = w->length;

        while (i < n && w->sorted[i] < c)
            new_sorted[j++] = w->sorted[i++];

        new_sorted[j++] = c;

        while (i < n)
            new_sorted[j++] = w->sorted[i++];

        new_sorted[j] = '\0';

        Word *next_word = find_word(new_sorted);

        if (next_word &&
            next_word->dp == max_len - depth - 1) {

            print_chains(next_word,
                         max_len,
                         chain,
                         depth + 1);
        }
    }
}

/* ---------------------------------------------------------
   Main function
--------------------------------------------------------- */
int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <dictionary_file> <starting_word>\n",
               argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Cannot open dictionary file.\n");
        return 1;
    }

    char word[MAX_WORD];

    // Load dictionary
    while (fgets(word, MAX_WORD, file)) {
        word[strcspn(word, "\r\n")] = 0;  // remove newline
        insert_word(word);
    }

    fclose(file);

    // Prepare starting word
    char sorted_start[MAX_WORD];
    sort_string(sorted_start, argv[2]);

    Word *start = find_word(sorted_start);

    if (!start) {
        printf("Starting word not in dictionary.\n");
        return 1;
    }

    // Compute longest chain
    int max_len = longest_chain(start);

    printf("Longest chain length: %d\n", max_len);

    char chain[256][MAX_WORD];
    print_chains(start, max_len, chain, 0);

    return 0;
}
