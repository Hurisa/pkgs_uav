#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
//#include <gazebo_msgs/ModelStates.h>
#include <hector_uav_msgs/EnableMotors.h>
#include <nav_msgs/Odometry.h>

#include <std_msgs/Bool.h>


#include <stdlib.h>

#include <iostream>
#include <string>
#include <random>


#define PI 3.14159265358979323846

using namespace std;
class Taker
{
public:
  double z;
  bool control;
  void getPos(const nav_msgs::Odometry& msg);
  void getAck(const std_msgs::Bool& msg);  
  
};

void Taker::getAck(const std_msgs::Bool& msg)
{
  Taker::control=msg.data;  
}

void Taker::getPos(const nav_msgs::Odometry& msg)
{
  Taker::z=msg.pose.pose.position.z;
}


int main(int argc, char **argv)
{
// Initialize the ROS system and become a node .

  ros::init(argc, argv, "takeoff");



  ros::NodeHandle nh;
  Taker taker;

 // ros::Subscriber sep_sub = nh.subscribe("/gazebo/model_states", 1, &LevyBoid::getPos, &boid);

// Create a publisher object .
  ros::Publisher publisher = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1);
  ros::Publisher pubState = nh.advertise<std_msgs::Bool>("takenoff", 1);

  ros::Subscriber alt = nh.subscribe("ground_truth/state", 10, &Taker::getPos, &taker);
  ros::Subscriber ack = nh.subscribe("ack", 10, &Taker::getAck, &taker);

  ros::ServiceClient client = nh.serviceClient<hector_uav_msgs::EnableMotors>("enable_motors");

  hector_uav_msgs::EnableMotors srv;

  srv.request.enable=true;

  while(!client.call(srv)){
    ROS_INFO("motors not started");
  }

  ROS_INFO("motors started");
  bool takeoff=false;
  geometry_msgs::Twist msg;
  ros::Rate rate(10);

  double targetAlt(1);

  std_msgs::Bool boolMsg;


  while (ros::ok())
  {
    //cout<<taker.z<<endl;
    if(!takeoff)
    {       
        msg.linear.z=0.2;
                //static double X1, X2;
        if(taker.z>=1)
        {
          takeoff=true;
        }
        //ROS_INFO("Going up");
        publisher.publish(msg);

        boolMsg.data=false;
        pubState.publish(boolMsg);
    }
    else
    {    
        msg.linear.z=0;
        //ROS_INFO("Reached desired altitude");

        boolMsg.data=true;

        pubState.publish(boolMsg);
        //system("roslaunch uav_map allOne.launch");
        //static double X1, X2;
        //publisher.publish(msg);
        //ROS_INFO("stable");
        if(!taker.control)
        {
          publisher.publish(msg);
        }
        else
        {
          publisher.shutdown();
        }
        
    }
    
    

    //ROS_INFO("going to spin");
    ros::spinOnce();
    rate.sleep();
  }
  return 0;
}
