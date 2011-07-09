noinst_LIBRARIES = libminisatp.a

libminisatp_a_SOURCES = Main.C Debug.C Debug.h Hardware.h Hardware_adders.C Hardware_clausify.C \
	Hardware_sorters.C Main.h MiniSat.C MiniSat.h PbParser.C PbParser.h PbSolver.C PbSolver.h \
	PbSolver_convert.C PbSolver_convertAdd.C PbSolver_convertBdd.C PbSolver_convertSort.C \
	SatELite.C SatELite.h Solver.h SolverTypes.h VarOrder.h ADTs/Global.C ADTs/Global.h \
	ADTs/FEnv.C ADTs/FEnv.h ADTs/File.C ADTs/File.h ADTs/Hash_standard.h ADTs/Heap.h \
	ADTs/Int.h ADTs/Map.h ADTs/Sort.h ADTs/StackAlloc.h ADTs/VecAlloc.h ADTs/VecMaps.h

# -D NDEBUG is necessary to have the "right" exit codes (10/20)
libminisatp_a_CXXFLAGS = -ffloat-store -D NDEBUG -D NO_GMP -IADTs -include Global.h -include Main.h -D_FILE_OFFSET_BITS=64
