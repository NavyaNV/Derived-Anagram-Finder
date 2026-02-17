Derived Anagram Finder
=====================

Author: Navya Narasimhappa Venkateshappa
Date: 2026-02-17

---

1. Problem Definition
---------------------

A derived anagram is a word that consists of all letters from a base word plus **exactly one extra letter**, with the letters rearranged.  

The task is to find the **longest derived anagram chain** starting from a given word, using words from a given dictionary file.  

Requirements:
- All words must be present in the dictionary.
- Multiple chains of the same maximum length must be reported.
- Dictionary may contain up to 1 million unique words.
- Words consist of printable ASCII characters (codes 33–126), lengths 1–255.

Example:

Dictionary:

abcdg  
abcd  
abcdgh  
abcek  
abck  
abc  
abcdp  
abcdghi  
bafced  
akjpqwmn  
abcelk  
baclekt  

Starting word: `abck`

Output chain:

abck -> abcek -> abcelk -> baclekt

---

2. Algorithm Overview
---------------------

1. **Dictionary Loading**:  
   - Read all words from the dictionary file.  
   - Store each word in a hash table keyed by its **sorted letters** (canonical form) for O(1) lookup.

2. **Canonical Form**:  
   - Each word is stored along with its sorted letters.  
   - Sorting letters simplifies detecting derived anagrams.

3. **Generating Next Words**:  
   - For a word of length `L`, each next candidate word has length `L+1`.  
   - Generate all possible next words by inserting **each printable ASCII character (33–126)** into the sorted word.  
   - Check if candidate exists in the dictionary (hash table lookup).

4. **Finding Longest Chain**:  
   - The graph is **acyclic** because each word only grows in length.  
   - Use **memoized DFS** (dynamic programming) to compute the **longest chain starting from each word**.  
   - Memoization ensures each word is processed **exactly once**.

5. **Printing Chains**:  
   - After computing the maximum chain length, recursively generate and print **all chains** of this length.

---

3. Time and Space Complexity
----------------------------

- Let N = number of words, L = average word length.  
- **Time Complexity**: O(N × 94 × L)  
  - 94 = number of printable ASCII characters  
- **Space Complexity**: ~O(N × L)  
  - Hash table + memoization arrays  
  - For 1M words, ~300–400 MB of memory.

---

4. Compilation Instructions
---------------------------

On Mac/Linux (Apple M1/M2/M3 or Intel):

```bash
clang -O3 -march=native derived_anagram.c -o derived
