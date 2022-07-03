#include <functional>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
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
        pose_subscription_=this->create_subscription<std_msgs::msg::Float64MultiArray>(
                "/arm_close_gamma", 10, std::bind(&Ros2Tcp::pos_callback, this, _1));
        PWM_subscription_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
                "/close_gamma", 10, std::bind(&Ros2Tcp::PWM_callback, this, _1));

        tcp_pkg.magic_num=997;//magic num to check

        //TCP server
        server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        server_addr.sin_family = AF_INET;  //使用IPv4地址
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
        server_addr.sin_port = htons(10000);  //端口
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
    void pos_callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg) {
        //RCLCPP_INFO(this->get_logger(), "sub pos");
        tcp_pkg.posx=msg->data[0];
        tcp_pkg.posy=msg->data[1];
        tcp_pkg.posz=msg->data[2];
        tcp_pkg.pitch=msg->data[3];
        tcp_pkg.timelen=msg->data[4];
    }
    void PWM_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(), "sub pwm");
        tcp_pkg.closepwm=msg->data[0];
        tcp_pkg.betapwm=msg->data[1];
        write(client_sock,(char *)&tcp_pkg,sizeof(Tcp_Pkg));//this faster, so pub heres
    }
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr pose_subscription_;
    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr PWM_subscription_;

    int server_sock;
    int client_sock;
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    Tcp_Pkg tcp_pkg;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Ros2Tcp>());
    rclcpp::shutdown();
    return 0;
}