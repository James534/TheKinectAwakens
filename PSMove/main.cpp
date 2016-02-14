#include <iostream>
#include <opencv/cv.hpp>
#include <unistd.h>

#include "psmoveapi/psmove.h"
#include "psmoveapi/psmove_fusion.h"
#include "psmoveapi/psmove_tracker.h"

#include "irrlicht/irrlicht.h"

#include "ShapeFilter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <pthread.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace cv;

pthread_t* clientThread;
pthread_mutex_t* clientLock;

void error(std::string s){
    std::cout<< s << std::endl;
}

void *run(void* stuff)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    portno = 1235;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("100.64.199.18");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    std::string something;

    pthread_mutex_lock(clientLock);
    std::string* txt = (std::string*) stuff;
    pthread_mutex_unlock(clientLock);
    std::string msg;
    while (true){
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);

        //pthread_mutex_lock(clientLock);
        std::cout << "sadf\n";
        something = *txt;
        n = write(sockfd,buffer,15);
        //thread_mutex_unlock(clientLock);

        if (n < 0)
             error("ERROR writing to socket");
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n < 0)
             error("ERROR reading from socket");

        printf("%s\n",buffer);
    }
    close(sockfd);
    pthread_exit(NULL);
}


void psMove(){

    PSMove *move;
    enum PSMove_Connection_Type ctype;
    int i;

    i = psmove_count_connected();
    printf("Connected controllers: %d\n", i);

    move = psmove_connect();

    if (move == NULL) {
        printf("Could not connect to default Move controller.\n"
               "Please connect one via USB or Bluetooth.\n");
        exit(1);
    }

    char *serial = psmove_get_serial(move);
    printf("Serial: %s\n", serial);
    free(serial);

    ctype = psmove_connection_type(move);
    switch (ctype) {
        case Conn_USB:
            printf("Connected via USB.\n");
            break;
        case Conn_Bluetooth:
            printf("Connected via Bluetooth.\n");
            break;
        case Conn_Unknown:
            printf("Unknown connection type.\n");
            break;
    }
/*
    for (i=0; i<10; i++) {
        psmove_set_leds(move, 0, 255*(i%3==0), 0);
        psmove_set_rumble(move, 255*(i%2));
        psmove_update_leds(move);
        usleep(10000*(i%10));
    }

    for (i=250; i>=0; i-=5) {
        psmove_set_leds(move, i, i, 0);
        psmove_set_rumble(move, 0);
        psmove_update_leds(move);
    }*/

    /* Enable rate limiting for LED updates */
    psmove_set_rate_limiting(move, PSMove_True);
    psmove_enable_orientation(move, PSMove_True);

    psmove_reset_orientation(move);
    std::cout<<psmove_has_orientation(move)<<std::endl;

    psmove_set_leds(move, 0, 0, 0);
    psmove_set_rumble(move, 0);
    psmove_update_leds(move);

    float a = 0, b = 0, c = 0, d = 0;

    while (ctype != Conn_USB && !(psmove_get_buttons(move) & Btn_PS)) {
        int res = psmove_poll(move);
        if (res) {
            if (psmove_get_buttons(move) & Btn_TRIANGLE) {
                printf("Triangle pressed, with trigger value: %d\n",
                        psmove_get_trigger(move));
                psmove_set_rumble(move, psmove_get_trigger(move));
            } else {
                psmove_set_rumble(move, 0x00);
            }

            //psmove_set_leds(move, 0, 0, psmove_get_trigger(move));
            psmove_set_leds(move, 255, 0, 0);
            psmove_set_rumble(move, 5);

            int x, y, z;
            psmove_get_accelerometer(move, &x, &y, &z);
            printf("accel: %5d %5d %5d\n", x, y, z);
            psmove_get_gyroscope(move, &x, &y, &z);
            printf("gyro: %5d %5d %5d\n", x, y, z);
            psmove_get_magnetometer(move, &x, &y, &z);
            printf("magnetometer: %5d %5d %5d\n", x, y, z);
            printf("buttons: %x\n", psmove_get_buttons(move));
            psmove_get_orientation(move, &a, &b, &c, &d);
            printf("%5f %5f %5f %5f\n", a, b, c, d);

            int battery = psmove_get_battery(move);

            if (battery == Batt_CHARGING) {
                printf("battery charging\n");
            } else if (battery == Batt_CHARGING_DONE) {
                printf("battery fully charged (on charger)\n");
            } else if (battery >= Batt_MIN && battery <= Batt_MAX) {
                //printf("battery level: %d / %d\n", battery, Batt_MAX);
            } else {
                printf("battery level: unknown (%x)\n", battery);
            }

            //printf("temperature: %d\n", psmove_get_temperature(move));

            int res = psmove_update_leds(move);/*
            if (res == 1)
                printf("Updated\n");
            else if (res == 2)
                printf("Dont need to update\n");
            else
                printf("Nothing\n");*/
        }
        //sleep(1);
    }

    psmove_disconnect(move);

}

Mat rgbFilter(Mat src, int r, int g, int b){
    Mat filtered;
    inRange(src, Scalar(0,0,0), Scalar(b, g, r), filtered);
    return filtered;
}

cv::Mat HSVFilter(cv::Mat mat, int lowH, int highH, int lowS, int highS, int lowV, int highV){
    //cv::Mat* mat = data->getImg();
    cv::Mat imgHSV;
    //cv::Mat imgThresh = cv::Mat(mat.clone());
    cv::Mat imgThresh = mat.clone();

    cv::cvtColor(mat, imgHSV, cv::COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

    cv::inRange(imgHSV, cv::Scalar(lowH, lowS, lowV),
            cv::Scalar(highH, highS, highV), imgThresh); //Threshold the image

    //morphological opening (remove small objects from the foreground)
    cv::erode(imgThresh, imgThresh, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
    cv::dilate(imgThresh, imgThresh, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

    //morphological closing (fill small holes in the foreground)
    cv::dilate(imgThresh, imgThresh, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
    cv::erode(imgThresh, imgThresh, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

    return imgThresh;
}

void OCV(std::string* str){

    ///PSMOVE SHIT
    PSMove *move;
    enum PSMove_Connection_Type ctype;
    int i;

    i = psmove_count_connected();
    printf("Connected controllers: %d\n", i);

    move = psmove_connect();

    if (move == NULL) {
        printf("Could not connect to default Move controller.\n"
               "Please connect one via USB or Bluetooth.\n");
        exit(1);
    }

    char *serial = psmove_get_serial(move);
    printf("Serial: %s\n", serial);
    free(serial);

    ctype = psmove_connection_type(move);
    switch (ctype) {
        case Conn_USB:
            printf("Connected via USB.\n");
            break;
        case Conn_Bluetooth:
            printf("Connected via Bluetooth.\n");
            break;
        case Conn_Unknown:
            printf("Unknown connection type.\n");
            break;
    }
    /* Enable rate limiting for LED updates */
    psmove_set_rate_limiting(move, PSMove_True);
    psmove_enable_orientation(move, PSMove_True);
    psmove_reset_orientation(move);

    psmove_set_leds(move, 0, 0, 0);
    psmove_set_rumble(move, 0);
    psmove_update_leds(move);
    namedWindow("hello world");
    std::cout<<psmove_has_orientation(move)<<std::endl;

    float a = 0, b = 0, c = 0, d = 5;

    ///SERVER SHIT----------------------------------------

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    portno = 1234;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("100.64.199.18");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    std::string msg;

    while (ctype != Conn_USB && !(psmove_get_buttons(move) & Btn_PS)) {
        int res = psmove_poll(move);
        if (res) {
            if (psmove_get_buttons(move) & Btn_TRIANGLE) {
                printf("Triangle pressed, with trigger value: %d\n",
                        psmove_get_trigger(move));
                psmove_set_rumble(move, psmove_get_trigger(move));
            } else {
                psmove_set_rumble(move, 0x00);
            }

            //pthread_mutex_lock(clientLock);
            *str = "hello world2\n";
            //pthread_mutex_unlock(clientLock);
            //psmove_set_leds(move, 0, 0, psmove_get_trigger(move));
            psmove_set_leds(move, 0, 0, 255);
            psmove_set_rumble(move, 5);

            int res = psmove_update_leds(move);
            //if (res == 1)
            //printf("Updated\n");
            psmove_get_orientation(move, &a, &b, &c, &d);
            msg = std::to_string(a) + " " + std::to_string(a) + " " + std::to_string(a) + " " + std::to_string(a);

            n = write(sockfd,msg.c_str(),msg.length());
            if (n < 0)
                 error("ERROR writing to socket");
            std::cout<< msg << std::endl;
            //bzero(buffer,256);
            /*
            n = read(sockfd,buffer,255);
            if (n < 0)
                 error("ERROR reading from socket");

            printf("%s\n",buffer);*/

            if (waitKey(1) == 27)break;
        }
    }

    psmove_disconnect(move);
    close(sockfd);
}

void svr(){

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    portno = 1234;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("10.21.45.161");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");


    ///PSMOVE SHIT---------------------------------------------------------------------
    PSMove *move;
    enum PSMove_Connection_Type ctype;
    int i;

    i = psmove_count_connected();
    printf("Connected controllers: %d\n", i);

    move = psmove_connect();

    if (move == NULL) {
        printf("Could not connect to default Move controller.\n"
               "Please connect one via USB or Bluetooth.\n");
        exit(1);
    }

    char *serial = psmove_get_serial(move);
    printf("Serial: %s\n", serial);
    free(serial);

    ctype = psmove_connection_type(move);
    switch (ctype) {
        case Conn_USB:
            printf("Connected via USB.\n");
            break;
        case Conn_Bluetooth:
            printf("Connected via Bluetooth.\n");
            break;
        case Conn_Unknown:
            printf("Unknown connection type.\n");
            break;
    }
    /* Enable rate limiting for LED updates */
    psmove_set_rate_limiting(move, PSMove_True);
    psmove_enable_orientation(move, PSMove_True);
    psmove_reset_orientation(move);

    psmove_set_leds(move, 0, 0, 0);
    psmove_set_rumble(move, 0);
    psmove_update_leds(move);
    //namedWindow("hello world");
    std::cout<<psmove_has_orientation(move)<<std::endl;

    float a = 0, b = 0, c = 0, d = 5;


    ///LOOP-----------------------------------------------------
    std::string msg = "hello world\n";
    //psmove_get_orientation(move, &a, &b, &c, &d);
    //msg = std::to_string(a) + " " + std::to_string(a) + " " + std::to_string(a) + " " + std::to_string(a);
    msg = "hello world\n";
    while (ctype != Conn_USB && !(psmove_get_buttons(move) & Btn_PS)) {

        int res = psmove_poll(move);

        if (psmove_get_buttons(move) & Btn_TRIANGLE) {
            psmove_reset_orientation(move);
        }

        psmove_set_leds(move, 0, 0, 255);
        psmove_set_rumble(move, 15);
        res = psmove_update_leds(move);

        psmove_get_orientation(move, &a, &b, &c, &d);
        msg = std::to_string(a) + " " + std::to_string(b) + " " + std::to_string(c) + " " + std::to_string(d) +"\n";
        //msg = "hello world\n";

        bzero(buffer,256);
        //fgets(buffer,255,stdin);
        strcpy(buffer, msg.c_str());
        //printf(buffer);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
             error("ERROR writing to socket");


        //sleep(1);
        //if (waitKey(1)==27)break;
    }

    psmove_disconnect(move);
    close(sockfd);
}

struct threadData{
    bool* keepRun;
    std::string* msg;
};

void *threadClient(void* td){

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    portno = 1234;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("10.19.190.253");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    threadData *tData = (threadData*) td;
    std::string *message = (std::string*) tData->msg;
    bool *keepRun = (bool*) tData->keepRun;
    while (*keepRun){
        bzero(buffer,256);
        //fgets(buffer,255,stdin);
        strcpy(buffer, message->c_str());
        //printf(buffer);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
             error("ERROR writing to socket");


        //sleep(1);
        //if (waitKey(1)==27)break;
    }
    close(sockfd);
    pthread_exit(NULL);
}
void psm(void* msg, void* kr){
    ///PSMOVE SHIT---------------------------------------------------------------------
    PSMove *move;
    enum PSMove_Connection_Type ctype;
    int i;

    i = psmove_count_connected();
    printf("Connected controllers: %d\n", i);

    move = psmove_connect();

    if (move == NULL) {
        printf("Could not connect to default Move controller.\n"
               "Please connect one via USB or Bluetooth.\n");
        exit(1);
    }

    std::string *message = (std::string*) msg;
    bool* keepRun = (bool*) kr;

    char *serial = psmove_get_serial(move);
    printf("Serial: %s\n", serial);
    free(serial);

    ctype = psmove_connection_type(move);
    switch (ctype) {
        case Conn_USB:
            printf("Connected via USB.\n");
            break;
        case Conn_Bluetooth:
            printf("Connected via Bluetooth.\n");
            break;
        case Conn_Unknown:
            printf("Unknown connection type.\n");
            break;
    }
    /* Enable rate limiting for LED updates */
    psmove_set_rate_limiting(move, PSMove_True);
    psmove_enable_orientation(move, PSMove_True);
    psmove_reset_orientation(move);
    std::cout<<psmove_has_orientation(move)<<std::endl;

    psmove_set_leds(move, 0, 0, 0);
    psmove_set_rumble(move, 0);
    psmove_update_leds(move);

    float a = 0, b = 0, c = 0, d = 5;

    ///LOOP-----------------------------------------------------
    while (ctype != Conn_USB && !(psmove_get_buttons(move) & Btn_PS)) {
        int res = psmove_poll(move);
        if (res) {
            if (psmove_get_buttons(move) & Btn_TRIANGLE) {
                psmove_reset_orientation(move);
                //psmove_set_rumble(move, psmove_get_trigger(move));
            } else {
                psmove_set_rumble(move, 0x00);
            }

            //psmove_set_leds(move, 0, 0, psmove_get_trigger(move));
            psmove_set_leds(move, 255, 0, 255);
            psmove_set_rumble(move, 5);

            //int x, y, z;
            //psmove_get_accelerometer(move, &x, &y, &z);
            //printf("accel: %5d %5d %5d\n", x, y, z);
            //psmove_get_gyroscope(move, &x, &y, &z);
            //printf("gyro: %5d %5d %5d\n", x, y, z);
            //psmove_get_magnetometer(move, &x, &y, &z);
            //printf("magnetometer: %5d %5d %5d\n", x, y, z);
            //printf("buttons: %x\n", psmove_get_buttons(move));
            psmove_get_orientation(move, &a, &b, &c, &d);
            *message = std::to_string(a) + " " + std::to_string(b) + " " + std::to_string(c) + " " + std::to_string(d) +"\n";
            //printf("%5f %5f %5f %5f\n", a, b, c, d);

            int res = psmove_update_leds(move);
        }
    }
    psmove_disconnect(move);
    *keepRun = false;
}

int main(int argc, char* argv[])
{
    std::string* str = nullptr;
    str = new std::string("hello world");
    bool* clientRun;
    clientRun = new bool(true);

    //clientLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    //clientThread = (pthread_t*)malloc(sizeof(pthread_t));
    //pthread_mutex_init(clientLock, NULL);
    //pthread_create(clientThread, NULL, run, (void*)str);

    //psMove();
    //return 0;
    //OCV(str);
    //svr();

    pthread_t thread;
    struct threadData td;
    td.keepRun = clientRun;
    td.msg = str;
    int rc = pthread_create(&thread, NULL,
                        threadClient, (void *)&td);
    //threadClient(str, clientRun);
    if (rc)
        std::cout<<"Couldnt create thread"<<std::endl;
    psm(str, clientRun);
    pthread_exit(NULL);
    //pthread_join(*clientThread, NULL);
    return 0;
}
