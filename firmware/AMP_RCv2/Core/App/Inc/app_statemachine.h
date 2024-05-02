#ifndef INC_APP_STATEMACHINE_H_
#define INC_APP_STATEMACHINE_H_

typedef enum // This is for the controller to switch states
{
	STATE_IDLE,		// idle state
	STATE_SLOW,		// car goes to slow speed
	STATE_FAST,		// car goes to max speed
	STATE_EBRAKE,	// after e-brake button pressed (cannot move)
	STATE_ERROR		// catch-all error state
} State_T;

void App_StateMachine_Init();
void App_StateMachine_Tick();
void App_StateMachine_ChangeState(State_T);

#endif /* INC_APP_STATEMACHINE_H_ */
