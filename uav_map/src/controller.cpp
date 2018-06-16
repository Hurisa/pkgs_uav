#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Float64.h"
#include "std_msgs/Bool.h"
#include <geometry_msgs/Twist.h>
//#include <gazebo_msgs/ModelStates.h>
#include <hector_uav_msgs/EnableMotors.h>
#include <nav_msgs/Odometry.h>

#include <tf/tf.h>
#include <nav_msgs/Odometry.h>

#include <stdlib.h>
#include <iostream>
#include <string>
#include <random>
#include <cmath>       // std::abs
#include <time.h>

using namespace std;

class Controller
{
public: 
	float x, y, yaw;
	float levyTheta, levyJump;
	float avoidTheta;
	bool jumpCompleted, takeoff;

	string state;

	void get_levyTheta(const std_msgs::Float32& msg);
	void get_levyJump(const std_msgs::Float32& msg);

	void get_avoidTheta(const std_msgs::Float32& msg);

	void getPos(const nav_msgs::Odometry& msg);
	void getYaw(const nav_msgs::Odometry& msg);

	void getLift(const std_msgs::Bool& msg);
};




void Controller::getLift(const std_msgs::Bool& msg)
{
	Controller::takeoff=msg.data;
}

void Controller::getYaw(const nav_msgs::Odometry& msg)
{

    
    tf::Quaternion q(
        msg.pose.pose.orientation.x,
        msg.pose.pose.orientation.y,
        msg.pose.pose.orientation.z,
        msg.pose.pose.orientation.w);


    tf::Matrix3x3 m(q);

    double roll, pitch, yaw;
    
    m.getRPY(roll, pitch, yaw);
    
	Controller::yaw=yaw;
}


void Controller::getPos(const nav_msgs::Odometry& msg)
{
	Controller::x=msg.pose.pose.position.x;
	Controller::y=msg.pose.pose.position.y;
}	
	
void Controller::get_levyTheta(const std_msgs::Float32& msg)
{
	Controller::levyTheta=msg.data;
	//cout<< Controller::levyTheta <<endl;
}

void Controller::get_levyJump(const std_msgs::Float32& msg)
{
	Controller::levyJump=msg.data;
}

void Controller::get_avoidTheta(const std_msgs::Float32& msg)
{
	Controller::avoidTheta=msg.data;
}


int main(int argc, char **argv)
{

	ros::init(argc, argv, "State_Machine");
	ros::NodeHandle nh;
	ros::Rate rate(10);

	Controller controller;
	controller.jumpCompleted=true;

	geometry_msgs::Twist vel_msg;

	std_msgs::Bool boolMsg;	

	ros::Publisher vel = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	ros::Publisher ack = nh.advertise<std_msgs::Bool>("ack", 1);

	ros::Subscriber levytheta_sub = nh.subscribe("theta", 10, &Controller::get_levyTheta, &controller);
	ros::Subscriber levyjump_sub  = nh.subscribe("jump", 10, &Controller::get_levyJump, &controller);
	ros::Subscriber avoid_sub  = nh.subscribe("avoid", 10, &Controller::get_avoidTheta, &controller);
	ros::Subscriber pos_sub  = nh.subscribe("ground_truth/state", 10, &Controller::getPos, &controller);
	ros::Subscriber yaw_sub  = nh.subscribe("ground_truth/state", 10, &Controller::getYaw, &controller);
	ros::Subscriber takeoff_sub  = nh.subscribe("takenoff", 10, &Controller::getLift, &controller);
	
	controller.jumpCompleted=true;

	float angle, jump;
	float xinit, yinit, distance, oldDistance;

	bool override=false;

	float angleErr=0.1;

	while (ros::ok()){
		
		//cout<<"*"<<controller.takeoff<<"*"<<endl;
		if(controller.takeoff)
		{	
			boolMsg.data=true;
	 		cout<<"--------------------------------------------------------------------"<<endl;
			vel_msg.linear.z=0;
			///cout<<controller.yaw<<endl;

			if(controller.avoidTheta==0)
			{

				if (controller.jumpCompleted)
				{
					angle=controller.levyTheta;
					jump=controller.levyJump;				

					xinit=controller.x;
			        yinit=controller.y;
			        distance=0;
			        oldDistance=0;
			        controller.jumpCompleted=false;
			        cout<<"got a new jump"<<endl;
				}
				else
				{

					if(abs(angle-controller.yaw)/angle>angleErr&&!override)
					{
						vel_msg.linear.x=0;
						cout<<"rotating"<<endl;
						float xx=cos(angle-controller.yaw);
						float yy=sin(angle-controller.yaw);

						if (atan2(yy,xx)<0)
						{
							vel_msg.angular.z=-1;
						}
						else if (atan2(yy,xx)>0)
						{
							vel_msg.angular.z=1;
						}
						controller.state="rotating";

					}
					else
					{
						vel_msg.linear.x=1;
						cout<<"moving forward"<<endl;
						//controller.jumpCompleted=true;
						vel_msg.angular.z=0;
						

					}
					/*else if((angle-controller.yaw)/angle<angleErr&&override)
					{
						vel_msg.linear.x=1;
						cout<<"moving forward"<<endl;
						//controller.jumpCompleted=true;
						vel_msg.angular.z=0;
						
					}
					else if((angle-controller.yaw)/angle<angleErr&&!override)
					{
						vel_msg.linear.x=1;
						//cout<<"Option3"<<endl;
						//controller.jumpCompleted=true;
						vel_msg.angular.z=0;
						
					}		
					else if((angle-controller.yaw)/angle>angleErr&&override)
					{
						vel_msg.linear.x=1;
						//cout<<"Option4"<<endl;
						//controller.jumpCompleted=true;
						vel_msg.angular.z=0;
						
					}*/						
					

					distance=sqrt(pow(xinit-controller.x,2)+pow(yinit-controller.y,2))+oldDistance;
					cout<<"distance "<<distance<<endl;
					cout<<"jump "<<jump<<endl;
					if (distance>jump)
					{
						controller.jumpCompleted=true;
						cout<<"Jump Completed"<<endl;
						override=false;					
					}
				}
			}
			else
			{	
				//distance=sqrt(pow(xinit-controller.x,2)+pow(yinit-controller.y,2));
				oldDistance=distance;
				cout<<"oldDistance: "<<oldDistance<<endl;
				xinit=controller.x;
			    yinit=controller.y;
			    vel_msg.linear.x=0.5;
				vel_msg.angular.z=1.2*controller.avoidTheta;
				override=true;


			}
			vel.publish(vel_msg);
			ack.publish(boolMsg);
		}
		else
		{
			boolMsg.data=false;
			ack.publish(boolMsg);
		}
		
        ros::spinOnce();
        rate.sleep();
    		cout<<"--------------------------------------------------------------------"<<endl;
  	}




}