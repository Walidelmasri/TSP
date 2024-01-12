#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
// #include "coordReader.c"
// #include "ompcInsertion.c"
// #include "ompnInsertion.c"
// #include "ompfInsertion.c"
#include <mpi.h>

int readNumOfCoords(char *fileName);
double **readCoords(char *filename, int numOfCoords);
double cInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile);
double fInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile);
double nInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile);

void fill_matrix(double *matrix, int numOfCoords)
{
    double(*mat)[numOfCoords] = (double(*)[numOfCoords])matrix;
    for (int i = 0; i < numOfCoords; i++)
    {
        mat[i][0] = i;
    }
}
void fill_DM(double *matrix, int numOfCoords, double **coords)
{
    double(*mat)[numOfCoords] = (double(*)[numOfCoords])matrix;
    for (int i = 0; i < numOfCoords; i++)
    {
        for (int j = 0; j < numOfCoords; j++)
        {
            mat[i][j] = sqrt(pow(coords[j][0] - coords[i][0], 2) + pow(coords[j][1] - coords[i][1], 2));
        }
    }
}
void print_DM(double *matrix, int numOfCoords)
{
    double(*mat)[numOfCoords] = (double(*)[numOfCoords])matrix;
    for (int i = 0; i < numOfCoords; i++)
    {
        for (int j = 0; j < numOfCoords; j++)
        {
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }
}

void print_matrix(double *matrix, int numOfCoords)
{
    double(*mat)[numOfCoords] = (double(*)[numOfCoords])matrix;
    for (int i = 0; i < numOfCoords; i++)
    {

        printf("%f ", mat[i][0]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    // setting up mpi

    int rank, commSize, from, to, localRows;
    int root = 0;
    // MPI_Init(NULL, NULL);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    int numOfCoords;
    // char *filename = argv[1];
    // char *outputFile = argv[2];
        char *filename = argv[1];
        char *outputFileC = argv[2];
        char *outputFileF = argv[3];
        char *outputFileN = argv[4];
    // // uncomment to run on personal pc
    // char *filename;
    // filename = "512_coords.coord";
    // // filename = "/Users/walidsalem/Desktop/SEM1/ParallelProgramming/Assignment1/COMP528_CA1_FILES/512_coords.coord";
    // char *outputFile;
    // outputFile = "c16ca2.txt";
    if (rank == root)
    {
        // Read number of coords in file
        numOfCoords = readNumOfCoords(filename);
        printf("Number of Coordinates: %d\n", numOfCoords);
        printf("Number of Commsize: %d\n", commSize);

    }
    numOfCoords = readNumOfCoords(filename);

    double minimumTotalCostC, minimumTotalCostF, minimumTotalCostN, *allC, *allF, *allN;
    double totalCostcInsertion, totalCostfInsertion, totalCostnInsertion, tStart, tEnd;
    int bestPositioncInsertion, bestPositionfInsertion, bestPositionnInsertion;
    int finalPrint;
    double *allPositions = (double *)malloc(numOfCoords * sizeof(double));
    double *costC = (double *)malloc(numOfCoords * sizeof(double));
    double *costF = (double *)malloc(numOfCoords * sizeof(double));
    double *costN = (double *)malloc(numOfCoords * sizeof(double));
    double *gatheredC = (double *)malloc(numOfCoords * numOfCoords * sizeof(double));
    double *distM = (double *)malloc(numOfCoords * numOfCoords * sizeof(double));
    // double **cost;
    double tolerance = 1e-9;
    double *coords = (double *)malloc(numOfCoords * numOfCoords * sizeof(double));
    if (rank == root)
    {
        allC = (double *)malloc(numOfCoords * sizeof(double));
        allF = (double *)malloc(numOfCoords * sizeof(double));
        allN = (double *)malloc(numOfCoords * sizeof(double));

        double **coords = (double **)malloc(numOfCoords * sizeof(double *));
        for (int i = 0; i < numOfCoords; i++)
        {
            coords[i] = (double *)malloc(numOfCoords * sizeof(double));
        }
        // Get coords and store in array
        coords = readCoords(filename, numOfCoords);
    tStart = MPI_Wtime();

        fill_DM(distM, numOfCoords, coords);
        // printf("Distance Matrix Done\n");
        // print_DM(distM, numOfCoords);
    }
    //     // int *fullY = (int *)malloc(numOfCoords * numOfCoords * sizeof(int));
    //     // int *fullZ = (int *)malloc(numOfCoords * numOfCoords * sizeof(int));

    MPI_Bcast(distM, numOfCoords * numOfCoords, MPI_DOUBLE, root, MPI_COMM_WORLD);
    // if (numOfCoords % commSize != 0)
    // {
    //     exit(-1);
    // }
    localRows = numOfCoords / commSize;
    from = rank * localRows;
    to = (rank + 1) * localRows;

    // printf("Rank %d Comsize = %d local rows = %d from = %d to = %d\n", rank, commSize, localRows, from, to);
    double *localC = (double *)malloc(localRows * sizeof(double));
    double *localF = (double *)malloc(localRows * sizeof(double));
    double *localN = (double *)malloc(localRows * sizeof(double));
    minimumTotalCostC = 100000000;
    minimumTotalCostF = 100000000;
    minimumTotalCostN = 100000000;
    finalPrint = 0;
    bestPositioncInsertion = -1;
    bestPositionfInsertion = -1;
    bestPositionnInsertion = -1;

    MPI_Scatter(costC, localRows, MPI_DOUBLE, localC, localRows, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Scatter(costF, localRows, MPI_DOUBLE, localF, localRows, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Scatter(costN, localRows, MPI_DOUBLE, localN, localRows, MPI_DOUBLE, root, MPI_COMM_WORLD);


    for (int startingPosition = from; startingPosition < to; startingPosition++)
    {
        totalCostcInsertion = cInsertionCost(numOfCoords, distM, startingPosition, finalPrint, outputFileC);
        localC[startingPosition - from] = totalCostcInsertion;
        totalCostfInsertion = fInsertionCost(numOfCoords, distM, startingPosition, finalPrint, outputFileF);
        localF[startingPosition - from] = totalCostfInsertion;
        totalCostnInsertion = nInsertionCost(numOfCoords, distM, startingPosition, finalPrint, outputFileN);
        localN[startingPosition - from] = totalCostnInsertion;

    }
    
    MPI_Gather(localC, localRows, MPI_DOUBLE, allC, localRows, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Gather(localF, localRows, MPI_DOUBLE, allF, localRows, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Gather(localN, localRows, MPI_DOUBLE, allN, localRows, MPI_DOUBLE, root, MPI_COMM_WORLD);


    MPI_Barrier(MPI_COMM_WORLD);


    if (rank == root)
    {
        for (int i = 0; i < numOfCoords; i++)
        {
            // printf("Position %d localC %f cost %f\n", i, localC[i], allC[i]);
        if (allC[i] < minimumTotalCostC)
        {
            minimumTotalCostC = allC[i];
            bestPositioncInsertion = i;
        }
        if (allF[i] < minimumTotalCostF - tolerance)
        {
            minimumTotalCostF = allF[i];
            bestPositionfInsertion = i;
        }
        if (allN[i] < minimumTotalCostN)
        {
            minimumTotalCostN = allN[i];
            bestPositionnInsertion = i;
        }

        }
    tEnd = MPI_Wtime();
        printf("\nProgram took %lf milliseconds\n", (tEnd - tStart) * 1000);

                printf("Best position Cheapest Insertion: %d\n", bestPositioncInsertion);
                printf("Best position Farthest Insertion: %d\n", bestPositionfInsertion);
                printf("Best position Nearest Insertion: %d\n", bestPositionnInsertion);

                finalPrint = 1;
                totalCostcInsertion = cInsertionCost(numOfCoords, distM, bestPositioncInsertion, finalPrint, outputFileC);
                totalCostfInsertion = fInsertionCost(numOfCoords, distM, bestPositionfInsertion, finalPrint, outputFileF);
                totalCostfInsertion = nInsertionCost(numOfCoords, distM, bestPositionnInsertion, finalPrint, outputFileN);
    }
    MPI_Finalize();
    return 0;
}