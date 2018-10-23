#include <Stepper.h>

int in1Pin = 2;
int in2Pin = 3;
int in3Pin = 4;
int in4Pin = 5;

Stepper motor(512, in1Pin, in3Pin, in2Pin, in4Pin);

void setup()
{
    motor.setSpeed(25);
}

void loop()
{
    int steps = 360;
    motor.step(steps);
    delay(100);

    steps = -360;
    motor.step(steps);
    delay(500); //Semicolon added
}
