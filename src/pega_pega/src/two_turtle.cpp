#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <geometry_msgs/msg/twist.hpp> 
#include  <turtlesim/msg/pose.hpp>
using std::placeholders::_1;

using namespace std::chrono_literals;

struct pose_{
  float x,y,yaw;
};

pose_ pose, pose1;

struct coordenadas{
  float x,y,yaw;
} coordenadas;

float conv(float a){           //função apicada na estrtegia de solução do problema de controle de orientação
      if(a >= -M_PI && a < 0) 
        return (a + 2*M_PI);
      return a;
}


class pega_pega : public rclcpp::Node
{
public:
  pega_pega()
  : Node("pega_pega")
  {
    auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
    
    this->declare_parameter<std::string>("name_turtle", "turtle");
   
    this->get_parameter("name_turtle", parameter_string_);

    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/" + parameter_string_ + "/cmd_vel", default_qos); //Publia comandos de velocidade

    subscription_ = this->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/" + parameter_string_ + "/pose", default_qos, std::bind(&pega_pega::topic_callback, this, _1));                 //recebe informações da posição x,y,yaw da tataruga 

    subscription1_ = this->create_subscription<turtlesim::msg::Pose>(
      "turtle1/turtle1/pose", default_qos, std::bind(&pega_pega::topic_callback1, this, _1));               //recebe informações da posição x,y,yaw da tataruga
    
    timer_ = this->create_wall_timer(
      10ms, std::bind(&pega_pega::timer_callback, this));  //Tarefa aplicada criada para executar o algoritmo a cada 10ms

  }

private:


  void timer_callback(){

    auto message = geometry_msgs::msg::Twist();

      /*Calculo do erro de posição x,y */
       err_lin = abs(trunc((pose1.x - pose.x)*1000)/1000);
       err_liny = abs(trunc((pose1.y - pose.y)*1000)/1000);
      /*Calculo do erro de posição x,y */


      if(sqrt(pow(err_lin,2) + pow(err_liny,2)) > 0.1){
         varx  = 2.5*err_lin ;   
         vary =  2.5*err_liny;   
         flg = 1;                              
         degrees = trunc(atan2(pose1.y - pose.y,pose1.x - pose.x)*1000)/1000;
      }else
       {degrees = pose1.yaw; flg = 0;}

      /*Resolve o Problema da descontinuidade (problema de orientação)*/
       a = conv(pose.yaw);
       b = conv(degrees);
       err_ang = trunc((b - a)*1000)/1000; //tart alvo , tarta  
       if(err_ang > 0 && abs(err_ang) > 0.7*2*M_PI ) err_ang = -1.0;
       if(err_ang < 0 && abs(err_ang) > 0.7*2*M_PI ) err_ang = +1.0;
              /* Calculo da ação de controle */
              I_ang = I_ang + err_ang*ki_ang*0.01;
              var_ang  = 2*err_ang + I_ang;
              /* Calculo da ação de controle */
      /*Resolve o Problema da descontinuidade */

      /*Saturação velocidade linear */
       if(varx > 3.5) varx  = 3.5;  
       if(vary > 3.5) vary  = 3.5; 
          message.linear.x = flg*sqrt(pow(varx,2) + pow(vary,2));
      /*Saturação velocidade linear */

      /*Saturação velocidade velocidade angular */
       if(var_ang > 2) var_ang  = 2; else if(var_ang < -2) var_ang  = -2;
       message.angular.z = var_ang;
      /*Saturação velocidade velocidade angular */
     
    publisher_->publish(message);
  }

  //COLETA INFORMAÇÕES DA POSIÇÃO DAS TARTRUGAS
  void topic_callback(const turtlesim::msg::Pose::SharedPtr msg) {
    pose.x = msg->x; pose.y = msg->y; pose.yaw = msg->theta;
  }

  void topic_callback1(const turtlesim::msg::Pose::SharedPtr msg) { //TARTARUGA NO CENTRO DA TELA
    pose1.x = msg->x; pose1.y = msg->y; pose1.yaw = msg->theta;
  }

  uint8_t flg = 1;
  float a = 0,b = 0,varx = 0,vary = 0,var_ang = 0;
  float err_lin = 0, err_liny = 0, err_ang = 0, ki_ang = 0.07, I_ang = 0,degrees = 0; 
  rclcpp::TimerBase::SharedPtr timer_;

  std::string parameter_string_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription1_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<pega_pega>());
  rclcpp::shutdown();
  return 0;
}
