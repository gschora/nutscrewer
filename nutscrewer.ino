#include <AccelStepper.h>
#include <SerialCommand.h>
SerialCommand SCmd;


int dirPin = 6;
int stepperPin = 5;
int enablePin = 4;
float maxSpeed = 600.0;
float speed = 600.0;
float accel = 600.0;

int stepsPerRotation = 200;
int rotations = 1;

int stepperControl = 0;

AccelStepper stepper(1,stepperPin,dirPin);

void setup(){
	Serial.begin(115200);

	// commands
	SCmd.addDefaultHandler(unrecognized);
	SCmd.addCommand("setmaxspeed",setMaxSpeed);
	SCmd.addCommand("setspeed",setRotationSpeed);
	SCmd.addCommand("setaccel",setAccel);
	// SCmd.addCommand("moveto",moveTo);
	// SCmd.addCommand("move",move);
	// SCmd.addCommand("runtoposition",runToPosition);
	// SCmd.addCommand("run",run);
	SCmd.addCommand("runspeed",runSpeed);
	SCmd.addCommand("stop",stop);
	SCmd.addCommand("getspeed",printSpeedMax);
	SCmd.addCommand("r",rotate);

	stepper.setEnablePin(enablePin);
	stepper.setPinsInverted(false,false,true); //enable pin needs to be inverted
	stepper.setMaxSpeed(maxSpeed);
	stepper.setAcceleration(accel);
	stepper.setSpeed(speed);

	printSpeedMax();
}

void loop(){
	SCmd.readSerial();

	if (stepper.distanceToGo() == 0) {
		stepperControl = 0;
	}


	switch (stepperControl) {
	case 0:
		stepper.disableOutputs();
		break;
	case 1:
		stepper.enableOutputs();
		stepper.run();
		break;
	case 2:
		stepper.enableOutputs();
		stepper.runSpeed();
		break;
	}


}
void setMaxSpeed(){ //float speed
	char *arg;
	arg = SCmd.next();
	if (arg != NULL) {
		maxSpeed=(float)atoi(arg); // Converts a char string to an integer
		stepper.setMaxSpeed(maxSpeed);
		Serial.println("setmaxspeed ok");
		printSpeedMax();
	}
}
void setRotationSpeed(){ //float speed
	char *arg;
	arg = SCmd.next();
	if (arg != NULL) {
		speed=(float)atoi(arg); // Converts a char string to an integer
		stepper.setSpeed(speed);
		Serial.println("setspeed ok");
		printSpeedMax();
	}
}
void setAccel(){ //float acceleration desired acceleration in steps per second
	char *arg;
	float accel;
	arg = SCmd.next();
	if (arg != NULL) {
		accel=(float)atoi(arg); // Converts a char string to an integer
		stepper.setAcceleration(accel);
		Serial.println("setaccel ok");
	}
}

void runSpeed(){ // implementing a constant speed as set by the most recent call to setSpeed()
	stepperControl = 2;
	Serial.println("runspeed ok");
}


void rotate(){ //long absolute
	char *arg;
	arg = SCmd.next();
	if (arg != NULL) {
		rotations=atoi(arg); // Converts a char string to an integer
		stepper.move(calcDistance());
		stepperControl = 1;
		Serial.println("moveto ok");
	} else {
		stepper.move(calcDistance());
		stepperControl = 1;
		Serial.println("moveto ok");
	}
}

void stop(){
	stepper.stop();
	stepperControl = 0;
	Serial.println("stop ok");
}

void printSpeedMax(){
	Serial.print("MaxSpeed: ");
	Serial.println(stepper.maxSpeed());
	Serial.print("speed: ");
	Serial.println(stepper.speed());
}

long calcDistance(){
	return stepsPerRotation * rotations;
}

void unrecognized()
{
	Serial.println("command not understood! try something else like");
	Serial.println("setmaxspeed");
	Serial.println("setspeed");
	Serial.println("setaccel");
	// Serial.println("moveto");
	// Serial.println("move");
	// Serial.println("runtoposition");
	// Serial.println("run");
	Serial.println("runspeed");
	Serial.println("r");
}
