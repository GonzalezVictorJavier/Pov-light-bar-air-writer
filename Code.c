/*
   ARREGLOS:
   -Le saque la immpresion de vuelta de la palabra
   -Se agrego el una inhibicion de la escritura de la eeprom
   -Tiene el oscilador seteado en 1Mhz
   -Tiene el espacio
   -No tiene PWM
   -Tiene un led trasero conectado a la pata RA5.3 que tiene que ir con una r independiente a GND en el lado contrario 
      a la barra de leds, sirve de referencia para la escritura, el led se enciende mientras se este imprimiendo una palabra,
      para que imprima correctamente se tiene que centrar la estela amarilla en el trayecto de la mano.
   -Tiene una nueva funcion que define el tiempo de la palabra, pareciera funcionar bien.
   
   PROBLEMAS:
   -Cuando se enciende en modo escritura y se genera una interrupcion por el sensor infrarojo, empieza a imprimir letras re loco,
      a pesar que la interrupcion esta inhibida hasta despues de definir el enter.
   -PAra que sea mas prolijo, podria definirse como corresponde el delay y usar delay_u en vez de delay_ms.
   -Se podria probar como funciona esta version con un osc de 32 khz.
   -Siendo muy ambicioso se podria programar unas funciones q midan el tiempo entre una y otra interrupcion, y haciendo alguna 
      prueba y error, por q no es preciso, se podria hacer q auto configure el tiempo de la palabra. El problema de esto es q lo
      q va a medri no es muy simetrico y las interrupciones de tiempo se deshabilitan en los delays con lo q estaria perdiendo 
      informacion. Una solucion es hacer mis propios delays, cosa q ya habia hecho, pero tuvieron algunos problemas.
*/
#include <16f819.h>                       
#fuses NOPROTECT
#fuses NOPUT
#fuses NOMCLR
#fuses INTRC_IO
//#fuses NOBROWNOUT
#use delay (clock=300000)                                                       //Está seteado a menos de 1Mhz porq necesito que  el delay sea mas preciso
#use fast_io(b) 
#define cant_letras 40                                                          //Hasta 39 letras
#define tiempo2 500  
#define tiempo3 100
#define antirebote_timer 100                                                    //VAL_TMR0 = 256 - (INT_TMR0 * FREC_OSC * 1/4 * 1/PRESCALER)
#bit leds0 = 0x05.1 
#bit leds1 = 0x05.0 
#bit leds2 = 0x05.7
#bit leds3 = 0x05.6
#bit leds4 = 0x06.7
#bit leds5 = 0x06.6
#bit leds6 = 0x06.5
#bit leds7 = 0x06.4
#bit in0 = 0x06.3 
#bit in1 = 0x06.6 
#bit in2 = 0x06.1
#bit in3 = 0x06.4
#bit in4 = 0x06.7
#bit in5 = 0x06.2
#bit in6 = 0x06.5 
#bit in7 = 0x05.6
#bit mode = 0x05.5                                                               //modo de impresion o toma de palabra 0 o 1
#bit eepromwrite = 0x18c.2                                                       //WREN: EEPROM Write Enable bit | 1 = Allows write cycles | 0 = Inhibits write to the EEPROM
void output_led(int16 );
int16 input_teclado(void );
int i = 0;
int a = 0;
char tecla2 = '\0';
char tecla = '\0';
char palabra[41] = {""};                                                         //antes era 20
int indice_pal;
int tiempo;
int antirebote(void);
char teclado(char);
void graba_palabra(char);
void imp_let(char ,int );
void busca_letra(char ,char *);
//----------------------------------------------------------------------------------------
#int_EXT
void EXT_isr(void)
{
   #ignore_warnings 216 
   enable_interrupts(INT_EXT);
   i = indice_pal;                                                                        
   delay_ms(250);                                                               //Hay que subir este delay porq le estoy pasando otro paramentro a la funcion delay_ms 
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
   setup_oscillator(OSC_1MHZ);                                                  //set INTRC to prescalef  1MHz 
   eepromwrite = 0;                                                             //Inhibe la escritura de la eeprom
   palabra[0] = '\0';                                                           //Sin esto, arrancando en modo escritura se volvia loco 
   do
   {
      if(mode == 0)
      {
         set_tris_b(0b11111111);
         set_tris_a(0b11111111);
         port_b_pullups(TRUE); 
         setup_timer_1 ( T1_INTERNAL|T1_DIV_BY_2 ); 
         enable_interrupts(GLOBAL); 
         disable_interrupts(INT_TIMER1);
         disable_interrupts(INT_EXT);
         set_timer1(0x00);
         palabra[0] = '\0';
         tecla = '\0';
         tecla2 = '\0';
         indice_pal = 0;
         delay_ms(300);
         while(mode == 0)
         {  
            if((tecla = teclado(tecla2)) != 0x0000)
               tecla2 = tecla;
         }      
      }
      else
      {
         i = 0;
         set_tris_b(0b00000001);
         set_tris_a(0b00100000);
         port_b_pullups(FALSE); 
         enable_interrupts(GLOBAL);
         disable_interrupts(INT_EXT);                                            //Deshabilito la interrupcion y la habilito mas tarde para q no me afecte  una interrupcion temprana y se vea todo lento         
         disable_interrupts(INT_TIMER1);
         ext_int_edge(H_TO_L);
         output_led(0b11111110);
         delay_ms(tiempo2);
         output_led(0b11111101);
         delay_ms(tiempo2);
         output_led(0b11111011);
         delay_ms(tiempo2);
         output_led(0b11110111);
         delay_ms(tiempo2);
         output_led(0b11101111);
         delay_ms(tiempo2);
         output_led(0b11011111);
         delay_ms(tiempo2);
         output_led(0b10111111);
         delay_ms(tiempo2);
         output_led(0b01111111);
         delay_ms(tiempo2);
         while(palabra[i] != '\0')
         {
            i++;
         }
         tiempo = (int)((int) 23 - (int)(i * 2));
//         tiempo = (int) (35/((i)));
         i = 0;
         enable_interrupts(INT_EXT);
         while(mode == 1)
         {            
            while(i > 0)
            {
               i--;
               imp_let(palabra[i], 0);
               output_led(0b00000000);
               output_led(0b00000000);
            }   
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
            output_led(letra_bin[0]);
            delay_ms(tiempo);
            output_led(letra_bin[1]);
            delay_ms(tiempo);
            output_led(letra_bin[2]);
            delay_ms(tiempo);
            output_led(letra_bin[3]);
            delay_ms(tiempo);
            output_led(letra_bin[4]);
            delay_ms(tiempo);
            output_led(0b00000000);
            delay_ms(tiempo);
         break;
         case 0:
            output_led(letra_bin[4]);
            delay_ms(tiempo);
            output_led(letra_bin[3]);
            delay_ms(tiempo);
            output_led(letra_bin[2]);
            delay_ms(tiempo);
            output_led(letra_bin[1]);
            delay_ms(tiempo);
            output_led(letra_bin[0]);
            delay_ms(tiempo);
            output_led(0b00000000);
            delay_ms(tiempo);
         break;
      }  
   }
   else
      if(letra == ' ')
      {
        switch(mod)
         {  
            case 1:
               output_led(0);
               delay_ms(tiempo);
               output_led(0);
               delay_ms(tiempo);
               output_led(0b00000000);
               delay_ms(tiempo);
               output_led(0b00000000);
               delay_ms(tiempo);
            break;
            case 0:
               output_led(0);
               delay_ms(tiempo);
               output_led(0);
               delay_ms(tiempo);
               output_led(0b00000000);
               delay_ms(tiempo);
               output_led(0b00000000);
               delay_ms(tiempo);
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
      case 0b11111110:                                                               //A,B,C,espacio
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






