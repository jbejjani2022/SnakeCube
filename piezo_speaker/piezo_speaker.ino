/* Arduino tutorial - Buzzer / Piezo Speaker
   More info and circuit: http://www.ardumotive.com/how-to-use-a-buzzer-en.html
   Dev: Michalis Vasilakis // Date: 9/6/2015 // www.ardumotive.com */


const int buzzer = 6; //buzzer to arduino pin 6


void setup(){
 
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 6 as an output

}

void loop(){
 
  tone(buzzer, 791); // G5
        delay(500);
        tone(buzzer, 747); // F#5
        delay(600);
        tone(buzzer, 705); // F5
        delay(1000);
        noTone(buzzer);
    delay(1000);
}