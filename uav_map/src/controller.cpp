#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Float64.h"
#include "std_msgs/Bool.h"
#include <geometry_msgs/Twist.h>
//#include <gazebo_msgs/ModelStates.h>
#include <hector_uav_msgs/EnableMotors.h>
#include <nav_msgs/Odometry.h>

#include <string>

//#include "edinbots_detection/Detector.h"
#include <sensor_msgs/PointCloud.h>

#include <tf/tf.h>
#include <nav_msgs/Odometry.h>

#include <geometry_msgs/Point32.h>

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
	bool jumpCompleted, takeoff, victim;
	string teleop;

	string state;

	void get_levyTheta(const std_msgs::Float32& msg);
	void get_levyJump(const std_msgs::Float32& msg);

	void get_avoidTheta(const std_msgs::Float32& msg);

	void getPos(const nav_msgs::Odometry& msg);
	void getYaw(const nav_msgs::Odometry& msg);

	void getLift(const std_msgs::Bool& msg);

	void getOperator(const std_msgs::String& msg);
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


void Controller::getOperator(const std_msgs::String& msg)
{

	cout<<"IN REMOTE CONTROL "<<endl;
	cout<<"msg data "<<msg.data<<"*"<<endl;
	Controller::teleop=msg.data;
	cout<<"controller teleop "<<Controller::teleop<<"*"<<endl;
}

	
int main(int argc, char **argv)
{

	ros::init(argc, argv, "State_Machine");
	ros::NodeHandle nh;
	ros::Rate rate(10);

	Controller controller;
	controller.victim=false;
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

	ros::Subscriber teleop_sub  = nh.subscribe("/gui/control", 10, &Controller::getOperator, &controller);
	
	controller.jumpCompleted=true;

	float angle, jump;
	float xinit, yinit, distance, oldDistance;

	bool override=false;

	string ns;
	ns=nh.getNamespace();
	ns.erase(ns.begin());
	ns.erase(ns.begin());
	//string cmd;

	//cmd=controller.teleop;

	//cout<<"HEREEEEEEEE "<<cmd<<endl;

	float angleErr=0.2;

	nh.setParam("liveVictimDetected", false);

	bool victimState;

	int counterSpin(0);

	while (ros::ok()){
		;
		//victimState=

		//cout<<ns<<endl;
		//cout<<"HEREEEEEEEE "<<controller.teleop<<endl;
		if(counterSpin>10)
		{
			nh.getParam("liveVictimDetected", victimState);
			cout<<victimState<<endl;
			if (ns!=controller.teleop && !victimState)
			{
				cout<<"controller teleop should be done here"<<controller.teleop<<endl;
				//cout<<"**************"<<controller.takeoff<<"*"<<endl;
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
					       //cout<<"got a new jump"<<endl;
						}
						else
						{

							if(abs(angle-controller.yaw)/angle>angleErr&&!override)
							{
								vel_msg.linear.x=0;
								//cout<<"rotating"<<endl;
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
								//cout<<"moving forward"<<endl;
								//controller.jumpCompleted=true;
								vel_msg.angular.z=0;
								

							}
				

							distance=sqrt(pow(xinit-controller.x,2)+pow(yinit-controller.y,2))+oldDistance;
							//cout<<"distance "<<distance<<endl;
							//cout<<"jump "<<jump<<endl;
							if (distance>jump)
							{
								controller.jumpCompleted=true;
								//cout<<"Jump Completed"<<endl;
								override=false;					
							}
						}
					}
					else
					{	
						//distance=sqrt(pow(xinit-controller.x,2)+pow(yinit-controller.y,2));
						oldDistance=distance;
						//cout<<"oldDistance: "<<oldDistance<<endl;
						xinit=controller.x;
					    yinit=controller.y;
					    vel_msg.linear.x=0.5;
						vel_msg.angular.z=1.2*controller.avoidTheta;
						override=true;


					}

					
					cout<<"controller teleop "<<controller.teleop<<endl;
					vel.publish(vel_msg);
					ack.publish(boolMsg);
				}
				else
				{
					boolMsg.data=false;
					ack.publish(boolMsg);
				}
			}
			else{

				
				if(victimState)
					{
						cout<<" I have found a live victim hoooooray!!!"<<endl;
					}
					else
					{
						cout<<"controller teleop "<<controller.teleop<<endl;
						cout<<"IN REMOTE CONTROL"<<endl;
					}	
			
			}

		}
		counterSpin++;
        ros::spinOnce();
        rate.sleep();
    		cout<<"--------------------------------------------------------------------"<<endl;
  	}




}