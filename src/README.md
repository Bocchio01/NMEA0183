# src folder

This folder contains the source code for the library NMEA0183 and all the sensors implemented, along with some utilities programs.

The library is written to be highly scalable and reusable for different NMEA0183 sensors.

So far, the following sensors are implemented and ready to be used:

- [MWV](https://gpsd.gitlab.io/gpsd/NMEA.html#_mwv_wind_speed_and_angle): Wind Speed and Angle
- [XDR](https://gpsd.gitlab.io/gpsd/NMEA.html#_xdr_transducer_measurement): Transducer Measurement

## Library structure

> [!TIP]
> A clean and well organized workspace is a must for a clean and well organized code.

The library is structured as follows:

```bash
src
├── sensors
│   ├── SENSOR_*.c
│   ├── SENSOR_*.h
│   └── ...
├── NMEA0183.c
├── NMEA0183.h
├── sensors.c
└── sensors.h
```

In particular:

- `NMEA0183.(c,h)`: Contains the implementation of the NMEA0183 protocol
- `sensors.(c,h)`: Defines the interface of a generic NMEA0183 sensor
- `sensors/`: Contains the implementation of each sensor that the library supports

All the code makes a heavy use of pointer to structures and function.

The main structure that contains all the information about the NMEA0183 ecosystem is the `NMEA0183_t` structure:

```c
typedef struct
{
	sentence_t *sentence; // All the information derived from the sentence and the sentence itself
	comunication_t *comunication; // All the information about the comunication (stage ans status)
	registered_sensor_t *registeredSensor; // All the information about the registered sensors (their reset, parser, printer functions and data)
} NMEA0183_t;
```

To know more about the library structure, please refer to the generated [documentation](.doxygen/index.html).

## Utilities

In the `utils` folder you can find:

- `benchmark.c`: a program that can be used to benchmark the library and save the results

## How to ...

All the following terminal commands are intended to be executed from the `src` folder.

To position yourself in the `src` folder, you can type in the terminal:

```bash
cd C:\Users\Path\To\The\Project\src
```

You can find in the [Usefull links](#usefull-links) section some links that can be useful to download the required software and to learn how to use it.

> [!WARNING]
> The `Makefile` is platform dependent and the one included here works for Windows.

### Compile the code

You can compile the code by typing:

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c99 -O2 *.c sensors/*.c -o main
./main.exe
```

Or, alternatively, you can use the `Makefile` included in the repository by typing:

```bash
make
```

### Generate the documentation

You can generate the documentation by typing:

```bash
doxygen .doxygen/Doxyfile
```

Or, alternatively:

```bash
make doxy
```

### Run the benchmark

You can run the benchmark by typing:

```bash
gcc NMEA0183.c sensors.c sensors/*.c utils/*.c -o utils/benchmark
./utils/benchmark.exe
```

Or, alternatively:

```bash
make bench
```

### Copy the code to the STM32 project

This assumes that you have already created a project named `STM_Windex` with STM32CubeMX inside the root folder of the repository and that you have already followed the steps described in the [root README.md](../README.md) file.

You can copy the code to the STM32 project by typing:

```bash
make toSTM
```

## Usefull links

Here follows some usefull links that are related to software that can help in speed up the development process.

- [MinGW](https://www.mingw-w64.org/): compiler and debugger. Needed to compile and debug the code without uploading it to the STM32
- [Visual Studio Code](https://code.visualstudio.com/): basically to go-to IDE for programming
- [C/C++ extension pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack): the extension that allows to work efficiently with C/C++ codebase in VSCode
- [Make](https://gnuwin32.sourceforge.net/packages/make.htm): needed to run the commands written in the `Makefile`
- [Doxygen](https://www.doxygen.nl/index.html): needed to generate the documentation

