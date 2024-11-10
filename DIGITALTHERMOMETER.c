#include<xc.h>

#define _XTAL_FREQ 4000000

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define EN PORTDbits.RD2
#define RW PORTDbits.RD1
#define RS PORTDbits.RD0

unsigned int a, b, c, d, e, f;
unsigned int adc;
float temperature, adc1;

void delay(unsigned int i)
{
    while(i--);
}

void lcd_command(unsigned char a)
{
    PORTC = a;
    EN = 1;
    RS = 0;
    RW = 0;
    delay(500);
    EN = 0;
}

void chr(unsigned char b)
{
    PORTC = b;
    EN = 1;
    RS = 1;
    RW = 0;
    delay(500);
    EN = 0;
}

void str(const char *d, char n)
{
    char o;
    for(o = 0; o < n; o++)
    {
        chr(d[o]);
        delay(500);
    }
}

void lcd_intialise()
{
    lcd_command(0x38);
    lcd_command(0x06);
    lcd_command(0x0c);
    lcd_command(0x01);
}

void __interrupt() adc_conv(void)
{
    if(PIR1bits.ADIF == 1)
    {
        adc = (ADRESH << 8);
        adc = adc + ADRESL;
        PIR1bits.ADIF = 0;
    }
}

void main(void)
{
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1;

    TRISD = 0x00;
    TRISC = 0x00;

    PORTC = PORTD = 0x00;
    lcd_intialise();

    lcd_command(0x80);
    str("SENSOR:", 7);

    ADCON0 = 0x41;
    ADCON1 = 0x8E;

    while(1)
    {
        ADCON0 = ADCON0 | 0x04;

        adc1 = adc / 2.046;
        temperature = adc1 * 100;

        lcd_command(0x89);
        a = (int)temperature / 10;
        b = (int)temperature % 10;
        c = a % 10;
        d = a / 10;
        e = d % 10;
        f = d / 10;
        chr(f + 0x30);
        chr(e + 0x30);
        chr('.');
        chr(c + 0x30);
        chr(b + 0x30);
    }
    return;
}
