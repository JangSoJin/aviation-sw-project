#include <ros.h>

#include <PID_v1.h>

#include <SharpIR.h>

#include <std_msgs/String.h>

ros::NodeHandle  nh;

std_msgs::String str_msg;

ros::Publisher chatter("chatter", &str_msg);

char hello[10];

char hello2[10];

char hello4[2] = ",";

char hello3[30];

//IR sesonr connection diagram

#define frontIr A0 //RX from front IR sensor 

#define rightIr A1 //RX from right IR sensor

#define backIr A2 //RX from back IR sensor

#define leftIr A3 //RX from left IR sensor

#define model 20150 //IR sensor model number

//IR sensor and Maxsonar sensor variable declaration

double Setpoint, altSetpoint;

double frontIrInput, frontIrOutput;

double rightIrInput, rightIrOutput;

double backIrInput, backIrOutput;

double leftIrInput, leftIrOutput;

double altSonarInput, altSonarOutput,lastSonarInput;

double Kp = 2.9; 

//Kp variable must declare before IR sensor object declaration

double Ki = 0; 

//Ki variable must declare before IR sensor object declaration

double Kd = 0; 

//Kd variable must declare before IR sensor object declaration



//IR sensor and MaxSonar object declaration
SharpIR fIr(frontIr, 25, 70, model); //acquire 25 samples only 70% corretiveness
SharpIR rIr(rightIr, 25, 70, model); //acquire 25 samples only 70% corretiveness
SharpIR bIr(backIr, 25, 70, model); //acquire 25 samples only 70% corretiveness
SharpIR lIr(leftIr, 25, 70, model); //acquire 25 samples only 70% corretiveness



//IR sensor and MaxSonar PID object declaration

PID frontPID(&frontIrInput, &frontIrOutput, &Setpoint, Kp, Ki, Kd, DIRECT);

PID rightPID(&rightIrInput, &rightIrOutput, &Setpoint, Kp, Ki, Kd, DIRECT);

PID leftPID(&leftIrInput, &leftIrOutput, &Setpoint, Kp, Ki, Kd, DIRECT);

PID backPID(&backIrInput, &backIrOutput, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup(){

  Serial.begin(57600);  //Setpoint setting

  Setpoint = 50;

  //SetOutputLimits setting

  frontPID.SetOutputLimits(30,100);

  rightPID.SetOutputLimits(30,100);

  backPID.SetOutputLimits(30,100);

  leftPID.SetOutputLimits(30,100);

  //SetMode setting

  frontPID.SetMode(AUTOMATIC);

  rightPID.SetMode(AUTOMATIC);

  backPID.SetMode(AUTOMATIC);

  leftPID.SetMode(AUTOMATIC);

  nh.initNode();

  nh.advertise(chatter);

}

void loop(){

  static uint16_t unAleoIn;

  static uint16_t unEleoIn;

      Setpoint = 30;

      frontIrInput = fIr.cm();

      backIrInput = bIr.cm(); 

      rightIrInput = rIr.cm();

      leftIrInput = lIr.cm();

      frontPID.Compute();

      backPID.Compute();

      leftPID.Compute();

      rightPID.Compute();

      unAleoIn = 1500-leftIrOutput+rightIrOutput;
      unEleoIn = 1500-frontIrInput+backIrInput;

  String a = String(unAleoIn, DEC);

  String b = String(unEleoIn, DEC);

  String c = a+","+b;

  

  c.toCharArray(hello3, 30);

  

  str_msg.data = hello3;

  chatter.publish( &str_msg );

  nh.spinOnce();

  delay(1000);

}
