/* 
NOTES
Do rotation

Keep in mind that the direction in which it moves might no be correct becasue the difference in angles.
Which could possibly be corrected from the orientation side since it will take the robots orientation 
in respect to the map.

*/

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include <gazebo_msgs/ModelStates.h>
#include <hector_uav_msgs/EnableMotors.h>


#include <tf/tf.h>
#include <nav_msgs/Odometry.h>

#include <stdlib.h>
#include <iostream>
#include <string>
#include <random>
#include <cmath>       // std::abs
#include <time.h>

using namespace std;


#define PI 3.14159265358979323846
class LevyBoid
{
public:
  double x, y, z;
  string mode;
  void getPos(const gazebo_msgs::ModelStates& msg);
};

void LevyBoid::getPos(const gazebo_msgs::ModelStates& msg)
{

            LevyBoid::x=msg.pose[1].position.x;
            LevyBoid::y=msg.pose[1].position.y;
            LevyBoid::z=msg.pose[1].position.z;
}





int main(int argc, char **argv)
{
// Initialize the ROS system and become a node .

  ros::init(argc, argv, "publish_velocity");
  LevyBoid boid;

  boid.mode="walk";
  ros::NodeHandle nh;

  ros::ServiceClient client = nh.serviceClient<hector_uav_msgs::EnableMotors>("robot/enable_motors");

  hector_uav_msgs::EnableMotors srv;

  srv.request.enable=true;




  ros::Subscriber sep_sub = nh.subscribe("/gazebo/model_states", 10, &LevyBoid::getPos, &boid);
// Create a publisher object .
  ros::Publisher publisher = nh.advertise<geometry_msgs::Twist>("robot/cmd_vel", 1);


struct timeval time;
gettimeofday(&time,NULL);

srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

double mu=1.5;
double sigma=10.0;
double muOne=mu-1;
double muTwo=2-mu;
double x, y, theta, jump;
double U1, U2, U3, phi, r;
double vmax=0.5;

ros::Rate rate(10);
     
geometry_msgs::Twist msg;
bool stop(false);

     //double xinit;
     //double yinit;

double dist;
double xinit;
double yinit;

bool takeoff=false;

int takeoff_counter=0;
int levy_counter=20;

  while(!client.call(srv)){
    ROS_INFO("motors not started");
  }

  ROS_INFO("motors started");

while (ros::ok()){

/*if(levy_counter<20)
{    
        cout<<"------------------------------------"<<endl;

	  		if(boid.mode=="walk"){
		        U1=double(rand())/double(RAND_MAX)*2-1;
		        U2=double(rand())/double(RAND_MAX)*2-1;
		        U3=double(rand())/double(RAND_MAX)*2-1;

		        U1 = U1*(PI/2);
		        U2 = (U2+1)/2;

		        phi = U3 * M_PI;


		        r = (sin(muOne * U1) / pow(cos(U1), 1/muOne) ) * pow((cos(muTwo * U1) / U2), (muTwo / muOne));

		        x = r * cos(phi);
		        y = r * sin(phi);

		        theta=atan2(y,x);
		        jump=sqrt(pow(x,2)+pow(y,2));
		        
		        cout<<"Generating new step"<<endl;
		        cout<<"jump "<<jump<<endl;

		        xinit=boid.x;
		        yinit=boid.y;


		        msg.linear.x=vmax*cos(theta);

		        msg.linear.y=vmax*sin(theta);
		        msg.linear.z=0;
		        boid.mode="jump";



	    	}
	    	else if(boid.mode=="jump"){ 
	    		cout<<"My Position (x,y) "<<boid.x<<" "<<boid.y<<endl;
	    		cout<<"My velocity will be (vx,vy) "<< msg.linear.x<<" "<< msg.linear.y<<endl;
	    		cout<<"My goal (x,y) "<<x<<" "<<y<<endl;
	    		dist=sqrt(pow(xinit-boid.x,2)+pow(yinit-boid.y,2));
	    		cout<<"continue previous one"<<endl;
	    		cout<<"distance "<<dist<<endl;

	    		if (abs(dist)>=jump)
	    		{
	    			boid.mode="walk";

	    		}

    	}
        cout<<"------------------------------------"<<endl;*/

    	if(!takeoff){     

        	msg.linear.z=0.1;
                //static double X1, X2;
        	if(boid.z>=1)
        	{
          		takeoff=true;
        	}
        	ROS_INFO("Going up");
    	}
    	else{    
        msg.linear.z=0;
        ROS_INFO("Reached desired altitude");
    	}

        cout<<"------------------------------------"<<endl;


        if(levy_counter>0 && takeoff){
        	msg.linear.x=0;
        	msg.linear.y=0; 
        	msg.linear.z=0;
        	levy_counter--;

        }

        else{
	  		if(boid.mode=="walk" && takeoff){
		        U1=double(rand())/double(RAND_MAX)*2-1;
		        U2=double(rand())/double(RAND_MAX)*2-1;
		        U3=double(rand())/double(RAND_MAX)*2-1;

		        U1 = U1*(PI/2);
		        U2 = (U2+1)/2;

		        phi = U3 * M_PI;


		        r = (sin(muOne * U1) / pow(cos(U1), 1/muOne) ) * pow((cos(muTwo * U1) / U2), (muTwo / muOne));

		        x = r * cos(phi);
		        y = r * sin(phi);

		        theta=atan2(y,x);
		        jump=sqrt(pow(x,2)+pow(y,2))/10;
		        
		        cout<<"Generating new step"<<endl;
		        cout<<"jump "<<jump<<endl;

 				xinit=boid.x;
		        yinit=boid.y;

		        msg.linear.x=vmax*cos(theta);

		        msg.linear.y=vmax*sin(theta);
		        msg.linear.z=0;
		        boid.mode="jump";
	    	}
	    	else if(boid.mode=="jump" && takeoff){ 
	    		//cout<<"My Position (x,y) "<<boid.x<<" "<<boid.y<<endl;
	    		//cout<<"My goal (x,y) "<<x<<" "<<y<<endl;
	    		//dist=sqrt(pow(x-boid.x,2)+pow(y-boid.y,2));
	    		dist=sqrt(pow(xinit-boid.x,2)+pow(yinit-boid.y,2));
	    		cout<<"continue previous one"<<endl;
	    		cout<<"distance "<<dist<<endl;
	    		cout<<"jump "<<jump<<endl;

	    		if (abs(dist)>=jump)
	    		{
	    			boid.mode="walk";

	    		}

    	}
    	cout<<"------------------------------------"<<endl;
    	}	
        publisher.publish(msg);
        levy_counter--;
        ros::spinOnce();
        rate.sleep();
  }
  return 0;
}

