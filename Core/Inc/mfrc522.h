/*
 * NXP MFRC522 library
 * by Erazem Kokot
 *
 * MFRC522 datasheet: https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf
 */

#ifndef INC_MFRC522_H_
#define INC_MFRC522_H_

/* Includes */
#include "main.h"

/* Pin and port definitions */
#define MFRC522_PIN_CS     GPIO_PIN_11 // D10 (PA11)
#define MFRC522_PORT_CS    GPIOA
#define MFRC522_PIN_MOSI   GPIO_PIN_15 // D11 (PB15)
#define MFRC522_PORT_MOSI  GPIOB
#define MFRC522_PIN_MISO   GPIO_PIN_14 // D12 (PB14)
#define MFRC522_PORT_MISO  GPIOB
#define MFRC522_PIN_SCLK   GPIO_PIN_12 // D13 (PA12)
#define MFRC522_PORT_SCLK  GPIOA

/* Status enumeration */
typedef enum {
	RFID_OK = 0, RFID_NOTAGERR, RFID_ERR, RFID_TIMEOUT,
} MFRC522_Status_t;

/* Exported functions */
extern void MFRC522_Init(void);
extern uint8_t MFRC522_Version();
extern MFRC522_Status_t MFRC522_CheckCard(uint8_t *id, uint8_t *type);
extern MFRC522_Status_t MFRC522_CompareIDs(uint8_t *id1, uint8_t *id2);
extern void MFRC522_PrettyPrint(unsigned char *in, unsigned int size, char **out);

/*
 * Register definitions (chapter 9)
 */

/* Page 0: Command and status */
#define MFRC522_COMMAND_REG         0x01 // starts and stops command execution
#define MFRC522_COML_EN_REG         0x02 // enable and disable interrupt request control bits
#define MFRC522_DIVL_EN_REG         0x03 // enable and disable interrupt request control bits
#define MFRC522_COML_RQ_REG         0x04 // interrupt request bits
#define MFRC522_DIVL_RQ_REG         0x05 // interrupt request bits
#define MFRC522_ERROR_REG           0x06 // error bits showing the error status of the last command executed
#define MFRC522_STATUS1_REG         0x07 // communication status bits
#define MFRC522_STATUS2_REG         0x08 // receiver and transmitter status bits
#define MFRC522_FIFO_DATA_REG       0x09 // input and output of 64 byte FIFO buffer
#define MFRC522_FIFO_LEVEL_REG      0x0a // number of bytes stored in the FIFO buffer
#define MFRC522_WATER_LEVEL_REG     0x0b // level for FIFO underflow and overflow warning
#define MFRC522_CONTROL_REG         0x0c // miscellaneous control registers
#define MFRC522_BIT_FRAMING_REG     0x0d // adjustments for bit-oriented frames
#define MFRC522_COLL_REG            0x0e // bit position of the first bit-collision detected on the RF interface

/* Page 1: Command */
#define MFRC522_MODE_REG            0x11 // defines general modes for transmitting and receiving
#define MFRC522_TX_MODE_REG         0x12 // defines transmission data rate and framing
#define MFRC522_RX_MODE_REG         0x13 // defines reception data rate and framing
#define MFRC522_TX_CONTROL_REG      0x14 // controls the logical behavior of the antenna driver pins TX1 and TX2
#define MFRC522_TX_ASK_REG          0x15 // controls the setting of the transmission modulation
#define MFRC522_TX_SEL_REG          0x16 // selects the internal sources for the antenna driver
#define MFRC522_RX_SEL_REG          0x17 // selects internal receiver settings
#define MFRC522_RX_THRESHOLD_REG    0x18 // selects thresholds for the bit decoder
#define MFRC522_DEMOD_REG           0x19 // defines demodulator settings
#define MFRC522_MF_TX_REG           0x1c // controls some MIFARE communication transmit parameters
#define MFRC522_MF_RX_REG           0x1d // controls some MIFARE communication receive parameters
#define MFRC522_SERIAL_SPEED_REG    0x1f // selects the speed of the serial UART interface

/* Page 2: Configuration */
#define MFRC522_CRC_RESULT_REG_H    0x21 // shows the MSB and LSB values of the CRC calculation (higher bits)
#define MFRC522_CRC_RESULT_REG_L    0x22 // shows the MSB and LSB values of the CRC calculation (lower bits)
#define MFRC522_MOD_WIDTH_REG       0x24 // controls the ModWidth setting
#define MFRC522_RFC_FG_REG          0x26 // configures the receiver gain
#define MFRC522_GS_N_REG            0x27 // selects the conductance of the antenna driver pins TX1 and TX2 for modulation
#define MFRC522_CW_GS_P_REG         0x28 // defines the conductance of the p-driver output during periods of no modulation
#define MFRC522_MOD_GS_P_REG        0x29 // defines the conductance of the p-driver output during periods of modulation
#define MFRC522_T_MODE_REG          0x2a // defines settings for the internal time
#define MFRC522_T_PRESCALER_REG     0x2b // defines settings for the internal time
#define MFRC522_T_RELOAD_REG_H      0x2c // defines the 16-bit timer reload value (higher bits)
#define MFRC522_T_RELOAD_REG_L      0x2d // defines the 16-bit timer reload value (lower bits)
#define MFRC522_T_COUNTER_REG_H     0x2e // shows the 16-bit timer value (higher bits)
#define MFRC522_T_COUNTER_REG_L     0x2f // shows the 16-bit timer value (lower bits)

/* Page 3: Test register */
#define MFRC522_TEST_SEL1_REG       0x31 // general test signal configuration
#define MFRC522_TEST_SEL2_REG       0x32 // general test signal configuration and PRBS control
#define MFRC522_TEST_PIN_EN_REG     0x33 // enables pin output driver on pins D1 to D7
#define MFRC522_TEST_PIN_VAL_REG    0x34 // defines the values for D1 to D7 when it is used as an I/O bus
#define MFRC522_TEST_BUS_REG        0x35 // shows the status of the internal test bus
#define MFRC522_AUTO_TEST_REG       0x36 // controls the digital self test
#define MFRC522_VERSION_REG         0x37 // shows the software version
#define MFRC522_ANALOG_TEST_REG     0x38 // controls the pins AUX1 and AUX2
#define MFRC522_TEST_DAC1_REG       0x39 // defines the test value for TestDAC1
#define MFRC522_TEST_DAC2_REG       0x3a // defines the test value for TestDAC2
#define MFRC522_TEST_ADC_REG        0x3b // shows the value of ADC I and Q channels

/*
 * Command set (chapter 10)
 */
#define MFRC522_COMMAND_IDLE        0b0000 // no action, cancels current command execution
#define MFRC522_COMMAND_MEM         0b0001 // stores 25 bytes into the internal buffer
#define MFRC522_COMMAND_GEN_RAND_ID 0b0010 // generates a 10-byte random ID number
#define MFRC522_COMMAND_CALC_CRC    0b0011 // activates the CRC coprocessor or performs a self test
#define MFRC522_COMMAND_TRANSMIT    0b0100 // transmits data from the FIFO buffer
#define MFRC522_COMMAND_NO_CHANGE   0b0111 // no command change, can be used to modify the CommandReg register bits without affecting the command, for example, the PowerDown bit
#define MFRC522_COMMAND_RECEIVE     0b1000 // activates the receiver circuits
#define MFRC522_COMMAND_TRANSCEIVE  0b1100 // transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
#define MFRC522_COMMAND_MF_AUTHENT  0b1110 // performs the MIFARE standard authentication as a reader
#define MFRC522_COMMAND_SOFT_RESET  0b1111 // resets the MFRC522

/*
 * PICCs
 */
#define MFRC522_PICC_REQ_IDL        0x26
#define MFRC522_PICC_REQ_ALL        0x52
#define MFRC522_PICC_ANTICOLLISION  0x93
#define MFRC522_PICC_SELECT_TAG     0x93
#define MFRC522_PICC_AUTHENT_1A     0x60 // authentication key A
#define MFRC522_PICC_AUTHENT_1B     0x61 // authentication key B
#define MFRC522_PICC_READ           0x30 // read block
#define MFRC522_PICC_WRITE          0xA0 // write block
#define MFRC522_PICC_DECREMENT      0xC0
#define MFRC522_PICC_INCREMENT      0xC1
#define MFRC522_PICC_RESTORE        0xC2 // transfer block data to the buffer
#define MFRC522_PICC_TRANSFER       0xB0 // save the data in the buffer
#define MFRC522_PICC_HALT           0x50 // sleep

#define MFRC522_MAX_LEN             16

#endif /* INC_MFRC522_H_ */
