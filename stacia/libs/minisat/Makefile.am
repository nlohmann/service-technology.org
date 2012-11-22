noinst_LIBRARIES = libminisat.a

libminisat_a_SOURCES = Vec.h System.h System.cc SimpSolver.h SimpSolver.cc \
Map.h Options.h Options.cc ParseUtils.h  IntTypes.h  Sort.h XAlloc.h Alloc.h SolverTypes.h \
 Solver.h Solver.cc Main.cc Heap.h Alg.h Queue.h Dimacs.h Heap.h 

# -D NDEBUG is necessary to have the "right" exit codes (10/20)
libminisat_a_CXXFLAGS = -ffloat-store -D NDEBUG
