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
// double cInsertionCost(int numOfCoords, double **distM, int startingPosition, int finalPrint, char *outputFile);
// find the 'element' position in a array
// int find_index(int arr[], int size, int element)
// {
//     for (int i = 0; i < size; i++)
//     {
//         if (arr[i] == element)
//         {
//             return i;
//         }
//     }
//     return -1; // element not found
// }

double cInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile)
{

    // // Read number of coords in file
    // numOfCoords = readNumOfCoords(filename);
    // printf("Number of Coordinates: %d\n", numOfCoords);
    int numThreads = omp_get_max_threads();
    // printf("Threads: %d\n", numThreads);
    // variables for timing the code
    // printf("still running");
    double start, finish;
    double totalCost = 0;
    // global variable that is given as a private variable for threads for computation of cheapest
    double minimalCost;
    // global variables to store the position and the minimum cost to visit after the parallel computation
    int minimalPosition;
    double minCost;
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
    int *minimalThreadPosition = (int *)malloc(numThreads * sizeof(int));
    // array to store indexA and indexB (to be computed)
    int *indexThread = (int *)malloc(numThreads * sizeof(int));
    int *indexThreadB = (int *)malloc(numThreads * sizeof(int));
    // array to store minimal cost (to be computed)
    double *minimalThreadCost = (double *)malloc(numThreads * sizeof(double));
    double (*dM)[numOfCoords] = (double(*)[numOfCoords])distM;


    // filling up tour array with starting position at the beginning
    for (int i = 0; i < numOfCoords + 1; i++)
    {
        tour[i] = startingPosition;
    }

    // start the timer right after the coords are loaded
    start = omp_get_wtime();

// add all verticies to toVisit array at first as none of them are visited
// assuming that the code always starts from position 0
#pragma omp for
    for (int counter = 0; counter < numOfCoords; counter++)
    {
        toVisit[counter] = counter;
    }

    toVisit[startingPosition] = -1;
    // counter for the entire journey
    int visitCount = 0;
    // journey visits all of the coords except for position 0 as i start and end there
    while (visitCount < numOfCoords - 1)
    {
        minCost = 1000000;
        indexA = -1;
        indexB = -1;
// printf("Here's Visit %d\n", visitNumber);
// parallelizing the calculation of the minimal distance
#pragma omp parallel
        {
            // local variable to get thread number
            int threadID = omp_get_thread_num();
            // setting the minimum for each thread to a number that is higher than all of the distances
            minimalThreadCost[threadID] = 100000000;
// pragma omp for loop with collapse set at 2 to calculate the distances for all of the verticies
// private minimalCost as each thread will calculate a minimalCost to compare to
#pragma omp for collapse(2) private(minimalCost)
            // loop over all of the verticies
            for (int nextCheck = 0; nextCheck < numOfCoords; nextCheck++)
            {
            //     // loop to check the index the minimum position will be stored at
                for (int i = 0; i < visitCount + 1; i++)
                {
            //         // nextPosition is a variable to store the next destination to check in the for loop
                    int nextPosition = toVisit[nextCheck];
            //         // if the position has not been sert to 0 (visited)
                    if (nextPosition != -1)
                    {
                        minimalCost = dM[tour[i]][nextPosition] + dM[nextPosition][tour[(i + 1)]] - dM[tour[i]][tour[(i + 1)]];
            //             // if the calculated minimal cost is less than the threads minimal cost
            //             // store minimal cost, position to visit and indicies before and after in tour
                        if (minimalCost < minimalThreadCost[threadID])
                        {
                            minimalThreadPosition[threadID] = nextPosition;
                            minimalThreadCost[threadID] = minimalCost;
                            // int indexTemp = find_index(tour, visitCount + 2, tour[i]);
                            int indexTemp = i;
                            indexThread[threadID] = indexTemp;
                            indexThreadB[threadID] = i + 1;
                        }
                    }
                }
            }
// pragma omp single to find the minimum cost among all of the threads
#pragma omp single
            {
//                 // loop over the number of threads
                for (int j = 0; j < numThreads; j++)
                {
                    // debugging... thank you so much for the help :)
                    // printf("Minimum Position from Thread %d is %d with cost %f\n", j, minimalPosition, minCost);
                    if (minimalThreadCost[j] < minCost)
                    {
                        minCost = minimalThreadCost[j];
                        minimalPosition = minimalThreadPosition[j];
                        indexA = indexThread[j];
                        indexB = indexThreadB[j];
                    }
                }
                // at this stage the minimup position has been chosen and stored in global variables
                // minimalPosition variable to add to tour will be used outside the parallel section to set as visited
                // if it is the first visit store position as the first position to go to
                // if(visitCount == 0){
                //     tour[visitCount + 1] = minimalPosition;
                // }
                // else{
                // if indexB is 0, it is definitely the index of the first 0 which is the starting point
                // so it is changed to last position in tour + 1
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
                tour[(indexA + 1)] = minimalPosition;
                //     toVisit[minimalPosition] = 0;

                // }
            }
            // set the value of the position added to tour as 0 for if condition in the earlier loop
            toVisit[minimalPosition] = -1;
        }
        // debugging purposes
        // printf("Visit Count: %d and Minimal Position: %d with Position %d before and Position %d after\n", visitCount, minimalPosition, indexA, indexB);
        // increment visitCount counter for while loop
        totalCost = totalCost + minCost;
        visitCount++;
    }
    // print out the visiting order, uncomment it out if it is needed
    // printf("Visiting Order: ");
    // for(int i = 0; i < numOfCoords + 1; i++){
    //     printf("%d ", tour[i]);
    // }

    // end the timer
    finish = omp_get_wtime();
    if (finalPrint == 1)
    {    
        printf("Time taken using %d threads: %f seconds\n", numThreads, finish - start);
        printf("Visiting Order: ");
        for (int i = 0; i < numOfCoords + 1; i++)
        {
            printf("%d ", tour[i]);
        }
        writeTourToFile(tour, numOfCoords + 1, outputFile);

    }
    // writeTourToFile(tour, numOfCoords + 1, outputFile);
    // free all memory allocated to the different thread arrays
    // for (int i = 0; i < numOfCoords; i++)
    // {
    //     free(coords[i]);
    //     free(distM[i]);
    // }
    // free(coords);
    // free(distM);
    // free(toVisit);
    // free(tour);
    // free(minimalThreadCost);
    // free(minimalThreadPosition);
    // free(indexThread);
    // free(indexThreadB);
    return totalCost;
}