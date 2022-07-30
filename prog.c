#define F_CPU 16000000L
#include <avr/io.h>			// obsługa portów
#include <stdlib.h> 		// abs()
#include <util/delay.h> 	// _delay_ms()
#include <math.h>   		// pow(a,x)
#include "hd44780.c"
#include <avr/interrupt.h> 

#define BTN_PD0_PRESSED   !(PIND & (1<<PD0) )


char * str(int16_t number)
{
    static char buffer[6];
    itoa(number,buffer,10);
    return buffer;
}

uint16_t adc(uint8_t kanal) 
{    
    ADMUX = (ADMUX & 0b11111000) | kanal; 
    ADCSRA |= (1<<ADSC); // flaga wystartowania  adcu
    while(ADCSRA & (1<<ADSC)); // dopuki flaga podniesiona...
    return ADCW;  // zwróc wynik
}

ISR(TIMER1_COMPA_vect) //procedura przerwania TMIERA1 Compare Match A (przerwanie co 1sek.)
{

    // tu coś sie może dziać co jedną sekundę

} 
    
int main(void)
{  
    // INICJALIZACJA PORTÓW ATMEGI
    // LCD   0=we,1=wy                stany
    DDRC  = 0b11111111;    PORTC = 0b00000000;	//LCD
    DDRD  = 0b11111100;    PORTD = 0b00000011;	//LED

    //  INICJALIZACJA PRZETWORNIKA ADC
    ADCSRA |= (1<<ADEN); // włącz ADC
    ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // preskaler = 16    
    ADMUX |= (1<<REFS0); // ref VCC

    // LCD
    LCD_Initalize();
	LCD_WriteText("SWR");
        
        
    // PRZERWANIE OD TIMERA    
    TCCR1B  |= (1<<WGM12);  // przerwanie Compare Match A 
    TCCR1B  |= (1<<CS12);  //ustawienie preskalera 1024
    TIMSK   |= (1<<OCIE1A);            
    
    OCR1A = 62500;                  //wartość rejestru porówania równa wartości częstotliwości zliczania impulsów 
                                    
    sei(); 
    

	while(1)
    {
        // wyswietlenie napiec
        LCD_WriteTextPos("    ",0,1);
        LCD_WriteTextPos(str(adc(0)),0,1);
        LCD_WriteTextPos("    ",5,1);
        LCD_WriteTextPos(str(adc(1)),5,1);
        
        // liczenie swr
        int16_t vol0 = adc(0);
        int16_t vol1 = adc(1);
        int16_t swr = ((vol0 + vol1)*10 / (vol0 - vol1));
        
       // wyswietlenie swr
        LCD_WriteTextPos("1:", 5,0);
        LCD_WriteTextPos("      ",7,0);
                
        if(swr > 0)
        {
            int16_t swrA = swr/10;
            int16_t swrB = (swr - (swrA*10));
            
            LCD_WriteTextPos(str(swrA), 7, 0);    
            LCD_WriteTextPos(".", 8+(swrA>=10), 0);    
            LCD_WriteTextPos(str(swrB), 9+(swrA>=10), 0);    
            
        }else{
            LCD_WriteTextPos("inf", 7, 0);    
        }

        _delay_ms(200);        
       
	}// end main loop

}// end main()
