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



//Disse parameterne kan endres
int threshold_size = 15;
int longestStability = 8;
int total_sensors = 4;

int extract_data = 0;
int allNegative = 0;
int fillFirstData = 0;
float sum = 0;


void MySleep(unsigned long p_uMillisecs){
    usleep(p_uMillisecs * 1000);
}



float getMax(){
    float max = opto_data_raw[0][0][2];
    int i;
    for (i = 0; i < threshold_size; i++){
        if(opto_data_raw[0][i][2] > max){
            max = opto_data_raw[0][i][2];
        }
    }

    //std::cout << "Max" << max << "\n";
    /* if(max > 3){
         for (i = 0; i < threshold_size; i++){
 //          std::cout << "Z: " << opto_data_raw[0][i][2] << "\n";
         }
     } */
    return max;
}

void clearVectorArray(){
    int i;
    for(i=0;i<4;i++){
        opto_data_raw[i].clear();
    }
}

void clearVectorArray2(){
    int i;
    for(i=0;i<4;i++){
        extracted_data[i].clear();
    }
}

void emptyAllData(std::vector<std::vector<float>> lst[],int size){
    int i;
    for(i=0;i<size;i++){
        lst[i].clear();
    }
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
    float prev = opto_data_raw[0][i][2];
    int stabilityCount = 0;
    int j;

    for (j=i+1; j<threshold_size; j++) {
        float current = opto_data_raw[0][j][2];
        if (fabs(prev - current) < boundary) {
            stabilityCount++;
        }
        else{
            return stabilityCount;
        }
    }
    return stabilityCount;
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

void fillTheFirstElements(int startpos){
    std::vector<float> tmp;
    int i;
    for (i=startpos; i<threshold_size; i++) {
        tmp.push_back(opto_data_raw[0][i][0]);
        tmp.push_back(opto_data_raw[0][i][1]);
        tmp.push_back(opto_data_raw[0][i][2]);
    }
    extracted_data[0].push_back(tmp);
}
void savePoints(){
    std::cout << "Save data\n";
    std::vector<float> tmp;
    int i;
    for (i=0; i<threshold_size; i++) {
        tmp.push_back(opto_data_raw[0][i][0]);
        tmp.push_back(opto_data_raw[0][i][1]);
        tmp.push_back(opto_data_raw[0][i][2]);
    }
    extracted_data[0].push_back(tmp);
}
//x = 0, y = 1, z = 2
void isOnGround(){
    if(opto_data_raw[0].size() == threshold_size){

        int i;
        float prev = opto_data_raw[0][0][2];
        int saveData = 0;
        int longestSequence = 0;
        for (i=1; i<threshold_size; i++) {
            float current = opto_data_raw[0][i][2];

            if (bothNegative(prev, current)) {
                if ((longestSequence=checkStability(i)) > longestStability) {
                    saveData = 1;
                    break;
                }
                else if(i > longestStability){
                    break;
                }
            }
        }
        float max = getMax();
        std::cout << "Innsamling av data" << "\n";
        std::cout << "Average: " << (sum/threshold_size) << "\n";
        std::cout << "Max: " << max << "\n";
        std::cout << "Sequence: " << longestSequence << "\n";

        if(saveData){

            for (i = 0; i < threshold_size; i++){
                //std::cout << "Z: " << opto_data_raw[0][i][2] << "\n";
            }

            if(fillFirstData || extract_data){
                extract_data = 0;
                fillFirstData = 0;
                std::cout << "Extracted " << "\n";
                for (i = 0; i < extracted_data[0].size(); i++){
                    std::cout << "Z: " << extracted_data[0][i][2] << "\n";
                }
                clearVectorArray2();
            }
            else if(int startpos = getStartPos() != -1){
                fillTheFirstElements(startpos);
                fillFirstData = 1;
            }
            if(!extract_data){
                extract_data = 1;
            }
            std::cout << "\n";
        }
        else if(extract_data){
            savePoints();
        }
        emptyAllData(opto_data_raw,total_sensors);
        //clearVectorArray();
        sum = 0;
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
