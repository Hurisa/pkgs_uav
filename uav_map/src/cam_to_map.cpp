#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include <gazebo_msgs/ModelStates.h>
#include <hector_uav_msgs/EnableMotors.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud.h>
#include "std_msgs/Float32.h"

#include "edinbots_detection/Detector.h"

#include <geometry_msgs/Point.h>
#include <geometry_msgs/Point32.h>

//#include <tf/tf.h>
//#include <nav_msgs/Odometry.h>

#include <stdlib.h>
#include <iostream>
#include <string>
#include <random>
#include <cmath>       // std::abs
#include <time.h>
#include <vector>
#include <math.h>  

using namespace std;


class Translator
{
public: 
	vector<float> x, y;

	void getVictim(const sensor_msgs::PointCloud& msg);
	vector<geometry_msgs::Point32> points;
	//xc yc;
	//
}; 

void Translator::getVictim(const sensor_msgs::PointCloud& msg)
{
	points = msg.points;
}

int main(int argc, char **argv)
{

	ros::init(argc, argv, "cam_to_map");
	ros::NodeHandle nh;
	ros::Rate rate(10);
	
	Translator translator;
	geometry_msgs::Point point;




	ros::Subscriber victims_sub = nh.subscribe("detection/victim_markers", 10, &Translator::getVictim, &translator);
	ros::Publisher victims = nh.advertise<geometry_msgs::Point>("/ground/poi", 1);


	int numberOfDetection = translator.points.size();

	if( numberOfDetection){
		for (int i = 0; i < numberOfDetection; i++){
			geometry_msgs::Point32 tempPoint = translator.points[i];
			victims.publish(tempPoint);
		}
	}
	


	while (ros::ok()){


		//point.x=translator.x[0];
		//point.y=translator.y[0];

		victims.publish(point);

        ros::spinOnce();
        rate.sleep();
  	}
  return 0;

}
