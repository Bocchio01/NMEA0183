.PHONY: Main Launch ToSTM clean

CC = gcc
SRCS = main.c NMEA0183.c sensors.c sensors/*.c
TARGET = main

STM_Inc = ..\STM_Windex\Core\Inc
STM_Src = ..\STM_Windex\Core\Src

all: Launch

Main: $(SRCS)
	$(CC) $(SRCS) -o $(TARGET)
	@echo =================================

Launch: Main
	@$(TARGET).exe

ToSTM: Main
	@copy NMEA0183.h $(STM_Inc)
	@copy sensors.h $(STM_Inc)
	@copy sensors\*.h $(STM_Inc)\sensors

	@copy NMEA0183.c $(STM_Src)
	@copy sensors.c $(STM_Src)
	@copy sensors\*.c $(STM_Src)\sensors

	@echo Copy operation complete

clean:
	del /Q $(TARGET).exe