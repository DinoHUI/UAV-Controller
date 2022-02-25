#include <ros/ros.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/AttitudeTarget.h>
#include "mavros_msgs/PositionTarget.h"
//#include "object_detector/States.h"
//#include "drone_controller/Error.h"
#include <mavros_msgs/CommandTOL.h>

static ros::NodeHandle nh;
static mavros_msgs::State               current_state;
static mavros_msgs::AttitudeTarget      UAV_AttitudeTarget;
static geometry_msgs::PoseStamped       UAV_pose_sub,UAV_pose_pub;
static geometry_msgs::PositionTarget    UAV_position_sub,UAV_position_pub;
static mavros_msgs::CommandTOL          land_client;
ros::Time lastTime;

//        //class
//class controller
//{
//    public:
//        ros::NodeHandle nh
//        ros::Subscriber sub;
//        ros::Publisher pub;
//        ros::Publisher pub1;
//        ros::Time lastTime;
//        ros::ServiceClient land_client;
//    }


//definding element in uav_pose_sub and uav_lp
void uav_pose_sub(const geometry_msgs::PoseStamped::ConstPtr& pose){
    UAV_pose_sub.pose.position.x = pose->pose.position.x;
    UAV_pose_sub.pose.position.y = pose->pose.position.y;
    UAV_pose_sub.pose.position.z = pose->pose.position.z;
    UAV_pose_sub.pose.orientation.w = pose->pose.orientation.w;
    UAV_pose_sub.pose.orientation.x = pose->pose.orientation.x;
    UAV_pose_sub.pose.orientation.y = pose->pose.orientation.y;
    UAV_pose_sub.pose.orientation.z = pose->pose.orientation.z;
    UAV_lp << UAV_pose_sub.pose.position.x,UAV_pose_sub.pose.position.y,UAV_pose_sub.pose.position.z,
              UAV_pose_sub.pose.orientation.w,UAV_pose_sub.pose.orientation.x,UAV_pose_sub.pose.orientation.y,UAV_pose_sub.pose.orientation.z;
}

void uav_pose_pub(Vec7 posepub){
    UAV_pose_pub.header.frame_id = "world";
    UAV_pose_pub.pose.position.x = posepub[0];
    UAV_pose_pub.pose.position.y = posepub[1];
    UAV_pose_pub.pose.position.z = posepub[2];
    UAV_pose_pub.pose.orientation.w = posepub[3];
    UAV_pose_pub.pose.orientation.x = posepub[4];
    UAV_pose_pub.pose.orientation.y = posepub[5];
    UAV_pose_pub.pose.orientation.z = posepub[6];
}

int main (int argc, argv)
{
        //subscriber and publisher
        //publish the target position
        ros::publisher      UAV_position_pub = nh.advertise<mavros_msgs::PositionTarget>("targetposition_topic",10) ; //dont know what topic shold be
        //subcribe to the kalman filter data
        ros::Subscriber     UAV_position_sub = nh.subscribe("/predicted_states", 10, &Controller::controllerCallBack, this);
        //landing
        ros::ServiceClient  land_client = nh.serviceClient<mavros_msgs::CommandTOL>("mavros/cmd/land");
        lastTime = ros::Time::now(); // ROS time initialization throw ros time

        if() //marker detected, fly to the target
        {
        // Position target object to publish
                   mavros_msgs::PositionTarget pos;

                   //FRAME_LOCAL_NED to move WRT to body_ned frame
                   pos.coordinate_frame = mavros_msgs::PositionTarget::FRAME_BODY_NED;

                   pos.header.stamp = ros::Time::now(); // Time header stamp
                   //pos.header.frame_id = "base_link"; // "base_link" frame to compute odom
                   //pos.type_mask = 1987; // Mask for Vx, Vy, Z pos and Yaw rate
                   pos.position.z = zpos;
                   pos.velocity.x = vx;
                   pos.velocity.y = vy;
                   pos.yaw_rate = vthe;

                   printf("Proportional Vx, Vy, Vthe and Zpos values at (%f,%f,%f,%f) \n", vx, vy, vthe, zpos);
                   pub.publish(pos);

                   //printf("Error at Vx, Vy, Theta and Z are (%f,%f,%f,%f) \n", ErX, ErY, ErTheta, ErZ);
                   //pub1.publish(er);
               }
        }


        if() //close to the target, start descending
        {
            zpos-=0.1  //descend rate
        }
        // Drone service for automatic langind when it reaches an specific altitude
        if(zpos <= 0.9)
        {
            mavros_msgs::CommandTOL land_cmd; // Set all the descend parameters to Zero
            land_cmd.request.yaw = 0;
            land_cmd.request.latitude = 0;
            land_cmd.request.longitude = 0;
            land_cmd.request.altitude = 0;

            // When it lands, everything goes to zero
            if (!(land_client.call(land_cmd) && land_cmd.response.success))
            {
                // Publish the service of landing
                ROS_INFO("Landing");
                // Print final Error
                printf("Error at Vx, Vy, Theta and Z are (%f,%f,%f,%f) \n", ErX, ErY, ErTheta, ErZ);
                pub1.publish(er);
                ros::shutdown(); // Shutdowm the node
            }
        }
      ros::spinOnce();
      return 0;
}
