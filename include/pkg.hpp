//
// Created by demons on 2022/7/2.
//

#ifndef NPU_COM_ROS2_NPU_COM_TCP_HPP
#define NPU_COM_ROS2_NPU_COM_TCP_HPP
struct Tcp_Pkg
{
    double posx=0;
    double posy=0;
    double posz=0;
    double pitch=0;
    double timelen=0;
    short closepwm=0;
    short betapwm=0;
    int magic_num=0;
};
#endif //NPU_COM_ROS2_NPU_COM_TCP_HPP
