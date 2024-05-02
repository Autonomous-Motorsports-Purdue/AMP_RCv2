#include "spi.h"
#include "usart.h"

#include "LoRa.h"

#include "app_statemachine.h"

#define TICKS_PER_SEC	10
#define EBRAKE_MIN_SEC	5	// minimum seconds to remain in e-brake state

#define LORA_START_BYTE	0xFE	// HEX VALUE
#define LORA_STOP_BYTE	0xFF	// HEX VALUE

// maximum forward/reverse values for fast/slow state
#define SPEED_FWD_FAST	100
#define SPEED_REV_FAST	-80
#define SPEED_FWD_SLOW	50
#define SPEED_REV_SLOW	-30

// below are binary masks to send the button states in 1 byte
#define BTN_EBRAKE	0b00000001
#define BTN_AUTO	0b00000010
#define BTN_SLOW	0b00000100

State_T current_state;		// variable holding current controller state
uint32_t ticks_in_state;	// variable holding number of ticks in current state
uint32_t temp_data;			// to hold miscellaneous data within a state

uint8_t thrust;			// variable holding current thrust value
uint8_t steering;		// variable holding current steering value
char buttons;			// variable holding activated buttons

// function to be called before tick() function
void App_StateMachine_Init()
{
	// default variable values
	ticks_in_state = 0;
	temp_data = 0;
	thrust = 0;
	steering = 128;
	buttons = 0;
	// initialize LoRa
	LoRa lora;
	lora = newLoRa();

	lora.CS_port = LORA_NSS_GPIO_Port;
	lora.CS_pin = LORA_NSS_Pin;
	lora.reset_port = LORA_RST_GPIO_Port;
	lora.reset_pin = LORA_RST_Pin;
	lora.DIO0_port = LORA_DIO0_GPIO_Port;
	lora.DIO0_pin = LORA_DIO0_Pin;
	lora.hSPIx = &hspi1;

	lora.frequency = 915;

	LoRa_reset(&lora);
	uint16_t lora_debug = LoRa_init(&lora);
	if (lora_debug == LORA_OK)
	{
		HAL_UART_Transmit(&huart2, (unsigned char *) "LoRa OK\r\n", 9, 10);
	}
	else
	{
		HAL_UART_Transmit(&huart2, (unsigned char *) "LoRa FAILED\r\n", 13, 10);
	}
	// set current state to idle
	App_StateMachine_ChangeState(STATE_IDLE);
}

// function to be called on timer interrupt
void App_StateMachine_Tick()
{
	// statements to be called regardless of state
	ticks_in_state += 1;
	// TODO need lora driver send function here
	// run state-specific code
	switch (current_state)
	{
		case (STATE_IDLE):
		{
			App_StateMachine_ChangeState(STATE_EBRAKE);
			break;
		}

		case (STATE_SLOW):
		{
			// TODO
			break;
		}

		case (STATE_FAST):
		{
			// TODO
			break;
		}

		case (STATE_EBRAKE):
		{
			if (ticks_in_state > (EBRAKE_MIN_SEC * TICKS_PER_SEC))
			{
				// TODO only allow leaving after set amount of time
			}
			break;
		}

		case (STATE_ERROR):
		{
			// TODO
			break;
		}
	}
}

void App_StateMachine_ChangeState(State_T new_state)
{
	// statements to be called regardless of state transition
	ticks_in_state = 0;
	temp_data = 0;
	// change state
	current_state = new_state;
}
