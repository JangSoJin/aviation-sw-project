#include <std_msgs/String.h>

#include <stdio.h>

#include <iostream>

#include <cstdlib>

#include <ros/ros.h>

#include <geometry_msgs/PoseStamped.h>

#include <mavros_msgs/CommandBool.h>

#include <mavros_msgs/SetMode.h>

#include <mavros_msgs/State.h>



double roll=0;

double pitch=0;

double y=0;

double w=0;

int count =0;



mavros_msgs::State current_state;

void state_cb(const mavros_msgs::State::ConstPtr& msg){

current_state = *msg;

}



void chatterCallback(const std_msgs::String::ConstPtr& msg){



const char * c = msg->data.c_str();

char t[30];

strncpy(t,c,30);



char * temp =strtok(t,",");

while(temp != NULL)

{

if(count == 0)

{

y=atoi(temp);

count++;

}

else if(count ==1)

{

w=atoi(temp);

count++;

if( count ==2)

count =0;

}

temp= strtok(NULL , " ");

}



roll=roll+(y-1500)/25;

pitch=pitch+(w-1500)/25;



printf("%f, %f \n" , roll,pitch);

}



int main(int argc, char **argv)

{

ros::init(argc, argv, "beginner_tutorials_node");

ros::NodeHandle nh;

ros::Subscriber sub = nh.subscribe("chatter", 1000, chatterCallback);

ros::Subscriber state_sub =

nh.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);

ros::Publisher local_pos_pub =

nh.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 10);

ros::ServiceClient arming_client =

nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");

ros::ServiceClient set_mode_client =

nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");



//the setpoint publishing rate MUST be faster than 2Hz

ros::Rate rate(20.0);



// wait for FCU connection

while(ros::ok() && current_state.connected){

ros::spinOnce();

rate.sleep();

}



geometry_msgs::PoseStamped pose;

pose.pose.position.x = 0;

pose.pose.position.y = 0;

pose.pose.position.z = 3;



//send a few setpoints before starting

for(int i = 100; ros::ok() && i > 0; --i){

local_pos_pub.publish(pose);

ros::spinOnce();

rate.sleep();

}

mavros_msgs::SetMode offb_set_mode;

offb_set_mode.request.custom_mode = "OFFBOARD";



mavros_msgs::CommandBool arm_cmd;

arm_cmd.request.value = true;


ros::Time last_request = ros::Time::now();



while(ros::ok()){
        /*
        if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.success){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        } else {
            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
            }
        }*/
pose.pose.position.x = pitch;

pose.pose.position.y = roll;
pose.pose.position.z = 3;

//기본 z값으로 오프모드시 3미터 위에 떠있도록함

local_pos_pub.publish(pose);

ros::spinOnce();

rate.sleep();

}



return 0;

}
