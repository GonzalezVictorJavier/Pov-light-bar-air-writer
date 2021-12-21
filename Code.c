/*
   -Le saque la immpresion de vuelta de la palabra
   -Se agrego el una inhibicion de la escritura de la eeprom
*/
#include <16f819.h>                       //pic a utilizar
#fuses NOPROTECT
#fuses NOPUT
#fuses NOMCLR
#fuses INTRC_IO
#use delay (clock=1000000)            //Fosc=1Mhz
#use fast_io(b) 
#define cant_letras 40                                                           //antes era 10
#define tiempo2 3000  
#define tiempo3 100
#bit leds0 = 0x05.0 
#bit leds1 = 0x06.1 
#bit leds2 = 0x06.2
#bit leds3 = 0x06.3
#bit leds4 = 0x06.4
#bit leds5 = 0x06.5
#bit leds6 = 0x06.6
#bit leds7 = 0x06.7
#bit in0 = 0x05.1 
#bit in1 = 0x06.1 
#bit in2 = 0x06.2
#bit in3 = 0x06.3
#bit in4 = 0x06.4
#bit in5 = 0x06.5
#bit in6 = 0x06.6
#bit in7 = 0x06.7
#bit mode = 0x05.2                                                               //modo de impresion o toma de palabra 1 o 0
#bit sensor = 0x05.3
#bit eepromwrite = 0x18c.2                                                       //WREN: EEPROM Write Enable bit | 1 = Allows write cycles | 0 = Inhibits write to the EEPROM
void output_led(int16 );
int16 input_teclado(void );
int i = 0;
int a = 0;
char tecla2 = '\0';
char tecla = '\0';
char palabra[41] = {""};                                                         //antes era 20
int indice_pal;                                                                  //Indice de final de palabra
int tiempo;                                                                      //Tiempo entre columnas que componen las letras, en milisegundos   
int antirebote(void);
char teclado(char);
#inline void graba_palabra(char);                                                //graba la palabra escrita, es inline para evitar que moleste la warning216
void imp_let(char ,int );
void busca_letra(char ,char *);
void output(int16 ,int ,int ,int );                                              //Imprime una letra aplicando un pwm,PARAMETROS:letra,tiempo_total,tiempo1,tiempo0
#define time1 1                                                                  //Tiempo que dura un "1" del pwm, en milisegundos
#define time0 5                                                                  //Tiempo que dura un "0" del pwm, en milisegundos
#define borde_izquierdo 400
//----------------------------------------------------------------------------------------
#int_EXT
void EXT_isr(void)
{
   #ignore_warnings 216 
   enable_interrupts(INT_EXT);
   i = indice_pal;                                                                        
   delay_ms(borde_izquierdo);
   #ignore_warnings NONE
}
//-----------------------------------------------------------------------------------------
#int_timer1
void TIMER1_isr( )
{
   a++;
   if(a == 5)
   {
      graba_palabra(tecla2);
      disable_interrupts(INT_TIMER1);
      a = 0;
   }   
   set_timer1(0x00);
}
//----------------------------------------------------------------------------------------
void main(void)
{
   setup_oscillator(OSC_4MHZ); //set INTRC to prescalef  1MHz 
   eepromwrite = 0;                                                              //Inhibe la escritura de la eeprom
   do
   {
      if(mode == 0)
      {
         i = 0;
         set_tris_b(0b00000001);
         set_tris_a(0b00001100);
         port_b_pullups(FALSE); 
         enable_interrupts(GLOBAL);
         enable_interrupts(INT_EXT);
         disable_interrupts(INT_TIMER1);
         ext_int_edge(L_TO_H);
         output(0b11111110,tiempo2,time1,time0);
         output(0b11111101,tiempo2,time1,time0);
         output(0b11111011,tiempo2,time1,time0);
         output(0b11110111,tiempo2,time1,time0);
         output(0b11101111,tiempo2,time1,time0);
         output(0b11011111,tiempo2,time1,time0);
         output(0b10111111,tiempo2,time1,time0);
         output(0b01111111,tiempo2,time1,time0);
         while(palabra[i] != '\0')
         {
            i++;
         }
         tiempo = (int) (150/((int)(i * 3)));
//         tiempo = 10;
         i = 0;
         while(mode == 1)
         {            
            while(i > 0)
            {
               i--;
               imp_let(palabra[i], 0);
            }   
         }
      }
      else
      {
         set_tris_b(0b11111111);
         set_tris_a(0b00001110);
         port_b_pullups(TRUE); 
         setup_timer_1 ( T1_INTERNAL|T1_DIV_BY_8 ); 
         enable_interrupts(GLOBAL); 
         disable_interrupts(INT_TIMER1);
         disable_interrupts(INT_EXT);
         set_timer1(0x00);
         palabra[0] = '\0';
         tecla = '\0';
         tecla2 = '\0';
         indice_pal = 0;
         delay_ms(300);
         while(mode == 1)
         {  
            if((tecla = teclado(tecla2)) != 0x0000)
               tecla2 = tecla;
         }      
      }
   }
   while(TRUE);
}
//----------------------------------------------------------------------------------------
void imp_let(char letra ,int mod)
{
   int letra_bin[5];
   if(letra != '\0' && letra != ' ')
   {
      busca_letra(letra ,letra_bin);
      switch(mod)
      {  
         case 1:
            output(letra_bin[0],tiempo,time1,time0);
            output(letra_bin[1],tiempo,time1,time0);
            output(letra_bin[2],tiempo,time1,time0);
            output(letra_bin[3],tiempo,time1,time0);
            output(letra_bin[4],tiempo,time1,time0);
            output(0b00000000,tiempo,time1,time0);
         break;
         case 0:
            output(letra_bin[4],tiempo,time1,time0);
            output(letra_bin[3],tiempo,time1,time0);
            output(letra_bin[2],tiempo,time1,time0);
            output(letra_bin[1],tiempo,time1,time0);
            output(letra_bin[0],tiempo,time1,time0);
            output(0b00000000,tiempo,time1,time0);
         break;
      }  
   }
   else
      if(letra == ' ')
      {
        switch(mod)
         {  
            case 1:
               output(0,tiempo,time1,time0);
               output(0b00000000,tiempo,time1,time0);
            break;
            case 0:
               output(0,tiempo,time1,time0);
               output(0b00000000,tiempo,time1,time0);
            break;
         }     
      }
}
//----------------------------------------------------------------------------------------
int16 input_teclado(void )
{
   int16 teclado = 0;
   if(in7 == 1)
      {
         teclado = teclado + 128;
      }
   if(in6 == 1)
      {
         teclado = teclado + 64;
      }
   if(in5 == 1)
      {
         teclado = teclado + 32;
      }
   if(in4 == 1)
      {
         teclado = teclado + 16;
      }
   if(in3 == 1)
      {
         teclado = teclado + 8;
      }
   if(in2 == 1)
      {
         teclado = teclado + 4;
      }
   if(in1 == 1)
      {
         teclado = teclado + 2;
      }
   if(in0 == 1)
      {
         teclado = teclado + 1;
      }  
   return teclado;
}
//----------------------------------------------------------------------------------------
void output_led(int16 led_dec)
{
   if(led_dec >= 128)
      {
         leds7 = 1;
         led_dec = led_dec - 128;
      }
   else
      {
         leds7 = 0;
      }
   if(led_dec >= 64)
      {
         leds6 = 1;
         led_dec = led_dec - 64;
      }
   else
      {
         leds6 = 0;
      }
   if(led_dec >= 32)
      {
         leds5 = 1;
         led_dec = led_dec - 32;
      }
   else
      {
         leds5 = 0;
      }
   if(led_dec >= 16)
      {
         leds4 = 1;
         led_dec = led_dec - 16;
      }
   else
      {
         leds4 = 0;
      }
   if(led_dec >= 8)
      {
         leds3 = 1;
         led_dec = led_dec - 8;
      }
   else
      {
         leds3 = 0;
      }
   if(led_dec >= 4)
      {
         leds2 = 1;
         led_dec = led_dec - 4;
      }
   else
      {                 
         leds2 = 0;
      }
   if(led_dec >= 2)
      {
         leds1 = 1;
         led_dec = led_dec - 2;
      }
   else
      {
         leds1 = 0;
      }
   if(led_dec >= 1)
      {
         leds0 = 1;
         led_dec = led_dec - 1;
      }
   else
      {
         leds0 = 0;
      }      
}
//------------------------------------------------------------------------------
void busca_letra(char letra ,char *letra_vec)
{
   *(letra_vec) =     read_eeprom(((letra - 97) * 5));
   *(letra_vec + 1) = read_eeprom(((letra - 97) * 5) + 1);
   *(letra_vec + 2) = read_eeprom(((letra - 97) * 5) + 2);
   *(letra_vec + 3) = read_eeprom(((letra - 97) * 5) + 3);
   *(letra_vec + 4) = read_eeprom(((letra - 97) * 5) + 4);
}
//-----------------------------------------------------------------------------
void graba_palabra(char letra)
{
   palabra[indice_pal] = letra;
   palabra[indice_pal + 1] = '\0';
   indice_pal ++;
   tecla = tecla2 = '\0';
}
//-----------------------------------------------------------------------------
char teclado(char letra)
{
   switch(antirebote())
   {
      case 0b11111110:                                                               //A,B,C
         switch(letra)
         {
            case  'a':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'b';
            break;
            case 'b':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'c';
            break;
            case 'c':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return ' ';
            break;
            default :
               if(letra != '\0')
                  graba_palabra(letra);
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'a';
            break;
         }
      break;
      case 0b11111101:                                                               //D,E,F
         switch(letra)
         {
            case  'd':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'e';
            break;
            case 'e':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'f';
            break;
            default :
               if(letra != '\0')
                  graba_palabra(letra);
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'd';
            break;
         }
      break;
      case 0b11111011:                                                               //G,H,I
         switch(letra)
         {
            case  'g':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'h';
            break;
            case 'h':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'i';
            break;
            default :
               if(letra != '\0')
                  graba_palabra(letra);
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'g';
            break;
         }
      break;
      case 0b11110111:                                                               //J,K,L
         switch(letra)
         {
            case  'j':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'k';
            break;
            case 'k':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'l';
            break;
            default :
               if(letra != '\0')
                  graba_palabra(letra);
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'j';
            break;
         }
      break;
      case 0b11101111:                                                               //M,N,O
         switch(letra)
         {
            case  'm':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'n';
            break;
            case 'n':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'o';
            break;
            default :
               if(letra != '\0')
                 graba_palabra(letra);
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'm';
            break;
         }
      break;
      case 0b11011111:                                                               //P,Q,R,S
         switch(letra)
         {
            case  'p':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'q';
            break;
            case 'q':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'r';
            break;
            case 'r':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 's';
            break;
            default :
               if(letra != '\0')
                  graba_palabra(letra);
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'p';
            break;
         }
      break;
      case 0b10111111:                                                               //T,U,V
         switch(letra)
         {
            case  't':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'u';
            break;
            case 'u':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'v';
            break;
            default :
               if(letra != '\0')
                  graba_palabra(letra);
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 't';
            break;
         }
      break;
      case 0b01111111:                                                               //W,X,Y,Z
         switch(letra)
         {
            case 'w':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'x';
            break;
            case  'x':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'y';
            break;
            case 'y':
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'z';
            break;
            default :
               if(letra != '\0')
                 graba_palabra(letra);
               enable_interrupts(INT_TIMER1);
               set_timer1(0x00);
               return 'w';
            break;
         }
      break;
      default:
         return 0x0000;
      break;
   }
}
//------------------------------------------------------------------------------
int antirebote(void)
{
   int aux;
   int16 cont;
   aux = input_teclado();
   cont = 0;
   while(cont < 20 && aux != 0b11111111)
   {
      if(input_teclado() == 0b11111111)
      {
         cont ++;
      }
   }
   return aux;
}
//---------------------------------------------------------------------------------------
void output(int16 letra ,int tiempo_total ,int tiempo1 ,int tiempo0 )
{
   int aux = 0;
   while(aux <= tiempo_total)
   {
      output_led(letra);
      delay_ms(tiempo1);
      output_led(0b00000000);
      delay_ms(tiempo0);
      aux = aux + tiempo1 + tiempo0;
   }
}






