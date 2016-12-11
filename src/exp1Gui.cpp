#include <iostream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "ros/ros.h"
#include <unistd.h>
#include "geometry_msgs/WrenchStamped.h"
#include <stdlib.h>


std::vector<std::vector<float>> opto_data_raw[4];
int threshold_size = 15;
int total_sensors = 4;
float sum = 0;


void MySleep(unsigned long p_uMillisecs){
    usleep(p_uMillisecs * 1000);
}



void findMax(){
    float max = opto_data_raw[0][0][2];
    int i;
    for (i = 0; i < threshold_size; i++){
        if(opto_data_raw[0][i][2] > max){
            max = opto_data_raw[0][i][2];
        }
    }
    std::cout << "Max" << max << "\n";
   /* if(max > 3){
        for (i = 0; i < threshold_size; i++){
//            std::cout << "Z: " << opto_data_raw[0][i][2] << "\n";
        }
    }*/
}

void clearVectorArray(){
    int i;
    for(i=0;i<4;i++){
        opto_data_raw[i].clear();
    }
}

int checkStability(float prev, float current){
    float boundary = 0.5;
    if(fabs(prev-current) < boundary){
        return 1;
    }
    return 0;
}

//x = 0, y = 1, z = 2
void isOnGround(){
    if(opto_data_raw[0].size() == threshold_size){
        std::cout << "Average" << (sum/threshold_size) << "\n";
        int i;
        float prev = opto_data_raw[0][0][2];
        int stable = 1;
        for (i=1; i<threshold_size; i++){
            float current = opto_data_raw[0][i][2];
            if(checkStability(prev,current)){
                //OK
            }else{
                stable = 0;
                //std::cout << "Not stable HAHA" << "\n";
            }
        }
        if(stable){
            std::cout << "It is stable" << "\n";
        }
        else{
            std::cout << "It is not stable" << "\n";
            for (i = 0; i < threshold_size; i++){
                std::cout << "Z: " << opto_data_raw[0][i][2] << "\n";
            }
        }
        findMax();
        clearVectorArray();
        sum = 0;
    }
}


void isOnGround2(){
    int i;
    float prev = opto_data_raw[0][0][3];

    for (i=1; i<threshold_size; i++){
        float current = opto_data_raw[0][i][3];
        checkStability(prev,current);

    }
}


void getPoints(int sensornr, float opto_lst[3]){
    std::vector<float> tmp;
    tmp.push_back(opto_lst[0]);
    tmp.push_back(opto_lst[1]);
    tmp.push_back(opto_lst[2]);

    opto_data_raw[sensornr].push_back(tmp);

/*
    std::cout << "Sensor" << sensornr << "\n";
    std::cout << "first " << (sizeof opto_data_raw/ sizeof opto_data_raw[0]) << "\n";
    std::cout << "second " << opto_data_raw[sensornr].size() << "\n";
    std::cout << "third" << opto_data_raw[sensornr][0].size() << "\n";

    std::cout << "x:" << opto_data_raw[sensornr][0][0] << "\n";
    std::cout << "y:" << opto_data_raw[sensornr][0][1] << "\n";
    std::cout << "z:" << opto_data_raw[sensornr][0][2] << "\n";
*/
    isOnGround();
    //checkOptoforceArray();
}

void optoforceCallback0(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    sum = sum+msg->wrench.force.z;
    getPoints(0,optoforce_lst);
}



void optoforceCallback1(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    getPoints(1,optoforce_lst);

}

void optoforceCallback2(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    getPoints(2,optoforce_lst);
}

void optoforceCallback3(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    getPoints(3,optoforce_lst);
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

    do {
        printf("1 - Optoforce\n"
               "0 - Exit\n> ");

        inputChar = getchar();
        std::cin.ignore(1000,'\n');

        switch(inputChar){
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
