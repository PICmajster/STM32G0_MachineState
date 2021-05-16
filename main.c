/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

File    : main.c
Autor : strefapic.blogspot.com
MCU: STM32G071KBT6

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stm32g071xx.h>

volatile uint8_t SoftTimer1 = 0  ;
void LED_OFF(void);
void LED_ON(void);

typedef enum {INITIAL, OFF_STATE, ON_STATE} state_t ; //nowy typ ze stanami maszyny

typedef struct {
state_t state ;
}state_machine_t;

state_machine_t MyMachine; //tworzymy nowy obiekt MyMachine o typie strukturalnym state_machine_t

typedef int(*sm_handler_t)(state_machine_t* ev); //nowy typ wskaznika na funkcje o nazwie sm_handler_t
/*funkcje handlery*/
static int sm_handler_state_INITIAL(state_machine_t* ev);
static int sm_handler_state_OFF_STATE(state_machine_t* ev);
static int sm_handler_state_ON_STATE(state_machine_t* ev);
/*glowny event maszyny stanow*/
int sm_event_handler(state_machine_t* ev);
/*tablica wskaznikow na funkcje/handlery*/
sm_handler_t handlers[] = {sm_handler_state_INITIAL, sm_handler_state_OFF_STATE, sm_handler_state_ON_STATE}; 

int main(void) {

SystemInit();
RCC->IOPENR |= RCC_IOPENR_GPIOAEN; //Open clock for GPIOA
/* PA8 set Out for LED*/
GPIOA->MODER &= ~GPIO_MODER_MODE8_1 ; /* MODE8 -> 0b01  */
/* SysTick config*/
SysTick_Config(SystemCoreClock * 0.5); //ok 0.5 s

MyMachine.state = INITIAL; //ustawiamy stan poczatkowy maszyny

while(1){
sm_event_handler(&MyMachine); //odpalamy glowny event maszyny MyMachine
}
}

int sm_event_handler(state_machine_t* ev){

return handlers[ev->state](ev);//wywolanie funkcji/handlera, indeksem tablicy jest aktualny stan maszyny
}

/*handler for state INITIAL*/
static int sm_handler_state_INITIAL(state_machine_t* ev){
 
if(!SoftTimer1){
   SoftTimer1 = 2; /*SysTick x SoftTimer1 = 0.5s x 2 = 1s*/
   ev->state = OFF_STATE;
   return 1;
 
 }
 return 0;
}
/*handler for state OFF_STATE*/
static int sm_handler_state_OFF_STATE(state_machine_t* ev){

if(!SoftTimer1){
   SoftTimer1 = 4 ; /*SysTick x SoftTimer1 = 0.5s x 4 = 2s*/
   LED_OFF();
   ev->state = ON_STATE ;
   return 1;
 
 }
return 0;
}
/*handler for state ON_STATE*/
static int sm_handler_state_ON_STATE(state_machine_t* ev){

if(!SoftTimer1){
    SoftTimer1 = 6 ; /*SysTick x SoftTimer1 = 0.5s x 6 = 3s*/
    LED_ON();
    ev->state = OFF_STATE ;
    return 1;
 
 }
return 0;
}

void LED_OFF(void){
GPIOA->ODR &= ~GPIO_ODR_OD8;
}

void LED_ON(void){
GPIOA->ODR |= GPIO_ODR_OD8;
}

/*Interrupt Handler for SysTick*/
void SysTick_Handler(void){

uint8_t x;
    x = SoftTimer1 ;
    if (x) SoftTimer1 = --x ;
   
}       