#include <Vector.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <EventJoystick.h>



RF24 radio(9, 10); //CE, CSN

const byte address[6] = "00001";

int vertical, neutral_vertical;
int horizontal, neutral_horizontal;
int count = 0;

EventJoystick ej1(A0, A1);


typedef Vector<String> Elements;
Elements colors;

const int motorPin = 2; 

char current_message[32] = "";
char previous_message[32] = "";

void onEj1Changed(EventJoystick& ej) {
  Serial.print("ej1 changed. X position: ");
  Serial.print(ej.x.position());
  Serial.print(", Y position: ");
  Serial.println(ej.y.position());
}

void convertAndSendRGB(char* rgbString) {
  // Parse RGB values from the string
  char* token = strtok(rgbString, ",");
  int red = atoi(token);

  token = strtok(NULL, ",");
  int green = atoi(token);

  token = strtok(NULL, ",");
  int blue = atoi(token);

  // Print the parsed RGB values for verification
  Serial.print("Red: ");
  Serial.println(red);
  Serial.print("Green: ");
  Serial.println(green);
  Serial.print("Blue: ");
  Serial.println(blue);

  // Now you can use the 'red', 'green', and 'blue' variables to control your LEDs
  // Replace the following lines with your actual LED control code
  analogWrite(9, red);    // Assuming you are using PWM pin 9 for the red LED
  analogWrite(10, green); // Assuming you are using PWM pin 10 for the green LED
  analogWrite(11, blue);  // Assuming you are using PWM pin 11 for the blue LED
}


void buzzMotor(unsigned int duration) {
  digitalWrite(motorPin, HIGH); // Turn on the motor
  delay(duration);               // Keep the motor on for the specified duration
  digitalWrite(motorPin, LOW);  // Turn off the motor
}

void setup()
{
pinMode(A2, INPUT);
pinMode(A3, INPUT);
pinMode(A4, INPUT);

pinMode(motorPin, INPUT);
//pinMode(3, INPUT);
//pinMode(4, INPUT);
pinMode(5, OUTPUT);
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);

pinMode(10, OUTPUT);

Serial.begin(9600);

/*should default as receiver and only act as transmitter WHEN button 2 is pressed */
radio.begin();
radio.openReadingPipe(0, address);
radio.setPALevel(RF24_PA_MIN);
radio.startListening();
Serial.begin(9600);

/* Setting the joystick event handler to the function onEj1Changed */
ej1.setChangedHandler(onEj1Changed);

/*Optional Number of increments*/
/*ej1.setNumIncrements(15);*/

neutral_horizontal = analogRead(A1);

}
void loop()
{
	ej1.update();

	if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    memcpy(previous_message, current_message, strlen(current_message) + 1);
    memcpy(current_message, text, strlen(text) + 1);
  	}

	if (strlen(current_message) != 0 && current_message != previous_message){
		buzzMotor(1000);
		delay(1000);
	}
	
	if(analogRead(A4) == HIGH){
		convertAndSendRGB(current_message);
	}
	if(analogRead(A2) == HIGH)
	{
    int i = 0;
		while(analogRead(A3) == LOW)
		{
    vertical = ej1.y.position();
		horizontal = ej1.x.position();
		if(vertical > neutral_vertical && horizontal > neutral_horizontal)   
		{
			if(i != 0 && colors.at(i-1) != "{255,255,255}")
			{
				colors.push_back("255,255,255");
			}
			if(i == 0)
			{
				colors.push_back("255, 255, 255");
			}
		}

    if(vertical < neutral_vertical && horizontal > neutral_horizontal)   
		{
			if(i != 0 && colors[i-1] != "237.15,237.15,0")
			{
				colors.push_back("237.15,237.15,0");
			}
			if(i == 0)
			{
				colors.push_back("237.15,237.15,0");
			}
		}
    if(vertical < neutral_vertical && horizontal < neutral_horizontal)   
		{
			if(i != 0 && colors[i-1] != "0,0,255")
			{
				colors.push_back("0,0,255");
			}
			if(i == 0)
			{
				colors.push_back("0, 0, 255");
			}
		}
    if(vertical > neutral_vertical && horizontal < neutral_horizontal)   
		{
			if(i != 0 && colors[i-1] != "0,255,0")
			{
				colors.push_back("0,255,0");
			}
			if(i == 0)
			{
				colors.push_back("0, 255, 0");
			}
		}
		
  		radio.openWritingPipe(address);
  		radio.setPALevel(RF24_PA_MIN);
  		radio.stopListening();
    String lastColor = colors[sizeof(colors) / sizeof(colors[0]) - 1];
		char message[32];
    strncpy(message, lastColor.c_str(), sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0'; // Ensure null-termination
 		radio.write(&message, sizeof(message));
  		delay(1000);
		
		radio.startListening();

		}
		
		//displaying colors
		
	}

}

