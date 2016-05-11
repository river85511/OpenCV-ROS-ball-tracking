/*
 * rosserial Servo Control Example
 *
 * This sketch demonstrates the control of hobby R/C servos
 * using ROS and the arduiono
 * 
 * For the full tutorial write up, visit
 * www.ros.org/wiki/rosserial_arduino_demos
 *
 * For more information on the Arduino Servo Library
 * Checkout :
 * http://www.arduino.cc/en/Reference/Servo
 */

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <Servo.h> 
#include <ros.h>
#include <std_msgs/UInt16.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float32MultiArray.h"

ros::NodeHandle  nh;

Servo servo;  //UP DOWN
Servo servo1; //LEFT RIGHT

void servo_cb( const std_msgs::Float32MultiArray& cmd_msg){
  servo.write(cmd_msg.data[0]); //set servo angle, should be from 0-180  
  servo1.write(cmd_msg.data[1]);
  digitalWrite(13, HIGH-digitalRead(13));  //toggle led  
}


ros::Subscriber<std_msgs::Float32MultiArray> sub("servo", servo_cb);

void setup(){
  pinMode(13, OUTPUT);

  nh.initNode();
  nh.subscribe(sub);
  
  servo.attach(9); //attach it to pin 9//UP DOWN
  servo1.attach(8); ////LEFT RIGHT
}

void loop(){
  nh.spinOnce();
  delay(0.1);
}
