#include <chrono>
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
#include "pkg.hpp"
using std::placeholders::_1;

using namespace std::chrono_literals;

//not finish because of unuse

bool need_exit= false;

class Tcp2Ros : public rclcpp::Node
{
public:
    Tcp2Ros()
            : Node("minimal_publisher")
    {
        //publisher_ = this->create_publisher<npu_msg::msg::Armpkg>("topic", 10);
        //timer_ = this->create_wall_timer(500ms, std::bind(&Ros_Tcp::timer_callback, this));

        //TCP server
        server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        server_addr.sin_family = AF_INET;  //使用IPv4地址
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
        server_addr.sin_port = htons(10000);  //端口
        bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

        listen(server_sock, 20);

        socklen_t clnt_addr_size = sizeof(client_addr);
        //client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &clnt_addr_size);
        // wei shen m shi zu se
    }

    void work_loop()
    {
        RCLCPP_INFO(this->get_logger(), "work_loop_in");
        while(rclcpp::ok() && !need_exit)
        {
            RCLCPP_INFO(this->get_logger(), "work_loop?????");
            rclcpp::spin_some(shared_from_this());
            //int strLen = read(client_sock, buffer, sizeof(buffer));

        }
    }

    ~Tcp2Ros() override
    {
        close(server_sock);
        close(client_sock);
    }

private:
    void timer_callback()
    {
        RCLCPP_INFO(this->get_logger(), "Publishing: '%lf'");
    }
    rclcpp::TimerBase::SharedPtr timer_;

    int server_sock;
    int client_sock;
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    Tcp_Pkg tcp_pkg;
};

void ExitHandler(int sig)
{
    (void)sig;
    need_exit = true;
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    //rclcpp::spin(std::make_shared<Ros_Tcp>());
    auto Ros_Tcp_Node = std::make_shared<Tcp2Ros>();
    signal(SIGINT,ExitHandler);
    Ros_Tcp_Node->work_loop();
    rclcpp::shutdown();
    return 0;
}

