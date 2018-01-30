#include <avr/io.h>
#include <avr/interrupt.h>
#define T0COUNT 0
uint16_t  volatile command=0x0000, aux=0x0000;
uint16_t i=8192;
void RC5_init(void){
		EICRA = (1<<ISC01); 		//FE INT0
		DDRD &= ~(1<<PD0);			// INT0 as input
		PORTD |= (1<<PD0);			// Activate internal pull-up resistors
		EIMSK |= (1<<INT0);			// INT0 enabled
		DDRC |= (1<<PC4);
}

void timer_init(void){
	TCCR0B = 0;				//para o temporizador
	TIFR0 |= (7<<TOV0);		//para interrupcoes
	TCCR0A = 0;				//modo normal
	TIMSK0 |= (1<<TOIE0);		//permite interrupcao quando ocorre overflow
	TCNT0 = 228;			//valor inicial de T0COUNT para 3/4 de bit
}

ISR (TIMER0_OVF_vect){
	TCNT0=150;                  //1 bit

	if((PIND & (1<<PD0))!=0){           //le algo em PD0
		command=command+i;            //soma bit a bit
	}

	if(i==1){
		aux=command;
		command=0x0000;                       //reset do command
		i=8192;                               //valor inicial de i  2^13
		//TIMSK0 &= ~(1<<TOIE0);		//desativa interrupcao
		TCCR0B = 0;				//para o temporizador
		TCNT0 = 228;			//valor inicial de T0COUNT para 1/4 de bit
		EIMSK |= (1<<INT0);      //ativa interrupcao int0
	}
	else i=i/2;		//valor inicial de T0COUNT para 1 bit
}

ISR (INT0_vect){
	TCCR0B = 4;				//TP=256 e CP=1 por omissao, inicia o timer
	EIMSK &= ~(1<<INT0);			// INT0 disabled
	PORTC = PORTC ^ (1<<PC4);      //ativar/desativar PC4
}


uint16_t RC5_NewCommandReceived(void){
	if(aux>10000) return 0;
	else return aux;
}

uint8_t RC5_GetToggleBit(uint16_t command){
	return ((command & 0x800) >> 11);
}
uint8_t RC5_GetAddressBits(uint16_t command){
	return ((command & 0x7C0) >> 6);
}
uint8_t RC5_GetCommandBits(uint16_t command){
	return (command & 0x3F);
}