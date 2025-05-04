#include "webQuery.h"

void webQuery(struct listNode *urlHead)
{
    char *queryTerms[MAX_USER_INPUT];
    int numOfTerms;

    while (1)
    {
        char userInput[MAX_USER_INPUT];

        // Keep prompting until valid input is received
        while (1)
        {
            printf("\n");
            getUserInput(userInput);

            if (isUserInputEmpty(userInput))
                return; // Exit if input is empty

            numOfTerms = tokenizeUserInput(userInput, queryTerms);

            if (isUserInputValid(queryTerms, numOfTerms))
                break;

            fflush(stdout);
            fprintf(stderr, "Please enter a query containing only lower-case letters.\n");
        }

        // Print the query
        printQueryTerms(queryTerms, numOfTerms);

        // Calculate IDF
        printf("IDF scores are\n");
        int documentCount = getAddrCount(urlHead, 0);
        int atLeastOneOccurenceInDocumentArr[numOfTerms];

        for (int i = 0; i < numOfTerms; i++)
        {
            int atLeastOneOccurence = getAtLeastOneDocumentOccurrence(urlHead, queryTerms[i], 0);
            atLeastOneOccurenceInDocumentArr[i] = atLeastOneOccurence;
            printf("IDF(%s): %.7f\n", queryTerms[i],
                   calcInverseDocumentFrequency(documentCount, atLeastOneOccurence));
        }

        // Score and sort
        printf("Web pages:\n");
        calcScore(urlHead, queryTerms, numOfTerms,
                  atLeastOneOccurenceInDocumentArr, documentCount);
        sortList(urlHead);
        printAddressesWithScores(urlHead, 1);
    }
}

void getUserInput(char *userInput)
{
    printf("Enter a web query: ");
    if (!fgets(userInput, MAX_USER_INPUT, stdin))
    {
        userInput[0] = '\0';
        return;
    }

    size_t len = strlen(userInput);
    if (len > 0 && userInput[len - 1] == '\n')
        userInput[len - 1] = '\0';
}

int tokenizeUserInput(char *userInput, char **queryTerms)
{
    int noOfTerms = 0;
    char *token = strtok(userInput, " \n");

    while (token != NULL)
    {
        queryTerms[noOfTerms++] = token;
        token = strtok(NULL, " \n");
    }

    return noOfTerms;
}

int isUserInputEmpty(char *userInput)
{
    return userInput[0] == '\0';
}

int isUserInputValid(char **queryTerms, int numOfTerms)
{
    for (int i = 0; i < numOfTerms; i++)
    {
        const char *term = queryTerms[i];
        for (int j = 0; term[j] != '\0'; j++)
        {
            if (!islower(term[j]) || !isalpha(term[j]))
                return 0;
        }
    }
    return 1;
}

void printQueryTerms(char **queryTerms, int numOfTerms)
{
    printf("Query is \"");
    for (int i = 0; i < numOfTerms; i++)
    {
        printf("%s", queryTerms[i]);
        if (i < numOfTerms - 1)
            printf(" ");
    }
    printf("\".\n");
}

double calcTermFrequency(int termTotal, int documentTermTotal)
{
    return (double)termTotal / documentTermTotal;
}

double calcInverseDocumentFrequency(int documentCount, int atLeastOneOccurence)
{
    return log((1.0 + documentCount) / (1.0 + atLeastOneOccurence));
}
