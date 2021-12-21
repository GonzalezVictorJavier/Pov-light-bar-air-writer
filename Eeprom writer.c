#include <16f819.h>                       //pic a utilizar      
#fuses NOWDT
#fuses NOPROTECT
#fuses XT
#fuses NOPUT
#fuses NOLVP
#fuses NOMCLR
#fuses INTRC_IO
#use delay (clock=4000000)            //Fosc=4Mhz
#use fast_io(b) 
void graba_eeprom(char ,char * );
void main(void)
{
   int i = 0;
   int j = 0;
   char letra[4];
   set_tris_b(0b00000011);
   set_tris_a(0b00000000);
   disable_interrupts(GLOBAL);
   setup_oscillator(OSC_4MHZ);      //set INTRC to prescaler of 8MHz 
   for(i = 97, j = 0;i <= 122;i++, j = j +4)
   {
      graba_eeprom(i ,letra);
      write_eeprom(j, *(letra));
      write_eeprom(j + 1, *(letra + 1));
      write_eeprom(j + 2, *(letra + 2));
      write_eeprom(j + 3, *(letra + 3));      
   }
   while(TRUE)
   {
      delay_ms(300);
      output_a(0b11111111);
      delay_ms(300);
      output_a(0b00000000);
   }
}
void graba_eeprom(char letra ,char * p_letra )
{  
   switch(letra)
   {  
      case 'a':
         *(p_letra) =    0b11111110;
         *(p_letra + 1)= 0b00001001;
         *(p_letra + 2)= 0b00001001;
         *(p_letra + 3)= 0b11111110;      
      break;
      case 'b':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b10001001;
         *(p_letra + 2)= 0b10001001;
         *(p_letra + 3)= 0b01110110;      
      break;
      case 'c':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b10000001;
         *(p_letra + 2)= 0b10000001;
         *(p_letra + 3)= 0b10000001;
      break;
      case 'd':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b10000001;
         *(p_letra + 2)= 0b10000001;
         *(p_letra + 3)= 0b01111110;      
      break;
      case 'e':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b10001001;
         *(p_letra + 2)= 0b10001001;
         *(p_letra + 3)= 0b10000001;      
      break;
      case 'f':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b00001001;
         *(p_letra + 2)= 0b00001001;
         *(p_letra + 3)= 0b00000001;      
      break;
      case 'g':
         *(p_letra)=     0b01111110;
         *(p_letra + 1)= 0b10001001;
         *(p_letra + 2)= 0b10001001;
         *(p_letra + 3)= 0b01110001;      
      break;
      case 'h':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b00011000;
         *(p_letra + 2)= 0b00011000;
         *(p_letra + 3)= 0b11111111;      
      break;
      case 'i':
         *(p_letra)=     0b10000001;
         *(p_letra + 1)= 0b11111111;
         *(p_letra + 2)= 0b10000001;
         *(p_letra + 3)= 0b00000000;      
      break;
      case 'j':
         *(p_letra)=     0b10000000;
         *(p_letra + 1)= 0b10000001;
         *(p_letra + 2)= 0b01111111;
         *(p_letra + 3)= 0b00000001;      
      break;
      case 'k':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b00010100;
         *(p_letra + 2)= 0b00100010;
         *(p_letra + 3)= 0b11000001;      
      break;
      case 'l':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b10000000;
         *(p_letra + 2)= 0b10000000;
         *(p_letra + 3)= 0b10000000;      
      break;
      case 'm':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b00000100;
         *(p_letra + 2)= 0b00000100;
         *(p_letra + 3)= 0b11111111;      
      break;
      case 'n':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b00000100;
         *(p_letra + 2)= 0b00010000;
         *(p_letra + 3)= 0b11111111;      
      break;
      case 'o':
         *(p_letra)=     0b00111100;
         *(p_letra + 1)= 0b10000001;
         *(p_letra + 2)= 0b10000001;
         *(p_letra + 3)= 0b00111100;      
      break;
      case 'p':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b00001001;
         *(p_letra + 2)= 0b00001001;
         *(p_letra + 3)= 0b00000110;
      break;
      case 'q':
         *(p_letra)=     0b00111100;
         *(p_letra + 1)= 0b10000001;
         *(p_letra + 2)= 0b10100001;
         *(p_letra + 3)= 0b11111100;      
      break;
      case 'r':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b00001001;
         *(p_letra + 2)= 0b00101001;
         *(p_letra + 3)= 0b10000110;      
      break;
      case 's':
         *(p_letra)=     0b10000110;
         *(p_letra + 1)= 0b10001001;
         *(p_letra + 2)= 0b10001001;
         *(p_letra + 3)= 0b01110001;      
      break;
      case 't':
         *(p_letra)=     0b00000001;
         *(p_letra + 1)= 0b11111111;
         *(p_letra + 2)= 0b11111111;
         *(p_letra + 3)= 0b00000001;      
      break;
      case 'u':
         *(p_letra)=     0b01111111;
         *(p_letra + 1)= 0b10000000;
         *(p_letra + 2)= 0b10000000;
         *(p_letra + 3)= 0b01111111;      
      break;
      case 'v':
         *(p_letra)=     0b00011111;
         *(p_letra + 1)= 0b11000000;
         *(p_letra + 2)= 0b11000000;
         *(p_letra + 3)= 0b00011111;      
      break;
      case 'w':
         *(p_letra)=     0b11111111;
         *(p_letra + 1)= 0b01100000;
         *(p_letra + 2)= 0b01100000;
         *(p_letra + 3)= 0b11111111;      
      break;
      case 'x':
         *(p_letra)=     0b10000001;
         *(p_letra + 1)= 0b00111100;
         *(p_letra + 2)= 0b00111100;
         *(p_letra + 3)= 0b10000001;      
      break;
      case 'y':
         *(p_letra)=     0b00000011;
         *(p_letra + 1)= 0b11111000;
         *(p_letra + 2)= 0b11111000;
         *(p_letra + 3)= 0b00000011;      
      break;
      case 'z':
         *(p_letra)=     0b10010001;
         *(p_letra + 1)= 0b10110001;
         *(p_letra + 2)= 0b10011001;
         *(p_letra + 3)= 0b10010011;      
      break;
   }
}

