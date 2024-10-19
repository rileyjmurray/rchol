#EXE = driver driver_parallel
EXE = driver_parallel
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp rchol/*.cpp rchol_lap/*.cpp util/*.cpp csr/*.cpp))
OBJS := $(filter-out ex_laplace.o ex_laplace_parallel.o csr/driver.o,$(OBJS))

CFLAG = -Wall -O3 -Irchol/ -Irchol_lap/ -Iutil/ -Icsr/ ${CFLAGS}
LDFLAG = -lmkl_intel_ilp64 -mkl

# METIS_INC = $(TACC_METIS_INC)
# METIS_LIB = $(TACC_METIS_LIB)

# METIS is required for parallel
ifneq ($(and $(METIS_INC),$(METIS_LIB)),)
CFLAG += -I$(METIS_INC) -I$(AMD_INC)
LDFLAG += -L$(METIS_LIB) -lmetis -L$(AMD_LIB) -lamd
else ifneq ($(and $(MTMETIS_INC),$(MTMETIS_LIB)),)
CFLAG += -I$(MTMETIS_INC) -I$(AMD_INC)
LDFLAG += -L$(MTMETIS_LIB) -lmtmetis -L$(AMD_LIB) -lamd
else
POBJ = rchol/find_separator.o rchol/rchol_parallel.o
OBJS := $(filter-out $(POBJ),$(OBJS))
EXE := $(filter-out driver_parallel,$(EXE))
endif

# link TBB malloc proxy
TBB_MALLOC_PROXY = -L$(TBBROOT)/lib/intel64/gcc4.8/ -ltbbmalloc_proxy

ifneq ($(TBB_MALLOC_PROXY),)
LDFLAG += $(TBB_MALLOC_PROXY)
endif

all: $(EXE)

%.o: %.cpp %.hpp
	icpx -c $< -o $@ $(CFLAG)

%.o: %.cpp
	icpx -c $< -o $@ $(CFLAG)

driver: ex_laplace.o $(OBJS)
	icpx $^ -o $@ $(CFLAG) $(LDFLAG)  -L${MKLROOT}/lib -lmkl_intel_ilp64 -lmkl_tbb_thread -lmkl_core -lpthread -lm -ldl
	
driver_parallel: ex_parallel.o $(OBJS)
	icpx $^ -o $@ $(CFLAG) $(LDFLAG)  -L${MKLROOT}/lib -lmkl_intel_ilp64 -lmkl_tbb_thread -lmkl_core -lpthread -lm -ldl

	
run: $(EXE)
	./$(EXE)
	
clean:
	rm -f $(EXE) *.o rchol/*.o rchol_lap/*.o util/*.o csr/*.o
