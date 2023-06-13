ifdef GPROF
CFLAGS += -O1 -pg
GPROFFLAG = -pg
else
CFLAGS += -O1
endif

CFLAGS += -fopenmp -I. 
ifndef $(SRCFILES)
SRCFILE=parallel_phase1.c sequential_phase1.c parallel_phase2.c sequential_phase2.c parallel_phase3.c sequential_phase3.c
endif

EXEC_FILE=Test
OBJ_FILE=$(patsubst %.c,%.o,$(SRCFILE))

all:  $(EXEC_FILE)

$(EXEC_FILE): $(OBJ_FILE) main.o
	gcc -o $(EXEC_FILE) -fopenmp $(OBJ_FILE) main.o -lm $(GPROFFLAG) 

$(OBJ_FILE): $(SRCFILE) $(HEADERS)
	gcc $(CFLAGS) -c $(SRCFILE)

main.o: main.c $(HEADERS)
	gcc $(CFLAGS) -c main.c

clean:
	rm -f $(EXEC_FILE) $(OBJ_FILE) main.o gmon.out
