.PHONY: Main Launch ToSTM Documentation GenerateDataset clean

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

Documentation: Doxyfile
	@doxygen
	@echo Documentation generated

GenerateDataset: utils/generate_dataset.c
	$(CC) NMEA0183.c sensors.c utils/generate_dataset.c -o utils/generate_dataset
	@utils/generate_dataset.exe
	@echo Dataset generated

clean:
	del /Q $(TARGET).exe