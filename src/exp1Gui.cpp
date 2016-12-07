#include <iostream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "ros/ros.h"

#include <dynamic_reconfigure/DoubleParameter.h>
#include <dynamic_reconfigure/Reconfigure.h>
#include <dynamic_reconfigure/Config.h>

#include "dyret_common/GetGaitControllerStatus.h"
#include "dyret_common/ActionMessage.h"
#include "dyret_common/Trajectory.h"
#include "dyret_common/GetGaitEvaluation.h"

#include "dyret_utils/wait_for_ros.h"
#include "dyret_utils/timeHandling.h"
#include "dyret_utils/opto.h"
#include "dyret_utils/optodaq.h"

#include <unistd.h>

#include "external/sferes/phen/parameters.hpp"
#include "external/sferes/gen/evo_float.hpp"
#include "external/sferes/ea/nsga2.hpp"
#include "external/sferes/eval/eval.hpp"
#include "external/sferes/stat/pareto_front.hpp"
#include "external/sferes/modif/dummy.hpp"
#include "external/sferes/run.hpp"
#include <boost/program_options.hpp>

#include "geometry_msgs/WrenchStamped.h"
#include <stdlib.h>

//ros::ServiceClient get_gait_evaluation_client;
//ros::Publisher trajectoryMessage_pub;
//ros::ServiceClient gaitControllerStatus_client;

//FILE * evoFitnessLog;
//FILE * evoParamLog_gen;
//FILE * evoParamLog_phen;
//bool singleObjective;
//double globalSpeed;

//int currentIndividual;


std::vector<std::vector<float> > opto_data_raw[4];
void MySleep(unsigned long p_uMillisecs)
{
    usleep(p_uMillisecs * 1000);
}

/*
void checkOptoforceArray(){
    int threshold_size = 100;
    if(opto_data_raw[3]){
        if(opto_data_raw[sensornr].size() == threshold_size){

        }

    }
}
*/


void getPoints(int sensornr, float opto_lst[3]){
    printf("----------------------------------Getpoints-------------------------------------------------------\n");
    std::vector<float> tmp;
    tmp.push_back(opto_lst[0]);
    tmp.push_back(opto_lst[1]);
    tmp.push_back(opto_lst[2]);
    
    printf("x: %f\n", opto_lst[0]);
    printf("y: %f\n", opto_lst[1]);
    printf("z: %f\n", opto_lst[2]);

    opto_data_raw[sensornr].push_back(tmp);
    std::cout << "Sensor" << sensornr << "\n"; 
    std::cout << "first " << (sizeof opto_data_raw/ sizeof opto_data_raw[0]) << "\n";
    std::cout << "second " << opto_data_raw[sensornr].size() << "\n";
    std::cout << "third" << opto_data_raw[sensornr][0].size() << "\n";
    
    std::cout << "x:" << opto_data_raw[sensornr][0][0] << "\n";
    std::cout << "y:" << opto_data_raw[sensornr][0][1] << "\n";
    std::cout << "z:" << opto_data_raw[sensornr][0][2] << "\n";
//    std::cout << "vector: " << opto_0[0][0] << "\n";
}

//const std_msgs::String::ConstPtr&
void optoforceCallback0(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    getPoints(0,optoforce_lst);
/*    
      printf("Sensor 0\n");
      printf("x0: %f\n", msg->wrench.force.x);
      printf("y0: %f\n", msg->wrench.force.y);
      printf("z0: %f\n", msg->wrench.force.z);*/
}



void optoforceCallback1(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    getPoints(1,optoforce_lst);

/*  printf("Sensor 1\n");
    printf("x: %f\n", msg->wrench.force.x);
    printf("y: %f\n", msg->wrench.force.y);
    printf("z: %f\n", msg->wrench.force.z);*/
}

void optoforceCallback2(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    getPoints(2,optoforce_lst);
 
/*  printf("Sensor 2\n");
    printf("x: %f\n", msg->wrench.force.x);
    printf("y: %f\n", msg->wrench.force.y);
    printf("z: %f\n", msg->wrench.force.z);*/
}

void optoforceCallback3(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    getPoints(3,optoforce_lst);
/*  printf("Sensor 3\n");
    printf("x: %f\n", msg->wrench.force.x);
    printf("y: %f\n", msg->wrench.force.y);
    printf("z: %f\n", msg->wrench.force.z);*/
}

int main(int argc, char **argv){
    int inputChar;
    ros::init(argc, argv, "exp1Gui");
    ros::NodeHandle n;
    ros::Subscriber optoforce0 = n.subscribe("optoforce_0", 1000, optoforceCallback0);
    ros::Subscriber optoforce1 = n.subscribe("optoforce_1", 1000, optoforceCallback1);
    ros::Subscriber optoforce2 = n.subscribe("optoforce_2", 1000, optoforceCallback2);
    ros::Subscriber optoforce3 = n.subscribe("optoforce_3", 1000, optoforceCallback3);

    ros::AsyncSpinner spinner(2);
    spinner.start();
    printf("Start trening");
    do {
        printf("1 - Optoforce\n"
               "0 - Exit\n> ");

//        currentIndividual = 1;

        inputChar = getchar();
        std::cin.ignore(1000,'\n');

        switch(inputChar){
        case '1':
        {

        }
        break;
        
        case '0':
            printf("\tExiting program\n");
            break;
        default:
            printf("\tUndefined choice\n");
            break;
        };

        printf("\n");

    } while (inputChar != '0');
    return 0;
}
