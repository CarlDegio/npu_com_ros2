#include <functional>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
//#include "std_msgs/msg/float64_multi_array.hpp"
//#include "npu_msg/msg/armpkg.hpp"
//#include "npu_msg/msg/endset.hpp"
#include "pkg.hpp"
using std::placeholders::_1;


class Ros2Tcp : public rclcpp::Node
{
public:
    Ros2Tcp()
            : Node("minimal_publisher")
    {
        this->declare_parameter<std::string>("raspberry_ip", "192.168.149.54");
        this->declare_parameter<ushort>("ip_port", 10000);

        this->get_parameter("raspberry_ip",raspberry_ip);
        this->get_parameter("ip_port",ip_port);
        pose_subscription_=this->create_subscription<std_msgs::msg::Int16MultiArray>(
                "/arm_pos_time", 10, std::bind(&Ros2Tcp::pos_callback, this, _1));
        PWM_subscription_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
                "/arm_close_gamma", 10, std::bind(&Ros2Tcp::PWM_callback, this, _1));

        tcp_pkg.magic_num=997;//magic num to check

        //TCP server
        server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        server_addr.sin_family = AF_INET;  //使用IPv4地址
        server_addr.sin_addr.s_addr = inet_addr(raspberry_ip.c_str());  //具体的IP地址
        server_addr.sin_port = htons(ip_port);  //端口
        bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

        listen(server_sock, 20);

        socklen_t clnt_addr_size = sizeof(client_addr);
        RCLCPP_INFO(this->get_logger(), "waitting tcp client...");
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &clnt_addr_size);
        RCLCPP_INFO(this->get_logger(), "get tcp client successful");

    }

    ~Ros2Tcp() override
    {
        close(server_sock);
        close(client_sock);
    }

private:
    void pos_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg) {
        //RCLCPP_INFO(this->get_logger(), "sub pos");
        tcp_pkg.pwm1=msg->data[0];
        tcp_pkg.pwm2=msg->data[1];
        tcp_pkg.pwm3=msg->data[2];
        tcp_pkg.pwm4=msg->data[3];
        tcp_pkg.timelen1=msg->data[4];
    }
    void PWM_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg) {
        //RCLCPP_INFO(this->get_logger(), "sub pwm");
        tcp_pkg.closepwm=msg->data[0];
        tcp_pkg.betapwm=msg->data[1];
        tcp_pkg.timelen2=msg->data[2];
        write(client_sock,(char *)&tcp_pkg,sizeof(Tcp_Pkg));//this faster, so pub heres
    }
    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr pose_subscription_;
    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr PWM_subscription_;

    int server_sock;
    int client_sock;
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    Tcp_Pkg tcp_pkg;

    std::string raspberry_ip;
    ushort ip_port;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Ros2Tcp>());
    rclcpp::shutdown();
    return 0;
}