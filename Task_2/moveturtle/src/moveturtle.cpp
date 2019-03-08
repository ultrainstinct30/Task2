#include <bits/stdc++.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <turtlesim/Spawn.h>
#include <turtlesim/Kill.h>

using namespace std;

struct point{
    float x;
    float y;
};

turtlesim::Pose pos;

void position(const turtlesim::Pose::ConstPtr& msg){
    pos.x = msg->x;
    pos.y = msg->y;
    pos.theta = msg->theta;
}

int main(int argc, char** argv){
    ros::init(argc, argv, "movealongpath");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    ros::Subscriber sub =  n.subscribe("/turtle1/pose", 10, &position);
    vector<point> coordinates;
    ifstream myfile;
    myfile.open("/home/ultrainstinct/Tasks/Task_2/points.txt",std::ios::app);
    float value;
    if(myfile.is_open()){
        while(myfile>>value){
            point tmp;
            tmp.x = value*11.0/640.0;
            myfile>>value;
            tmp.y = value*11.0/400.0;
            coordinates.push_back(tmp);
        }
    }

    ros::ServiceClient clientkill = n.serviceClient<turtlesim::Kill>("kill");
    turtlesim::Kill::Request reqkill;
    turtlesim::Kill::Response reskill;
    reqkill.name = "turtle1";
    clientkill.call(reqkill, reskill);

    ros::ServiceClient clientspawn = n.serviceClient<turtlesim::Spawn>("spawn");
    turtlesim::Spawn::Request reqspawn;
    turtlesim::Spawn::Response resspawn;
    reqspawn.x = coordinates[0].x;
    reqspawn.y = coordinates[0].y;
    reqspawn.theta = 0;
    reqspawn.name = "turtle1";
    clientspawn.call(reqspawn, resspawn);
    int j=1, i=0;
    ros::Rate loop_rate(0.5);
    while(ros::ok()){
        /*if(i<6){
            i++;
            continue;
        }*/
        geometry_msgs::Twist msg;
        while(j<coordinates.size()){
            double theta = (coordinates[j].x!=coordinates[j-1].x)? atan((float)(coordinates[j].y-coordinates[j-1].y)/(float)(coordinates[j].x-coordinates[j-1].x)) : 1.570793;
            if(pos.theta>0 && theta<0 && pos.theta-theta>1.570793){
	            while(abs(3.14159+theta-pos.theta)>0.001){
	                msg.linear.x = 0;
	                msg.angular.z = 5*(3.14159+theta-pos.theta);
	                pub.publish(msg);
	                ros::spinOnce();
	            }
	        }
	        else{
	        	while(abs(theta-pos.theta)>0.001){
	        		msg.linear.x = 0;
	        		msg.angular.z = 5*(theta-pos.theta);
	        		pub.publish(msg);
	        		ros::spinOnce();
	        	}
	        }
            float dist = sqrt((coordinates[j].x - coordinates[j-1].x)*(coordinates[j].x - coordinates[j-1].x)+(coordinates[j].y - coordinates[j-1].y)*(coordinates[j].y - coordinates[j-1].y));
            do{
            	float currdist = sqrt((coordinates[j-1].x-pos.x)*(coordinates[j-1].x-pos.x)+(coordinates[j-1].y-pos.y)*(coordinates[j-1].y-pos.y));
                msg.linear.x = 5*(dist-currdist);
                msg.angular.z = 0;
                pub.publish(msg);
                ros::spinOnce();
            }while(sqrt((coordinates[j].x-pos.x)*(coordinates[j].x-pos.x)+(coordinates[j].y-pos.y)*(coordinates[j].y-pos.y)) >= 0.1);
            j++;
        }   
        msg.linear.x = 0;
        msg.angular.z = 0;
        pub.publish(msg);
        ros::spinOnce();
    }
}