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
// double fInsertionCost(int numOfCoords, double **distM, int startingPosition, int finalPrint, char *outputFile);

double fInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile)
{

    // variables to store the number of coords and number of threads
    int numThreads = omp_get_max_threads();
    // printf("Threads: %d\n", numThreads);

    // variables for timing the code
    double start, finish;
    // global variable that is given as a private variable for threads for computation of cheapest
    double Cost, value;
    double totalCost = 0;
    // global variables to store the position and the minimum cost to visit after the parallel computation
    int maxPosition;
    double maxCost, minFinalCost;
    double tolerance = 1e-9;
    // Array to store steps taken during journey
    //  global variables to store the indicies the position will be stored in betwen
    int indexA;
    int indexB;

    // Init variables
    // array to keep track of visited verticies
    int *toVisit = (int *)malloc(numOfCoords * sizeof(int));
    // array to store steps taken during journey
    int *tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    // arrays dedicated for threads only
    // initialized by setting their sizes to number of threads used
    // array to store the position to go to (to be computed)
    int *maxThreadPosition = (int *)malloc(numThreads * sizeof(int));
    // array to store indexA and indexB (to be computed)
    int *indexThread = (int *)malloc(numThreads * sizeof(int));
    int *indexThreadB = (int *)malloc(numThreads * sizeof(int));
    // array to store max and minimal costs (to be computed)
    double *maxThreadCost = (double *)malloc(numThreads * sizeof(double));
    double *minThreadCost = (double *)malloc(numThreads * sizeof(double));
    double (*dM)[numOfCoords] = (double(*)[numOfCoords])distM;

    // filling up tour array with 0s at the beginning
    for (int i = 0; i < numOfCoords + 1; i++)
    {
        tour[i] = startingPosition;
    }


    // start the timer right after the coords are loaded
    start = omp_get_wtime();

// calculate the distance matrix
// wrapped in pragma omp for with collapse set at 2
// #pragma omp for collapse(2)
//     for (int xx = 0; xx < numOfCoords; xx++)
//     {
//         for (int yy = 0; yy < numOfCoords; yy++)
//         {
//             double temp = sqrt(pow(coords[yy][0] - coords[xx][0], 2) + pow(coords[yy][1] - coords[xx][1], 2));
//             distM[xx][yy] = temp;
//         }
//     }
// debugging: uncomment to print the distance matrix
// printf("Distance Matrix\n");
// for(int x = 0; x < numOfCoords; x++){
// 	for(int y = 0; y < numOfCoords; y++){
// 		printf("%f    ", distM[x][y]);
// 	}
// 	printf("\n");
// }
// add all verticies to toVisit array at first as none of them are visited
// assuming that the code always starts from position 0
#pragma omp for
    for (int counter = 0; counter < numOfCoords; counter++)
    {
        toVisit[counter] = counter;
    }
    // counter for the entire journey
    toVisit[startingPosition] = -1;

    int visitCount = 0;
    // journey visits all of the coords except for position 0 as the trip starts and ends there
    while (visitCount < numOfCoords - 1)
    {
        maxCost = 0;
        minFinalCost = 1000000;
        indexA = -1;
        indexB = -1;
#pragma omp parallel
        {
            // local variable to get thread number
            int threadID = omp_get_thread_num();
            // setting the minimum and maximum for each thread to a number that is higher than all of the distances
            maxThreadCost[threadID] = 0;
            minThreadCost[threadID] = 1000000;
#pragma omp for private(value)
            for (int nextCheck = 0; nextCheck < numOfCoords; nextCheck++)
            {
                for (int positionBefore = 0; positionBefore < visitCount + 1; positionBefore++)
                {
                    int nextPosition = toVisit[nextCheck];
                    if (nextPosition != -1)
                    {
                        value = dM[tour[positionBefore]][nextPosition];
                        if (value > maxThreadCost[threadID])
                        {
                            maxThreadCost[threadID] = value;
                            maxThreadPosition[threadID] = nextPosition;
                        }
                    }
                }
            }
#pragma omp single
            {
                for (int j = 0; j < numThreads; j++)
                {
                    if (maxThreadCost[j] > maxCost)
                    {
                        maxPosition = maxThreadPosition[j];
                        maxCost = maxThreadCost[j];
                    }
                }
            }
#pragma omp for private(Cost)
            for (int positionBefore = 0; positionBefore < visitCount + 1; positionBefore++)
            {
                Cost = dM[tour[positionBefore]][maxPosition] + dM[maxPosition][tour[positionBefore + 1]] - dM[tour[positionBefore]][tour[positionBefore + 1]];
                if (Cost < minThreadCost[threadID])
                {
                    minThreadCost[threadID] = Cost;
                    // indexThread[threadID] = find_index(tour, visitCount + 2, tour[positionBefore]);
                    indexThread[threadID] = positionBefore;

                    indexThreadB[threadID] = positionBefore + 1;
                }
            }
#pragma omp single
            {
                for (int i = 0; i < numThreads; i++)
                {
                    if (minThreadCost[i] < minFinalCost)
                    {
                        minFinalCost = minThreadCost[i];
                        indexA = indexThread[i];
                        indexB = indexThreadB[i];
                    }
                }
                if (indexB == 0)
                {
                    indexB = visitCount + 1;
                }
                // loop to shift all of the elements for insertion
                for (int i = visitCount; i >= indexB; i--)
                {
                    int copy = tour[i];
                    tour[(i + 1)] = copy;
                }
                // indexA is index of position before in tour, so add 1 to index and store position
                tour[(indexA + 1)] = maxPosition;
            }

            //     toVisit[minimalPosition] = 0;
            toVisit[maxPosition] = -1;
        }
        visitCount++;
        totalCost = totalCost + minFinalCost;
    }
    // printf("Visiting Order: ");
    // for(int i = 0; i < numOfCoords + 1; i++){
    //     printf("%d ", tour[i]);
    // }

    // end the timer
    finish = omp_get_wtime();
    if (finalPrint == 1)
    {    
        printf("Time taken using %d threads: %f seconds\n", numThreads, finish - start);
        printf("Visiting Order for F: ");
        for (int i = 0; i < numOfCoords + 1; i++)
        {
            printf("%d ", tour[i]);
        }
        writeTourToFile(tour, numOfCoords + 1, outputFile);

    }
    // writeTourToFile(tour, numOfCoords + 1, outputFile);
    // // free all memory allocated to the different thread arrays
    // for (int i = 0; i < numOfCoords; i++)
    // {
    //     free(coords[i]);
    //     free(distM[i]);
    // }
    // free(coords);
    // free(distM);
    // free(toVisit);
    // free(tour);
    // free(maxThreadCost);
    // free(maxThreadPosition);
    // free(indexThread);
    // free(indexThreadB);
    return totalCost;
}