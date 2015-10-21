#include <AccelStepper.h>
#include <SerialCommand.h>

SerialCommand SCmd;

int dirPin = 6;
int stepperPin = 5;
int enablePin = 4;
int switchPin = 2;

float maxSpeed = 500.0;
float rotSpeed = 500.0;
float accel = 500.0;

int stepsPerRotation = 200;
int rotations = 1;
bool clockwise = false;

int stepperControl = 0;

int counter = 0;

AccelStepper stepper(1,stepperPin,dirPin);

void setup(){
	Serial.begin(115200);

	pinMode(switchPin, INPUT_PULLUP);

	// commands
	SCmd.addDefaultHandler(unrecognized);
	SCmd.addCommand("sms",setMaxSpeed);
	SCmd.addCommand("ss",setRotationSpeed);
	SCmd.addCommand("sa",setAccel);
	// SCmd.addCommand("moveto",moveTo);
	// SCmd.addCommand("move",move);
	// SCmd.addCommand("runtoposition",runToPosition);
	// SCmd.addCommand("run",run);
	SCmd.addCommand("rs",runSpeed);
	SCmd.addCommand("st",stop);
	SCmd.addCommand("gs",printSpeedMax);
	SCmd.addCommand("r",rotate);
	SCmd.addCommand("d",setDirection); // 0 = counterclockwise 1=clockwise
	SCmd.addCommand("gc",getCounter);
	SCmd.addCommand("sc",setCounter);

	stepper.setEnablePin(enablePin);
	stepper.setPinsInverted(false,false,true); //enable pin needs to be inverted
	stepper.setMaxSpeed(maxSpeed);
	stepper.setAcceleration(accel);
	stepper.setSpeed(rotSpeed);

	printSpeedMax();
}
int i = 0;
void loop(){
	SCmd.readSerial();

	if (stepperControl != 2 && stepper.distanceToGo() == 0) {
		stepperControl = 0;
	}

	switchRotate();

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
		rotSpeed=(float)atoi(arg); // Converts a char string to an integer
		stepper.setSpeed(rotSpeed);
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
		counter++;
		getCounter();
		// Serial.println("rotate ok");
	} else {
		stepper.move(calcDistance());
		stepperControl = 1;
		counter++;
		getCounter();
		// Serial.println("rotate ok");
	}
}

void stop(){
	stepper.stop();
	stepperControl = 0;
	Serial.println("stop ok");
}

void printSpeedMax(){
	Serial.print("MaxSpeed: ");
	Serial.println((stepper).maxSpeed());
	Serial.print("speed: ");
	Serial.println((int)stepper.speed());
}

void switchRotate(){
	// Serial.println(digitalRead(switchPin));
	if (!digitalRead(switchPin)){
		if (stepper.distanceToGo() == 0) {
			rotate();
		}
	}
}

void setDirection(){
	char *arg;
	arg = SCmd.next();
	if (arg != NULL) {
		clockwise=atoi(arg); // Converts a char string to an integer

	if (clockwise){
		Serial.println("rotation clockwise");
	} else {
		Serial.println("rotation counterclockwise");
	}

	}
}

void getCounter(){
	Serial.print("counter: ");
	Serial.println(counter);
}
void setCounter(){
	char *arg;
	arg = SCmd.next();
	if (arg != NULL) {
		counter=atoi(arg);
		getCounter();
	}
}

long calcDistance(){
	if(clockwise){
		return stepsPerRotation * rotations * (-1);
	}else {
		return stepsPerRotation * rotations;
	}

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
