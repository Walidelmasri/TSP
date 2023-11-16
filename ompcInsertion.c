#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>
// #include "coordReader.c"
#include<omp.h>

int readNumOfCoords(char *fileName);
double **readCoords(char *filename, int numOfCoords);
void *writeTourToFile(int *tour, int tourLength, char *filename);
int find_index(int arr[], int size, int element) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == element) {
            return i;
        }
    }
    return -1; // element not found
}

int main(int argc, char *argv[]){

    // int numOfCoords;
    // char *filename = argv[1];
    // char *outputFile = argv[2];
    char *filename;
    filename = "9_coords.coord";
    char *outputFile;
    outputFile = "testC4096";
	//Read number of coords in file
    numOfCoords = readNumOfCoords(filename);
    printf("Number of Coordinates: %d\n", numOfCoords);

    // double distM[numOfCoords][numOfCoords];

    //Always
	//Always
	//Always starting at position 0
	int nextPosition = 0;
    int minimalPosition;
    //Array to store steps taken during journey
	int tour[numOfCoords + 1];
    int tourSize;
    int currentIndex;
	//Array to keep track of the available options
    int position = 0;
	double minimalCost;
	double min = 0;
	double totalCost = 0;
	double singleCost = 0;
    double left;
    double right;
    int lastPosition = 0;
    int indexA;
    int indexB;


// Init variables
	int *toVisit = (int *)malloc(numOfCoords * sizeof(int));

	double **coords = (double **)malloc(numOfCoords * sizeof(double *));
	for(int i = 0; i < numOfCoords; i++){
		coords[i] = (double *)malloc(numOfCoords * sizeof(double));
	}
	for(int i = 0; i < numOfCoords + 1; i++){
		tour[i] = 0;
	}
    double **distM = (double **)malloc(numOfCoords * sizeof(double));
    for(int j = 0; j < numOfCoords; j++){
        distM[j] = (double *)malloc(numOfCoords * sizeof(double));
    }

    // Get coords and store in array
	coords = readCoords(filename, numOfCoords);

	//Calculate the distance matrix
	#pragma omp parallel for
	for(int xx = 0; xx < numOfCoords; xx++){
		for (int yy = 0; yy < numOfCoords; yy++){
			distM[xx][yy] = sqrt(pow(coords[yy][0] - coords[xx][0], 2) + pow(coords[yy][1] - coords[xx][1], 2));
		}
	}
	//Uncomment to print the distance matrix
	printf("Distance Matrix\n");
	for(int x = 0; x < numOfCoords; x++){
		for(int y = 0; y < numOfCoords; y++){
			printf("%f    ", distM[x][y]);
		}
		printf("\n");
	}
    for(int counter = 0; counter < numOfCoords; counter++){
		toVisit[counter] = counter;
	}
// #pragma omp parallel for private(min, tour) shared(toVisit)
#pragma omp parallel for 
    for(int visitNumber = 0; visitNumber < numOfCoords - 1; visitNumber++){
		#pragma omp critical
		{
        min = 1000000;
        printf("Here's Visit %d\n", visitNumber);
        for(int nextCheck = 0; nextCheck < numOfCoords; nextCheck++){
            nextPosition = toVisit[nextCheck];
            if(nextPosition != 0){
                // printf("Position %d Minimal Costs\n", nextPosition);
                for(int positionBefore = 0; positionBefore < visitNumber + 1; positionBefore++){

                    // printf("Minimal Cost from %d to %d to %d\n", tour[positionBefore], nextPosition, tour[positionBefore + 1]);
                    minimalCost = distM[tour[positionBefore]][nextPosition] + distM[nextPosition][tour[positionBefore + 1]] - distM[tour[positionBefore]][tour[positionBefore + 1]];
                    if(minimalCost < min){
                        minimalPosition = nextPosition;
                        min = minimalCost;
                        indexA = find_index(tour, visitNumber + 2, tour[positionBefore]);
                        indexB = find_index(tour, visitNumber + 2, tour[positionBefore + 1]);
                    }
				}
            }
            }
        
        // printf("Minimal Position to visit is position %d with position %d before and position %d after\n", minimalPosition, indexA, indexB);
        // printf("Minimal Cost from %d to %d to %d\n", tour[indexA], minimalPosition, tour[indexB]);
        // printf("Store position to tour\n");

        if(visitNumber == 0){
            tour[visitNumber + 1] = minimalPosition;
        }
        else{
            if(indexB == 0){indexB = visitNumber + 2;}
            for(int i = visitNumber; i >= indexB; i--){
                int temp = tour[i];
                tour[i + 1] = temp;
            }
        tour[indexA + 1] = minimalPosition;
		}
        }
		#pragma omp critical
		{
        toVisit[minimalPosition] = 0;
		}
	    // printf("Visiting Order: ");
	    // for(int i = 0; i < numOfCoords + 1; i++){
		    // printf("%d ", tour[i]);
	    // }
    }

	// printf("Visiting Order: ");
	// for(int i = 0; i < numOfCoords + 1; i++){
		// printf("%d ", tour[i]);
	// }
    printf("\n");
    writeTourToFile(tour, numOfCoords + 1, outputFile);
    return 0;
}