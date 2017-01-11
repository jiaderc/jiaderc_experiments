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
std::vector<std::vector<float>> extracted_data[4];
std::vector<std::vector<float>> rawDataS0;
std::vector<std::vector<float>> interestDataS0;


//Disse parameterne kan endres
/*int threshold_size = 15;
int longestStability = 8;*/

int threshold_size = 20;
int longestStability = 15;
int total_sensors = 4;

int extract_data = 0;
int allNegative = 0;
int fillFirstData = 0;
float sum = 0;


void MySleep(unsigned long p_uMillisecs){
    usleep(p_uMillisecs * 1000);
}

float getMax(){
    float max = rawDataS0.at(0).at(2);
    int i;
    for (i = 0; i < rawDataS0.size(); i++){
        if(rawDataS0.at(i).at(2) > max){
            max = rawDataS0.at(i).at(2);
        }
    }
    return max;
}

int bothNegative(float prev, float current){
    if(prev<0 && current < 0){
        return 1;
    }
    return 0;
}

//int checkStability(float prev, float current, int i){
int checkStability(int i){
    float boundary = 0.02;
    float prev = rawDataS0.at(i).at(2);
    int stabilityCount = 0;
    int j;

    for (j=i+1; j<threshold_size; j++) {
        float current = rawDataS0.at(j).at(2);
        if (fabs(prev - current) < boundary) {
            stabilityCount++;
        }
        else{
            return stabilityCount;
        }
    }
    return stabilityCount;
}

int checkStability2(int i){
    float boundary = 0.03;
    int stabilityCount = 0;
    int j,k;

    int min = -1;
    for(k=0;k<3;k++) {
        for (j = i + 1; j < threshold_size - 1; j++) {
            float current = rawDataS0.at(j).at(k);
            float next = rawDataS0.at(j + 1).at(k);
            if (fabs(next - current) < boundary) {
                stabilityCount++;
            }
            else {
                if(min == -1){
                    min = stabilityCount;
                }else if(stabilityCount<min){
                    min = stabilityCount;
                }
                break;
            }
        }
    }
    return min;
}

int getStartPos(){
    int i;
    for(i=0;i<threshold_size;i++){
        if(opto_data_raw[0][i][2] >= 0){
            return i;
        }
    }
    return -1;
}

void print_data(){
    int i;
    std::cout << "-----------START------------\n";
    std::cout << "Str: " << interestDataS0.size() << "\n";

    for(i=0;i<interestDataS0.size();i++){
        std::cout << interestDataS0.at(i).at(2) << "\n";
    }
    std::cout << "-------EEEEENNNND-----------\n";

}

void save_data(){
    int i;
    for(i=0;i<rawDataS0.size();i++){
        interestDataS0.push_back(rawDataS0.at(i));
    }
}
int saveData = 0;

void isOnGround(std::vector<std::vector<float>> rawData){
    if(rawData.size() == threshold_size){
        int i;
        float prev = rawData.at(0).at(2);

        int longestSequence = 0;

        if(saveData){
            save_data();
        }


        for (i=1; i<threshold_size; i++) {
            float current = rawData.at(i).at(2);
            if (bothNegative(prev, current)) {
                if ((longestSequence=checkStability(i)) > longestStability) {
                    std::cout << "YEEESS" << "\n";

                    if(saveData){
                        print_data();
                        interestDataS0.clear();
                    }
                    saveData = 1;
                    break;
                }
                else if(i > longestStability){
                    break;
                }
            }
        }


        std::cout << "Max: " << getMax() <<  "\n";
        rawDataS0.clear();
        sum = 0;
    }
}

void optoforceCallback0(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    std::vector<float> tmp;
    tmp.push_back(msg->wrench.force.x);
    tmp.push_back(msg->wrench.force.y);
    tmp.push_back(msg->wrench.force.z);
    rawDataS0.push_back(tmp);
    isOnGround(rawDataS0);
}

void optoforceCallback1(const geometry_msgs::WrenchStamped::ConstPtr& msg) {
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
   // getPoints(1,optoforce_lst);

}

void optoforceCallback2(const geometry_msgs::WrenchStamped::ConstPtr& msg) {
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
   // getPoints(2,optoforce_lst);
}

void optoforceCallback3(const geometry_msgs::WrenchStamped::ConstPtr& msg) {
    float optoforce_lst [3];
    optoforce_lst[0] = msg->wrench.force.x;
    optoforce_lst[1] = msg->wrench.force.y;
    optoforce_lst[2] = msg->wrench.force.z;
    //getPoints(3,optoforce_lst);
}

int main(int argc, char **argv){
    int inputChar;
    ros::init(argc, argv, "exp1Gui");
    ros::NodeHandle n;
    ros::Subscriber optoforce0 = n.subscribe("optoforce_0", 1000, optoforceCallback0);
    //ros::Subscriber optoforce1 = n.subscribe("optoforce_1", 1000, optoforceCallback1);
    //ros::Subscriber optoforce2 = n.subscribe("optoforce_2", 1000, optoforceCallback2);
    //ros::Subscriber optoforce3 = n.subscribe("optoforce_3", 1000, optoforceCallback3);
    ros::AsyncSpinner spinner(2);
    spinner.start();
    do {
        printf("The algorithm is running...\n"
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
