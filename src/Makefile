.PHONY: compile run bench toESP doxy clean

CC = gcc
FLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -O2
SRCS = *.c sensors/*.c
BENCHDIR = utils
BENCHTARGET = benchmark
TARGET = main

ESP_Path = ..\ESP_Windex\Src

all: run

compile: $(SRCS)
	$(CC) $(FLAGS) $(SRCS) -o $(TARGET)
	@echo Compilation complete

run: compile
	@$(TARGET).exe
	@echo Done runnig complete

bench:
	$(CC) NMEA0183.c sensors.c sensors/*.c $(BENCHDIR)/*.c -o $(BENCHDIR)/$(BENCHTARGET)
	@$(BENCHDIR)\$(BENCHTARGET).exe
	@echo Benchmarking complete

# We must NOT copy main.c and main.h to ESP project
toESP: compile
	@copy NMEA0183.h $(ESP_Path)
	@copy sensors.h $(ESP_Path)
	@copy sensors\*.h $(ESP_Path)\sensors

	@copy NMEA0183.c $(ESP_Path)
	@copy sensors.c $(ESP_Path)
	@copy sensors\*.c $(ESP_Path)\sensors

	@echo Copy operation to ESP project complete

doxy: Doxyfile
	@doxygen
	@echo Doxygen documentation generated

clean:
	del /Q $(TARGET).exe