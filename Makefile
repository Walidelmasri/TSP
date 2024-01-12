gomp-only : main-openmp-only.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
	mpicc -fopenmp -std=c99 main-openmp-only.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c -o gomp-only -lm
gcomplete : main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
	mpicc -fopenmp -std=c99 main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c -o gcomplete -lm
iserial : main-openmp-only.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
	mpiicc -fopenmp -std=c99 main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c -o iomp -lm
icomplete : main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
	mpicc -fopenmp -std=c99 main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c -o icomplete -lm
