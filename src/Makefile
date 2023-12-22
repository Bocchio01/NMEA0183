.PHONY: compile run bench toSTM doxy clean

CC = gcc
FLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -O2
SRCS = *.c sensors/*.c
BENCHDIR = benchmark
BENCHTARGET = benchmark
TARGET = main

STM_Path = ..\STM_Windex
STM_Path_Inc = $(STM_Path)\Core\Inc
STM_Path_Src = $(STM_Path)\Core\Src

all: run

compile: $(SRCS)
	$(CC) $(FLAGS) $(SRCS) -o $(TARGET)
	@echo Compilation complete

run: compile
	@$(TARGET).exe
	@echo Done runnig complete

bench:
	@$(CC) NMEA0183.c sensors.c sensors/*.c $(BENCHDIR)/*.c -o $(BENCHDIR)/$(BENCHTARGET)
	@$(BENCHDIR)\$(BENCHTARGET).exe
	@echo Benchmarking complete

# We must NOT copy main.c and main.h to STM project
toSTM: compile
	@copy NMEA0183.h $(STM_Path_Inc)
	@copy sensors.h $(STM_Path_Inc)
	@copy sensors\*.h $(STM_Path_Inc)\sensors

	@copy NMEA0183.c $(STM_Path_Src)
	@copy sensors.c $(STM_Path_Src)
	@copy sensors\*.c $(STM_Path_Src)\sensors

	@echo Copy operation to STM project complete

doxy: Doxyfile
	@doxygen
	@echo Doxygen documentation generated

clean:
	del /Q $(TARGET).exe