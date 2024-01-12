#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
// #include "coordReader.c"
#include <omp.h>

// int readNumOfCoords(char *fileName);
// double **readCoords(char *filename, int numOfCoords);
void *writeTourToFile(int *tour, int tourLength, char *filename);
double nInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile)
{
    int numThreads = omp_get_max_threads();
    // Always
    // Always
    // Always starting at position 0
    int maxPosition;
    // Array to store steps taken during journey
    int tourSize = 1;
    int currentIndex;
    // Array to keep track of the available options
    double maxCost, maxCost2;

    double value;
    double maximumTour;
    double max = 0;
    double totalCost = 0;
    int indexA;
    int indexB;

    // Init variables
    int *toVisit = (int *)malloc(numOfCoords * sizeof(int));
    // Array to store steps taken during journey
    int *tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    int *minimalThreadPosition = (int *)malloc(numThreads * sizeof(int));
    // array to store indexA and indexB (to be computed)
    int *indexThread = (int *)malloc(numThreads * sizeof(int));
    int *indexThreadB = (int *)malloc(numThreads * sizeof(int));
    double *minimalThreadCost = (double *)malloc(numThreads * sizeof(double));
    double(*dM)[numOfCoords] = (double(*)[numOfCoords])distM;

// double **coords = (double **)malloc(numOfCoords * sizeof(double *));
// for (int i = 0; i < numOfCoords; i++)
// {
//     coords[i] = (double *)malloc(numOfCoords * sizeof(double));
// }
#pragma omp for
    for (int i = 0; i < numOfCoords + 1; i++)
    {
        tour[i] = startingPosition;
    }
#pragma omp for
    for (int counter = 0; counter < numOfCoords; counter++)
    {
        toVisit[counter] = counter;
    }
    toVisit[startingPosition] = -1;

    int visitCount = 0;
    while (visitCount < numOfCoords - 1)
    {
        max = 1000000;
        indexA = -1;
        indexB = -1;
// printf("Here's Visit %d\n", visitNumber);
#pragma omp parallel
        {
            int threadID = omp_get_thread_num();
            minimalThreadCost[threadID] = 1000000000;
#pragma omp for collapse(2) private(value)
            for (int nextCheck = 0; nextCheck < numOfCoords; nextCheck++)
            {
                for (int positionBefore = 0; positionBefore < visitCount + 1; positionBefore++)
                {
                    int nextPosition = toVisit[nextCheck];
                    if (nextPosition != -1)
                    {
                        value = dM[tour[positionBefore]][nextPosition];
                        if (value < minimalThreadCost[threadID])
                        {
                            minimalThreadCost[threadID] = value;
                            minimalThreadPosition[threadID] = nextPosition;
                            indexThread[threadID] = positionBefore;
                            indexThreadB[threadID] = positionBefore + 1;
                        }
                    }
                }
            }
#pragma omp single
            {
                // loop over the number of threads
                for (int j = 0; j < numThreads; j++)
                {
                    // debugging... thank you so much for the help :)
                    // printf("Minimum Position from Thread %d is %d with cost %f\n", j, minimalPosition, minCost);
                    if (minimalThreadCost[j] < max)
                    {
                        max = minimalThreadCost[j];
                        maxPosition = minimalThreadPosition[j];
                        indexA = indexThread[j];
                        indexB = indexThreadB[j];
                    }
                }
            }
        }

        // printf("Value %d between Position %d\n",maxPosition ,indexA);

        maximumTour = 0;
        // for (int positionBefore = 0; positionBefore < visitNumber + 1; positionBefore++)
        // printf("Index A : %d Index B: %d Tour Size: %d\n", indexA, indexB, tourSize);
        if (indexA > 0 && indexA != tourSize)
        {

            // maxCost = distM[tour[positionBefore]][maxPosition] + distM[maxPosition][tour[positionBefore + 1]] - distM[tour[positionBefore]][tour[positionBefore + 1]];
            maxCost = dM[tour[indexA]][maxPosition] + dM[maxPosition][tour[indexA + 1]] - dM[tour[indexA]][tour[indexA + 1]];
            maxCost2 = dM[tour[indexA - 1]][maxPosition] + dM[maxPosition][tour[indexA]] - dM[tour[indexA - 1]][tour[indexA]];
            // printf("Cost between %d and %d is %d\n", tour[indexA-1], tour[indexA], maxCost2);
            // printf("Cost between %d and %d is %d\n", tour[indexA], tour[indexA+1], maxCost);

            // if (maxCost < maximumTour)
            if (maxCost >= maxCost2)
            {
                maximumTour = maxCost2;
                // indexA = find_index(tour, visitNumber + 2, tour[positionBefore]);
                // printf("%d %d\n", indexA, positionBefore);
                indexB = indexA;
                indexA = indexA - 1;
            }
            else
            {
                maximumTour = maxCost;
            }
        }
        else
        {
            if (visitCount != 0)
            {
                maxCost = dM[tour[indexA]][maxPosition] + dM[maxPosition][tour[indexA + 1]] - dM[tour[indexA]][tour[indexA + 1]];
                maxCost2 = dM[tour[tourSize - 1]][maxPosition] + dM[maxPosition][tour[tourSize]] - dM[tour[tourSize - 1]][tour[tourSize]];
                // printf("Visit Number %d best position %d\n", visitNumber, maxPosition);
                // printf("Cost between %d and %d is %f\n", tour[tourSize-1], tour[tourSize], maxCost2);
                // printf("Cost between %d and %d is %f\n", tour[indexA], tour[indexA+1], maxCost);
                if (maxCost2 < maxCost)
                {
                    maximumTour = maxCost2;
                    // indexA = find_index(tour, visitNumber + 2, tour[positionBefore]);
                    // printf("%d %d\n", indexA, positionBefore);
                    indexB = tourSize;
                    indexA = tourSize - 1;
                }
                else
                {
                    maximumTour = maxCost;
                    indexB = indexA + 1;
                }
            }
        }

        if (visitCount == 0)
        {
            tour[visitCount + 1] = maxPosition;
            // printf("%f\n", maximumTour);
        }
        else
        {
            if (indexB == 0)
            {
                indexB = visitCount + 1;
            }
            for (int i = visitCount; i >= indexB; i--)
            {
                int temp = tour[i];
                tour[i + 1] = temp;
            }
            tour[indexA + 1] = maxPosition;
        }

        toVisit[maxPosition] = -1;
        tourSize = tourSize + 1;
        // totalCost = totalCost + maximumTour;
        visitCount = visitCount + 1;
        //     printf("Visiting Order: ");
        // for (int i = 0; i < tourSize + 1; i++)
        // {
        //     printf("%d ", tour[i]);
        // }
        // printf("\n");
    }
    #pragma omp for
    for (int a = 0; a < numOfCoords; a++)
    {
        double cost = dM[tour[a]][tour[a + 1]];
        totalCost = totalCost + cost;
    }

    if (finalPrint == 1)
    {
        // printf("Time taken using %d threads: %f seconds\n", numThreads, finish - start);
        printf("Visiting Order: ");
        for (int i = 0; i < numOfCoords + 1; i++)
        {
            printf("%d ", tour[i]);
        }
        writeTourToFile(tour, numOfCoords + 1, outputFile);
    }
    // writeTourToFile(tour, numOfCoords + 1, outputFile);
    // for (int i = 0; i < numOfCoords; i++)
    // {
    //     free(coords[i]);
    //     free(distM[i]);
    // }
    // free(coords);
    // free(distM);
    // free(toVisit);
    // free(tour);
    return totalCost;
}