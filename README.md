# NMEA0183 (for Windex)

This repo contains the library to read data from a NMEA0183 ecosystem.

It's written to be highly scalable and reusable for different NMEA0183 sensors.

So far, the following sensors are implemented and ready to be used:

- CV7-E (Windex)

## Library structure

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

To know more about the library structure, please refer to the generated [documentation](doxygen/index.html).

## How to use with STM32

To use the library with STM32, follow these steps:

1. Open your `.ioc` file with STM32CubeMX and from the connectivity tab click on `USART2` and configure it as follows:

   1. Mode: Asynchronous

   2. Parameter Settings:
      1. Baud Rate: 4800 Bits/s
      2. Word Length: 8 Bits
      3. Parity: None
      4. Stop Bits: 1 Bit

   3. NVIC Settings:
      1. USART2 global interrupt: Enabled

2. Generate the code

3. Copy and paste the library files inside the `Core` folder of your STM32 project keeping the following structure:

```bash
Core
├── Inc
│   ├── sensors
│   │   ├── SENSOR_*.h
│   │   └── ...
│   ├── NMEA0183.h
│   └── sensors.h
└── Src
    ├── sensors
    │   ├── SENSOR_*.c
    │   └── ...
    ├── NMEA0183.c
    └── sensors.c
```

4. Open each `SENSOR_*.h` file (e.g. `mwv.h`), and substitute the following line:

```c
#include "SENSOR_*.h" // -> Previous line
#include "sensors/SENSOR_*.h" // -> New line
```

5. Open the `main.c` file and add the following code:

```c
/* USER CODE BEGIN Includes */
#include <string.h>

#include "NMEA0183.h"
#include "sensors/mwv.h"
#include "sensors/xdr.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define STM32
/* USER CODE END PD */

/* USER CODE BEGIN PV */
uint8_t UART2_rxBuffer[2] = {0};
NMEA0183_t *nmea0183;
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
// https://forum.digikey.com/t/easily-use-printf-on-stm32/20157/2
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
/* USER CODE END PFP */

/* USER CODE BEGIN 2 */
nmea0183 = NMEA0183_Init();
NMEA0183_RegisterSensor(nmea0183, MWV_Init());
NMEA0183_RegisterSensor(nmea0183, XDR_Init());

HAL_UART_Receive_IT(&huart2, UART2_rxBuffer, 1);
/* USER CODE END 2 */


/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	NMEA0183_CharacterHandler(nmea0183, UART2_rxBuffer[0]);
	HAL_UART_Receive_IT(&huart2, UART2_rxBuffer, 1);
}
/* USER CODE END 4 */

```

## How to test

To test the library, you can either connect the CV7-E sensor or simulate it using an external program such as `PuTTY` or `MATLAB`.

Notice that the test program written in `MATLAB` inside the `datalogs` folder is still not able to comunicate properly with the STM32.

## Windex connection

To be clarified.

<!--
| CV7-E  | Function         | STM32 |
| ------ | ---------------- | ----- |
| Blue   | Ground 0V        |       |
| Red    | Power Supply 12V |       |
| Yellow | +NMEA            |       |
| Green  | -NMEA            |       |

![Connection circuit](assets/img/circuit.png)
-->


## Usefull links

- [SammyB428/NMEA0183](https://github.com/SammyB428/NMEA0183): focused on modularity and simplicity of use, `C++`
- [ttlappalainen/NMEA0183](https://github.com/ttlappalainen/NMEA0183): complete and well written library, `C++`
- [jrcutler/NMEA0183](https://github.com/jrcutler/NMEA0183): quit and dirty approach, `C++`
