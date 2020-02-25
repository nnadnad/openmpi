   all: program

   program: dijkstra_mpi.o
        mpicc src/dijkstra_mpi.o -o program 

   dijkstra_mpi.o: dijkstra_mpi.c
        mpicc -o src/dijkstra_mpi.o src/dijkstra_mpi.c

   clean:
       rm -f src/dijkstra_mpi.o program core *~