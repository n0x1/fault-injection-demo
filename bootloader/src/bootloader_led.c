#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Hardware Imports
#include "inc/hw_memmap.h"    // Peripheral Base Addresses
#include "inc/hw_types.h"     // Boolean type
#include "inc/tm4c123gh6pm.h" // Peripheral Bit Masks and Registers
// #include "inc/hw_ints.h" // Interrupt numbers

// Driver API Imports
#include "driverlib/flash.h"     // FLASH API
#include "driverlib/interrupt.h" // Interrupt API
#include "driverlib/sysctl.h"    // System control API (clock/reset)

// Application Imports
#include "driverlib/gpio.h"
#include "uart/uart.h"

// Forward Declarations
void load_firmware(void);
void boot_firmware(void);
long program_flash(uint32_t , unsigned char *, unsigned int);
void Delay(void);
void start_game(void);
void init_GPIO(void);

// Firmware Constants
#define METADATA_BASE 0xFC00 // base address of version and firmware size in Flash
#define FW_BASE 0x10000      // base address of firmware in Flash

// FLASH Constants
#define FLASH_PAGESIZE 1024
#define FLASH_WRITESIZE 4

// Protocol Constants
#define OK ((unsigned char)0x00)
#define ERROR ((unsigned char)0x01)
#define UPDATE ((unsigned char)'U')
#define BOOT ((unsigned char)'B')
#define GAME ((unsigned char)'G')


int offset = 0;
int state = 0;
int num_packets;
#define PACKET_SIZE 1
uint8_t data[1024*3] = {0};
	 
#define GPIO_PORTF_CLK_EN  0x20

#define DELAY_VALUE        16000  

#define OFF_LED 0x00
#define BLUE_LED 0x04
#define RED_LED 0x02
#define GREEN_LED 0x08
#define BUTTON_1 0x10

int main(void) {

    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Check if the peripheral access is enabled.
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {
    }


    initialize_uarts(UART0);

    uart_write_str(UART0, "Welcome to the BWSI Fault Injection Tester!\n");
    uart_write_str(UART0, "Send \"G\" to begin!\n");

    int resp;
    while (1) {
        uint32_t instruction = uart_read(UART0, BLOCKING, &resp);

         if (instruction == GAME) {
            uart_write_str(UART0, "G");
            uart_write_str(UART0, "\n Lighting up BLUE; will turn RED If exited through some fault!!!");
            start_game();
        }
    }
}

void init_GPIO(void){
    
    // GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    SYSCTL_RCGCGPIO_R |= GPIO_PORTF_CLK_EN;     //enable clock for PORTF

    volatile int delay;
    for(delay = 0; delay < 3; delay++);  // Wait for clock to stabilize
    
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R = BUTTON_1;
    
    GPIO_PORTF_DEN_R  |= RED_LED | BLUE_LED | GREEN_LED;    //enable pins 1-3 on PORTF
	GPIO_PORTF_DIR_R  |= RED_LED | BLUE_LED | GREEN_LED;    //make output pins
    GPIO_PORTF_DIR_R &= ~BUTTON_1;

    GPIO_PORTF_PUR_R |= BUTTON_1;

    GPIO_PORTF_DEN_R  |= BUTTON_1;    //button 1
    
}

void start_game(void){
    
    initialize_uarts(UART0);
    init_GPIO();
    GPIO_PORTF_DATA_R = BLUE_LED;    

    state = 2;
	while(1)
	{  
        if (state == 0) {
            break;
        }
	}
              
    GPIO_PORTF_DATA_R = RED_LED;    

}

void Delay(void){
	volatile unsigned long i;
	for(i=0;i<DELAY_VALUE;i++);
}
