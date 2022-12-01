#include <unistd.h>
#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"


void pgando(const std_msgs::msg::String::SharedPtr msg){
      std::cout<<msg->data<<std::endl;
}

int main(int argc,char **argv){

  rclcpp::init(argc,argv);
  auto node = rclcpp::Node::make_shared("teleop1");
  auto subs = node->create_subscription<std_msgs::msg::String>("pague_o_aluguel",10,pgando);
  
  rclcpp::spin(node);
  rclcpp::shutdown();

return 0;
}