#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, const char **argv)
{
    int lowerBound, upperBound;

    if (argc != 3)
    {
        fprintf(stderr, "USAGE: %s lowerBound upperBound\n", argv[0]);
        return -1;
    }

    lowerBound = atoi(argv[1]);
    upperBound = atoi(argv[2]);

    if (lowerBound < 1 || upperBound < 1)
    {
        fprintf(stderr, "ERROR: both the lowerBound (%d) and the upperBound (%d) must be positive.\n",
                lowerBound, upperBound);
        return -2;
    }

    // running through the range and checking for primes
    for (int num = lowerBound; num <= upperBound; num++)
    {
        if (num < 2)
            continue; // Skipping numbers < 2 (1 is NOT prime)

        int isPrime = 1;   // Assuming the number is prime
        int j = sqrt(num); // Optimizing by checking up to sqrt(num)

        for (int i = 2; i <= j; i++)
        {
            if (num % i == 0)
            {
                isPrime = 0; // Not a prime number
                break;       // Exit loop early
            }
        }

        if (isPrime)
        {
            printf("%d\n", num); // printing each prime on a new line
        }
    }

    return 0;
}
