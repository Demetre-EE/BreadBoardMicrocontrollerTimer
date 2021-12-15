//button 1 switches the mode
//button 2 starts and stops the timer
#include <xc.h>

void timer();
void stopwatch();
void delay(int COUNTDOWN);
void setTimer();

//initialize button readers as 0
int new1 = 0;
int old1 = 0;
int old2 = 0;
int new2 = 0;
int mode = 1; //default mode is timer(mode 1)
int volatile counter = 0; //default counter is set to 0;
char volatile binary_pattern1[] = {0x3f, 0x06, 0x9B, 0x8F, 0xA6, 0xAD, 0xBD, 0x07, 0xBF, 0xA7}; //array 2 digit 7 segment display 0 to 9 pattern
int volatile a, b; //array indexes
//main

int main(void) {
    // set IO ports
    DDRB = 0b00110011; // Set Ports 1,2,5,6 as output and 2,3,6,7 as inputs
    DDRD = 0b10111111; // Set Port 6 as input and all others output

    while (1) { //beginning of main program
        old1 = new1;
        new1 = (PINB >> 3) & 1; //read button
        PORTD = 0;

        if (!old1 && new1) { //change mode if button is pressed
            mode++;
            delay(15);
            old1 = 0;
            new1 = 0;
        }
        //if the mode is 1
        if (mode == 1) {
            PORTB = 0b00010000; //7 segment display a 1
            timer(); //timer method where the timer feature takes place
        }            //if the mode is 1
        else if (mode == 2) {
            PORTB = 0b00110000; //7 segment display a 2
            stopwatch(); //jump to timer method where the timer feature takes place
        } else if (mode > 2) { //if mode is larger than 2
            mode = 1; //reset mode back to 1
        }

    }
}

void timer() { //begin timer method(feature))
    //reset button readers
    old2 = new2;
    new2 = (PIND >> 6) & 1;
    delay(20);
    old1 = 0;
    new1 = 0;

    while ((old2 == new2)&(old1 == new1)) { //do nothing until either button is pressed
        asm("NOP");
        //read both buttons
        old2 = new2;
        new2 = (PIND >> 6) & 1;
        old1 = new1;
        new1 = (PINB >> 3) & 1;
    }
    if (old1 == new1) { //execute only if button 1 is not pressed (when button 2 is pressed)
        //reset button readers
        delay(20); //buffer time
        old2 = 0;
        new2 = 0;
        old1 = 0;
        new1 = 0;

        setTimer(); //Go to setTimer method and set the starting timer value


        while ((!(new1 | old1))&(counter>0)) { //execute while button 1 is not pressed
            new2 = 0;
            old2 = 0;

            //set array indexes based on counter value
            a = (counter / 10) % 10;
            b = (counter % 10);

            //display number on the 2 digit 7 segment for roughly 1 second
            for (int i = 0; i < 31; i++) {
                PORTB = 0b00010010; //turn on first digit and off the second digit
                PORTD = binary_pattern1[b];
                delay(1); //small buffer
                PORTB = 0b00010001; //turn on second digit and off the first digit
                PORTD = binary_pattern1[a];
                delay(1); //small buffer
                old2 = new2;
                new2 = (PIND >> 6) & 1; //read button 2 input to see if user wants to stop the timer
            }
            //update counter
            asm("LDS    r20,(counter)");    //copy counter value to r20
            asm("DEC    r20");              //decrement r20 by 1
            asm("STS    (counter),r20");    //copy r20 value to counter



            //if button 2 is pressed
            if (new2 | old2) {
                //reset buttons
                delay(20);
                new2 = 0;
                old2 = 0;
                new1 = 0;
                old2 = 0;

                //pause at current 2 digit 7 segment display value until either button is pressed
                while (!(new2 | old2)&(!(new1 | old1))) {
                    //if button 2 is pressed resume timer
                    //if button 1 is pressed exit timer and change mode
                    PORTB = 0b00010010; //turn on first digit and off the second digit
                    PORTD = binary_pattern1[b];
                    delay(1); //small buffer
                    PORTB = 0b00010001; //turn on second digit and off the first digit
                    PORTD = binary_pattern1[a];
                    delay(1); //small buffer
                    old2 = new2;
                    new2 = (PIND >> 6) & 1; //read button 2 input to see if user wants to stop the timer
                    old1 = new1;
                    new1 = (PINB >> 3) & 1; //read button 1 input to see if user wants to stop the timer

                }
            }
        }
    }
    mode = 2; //change to stopwatch mode
}
//finish timer and switch

void stopwatch() {//begin timer method(feature))
    //reset button readers
    delay(20);
    old1 = 0;
    new1 = 0;
    old2 = 0;
    new2 = 0;

    while ((old2 == new2)&(old1 == new1)) { //do nothing until either button is pressed
        asm("NOP");
        //read both buttons
        old2 = new2;
        new2 = (PIND >> 6) & 1;
        old1 = new1;
        new1 = (PINB >> 3) & 1;
    }
    if (old1 == new1) { //execute only if button1 is not pressed (when button 2 is pressed)
        //reset buttons
        delay(20);
        old2 = 0;
        new2 = 0;
        old1 = 0;
        new1 = 0;

                            
                     
        counter = 0;

        while ((!(new1 | old1))&(counter<99)) { //execute while button 1 is not pressed
            new2 = 0;
            old2 = 0;

            //set array indexes based on counter value
            a = (counter / 10) % 10;
            b = (counter % 10);

            //display number on the 2 digit 7 segment for roughly 1 second
            for (int i = 0; i < 31; i++) {
                PORTB = 0b00110010; //turn on first digit and off the second digit
                PORTD = binary_pattern1[b];
                delay(1); //small buffer
                PORTB = 0b00110001; //turn on second digit and off the first digit
                PORTD = binary_pattern1[a];
                delay(1); //small buffer
                old2 = new2;
                new2 = (PIND >> 6) & 1;
            }

            //update counter
            asm("LDS    r20,(counter)");    //copy counter value to r20
            asm("INC    r20");              //increment r20 by 1
            asm("STS    (counter),r20");    //copy r20 value to counter

            //if button 2 is pressed
            if (new2 | old2) {
                //reset buttons
                delay(20);
                new2 = 0;
                old2 = 0;
                new1 = 0;
                old2 = 0;

                //pause at current 2 digit 7 segment display value until either button is pressed
                while (!(new2 | old2)&(!(new1 | old1))) {
                    //if button 2 is pressed resume timer
                    //if button 1 is pressed exit timer and change mode
                    PORTB = 0b00110010; //turn on first digit and off the second digit
                    PORTD = binary_pattern1[b];
                    delay(1); //small buffer
                    PORTB = 0b00110001; //turn on second digit and off the first digit
                    PORTD = binary_pattern1[a];
                    delay(1); //small buffer
                    old2 = new2;
                    new2 = (PIND >> 6) & 1; //read button 2 input to see if user wants to stop the timer
                    old1 = new1;
                    new1 = (PINB >> 3) & 1; //read button 1 input to see if user wants to stop the timer

                }
            }
        }
    }

    mode = 1; //change timer mode
}

//Set the timers starting value with the potentiometer
void setTimer() {
    //wait for button 2 to be pressed
    while (!(new2 | old2)) { 
        
        ADMUX |= (1 << REFS0); //select reference voltage
        
        //set prescaller to 128 and enable ADC 
        ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

        //select the ADC port on the microcontroller
        ADMUX = (ADMUX & 0xF0) | (0 & 0x0F);
        
        //Select ADC mode to single conversion
        ADCSRA |= (1 << ADSC);
        // wait until ADC conversion is complete
        while (ADCSRA & (1 << ADSC));
        uint16_t value = ADC;   //read analog value

        //convert the analog value (0 to 1023) to a counter value (0 to 99))
        counter = value / 10; 

        if (value > 999) {
            counter = 99;
        }
        //set array indexes
        a = (counter / 10) % 10;
        b = (counter % 10);

        //display number on the 2 digit 7 segment for roughly 1 second
        PORTB = 0b00010010; //turn on first digit and off the second digit
        PORTD = binary_pattern1[b];
        delay(1); //small buffer
        PORTB = 0b00010001; //turn on second digit and off the first digit
        PORTD = binary_pattern1[a];
        delay(1); //small buffer
        old2 = new2;
        new2 = (PIND >> 6) & 1; //read button 2
    }
}

//delay used for buffers
void delay(int COUNTDOWN) {

    long count = COUNTDOWN;
    //set Timer 0's timer mode to CTC
    TCCR0A |= (1 << WGM01);
    // set value to count up to
    OCR0A = 255;
    TCCR0B |= (1 << CS00) | (1 << CS02); //set prescale to 101 to value of prescale 1024
    while (count > 0) {
        while ((TIFR0 & (1 << TOV0)) == 0) { //check overflow until Timer 0 reaches 255
            asm("NOP");
        }
        TIFR0 |= (1 << TOV0);
        if (count > 0) {
            count -= 1;     
        }
    }
}
