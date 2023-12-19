.PHONY: Main Launch clean

CC = gcc
SRCS = main.c NMEA0183.c sensors_handler.c sensors/*.c
TARGET = main

all: Launch

Main: $(SRCS)
	$(CC) $(SRCS) -o $(TARGET)
	@echo =================================

Launch: Main
	@$(TARGET).exe

clean:
	del /Q $(TARGET).exe
