#include <avr/io.h>
#include <avr/interrupt.h>
#include "ExitDisp.h"


/*

class AvrExit : public ProcExit{
#define G1EXITPORT PORTB
#define G1END 7
#define G2EXITPORT PORTD
#define G2END 15

	public:
	
	void tugleExit(char exit_num, char state){
			if(exit_num <= G1END){ 
				if(state){
					UPBIT(G1EXITPORT, exit_num);
				}else{
					DOWNBIT(G1EXITPORT, exit_num);
				}
				return;
			}
			
			if(exit_num <= G2END){ 
				if(state){
					UPBIT(G2EXITPORT, (exit_num - 8));
				}else{
					DOWNBIT(G2EXITPORT, (exit_num - 8));
				}
				return;
			}
	}
};

Tugling alwayson[] = {{Tugling::ON, Tugling::ALWAYS},{0,0}};
Tugling alwaysoff[] = {{Tugling::OFF, Tugling::ALWAYS},{0,0}};
Tugling blink_70_70[] = {{Tugling::ON, 70},{Tugling::OFF,70}};
Tugling blink_20_50_50_20[] = {{Tugling::ON, 20}, {Tugling::OFF, 50}, {Tugling::ON, 50}, {Tugling::OFF, 20}};
Tugling blink_20_20_20_60[] = {{Tugling::ON, 20}, {Tugling::OFF, 20}, {Tugling::ON, 20}, {Tugling::OFF, 60}};

volatile static TuggleAlgorithm strob[] = { {alwayson, 1},
											{alwaysoff, 1},
											{blink_70_70, 2},
											{blink_20_50_50_20,4},
											{blink_20_20_20_60,4} };
*/

#define ALWAYS_ON 0
#define ALWAYS_OFF 1
#define STROB_START 2
#define STROB_STOP 4
volatile unsigned char strob_pos = STROB_STOP; 
											
#define NO_CHECK 0
#define YES_CHECK 1
volatile char checkflag = 0;

#define MAIN_SHORT_LIGHT 1
#define MAIN_LONG_LIGHT 2
#define MAIN_SIZE_LIGHT 3

enum CommandResult { NO, NEXTSTROB};

#define USER_PORT_PIN PINA
#define USER_PIN_NUM 0							
#define STROB_ON_BLINK_COUNT 3
#define STROB_ON_BLINK_WAIT_PERIOD 200 // 3 мигания за 2 секунды, проверка идет раз в 10мс

inline CommandResult checkUserCommand(){

	static char is_strob = 0;
	static char blink_count = 0;
	static char blink_was_released = 1;
	static char blibk_wait_count = 0;
	static char blibk_wait_start = 0;
	
	if(blibk_wait_start && (blibk_wait_count++ > STROB_ON_BLINK_WAIT_PERIOD)){
		blink_count = 0;
		blibk_wait_start = 0;
	}
	
	if ((USER_PORT_PIN & (1 << USER_PIN_NUM)) && blink_was_released){
	
		// если уже режим строба, то просто переключим
		if(is_strob)
			return NEXTSTROB;
			
		blink_count++;

		// соблюдены все условия, значит переключение строба
		if( blink_count == STROB_ON_BLINK_COUNT && blibk_wait_count <= STROB_ON_BLINK_WAIT_PERIOD){
			blink_count = 0;
			blibk_wait_count = 0;
			blink_was_released = 0;
			blibk_wait_start = 0;
			is_strob = 1;
			return NEXTSTROB;
		}
		
		if (blink_count == 1){
			blibk_wait_start = 1;
			blibk_wait_count = 0;
		}
		
	
		blink_was_released = 0;
	}else{
		blink_was_released = 1;
	}
	
	return NO;
}

void SetupTIMER1 (void)
{

	sei();
     TCCR1B = (1<<CS12);
     TCNT1 = 65536-50;        
     TIMSK |= (1<<TOIE1); 
}


ISR (TIMER1_OVF_vect)
{


//	INVBIT(PORTB,3);


	// run timer
	TCNT1 = 65536 - 1000; //  31220;
    TCCR1B = (1<<CS12);
    TIMSK |= (1<<TOIE1);


}

inline void interruptTimerFunc(){
	checkflag = YES_CHECK;
}
							
int main(){

SetupTIMER1();
DDRB = 0b00011111;

//UPBIT(PORTB,3);

	while (1) {
		;
	}

/*

	Exit main_short_light(MAIN_SHORT_LIGHT, strob[ALWAYS_ON]), // по умолчанию линии открыты, када идет питание лапм,
	     main_long_light (MAIN_LONG_LIGHT,  strob[ALWAYS_ON]), // они сразу же работают, как при обычной эксплуатации
		 main_size_light(MAIN_SIZE_LIGHT,  strob[ALWAYS_ON]);
	AvrExit avext;	 
	ExitDisp disp(&avext);

	disp.addExit(main_short_light);
	disp.addExit(main_long_light);
	disp.addExit(main_size_light);	
	
	disp.firstCheckExits();
	
	while(1){
		if (checkflag){
			checkflag = NO_CHECK;
			
			CommandResult res = checkUserCommand();
			switch(res){
				case NEXTSTROB:{
					if (strob_pos++ > STROB_STOP)
						strob_pos = STROB_START;
						
					main_short_light.reInit(MAIN_SHORT_LIGHT, strob[strob_pos]);
					main_long_light.reInit(MAIN_LONG_LIGHT, strob[strob_pos]);
					main_size_light.reInit(MAIN_SIZE_LIGHT, strob[ALWAYS_OFF]);
					
					disp.changeExitByNumber(main_short_light, MAIN_SHORT_LIGHT);
					disp.changeExitByNumber(main_long_light, MAIN_LONG_LIGHT);
					disp.changeExitByNumber(main_size_light, MAIN_SIZE_LIGHT);
				}
				case NO:
				default:{
					break;
				}
			}
			disp.checkExits();	
		}
	}

*/

}
