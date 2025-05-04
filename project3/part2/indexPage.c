#include "indexPage.h"


struct trie* indexPage(const char* url, int* pTotalNumOfTerms) {
    int sumOfTermContents = 0;
    struct trie* root = createNode();
    char* buffer = (char*) malloc(sizeof(char) * MAX_CHARACTERS);
    if (root == NULL || buffer == NULL)
        return NULL;

    int numOfBytes = getText(url, buffer, MAX_CHARACTERS);
    int isNewTerm = 0; //boolean that depicts if current term has been added
    int startIndex = 0;
    int endIndex = 0;

    printf("%s\n", url);
    int i;
    for (i = 0; i < numOfBytes; i++) {
        if (!isalpha(buffer[i]) && isNewTerm) {
            endIndex = i;
            buffer[endIndex] = '\0';

            printTermFromBuffer(buffer, startIndex, endIndex);

            addTermOccurrence(root, buffer + startIndex);

            sumOfTermContents++;
            isNewTerm = 0; //set to false, as the term was just added
        }
        else if (isalpha(buffer[i]) && !isNewTerm) {
            startIndex = i;
            isNewTerm = 1;
        }

        buffer[i] = tolower(buffer[i]); 
    }

    *pTotalNumOfTerms = sumOfTermContents;
    free(buffer);

    return root;
}

/* Print a term from index startIndex to endIndex. */
void printTermFromBuffer(char* buffer, int startIndex, int endIndex) {
    int i;

    printf("\t");
    for (i = startIndex; i < endIndex; i++)
        printf("%c", buffer[i]);
    printf("\n");
}

/* Adds a term to a given trie. 
    Returns 1 if the term is added and 0 if it is not. 
    */
int addTermOccurrence(struct trie* root, const char* term) {
    struct trie* tempRoot = root;
    int i;
    for (i = 0; term[i] != '\0'; i++) {
        int charIndex = term[i] - 'a';

        if (tempRoot->children[charIndex] == NULL) {
            tempRoot->children[charIndex] = createNode();
        }

        tempRoot->children[charIndex]->character = term[i];
        tempRoot = tempRoot->children[charIndex];
    }

    tempRoot->isEndOfTerm = 1;
    tempRoot->timesVisited++;

    return 1;
}

int getTermOccurence(struct trie* root, char* term, int level) {
    if (root == NULL)
      return 0;

    int i;
    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL && root->children[i]->character == term[level]) {
            if (level + 1 == strlen(term))
                return root->children[i]->timesVisited;
            else
                return getTermOccurence(root->children[i], term, level + 1);
        }
    }

    return 0;
}

/* Print the contents of a trie. */
void printTrieContents(const struct trie *root, int level, char* term) {
    if (root == NULL)
        return;

    if (root->isEndOfTerm) {
        term[level] = 0;
        printf("%s: %d\n", term, root->timesVisited);
    }

    int i;
    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            term[level] = i + 'a';
            printTrieContents(root->children[i], level+1, term);
        }
    }
}

/* Destroy a trie. */
int destroyTrie(struct trie* root) {
  if (root == NULL)
      return 0;

  int i;
  for(i = 0; i < ALPHABET_SIZE; i++)
      if (root->children[i] != NULL)
          destroyTrie(root->children[i]);

  free(root);

  return 1;
}

/* Create a node for the trie. */
struct trie* createNode(void) {
    struct trie *newNode = NULL;
    newNode = (struct trie*) malloc(sizeof(struct trie));

    if(newNode == NULL) {
        return NULL;
    }

    newNode->isEndOfTerm = 0; 
    newNode->timesVisited = 0;
    newNode->character = 0;

    int i;
    for (i = 0; i < ALPHABET_SIZE; i++)
        newNode->children[i] = NULL;

    return newNode;
}

/* Get the text from a given URL. */
int getText(const char* srcAddr, char* buffer, const int bufSize) {
    FILE *pipe;
    int bytesRead;

    snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

    pipe = popen(buffer, "r");
    if(pipe == NULL) {
        fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
        return 0;
    }

    bytesRead = fread(buffer, sizeof(char), bufSize-1, pipe);
    buffer[bytesRead] = '\0';

    pclose(pipe);

    return bytesRead;
}