//--------------------------------------------
// Example for a rotary encoder
// Bart Venneker 2015
//--------------------------------------------
// This example uses a rotary encoder to set the 
// value of a variable (rotaryCount)
//
// See http://youtu.be/KzT3aUE1-0Q for more info (in DUTCH!!)

#include <Arduino.h>

/*     Arduino Rotary Encoder Tutorial
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
 
 #define outputA 25
 #define outputB 26

 int counter = 0; 
 int aState;
 int aLastState;  

 void setup() { 
   pinMode (25,INPUT);
   pinMode (26,INPUT);
   
   Serial.begin (9600);
   // Reads the initial state of the outputA
   aLastState = digitalRead(outputA);   
 } 

 void loop() { 
   aState = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aState) { 
       counter ++;
     } else {
       counter --;
     }
     Serial.print("Position: ");
     Serial.println(counter);
   } 
   aLastState = aState; // Updates the previous state of the outputA with the current state
 }

