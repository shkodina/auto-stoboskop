#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <avr/sleep.h>

#include "alg.h"



typedef struct Exit_struct{
	Alg * alg;
	uchar num;
	uchar alg_pos;
	uint cur_time;
	uchar is_elapsed;


} Exit;

#define GTIMERLIGHTON 0
#define GTIMERLIGHTOFF 1
#define GTIMERSCOUNT 2
volatile SoftTimer g_timers[GTIMERSCOUNT];

#define LEFT

Tugling t_alwayson[] = {{TUG_ON, TUG_ALWAYS}};
Tugling t_alwaysoff[] = {{TUG_OFF, TUG_ALWAYS}};

#ifdef LEFT
Tugling t_blink_70_70[] = {{TUG_ON,70},{TUG_OFF,70}};
Tugling t_blink_mayak[] = {{TUG_ON, 50}, {TUG_OFF, 50},{TUG_ON, 50}, {TUG_OFF, 50}, {TUG_ON, 50}, {TUG_OFF, 250}};
Tugling t_blink_20_20_20_150[] = {{TUG_ON, 30}, {TUG_OFF, 30}, {TUG_ON, 30}, {TUG_OFF, 70}};
#endif

#ifdef RIGHT
Tugling t_blink_70_70[] = {{TUG_ON,70},{TUG_OFF,70}};
Tugling t_blink_mayak[] = {{TUG_ON, 50}, {TUG_OFF, 50},{TUG_ON, 50}, {TUG_OFF, 50}, {TUG_ON, 50}, {TUG_OFF, 250}};
Tugling t_blink_20_20_20_150[] = {{TUG_ON, 30}, {TUG_OFF, 30}, {TUG_ON, 30}, {TUG_OFF, 70}};
#endif

Alg alg[] = {
						{1,1,t_alwaysoff}, 			// 0 
				 		{1,1,t_alwayson}, 			// 1 
				 		{2,2,t_blink_70_70},  		// 2 	
				 		{6,6,t_blink_mayak},		// 3 			
				 		{4,4,t_blink_20_20_20_150}	// 4 			
}; 

#define STROB_START 2
#define STROB_STOP 4

#define  LIGHTPORT PORTB
#define  DDRLIGHTPORT DDRB
#define  LIGHTPIN PIND
#define  COMMANDMASK 0b00000100 
#define  LIGHTCOUNT 2
#define  LIGHTTIMERVALUE 1000

#define  GABARITLTPIN 4
#define  GABARITLT 2
#define  SHORTLTPIN 3
#define  SHORTLT 1
#define  LONGLTPIN 2
#define  LONGLT 0

#define  GEXITSCOUNT 3
volatile Exit g_exits [GEXITSCOUNT] = {{alg+1, LONGLTPIN}, {alg+1, SHORTLTPIN}, {alg+1, GABARITLTPIN}};

void SetupTIMER1 (void)
{
	TCCR1B = (1<<CS12);
	TCNT1 = 65536-11550;        
	TIMSK |= (1<<TOIE1); 
	sei();
}


ISR (TIMER1_OVF_vect)
{

	for (uchar i = 0; i < GEXITSCOUNT; i++ ){
		if(!(g_exits[i].is_elapsed)){
			if(	g_exits[i].cur_time++ >= g_exits[i].alg->tugling_arr[g_exits[i].alg_pos].ms10_period)
			{	
				g_exits[i].is_elapsed = TRUE;
			}
			
		}
	}

	for (uchar i = 0; i < GTIMERSCOUNT; i++ ){
		if(!(g_timers[i].is_elapsed))
			if(g_timers[i].cur++ >= g_timers[i].val){
				g_timers[i].is_elapsed = TRUE;
				g_timers[i].count++;
				//INVBIT(LIGHTPORT,2);
			}
					
	}
	
	//DEBUG
	//INVBIT(LIGHTPORT,3);
		
	// reinit timer
	TCNT1 = 65536 - 30; //  31220;
    TCCR1B = (1<<CS12);
    TIMSK |= (1<<TOIE1);
}

void blink(int x){
	for (int i=0; i<x; i++){
		UPBIT(LIGHTPORT,3);
		_delay_ms(1500);
		DOWNBIT(LIGHTPORT,3);
		_delay_ms(1500);


	}

	_delay_ms(2500);

}

void checkCommand(){

	static	uchar is_strob = FALSE;
	static	uchar was_off = FALSE;
	static	uchar next_strob = FALSE;
	static	uchar strob_pos = STROB_STOP;

	// chaeck command

	//is_strob = TRUE;

	if (is_strob){
		if (LIGHTPIN & COMMANDMASK){
			if (was_off && is_strob){
				next_strob = TRUE;
				strob_pos++;
			}
			was_off = FALSE;
		}else{
			was_off = TRUE;
		}

		if (next_strob){
			if (strob_pos > STROB_STOP)
				strob_pos = STROB_START;

			g_exits[GABARITLT].alg = alg;
			g_exits[SHORTLT].alg = alg;
			g_exits[LONGLT].alg = alg + strob_pos;

			for (uchar i = 0; i < GEXITSCOUNT; i++ ){
				g_exits[i].is_elapsed = TRUE;
				g_exits[i].alg_pos = g_exits[i].alg->pos;
			}

			next_strob = FALSE;
		}
	}else{
		if (LIGHTPIN & COMMANDMASK){
			if (g_timers[GTIMERLIGHTON].is_elapsed){
				if(!g_timers[GTIMERLIGHTON].cur){ //first start
					g_timers[GTIMERLIGHTON].is_elapsed = FALSE;
					return;
				}else{
					if(g_timers[GTIMERLIGHTON].count >= LIGHTCOUNT * 3){ // facke alarm just long light
						//g_timers[GTIMERLIGHTON].cur = 0;
						//g_timers[GTIMERLIGHTON].count = 0;
						return;
					}else{
						g_timers[GTIMERLIGHTON].cur = 0;
						g_timers[GTIMERLIGHTON].is_elapsed = FALSE;
					}
				}
			}
		}else{
			// DEBUG
			//blink(g_timers[GTIMERLIGHTON].count);
			if (g_timers[GTIMERLIGHTON].count >= LIGHTCOUNT && 
				g_timers[GTIMERLIGHTON].count < LIGHTCOUNT * 2){
				is_strob = TRUE;
			}else{
				g_timers[GTIMERLIGHTON].cur = 0;
				g_timers[GTIMERLIGHTON].count = 0;
				g_timers[GTIMERLIGHTON].is_elapsed = TRUE;
			}
		}
	}
}

int main(){

	DDRLIGHTPORT = 0b00011111;

	for (uchar i = 0; i < GEXITSCOUNT; i++ ){
		g_exits[i].is_elapsed = TRUE;
		g_exits[i].alg_pos = g_exits[i].alg->pos;
	}



	g_timers[GTIMERLIGHTON].val = LIGHTTIMERVALUE;
	g_timers[GTIMERLIGHTON].is_elapsed = TRUE;

	SetupTIMER1();



	sleep_enable();

	while(1){

		for (uchar i = 0; i < GEXITSCOUNT; i++ ){
			if(g_exits[i].is_elapsed){
				if(++g_exits[i].alg_pos >= g_exits[i].alg->len){
					g_exits[i].alg_pos = 0;
				}

				if(g_exits[i].alg->tugling_arr[g_exits[i].alg_pos].state == TUG_ON){
					UPBIT(LIGHTPORT,g_exits[i].num);
				}else{
					DOWNBIT(LIGHTPORT,g_exits[i].num);
				}

				g_exits[i].is_elapsed = FALSE;
				g_exits[i].cur_time = 0;
			}
		}
		checkCommand();
		sleep_cpu();
	}
}


