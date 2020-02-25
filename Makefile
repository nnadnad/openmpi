paralel: 
	@echo "Compiling parallel version of dijkstra ..."
	mpicc src/dijkstra_mpi.c -o src/dijkstra_mpi
	scp dijkstra_mpi 13517035@167.205.35.151:~
	scp dijkstra_mpi 13517035@167.205.35.152:~
	scp dijkstra_mpi 13517035@167.205.35.153:~
	scp dijkstra_mpi 13517035@167.205.35.154:~
	scp dijkstra_mpi 13517035@167.205.35.155:~
	@echo "Parallel dijkstra compiled."
serial:
	@echo "Compiling serial version of dijkstra ..." 
	gcc src/dijkstra_serial.c -o src/dijkstra_serial 
	@echo "Serial dijkstra compiled."

ifeq (mpirun, $(firstword $(MAKECMDGOALS)))
  runarg := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
  $(eval $(runarg):;@true)
endif
mpirun: src/dijkstra_mpi
	@echo "Opening parallel dijkstra..."
	mpirun -np 6 --hostfile src/mpi_hostfile src/dijkstra_mpi $(runarg)

ifeq (serialrun, $(firstword $(MAKECMDGOALS)))
  runargs := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
  $(eval $(runargs):;@true)
endif

serialrun: src/dijkstra_serial
	@echo "Opening serial dijkstra..."
	./src/dijkstra_serial $(runargs)


