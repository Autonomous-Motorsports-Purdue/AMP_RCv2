#ifndef INC_APP_STATEMACHINE_H_
#define INC_APP_STATEMACHINE_H_

#define INITIAL_GAMESTATE	GAMESTATE_IDLE
#define IDLE_SLEEP_TICKS	150000



typedef enum // This is for the controller to switch states
{
	STATE_IDLE,			// car cannot move
	STATE_SLOW,		// car goes a slow speed
	STATE_FAST,		//  CAR GOES AT MAX SPEED
	STATE_EMERGENCY_STOP,		// STOPS CART COMPLETLY UNTIL IT IS RESET
	STATE_TEST,		// Playing start animation for player B

} ControllerState;

typedef struct
{
	ControllerState state;
	int speed_max; // Speed of cart
	int speed_min;
	int thrust;
	int steer;
	int thrust_input;
	int steer_input;
	int e_brake;

} Cont_Info;

void App_StateMachine_Init();
Cont_Info newController();
Cont_Info App_StateMachine_Update(Cont_Info controller, int mode);
Cont_Info Controller_steer_thrust(Cont_Info controller);
Cont_Info Controller_setting(Cont_Info controller);
//char* Controller_data(Cont_Info Controller, LoRa lora_1);
//char* Controller_send_data(Cont_Info Controller, LoRa lora_1);


#endif /* INC_APP_STATEMACHINE_H_ */
