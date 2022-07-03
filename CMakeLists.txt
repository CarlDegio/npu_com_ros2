cmake_minimum_required(VERSION 3.5)
project(npu_com_ros2)


# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

include_directories(include)

add_executable(topic2tcp src/topic_to_tcp.cpp include/pkg.hpp)
#add_executable(tcp2topic src/tcp_to_topic.cpp include/pkg.hpp)
ament_target_dependencies(topic2tcp rclcpp std_msgs)
#ament_target_dependencies(tcp2topic rclcpp std_msgs)

install(TARGETS
  topic2tcp
  DESTINATION lib/${PROJECT_NAME})

ament_package()
