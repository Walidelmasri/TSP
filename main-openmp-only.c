#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
// #include "coordReader.c"
// #include "ompcInsertion.c"
// #include "ompnAddition.c"

// #include "ompfInsertion.c"
// #include <mpi.h>


int readNumOfCoords(char *fileName);
double **readCoords(char *filename, int numOfCoords);
double cInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile);
double fInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile);
double nInsertionCost(int numOfCoords, double *distM, int startingPosition, int finalPrint, char *outputFile);
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
int main(int argc, char *argv[])
{
    //setting up mpi
    // int rank;
    // MPI_Init(NULL, NULL);
    // MPI_Comm_rank(MPI_COMM_WORLD);
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // printf("Hi from process ranked %d\n", rank);
    // MPI_Finalize();


    int numOfCoords;


    // uncomment to run on personal pc
    // char *filename;
    // filename = "16_coords.coord";
    // char *outputFile;
    // filename = "/Users/walidsalem/Desktop/SEM1/ParallelProgramming/Assignment1/COMP528_CA1_FILES/512_coords.coord";
    // outputFile = "c16ca2.txt";
    char *filename = argv[1];
    char *outputFileC = argv[2];
    char *outputFileF = argv[3];
    char *outputFileN = argv[4];
    // char *outputFileC = "c9.txt";
    // char *outputFileF = "f9.txt";
    // char *outputFileN = "n9.txt";
    double minimumTotalCostC, minimumTotalCostF, minimumTotalCostN;
    double totalCostcInsertion, totalCostfInsertion, totalCostnInsertion;
    double tolerance = 1e-9;
    int bestPositioncInsertion, bestPositionfInsertion, bestPositionnInsertion;
    int finalPrint;
    // Read number of coords in file
    numOfCoords = readNumOfCoords(filename);
    printf("Number of Coordinates: %d\n", numOfCoords);
    double **coords = (double **)malloc(numOfCoords * sizeof(double *));
    for (int i = 0; i < numOfCoords; i++)
    {
        coords[i] = (double *)malloc(numOfCoords * sizeof(double));
    }
    // double **distM = (double **)malloc(numOfCoords * sizeof(double));
    // for (int j = 0; j < numOfCoords; j++)
    // {
    //     distM[j] = (double *)malloc(numOfCoords * sizeof(double));
    // }
    double *distM = (double *)malloc(numOfCoords * numOfCoords * sizeof(double));
    // Get coords and store in array
    coords = readCoords(filename, numOfCoords);
    fill_DM(distM, numOfCoords, coords);
    // Calculate the distance matrix
    // for (int xx = 0; xx < numOfCoords; xx++)
    // {
    //     for (int yy = 0; yy < numOfCoords; yy++)
    //     {
    //         distM[xx][yy] = sqrt(pow(coords[yy][0] - coords[xx][0], 2) + pow(coords[yy][1] - coords[xx][1], 2));
    //     }
    // }
    minimumTotalCostC = 100000000;
    minimumTotalCostF = 100000000;
    minimumTotalCostN = 100000000;

    finalPrint = 0;
    bestPositioncInsertion = -1;
    bestPositionnInsertion = -1;
    bestPositionnInsertion = -1;
    for(int startingPosition = 0; startingPosition < numOfCoords - 1; startingPosition++)
    {
        totalCostcInsertion = cInsertionCost(numOfCoords, distM, startingPosition, finalPrint, outputFileC);
        if (totalCostcInsertion < minimumTotalCostC)
        {
            minimumTotalCostC = totalCostcInsertion;
            bestPositioncInsertion = startingPosition;
        }
        totalCostfInsertion = fInsertionCost(numOfCoords, distM, startingPosition, finalPrint, outputFileF);
        if (totalCostfInsertion < minimumTotalCostF - tolerance)
        {
            minimumTotalCostF = totalCostfInsertion;
            bestPositionfInsertion = startingPosition;
        }
        totalCostnInsertion = nInsertionCost(numOfCoords, distM, startingPosition, finalPrint, outputFileN);
        if (totalCostnInsertion < minimumTotalCostN - tolerance)
        {
            minimumTotalCostN = totalCostnInsertion;
            bestPositionnInsertion = startingPosition;
        }
        printf("Position %d\n", startingPosition);

    }
    // bestPositioncInsertion = 511;
    printf("Best position Cheapest Insertion: %d\n", bestPositioncInsertion);
    printf("Best position Farthest Insertion: %d\n", bestPositionfInsertion);
    printf("Best position Nearest Insertion: %d\n", bestPositionnInsertion);

    finalPrint = 1;
    totalCostcInsertion = cInsertionCost(numOfCoords, distM, bestPositioncInsertion, finalPrint, outputFileC);
    totalCostfInsertion = fInsertionCost(numOfCoords, distM, bestPositionfInsertion, finalPrint, outputFileF);
    totalCostfInsertion = nInsertionCost(numOfCoords, distM, bestPositionnInsertion, finalPrint, outputFileN);

}