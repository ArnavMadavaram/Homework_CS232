#include "webQuery.h"

void webQuery(struct listNode *urlHead)
{
    char *queryTerms[MAX_USER_INPUT];
    int numOfTerms;

    while (1)
    { // iterate until user enters empty input 
        do
        {
            printf("\n");
            char userInput[MAX_USER_INPUT];
            getUserInput(userInput);
            numOfTerms = tokenizeUserInput(userInput, queryTerms);

            if (isUserInputEmpty(userInput))
                return; // return control back to main 

            if (!isUserInputValid(queryTerms, numOfTerms))
            {
                fflush(stdout);
                fprintf(stderr, "Please enter a query containing only lower-case letters.\n");
            }

        } while (!isUserInputValid(queryTerms, numOfTerms));

        // print query information to stdout
        printQueryTerms(queryTerms, numOfTerms);

        printf("IDF scores are\n");
        int documentCount = getAddrCount(urlHead, 0);
        int atLeastOneOccurenceInDocumentArr[numOfTerms];

        for (int i = 0; i < numOfTerms; i++)
        {
            int atLeastOneOccurence = getAtLeastOneDocumentOccurrence(urlHead, queryTerms[i], 0);
            atLeastOneOccurenceInDocumentArr[i] = atLeastOneOccurence;
            printf("IDF(%s): %.7f\n", queryTerms[i], calcInverseDocumentFrequency(documentCount, atLeastOneOccurence));
        }

        printf("Web pages:\n");
        calcScore(urlHead, queryTerms, numOfTerms, atLeastOneOccurenceInDocumentArr, documentCount);
        sortList(urlHead);
        printAddressesWithScores(urlHead, 1);
    }
}

void getUserInput(char *userInput)
{
    printf("Enter a web query: ");
    if (!fgets(userInput, MAX_USER_INPUT, stdin))
        userInput[0] = '\0';

    userInput[strlen(userInput) - 1] = '\0';
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
        for (int j = 0; j < strlen(queryTerms[i]); j++)
            if (!isalpha(queryTerms[i][j]) || !islower(queryTerms[i][j]))
                return 0; // Returns false

    return 1; // Returns true
}

/* Print all the query terms. */
void printQueryTerms(char **queryTerms, int numOfTerms)
{
    printf("Query is \"");
    for (int i = 0; i < numOfTerms; i++)
    {
        printf("%s", queryTerms[i]);
        if (i != numOfTerms - 1) {
            printf(" ");
        }
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