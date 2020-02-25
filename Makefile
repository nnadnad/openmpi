SRC := src/dijkstra.c
MPISRC := src/dijkstra_mpi.c

default: dijkstra_mpi

dijkstra: $(SRC)
	gcc -O3 -Wall -Wextra -o $@ $<

dijkstra_mpi: $(MPISRC)
	mpicc -O3 -Wall -Wextra -o $@ $<


clean: 
	rm -f dijkstra dijkstra_mpi