#include "spi.h"
#include "usart.h"
#include "math.h"

#include "LoRa.h"

#include "ssd1306.h" //https://github.com/afiskon/stm32-ssd1306

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

uint16_t lora_debug;		// variable holding LoRa debug data

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


	// initialize OLED
	ssd1306_Init();
	ssd1306_Fill(SSD1306_COLOR_BLACK);

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
	lora_debug = LoRa_init(&lora);
	if (lora_debug == LORA_OK)
	{
		HAL_UART_Transmit(&huart2, (unsigned char *) "LoRa OK\r\n", 9, 10);
	}
	else
	{
		HAL_UART_Transmit(&huart2, (unsigned char *) "LoRa FAILED\r\n", 13, 10);
	}

	// initialize OLED
	


	// set current state to idle
	App_StateMachine_ChangeState(STATE_IDLE);
}

// function to be called on timer interrupt
void App_StateMachine_Tick()
{
	// statements to be called regardless of state
	ticks_in_state += 1;

	ssd1306_Fill(SSD1306_COLOR_BLACK); // clear OLED display
	ssd1306_SetCursor(0, 0); // set cursor to top left corner
	if (lora_debug == LORA_OK)
	{
		ssd1306_WriteString("LoRa OK", Font_11x18, SSD1306_COLOR_WHITE);
	}
	else
	{
		ssd1306_WriteString("LoRa FAILED", Font_11x18, SSD1306_COLOR_WHITE);
	}

	sssd1306_SetCursor(0, 20);
	ssd1306_WriteString("State: ", Font_11x18, SSD1306_COLOR_WHITE);




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
			ssd1306(SetCursor(0, 40));
			ssd1306_WriteString("SLOW", Font_11x18, SSD1306_COLOR_WHITE);
			Draw_Speedometer();
			break;
		}

		case (STATE_FAST):
		{
			// TODO
			ssd1306(SetCursor(0, 40));
			ssd1306_WriteString("FAST", Font_11x18, SSD1306_COLOR_WHITE);
			Draw_Speedometer();
			break;
		}

		case (STATE_EBRAKE):
		{
			ssd1306_SetCursor(0, 40);
			ssd1306_WriteString("EBRAKE", Font_11x18, SSD1306_COLOR_WHITE);
			if (ticks_in_state > (EBRAKE_MIN_SEC * TICKS_PER_SEC))
			{
				// TODO only allow leaving after set amount of time
			}
			break;
		}

		case (STATE_ERROR):
		{
			// TODO
			ssd1306(SetCursor(0, 40));
			ssd1306_WriteString("ERROR", Font_11x18, SSD1306_COLOR_WHITE);
			break;
		}
	}


	// update oled display
	ssd1306_UpdateScreen();
}

void App_StateMachine_ChangeState(State_T new_state)
{
	// statements to be called regardless of state transition
	ticks_in_state = 0;
	temp_data = 0;
	// change state
	current_state = new_state;
}


void Draw_Speedometer()
{
	// draw circle outline
	ssd1306_DrawArc(64, 32, 30, 45, 315, SSD1306_COLOR_WHITE);
	ssd1306_DrawArc(64, 32, 20, 45, 315, SSD1306_COLOR_WHITE);
	ssd1306_Line(64, 12, 64, 52, SSD1306_COLOR_WHITE);
	ssd1306_Line(34, 32, 94, 32, SSD1306_COLOR_WHITE);

	// draw speed indicator
	for (int i = 20; i < 30; i++)
	{
		ssd1306_DrawArc(64, 32, i, 45, 45 + abs(thrust/SPEED_FWD_FAST * 270), SSD1306_COLOR_WHITE);
	}

	// draw speed number in middle
	ssd1306_SetCursor(54, 24);
	char speed[2];
	sprintf(speed, "%d", thrust);
	ssd1306_WriteString(speed, Font_11x18, SSD1306_COLOR_WHITE);



}

void Draw_Steering() 
{
	// draw hollow rectangle along bottom
	ssd1306_DrawRectangle(4, 48, 124, 60, SSD1306_COLOR_WHITE);

	// draw filled rectangle for steering starting in the center and going to where the steering is pointing
	ssd1306_FillRectangle(64, 48, 64 + (steering), 60, SSD1306_COLOR_WHITE); // TODO: check what the steering variable is holding

	// write steering text on the top right of the rectangle
	ssd1306_SetCursor(96, 100);
	char steer[3];
	sprintf(steer, "%d", steering);
	ssd1306_WriteString(steer, Font_11x18, SSD1306_COLOR_WHITE);


}
