.PHONY: Copy_to_ESP_project

ESP32_SensorNode_Path = ..\ESP32_Windex\SensorNode\src\NMEA0183
ESP32_BaseNode_Path = ..\ESP32_Windex\BaseNode\src\NMEA0183

Copy_to_ESP_project:
	@xcopy /E /Y src $(ESP32_SensorNode_Path)
	@xcopy /E /Y src $(ESP32_BaseNode_Path)

	@echo Copy operation to ESP project complete