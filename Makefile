mpi : dijkstra_mpi.o  \
        cc -o mpi dijkstra_mpi.o  \

main.o : src/dijkstra_mpi.c
        mpicc -c src/dijkstra_mpi.c
clean :
        rm edit dijkstra_mpi.o  \