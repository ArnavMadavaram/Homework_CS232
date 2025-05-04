#include "crawler.h"

int contains(const struct listNode *pNode, const char *addr)
{
    while (pNode != NULL)
    {
        if (strncmp(pNode->addr, addr, MAX_ADDR_LENGTH) == 0)
            return 1;
        pNode = pNode->next;
    }
    return 0;
}

void insertBack(struct listNode *pNode, const char *addr, struct trie *root, int *pTotalNumOfTerms)
{
    while (pNode->next != NULL)
        pNode = pNode->next;

    struct listNode *newNode = malloc(sizeof(struct listNode));
    if (newNode == NULL)
        return;

    newNode->next = NULL;
    strncpy(newNode->addr, addr, MAX_ADDR_LENGTH);
    newNode->trieRoot = root;
    newNode->score = 0;
    newNode->termCount = *pTotalNumOfTerms;

    pNode->next = newNode;
}

void printAddressesWithScores(const struct listNode *pNode, int level)
{
    while (pNode != NULL && level <= 3)
    {
        if (fabs(pNode->score) > 0.000001)
        {
            printf("%d. %s (score: %.4f)\n", level, pNode->addr, pNode->score);
            level++;
        }
        pNode = pNode->next;
    }
}

int getAddrCount(struct listNode *pNode, int addrCount)
{
    while (pNode != NULL)
    {
        addrCount++;
        pNode = pNode->next;
    }
    return addrCount;
}

int getAtLeastOneDocumentOccurrence(struct listNode *pNode, char *term, int count)
{
    while (pNode != NULL)
    {
        if (pNode->trieRoot != NULL)
        {
            int termCount = getTermOccurence(pNode->trieRoot, term, 0);
            if (termCount > 0)
                count++;
        }
        pNode = pNode->next;
    }
    return count;
}

void calcScore(struct listNode *pNode, char **queryTerms, int numOfTerms, int *atLeastOneOccurenceInDocumentArr, int documentCount)
{
    while (pNode != NULL)
    {
        double score = 0;
        for (int i = 0; i < numOfTerms; i++)
        {
            int documentTermTotal = pNode->termCount;
            int termTotal = getTermOccurence(pNode->trieRoot, queryTerms[i], 0);
            double tf = calcTermFrequency(termTotal, documentTermTotal);
            double idf = calcInverseDocumentFrequency(documentCount, atLeastOneOccurenceInDocumentArr[i]);
            score += tf * idf;
        }
        pNode->score = score;
        pNode = pNode->next;
    }
}

void sortList(struct listNode *pNode)
{
    if (pNode == NULL)
        return;

    int swapped;
    struct listNode *ptr1;
    struct listNode *lptr = NULL;

    do
    {
        swapped = 0;
        ptr1 = pNode;

        while (ptr1->next != lptr)
        {
            if (ptr1->score < ptr1->next->score)
            {
                swapNodes(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

void swapNodes(struct listNode *pNode1, struct listNode *pNode2)
{
    double tempScore = pNode1->score;
    pNode1->score = pNode2->score;
    pNode2->score = tempScore;

    struct trie *tempTrie = pNode1->trieRoot;
    pNode1->trieRoot = pNode2->trieRoot;
    pNode2->trieRoot = tempTrie;

    char tempAddr[MAX_ADDR_LENGTH];
    strncpy(tempAddr, pNode1->addr, MAX_ADDR_LENGTH);
    strncpy(pNode1->addr, pNode2->addr, MAX_ADDR_LENGTH);
    strncpy(pNode2->addr, tempAddr, MAX_ADDR_LENGTH);

    int tempTermCount = pNode1->termCount;
    pNode1->termCount = pNode2->termCount;
    pNode2->termCount = tempTermCount;
}

void destroyList(struct listNode *pNode)
{
    struct listNode *temp;
    while (pNode != NULL)
    {
        temp = pNode;
        pNode = pNode->next;
        destroyTrie(temp->trieRoot);
        free(temp);
    }
}

void webCrawl(FILE *file, struct listNode *urlHead, int MAX_N)
{
    int n = 0;
    int hopNum = 0;
    int hopsLimit = 0;
    char url[MAX_ADDR_LENGTH];
    strcpy(urlHead->addr, "");
    urlHead->next = NULL;

    while (fscanf(file, "%s %d", url, &hopsLimit) == 2 && n < MAX_N)
    {
        hopNum = 0;

        while (hopNum <= hopsLimit && n < MAX_N)
        {
            if (!contains(urlHead, url))
            {
                int totalNumOfTerms = 0;
                struct trie *trieRoot = indexPage(url, &totalNumOfTerms);
                n++;

                if (n == 1)
                {
                    urlHead->trieRoot = trieRoot;
                    urlHead->termCount = totalNumOfTerms;
                    urlHead->score = 0;
                    urlHead->next = NULL;
                    strncpy(urlHead->addr, url, MAX_ADDR_LENGTH);
                }
                else
                {
                    insertBack(urlHead, url, trieRoot, &totalNumOfTerms);
                }
            }

            hopNum++;
            if (hopNum <= hopsLimit && n < MAX_N)
            {
                if (!getLink(url, url, MAX_ADDR_LENGTH))
                    break;
            }
            else
            {
                break;
            }
        }
    }
}

int getLink(const char *srcAddr, char *link, const int maxLinkLength)
{
    const int bufSize = 1000;
    char buffer[bufSize];
    int numLinks = 0;

    snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getLinks.py", srcAddr);

    FILE *pipe = popen(buffer, "r");
    if (pipe == NULL)
    {
        fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
        return 0;
    }

    fscanf(pipe, "%d\n", &numLinks);

    if (numLinks > 0)
    {
        double r = (double)rand() / ((double)RAND_MAX + 1.0);
        for (int linkNum = 0; linkNum < numLinks; linkNum++)
        {
            fgets(buffer, bufSize, pipe);
            if (r < (linkNum + 1.0) / numLinks)
                break;
        }

        strncpy(link, buffer, maxLinkLength);
        link[maxLinkLength - 1] = '\0';

        char *pNewline = strchr(link, '\n');
        if (pNewline != NULL)
            *pNewline = '\0';
    }

    pclose(pipe);
    return numLinks > 0;
}
