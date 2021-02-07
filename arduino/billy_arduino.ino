
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int64.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle  nh;

const int motorAE = 11;
const int motorA1 = 12;
const int motorA2 = 13;
const int motorBE = 6;
const int motorB1 = 8;
const int motorB2 = 9;
const int encoderA1 = 3;
const int encoderA2 = 5;
const int encoderB1 = 2;
const int encoderB2 = 4;

volatile int count_encoderA = 0;
volatile int count_encoderB = 0;
bool correction_encoderB;

bool motorA_direction = true;
bool motorB_direction = true;

char hello[13] = "hello world!";

void callback_message_cmd_vel_direction( const std_msgs::Int16& toggle_msg){

  switch(toggle_msg.data) {
    case 0:
      digitalWrite(motorA2, LOW);
      digitalWrite(motorA1, LOW);
      digitalWrite(motorB2, LOW);
      digitalWrite(motorB1, LOW);
      break;
    case 123:
      digitalWrite(motorA2, HIGH);
      digitalWrite(motorA1, LOW);
      digitalWrite(motorB2, HIGH);
      digitalWrite(motorB1, LOW);
      correction_encoderB = false;
      break;
    case 321:
      digitalWrite(motorA2, LOW);
      digitalWrite(motorA1, HIGH);
      digitalWrite(motorB2, LOW);
      digitalWrite(motorB1, HIGH);
      correction_encoderB = true;
      break;
  }
  
}

void back_forw_motor(){
  analogWrite(motorAE, 60);
  analogWrite(motorBE, 120);
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
  delay(1000);

  analogWrite(motorAE, 120);
  analogWrite(motorBE, 60);
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
  delay(1000);
}

void callback_v_r(const std_msgs::Int16& msg) {
  byte temp = msg.data;
  
  if(msg.data >= 1000){
    //Backward
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    correction_encoderB = true;
  }
  else {
    //Forward
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
  }
  
  temp = msg.data % 1000;
  analogWrite(motorAE, temp);
}

void callback_v_l(const std_msgs::Int16& msg) {
  byte temp = msg.data;
  
  if(msg.data >= 1000){
    //Backward
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, HIGH);
    correction_encoderB = true;
  }
  else {
    //Forward
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
  }
  
  temp = msg.data % 1000;
  analogWrite(motorBE, temp);
}

void record_encoderA(){
  
  if(digitalRead(encoderA2) == LOW) {
    count_encoderA ++;
  } else {
    count_encoderA --;
  }
  
}

void record_encoderB(){
  
  //if(digitalRead(encoderB2) == HIGH) {
  if(correction_encoderB == true) {
    count_encoderB ++;
  } else {
    count_encoderB --;
  }
}

//ros::Subscriber<geometry_msgs::Twist> sub_cmd_vel_twist("cmd_vel", callback_message_cmd_vel_twist );
//ros::Subscriber<std_msgs::Int16> sub_cmd_vel_int16("cmd_vel_direction", callback_message_cmd_vel_direction );
ros::Subscriber<std_msgs::Int16> sub_v_r("v_r", callback_v_r );
ros::Subscriber<std_msgs::Int16> sub_v_l("v_l", callback_v_l );

//Publishers ------------------
std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);
std_msgs::Int64 encoderA;
ros::Publisher odomA("odomA", &encoderA);
std_msgs::Int64 encoderB;
ros::Publisher odomB("odomB", &encoderB);

void setup()
{
Serial.begin(57600);
  
  // Init pins
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorAE, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(motorBE, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(encoderA1), record_encoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderB1), record_encoderB, RISING);
  pinMode(encoderA2, INPUT);
  pinMode(encoderB2, INPUT);
  
  nh.initNode();
  nh.advertise(chatter);
  nh.advertise(odomA);
  nh.advertise(odomB);
  //nh.subscribe(sub_cmd_vel_direction);
  nh.subscribe(sub_v_l);
  nh.subscribe(sub_v_r);
}

void loop()
{
  
  str_msg.data = hello;
  encoderA.data = count_encoderA;
  encoderB.data = count_encoderB;
  chatter.publish( &str_msg );
  odomA.publish( &encoderA );
  odomB.publish( &encoderB );
  
  //back_forw_motor();
  
  nh.spinOnce();
  delay(500);
}
