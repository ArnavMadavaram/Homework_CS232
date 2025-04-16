/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  

/* TODO: structure definitions */

#define ALPHABET_SIZE 26
#define BUFFER_SIZE 300001
#define MAX_WORD_LEN 256

// Each trie node holds a word count and pointers to child nodes for each alphabet letter
typedef struct TrieNode {
  int count;
  struct TrieNode* children[ALPHABET_SIZE];
} TrieNode;

/* NOTE: int return values can be used to indicate errors (typically non-zero)
   or success (typically zero return value) */

/* TODO: change this return type */
int indexPage(const char* url, TrieNode* root);

int addWordOccurrence(const char* word, const int wordLength, TrieNode* root);

void printTrieContents(TrieNode* root);

int freeTrieMemory(TrieNode* root);

int getText(const char* srcAddr, char* buffer, const int bufSize);

int main(int argc, char** argv){
  /* TODO: write the (simple) main function

   argv[1] will be the URL to index, if argc > 1 */

  // Check for command-line input
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <URL>\n", argv[0]);
    return 1;
  }

  // Create and initialize the root of the trie
  TrieNode* root = malloc(sizeof(TrieNode));
  if (!root) {
    fprintf(stderr, "ERROR: Memory allocation failed\n");
    return 1;
  }

  root->count = 0;
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    root->children[i] = NULL;
  }

  // Index the webpage content
  if (indexPage(argv[1], root) != 0) {
    fprintf(stderr, "ERROR: Failed to index page.\n");
    freeTrieMemory(root);
    return 1;
  }

  // Display word counts
  printTrieContents(root);

  // Clean up dynamically allocated memory
  freeTrieMemory(root);

  return 0;
}

/* TODO: define the functions corresponding to the above prototypes */

/* TODO: change this return type */
int indexPage(const char* url, TrieNode* root)
{
  // Fetch webpage text into buffer
  char buffer[BUFFER_SIZE];
  int bytesRead = getText(url, buffer, sizeof(buffer));

  if (bytesRead <= 0) {
    fprintf(stderr, "ERROR: failed to retrieve content from %s\n", url);
    return 1;
  }

  // Print the URL before parsing
  printf("%s\n", url);

  int i = 0;
  while (buffer[i] != '\0') {
    // Skip non-letter characters
    while (buffer[i] && !isalpha(buffer[i])) i++;

    char word[MAX_WORD_LEN];
    int len = 0;

    // Build a lowercase word from consecutive letters
    while (buffer[i] && isalpha(buffer[i]) && len < MAX_WORD_LEN - 1) {
      word[len++] = tolower(buffer[i++]);
    }

    // If a word was built, insert it into the trie
    if (len > 0) {
      word[len] = '\0';
      addWordOccurrence(word, len, root);
      printf("\t%s\n", word);  // Print the word with tab indentation
    }
  }

  return 0;
}

int addWordOccurrence(const char* word, const int wordLength, TrieNode* root)
{
  // Traverse the trie to insert the word
  TrieNode* current = root;
  for (int i = 0; i < wordLength; i++) {
    int idx = word[i] - 'a';
    if (idx < 0 || idx >= ALPHABET_SIZE) continue;

    // Allocate new node if missing
    if (current->children[idx] == NULL) {
      current->children[idx] = malloc(sizeof(TrieNode));
      if (!current->children[idx]) return 1;

      current->children[idx]->count = 0;
      for (int j = 0; j < ALPHABET_SIZE; j++) {
        current->children[idx]->children[j] = NULL;
      }
    }

    current = current->children[idx];
  }

  // Increment count at the last node
  current->count++;
  return 0;
}

void printTrieContentsHelper(TrieNode* node, char* buffer, int depth)
{
  if (node->count > 0) {
    buffer[depth] = '\0';
    printf("%s: %d\n", buffer, node->count);  // Print word and count
  }

  // Recursively check all children
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (node->children[i]) {
      buffer[depth] = 'a' + i;
      printTrieContentsHelper(node->children[i], buffer, depth + 1);
    }
  }
}

void printTrieContents(TrieNode* root)
{
  char buffer[MAX_WORD_LEN];
  printTrieContentsHelper(root, buffer, 0);  // Begin recursive print
}

int freeTrieMemory(TrieNode* root)
{
  if (!root) return 0;

  // Recursively free all child nodes
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (root->children[i]) {
      freeTrieMemory(root->children[i]);
    }
  }

  // Free the current node
  free(root);
  return 0;
}

/* You should not need to modify this function */
int getText(const char* srcAddr, char* buffer, const int bufSize){
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
	    buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize-1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}
