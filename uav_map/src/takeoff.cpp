#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include <gazebo_msgs/ModelStates.h>
#include <hector_uav_msgs/EnableMotors.h>


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
  void getPos(const gazebo_msgs::ModelStates& msg); 
  
};

void Taker::getPos(const gazebo_msgs::ModelStates& msg)
{

            Taker::z=msg.pose[1].position.z;
}


int main(int argc, char **argv)
{
// Initialize the ROS system and become a node .

  ros::init(argc, argv, "takeoff");



  ros::NodeHandle nh;
  Taker taker;

 // ros::Subscriber sep_sub = nh.subscribe("/gazebo/model_states", 1, &LevyBoid::getPos, &boid);

// Create a publisher object .
  ros::Publisher publisher = nh.advertise<geometry_msgs::Twist>("robot/cmd_vel", 1);
  

  ros::Subscriber alt = nh.subscribe("/gazebo/model_states", 10, &Taker::getPos, &taker);

  ros::ServiceClient client = nh.serviceClient<hector_uav_msgs::EnableMotors>("robot/enable_motors");

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

  while (ros::ok())
  {
    cout<<taker.z<<endl;
    if(!takeoff)
    {       
        msg.linear.z=0.1;
                //static double X1, X2;
        if(taker.z>=1)
        {
          takeoff=true;
        }
        ROS_INFO("Going up");
    }
    else
    {    
        msg.linear.z=0;
        ROS_INFO("Reached desired altitude");

        //static double X1, X2;
        //publisher.publish(msg);
        //ROS_INFO("stable");
    }
    
    publisher.publish(msg);

    ROS_INFO("going to spin");
    ros::spinOnce();
    rate.sleep();
  }
  return 0;
}
