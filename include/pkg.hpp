//
// Created by demons on 2022/7/2.
//

#ifndef NPU_COM_ROS2_NPU_COM_TCP_HPP
#define NPU_COM_ROS2_NPU_COM_TCP_HPP
struct Tcp_Pkg
{
    short pwm1=327;
    short pwm2=998;
    short pwm3=411;
    short pwm4=414;
    short timelen1=3000;
    short closepwm=200;
    short betapwm=500;
    short timelen2=3000;
    int magic_num=997;
};
#endif //NPU_COM_ROS2_NPU_COM_TCP_HPP
