#include <iostream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "ros/ros.h"
#include <unistd.h>
#include "geometry_msgs/WrenchStamped.h"
#include <stdlib.h>

std::vector<std::vector<float>> rawDataS0;
std::vector<std::vector<float>> stabilityLst;
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

int isStable(std::vector<std::vector<float>> stabArr){
    float boundary = 0.03;

    int stabilityCount = 0;
    int pos1 = stabArr.size()-2;
    int pos2 = stabArr.size()-1;

    if(stabArr.size() < 2){
        std::cout << "Noe er rarrt\n";
    }

    int i;
    for(i=0;i<3;i++){
        float current = stabArr.at(pos1).at(i);
        float next = stabArr.at(pos2).at(i);

        if (fabs(next - current) > boundary) {
            return 0;
        }
    }
    return 1;
}
int fyll = 0;
int startFyll = 0;

void print_s(){
    int i;

    std::cout << "-----------Start--------------";

    std::cout << interestDataS0.size() <<"\n";
    interestDataS0.resize(interestDataS0.size()-stabilityLst.size());

    for(i=0;i<interestDataS0.size();i++){
        std::cout << "x: "<<interestDataS0.at(i).at(0);
        std::cout << " y: "<<interestDataS0.at(i).at(1);
        std::cout << " z: "<<interestDataS0.at(i).at(2) << "\n";
    }
    std::cout << "-----------End--------------";
    interestDataS0.clear();
}
void checkStability3(){
    if(stabilityLst.size() > 2){
        if(fyll == 0 && isStable(stabilityLst)){
            fyll = 1;
        }
        else if(fyll) {
            int stabThres = 25;
            if (!isStable(stabilityLst)) {

                if (stabilityLst.size() > stabThres) {
                    startFyll = 1;
                    std::cout << "Gjore klart til å fylle\n";
                    int lastElement = stabilityLst.size()-1;
                    int secondLast = stabilityLst.size()-2;
                    interestDataS0.push_back(stabilityLst.at(secondLast));
                    interestDataS0.push_back(stabilityLst.at(lastElement));
                    //Vet at det skal fylles
                }
                fyll = 0;
                stabilityLst.clear();
            }else if( startFyll && stabilityLst.size() >= stabThres  ){
                startFyll = 0;
                print_s();
            }
        }
        else{
            fyll = 0;
            stabilityLst.clear();
        }
    }
}



void optoforceCallback0(const geometry_msgs::WrenchStamped::ConstPtr& msg)
{
    std::vector<float> tmp;
    tmp.push_back(msg->wrench.force.x);
    tmp.push_back(msg->wrench.force.y);
    tmp.push_back(msg->wrench.force.z);
    rawDataS0.push_back(tmp);

    if(startFyll){
        interestDataS0.push_back(tmp);
    }
    stabilityLst.push_back(tmp);

    checkStability3();
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