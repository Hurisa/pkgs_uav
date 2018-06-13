/* 
TEST THE WORLD FILE TEST
*/

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include <gazebo_msgs/ModelStates.h>
#include <hector_uav_msgs/EnableMotors.h>
#include <sensor_msgs/LaserScan.h>


//#include <tf/tf.h>
//#include <nav_msgs/Odometry.h>

#include <stdlib.h>
#include <iostream>
#include <string>
#include <random>
#include <cmath>       // std::abs
#include <time.h>
#include <vector>
#include <math.h>       /* isinf, sqrt */
//#include <dwa_planner.h>


using namespace std;

class Explorer
{
public:
  double x, y, targetTheta, angularMsg;
  int minGap=150;
  float minRange=1.5;
  vector<int> gapsRaw, gapI, gapF, gapLength;

  vector<float> readings;
  void getRange(const sensor_msgs::LaserScan& msg); // chooses the range

  void getGaps(const sensor_msgs::LaserScan& msg, int posI, int posF); //gets the gaps in vector positions

  void getVectors();

  float getAngle();


};

float Explorer::getAngle() //gets the angle differece
{

	float theta;
	vector<float> center;
	vector<float> angles;

//	cout<<"Explorer::gapLength.size()"<<Explorer::gapLength.size()<<endl;
	for(int i(0);i<Explorer::gapLength.size();i++){

		//cout<<"Explorer::gapF[i] "<<Explorer::gapF[i]<<endl;
		//cout<<"Explorer::gapI[i] "<<Explorer::gapI[i]<<endl;
		//cout<<"Explorer::gapLength[i] "<<Explorer::gapLength[i]<<endl;
		int z=0;
		//cout<<"test 0"<<endl;
		if (Explorer::gapLength[i]>Explorer::minGap){
//			cout<<"test"<<endl;
			center.push_back((Explorer::gapF[i]+Explorer::gapI[i])/2);
			//cout<<"center "<<center[z]<<" "<<"length "<<Explorer::gapLength[i]<<endl;
			

			angles.push_back(center[z]*260/1040-130);
			z++;
			//cout<<"angle "<< angles[z]<<endl;
		}
	}
    //cout<<angles.size()<<endl;

    //for(int i(0);i<angles.size();i++)
    //{
    //	cout<<angles[i]<<endl;
    //}
    theta=1000;
    for(int i=0;i<angles.size();i++)
    {
     if(angles[i]<theta)
    	theta=angles[i];
    }
    //cout<<"theta "<< theta<<endl;
    return theta;

}

//Inputs are the message and positions in the scan vector to check
void Explorer::getGaps(const sensor_msgs::LaserScan& msg, int posI, int posF) 
{

		Explorer::readings.clear();
		Explorer::gapsRaw.clear();
		//ROS_INFO("--------------------------------------------------------------------- ");
	    int i;
	    int counter=0;

        //cout<<"readings :"<< endl;
	    for (i = posI; i < posF; i++) {
	    	Explorer::readings.push_back(msg.ranges[i]);
          // cout << Explorer::readings[counter] << " ";

            if (isinf(Explorer::readings[counter])||Explorer::readings[counter]>Explorer::minRange){
            	Explorer::gapsRaw.push_back(i);
            }

            counter++;          
        }
        cout<<endl;
        //cout<<endl;
        //ROS_INFO("...................................................................... ");
        //cout<<"Raw Gaps"<<endl;
       	//for (int j = 0; j < gapsRaw.size(); j++) {
          //  cout << Explorer::gapsRaw[j] << " ";        
        //}
        //cout<<endl;

}

void Explorer::getVectors()
{
	    Explorer::gapI.clear();
		Explorer::gapF.clear();
		if(Explorer::gapsRaw.size()>0){
	        Explorer::gapI.push_back(Explorer::gapsRaw[0]);       	

	        for (int k(1); k<Explorer::gapsRaw.size();k++){

	        	if(Explorer::gapsRaw[k]>Explorer::gapsRaw[k-1]+1){
	        		//cout<<Explorer::gapsRaw.size()<<endl;
	        		//cout<<k<<endl;
	        		//cout<<Explorer::gapsRaw[k]<<" "<<Explorer::gapsRaw[k-1]+1<<endl;
	        		Explorer::gapF.push_back(Explorer::gapsRaw[k-1]);
	        		Explorer::gapI.push_back(Explorer::gapsRaw[k]);
	        		//cout <<" starts at : " << Explorer::gapsRaw[k] << endl; 
	        	}

	        }

	        if(Explorer::gapF.size()==Explorer::gapI.size()-1)
	        {
	        	Explorer::gapF.push_back(Explorer::gapsRaw[Explorer::gapsRaw.size()-1]);
	        }
        
	        int n;
	        n=Explorer::gapF.size();
	 		//cout<<"n is: "<< n <<endl;
	        int length;
	        Explorer::gapLength.clear();
	        for (int(a)=0;a<n;a++){
	        	length=(Explorer::gapF[a]-Explorer::gapI[a]);
	        	//ROS_INFO("TEST ");
	        	Explorer::gapLength.push_back(length);

	        }
	        //for(int i=0; i<Explorer::gapLength.size(); i++){
	    	  //  cout<<Explorer::gapLength[i]<<endl;
	    	//}

	        /*cout<<"Start :"<< endl;
	        for(int k(0); k<Explorer::gapI.size();k++)
	        {
	        cout << Explorer::gapI[k] << " ";
	        }
	        cout<<endl;
	        //cout<<endl;
	        cout<<"ends :"<< endl;
	        for(int k(0); k<Explorer::gapF.size();k++)
	        {
	        cout << Explorer::gapF[k] << " ";
	        }
	        //cout<<Explorer::readings.size()<<endl;
	        //cout<<Explorer::readings.size()<<endl;
	        cout<<endl;*/
    	}
}

void Explorer::getRange(const sensor_msgs::LaserScan& msg) //Accounts for the minimum range here
{
		float angleF, angleR, angleL, minAngle;
		vector<float> minAngles;

		minAngles.clear();

		Explorer::getGaps(msg, 360, 679); //Front 
		Explorer::getVectors();
		angleF=Explorer::getAngle();
		cout<<"front angle is "<< angleF<<endl;

		minAngles.push_back(angleF);

		Explorer::getGaps(msg, 0, 359); //Front 
		Explorer::getVectors();		
		angleL=Explorer::getAngle();
		cout<<"Right angle is "<< angleL<<endl;
		
		minAngles.push_back(angleL);

		Explorer::getGaps(msg, 680, 1039); //Front 
		Explorer::getVectors();
		angleR=Explorer::getAngle();
		cout<<"left angle is "<< angleR<<endl;

		minAngles.push_back(angleR);


		float minimumAngle=1000;
		int pos;
		for(int(i)=0;i<minAngles.size();i++){
		//cout<<"i "<<i<<endl;
			if(abs(minAngles[i])<abs(minimumAngle))
			{				
				//cout<<i<<endl;				
				minimumAngle=minAngles[i];
				pos=i;
				
			}

		}
		//cout<<pos<<endl;
		
		if (abs(minimumAngle)>5 && minimumAngle>0)
		{
			cout<<"Go Right"<<endl;
			Explorer::angularMsg=-1;
		}
		else if (abs(minimumAngle)>5 && minimumAngle<0)
		{
			cout<<"Go Left"<<endl;
			Explorer::angularMsg=1;
		}
		else if (abs(minimumAngle)<5)
		{
			cout<<"Go Forward"<<endl;
			Explorer::angularMsg=0;
		}


		/*Explorer::readings.clear();

		Explorer::gapsRaw.clear();
		//ROS_INFO("--------------------------------------------------------------------- ");
	    int i;
	    int counter=0;

         //cout<<"readings :"<< endl;
	    for (i = 360; i < 679; i++) {
	    	Explorer::readings.push_back(msg.ranges[i]);
           //cout << Explorer::readings[counter] << " ";

            if (isinf(Explorer::readings[counter])||Explorer::readings[counter]>Explorer::minRange){
            	Explorer::gapsRaw.push_back(i);
            }

            counter++;          
        }
        cout<<endl;
        //cout<<endl;
        //ROS_INFO("...................................................................... ");
        //cout<<"Raw Gaps"<<endl;
       	for (int j = 0; j < gapsRaw.size(); j++) {
            //cout << Explorer::gapsRaw[j] << " ";        
        }
        cout<<endl;*/
        //ROS_INFO("--------------------------------------------------------------------- ");
       
        /*Explorer::gapI.clear();
		Explorer::gapF.clear();
		//cout<<"vector is empty? "<<Explorer::gapI.empty()<<endl;
		//cout << "raw gaps size is: " <<Explorer::gapsRaw.size() <<endl; 
        
		if(Explorer::gapsRaw.size()>0){
	        Explorer::gapI.push_back(Explorer::gapsRaw[0]);       	

	        for (int k(1); k<Explorer::gapsRaw.size();k++){

	        	if(Explorer::gapsRaw[k]>Explorer::gapsRaw[k-1]+1){
	        		//cout<<Explorer::gapsRaw.size()<<endl;
	        		//cout<<k<<endl;
	        		//cout<<Explorer::gapsRaw[k]<<" "<<Explorer::gapsRaw[k-1]+1<<endl;
	        		Explorer::gapF.push_back(Explorer::gapsRaw[k-1]);
	        		Explorer::gapI.push_back(Explorer::gapsRaw[k]);
	        		//cout <<" starts at : " << Explorer::gapsRaw[k] << endl; 
	        	}

	        }

	        if(Explorer::gapF.size()==Explorer::gapI.size()-1)
	        {
	        	Explorer::gapF.push_back(Explorer::gapsRaw[Explorer::gapsRaw.size()-1]);
	        }
        
	        int n;
	        n=Explorer::gapF.size();
	 		//cout<<"n is: "<< n <<endl;
	        int length;
	        Explorer::gapLength.clear();
	        for (int(a)=0;a<n;a++){
	        	length=(Explorer::gapF[a]-Explorer::gapI[a]);
	        	//ROS_INFO("TEST ");
	        	Explorer::gapLength.push_back(length);

	        }
	        //for(int i=0; i<Explorer::gapLength.size(); i++){
	    	  //  cout<<Explorer::gapLength[i]<<endl;
	    	//}
	         //cout<<"Start :"<< endl;
	        for(int k(0); k<Explorer::gapI.size();k++)
	        {
	        //cout << Explorer::gapI[k] << " ";
	        }
	        cout<<endl;
	        //cout<<endl;
	        //cout<<"ends :"<< endl;
	        for(int k(0); k<Explorer::gapF.size();k++)
	        {
	         //cout << Explorer::gapF[k] << " ";
	        }
	        //cout<<Explorer::readings.size()<<endl;
	        //cout<<Explorer::readings.size()<<endl;
	        cout<<endl;
    	}*/

    		//cout<<"Explorer::gapF[0] "<<Explorer::gapF[0]<<endl;
			//cout<<"Explorer::gapI[0] "<<Explorer::gapI[0]<<endl;
    		//cout<<"Explorer::gapLength[0] "<<Explorer::gapLength[0]<<endl;

}







int main(int argc, char **argv)
{

	ros::init(argc, argv, "read_LaserScan");
	ros::NodeHandle nh;
	ros::Rate rate(10);
	
	Explorer explorer;
	geometry_msgs::Twist msg;

	ros::Subscriber range_sub = nh.subscribe("robot/scan", 10, &Explorer::getRange, &explorer);
	ros::Publisher ang_pub_dwa = nh.advertise<geometry_msgs::Twist>("robot/cmd_vel", 1);
	

	while (ros::ok()){

		msg.angular.z=explorer.angularMsg;

		ang_pub_dwa.publish(msg);


        ros::spinOnce();
        rate.sleep();
  	}
  return 0;

}