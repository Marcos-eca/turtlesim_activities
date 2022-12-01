#include <chrono>
#include <memory>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include "rclcpp/rclcpp.hpp"
#include  <turtlesim/msg/pose.hpp>
#include <geometry_msgs/msg/twist.hpp> 

#define RAIO 1.5
#define LIM_VEL 2.5
#define LIM_INF 2.0
#define LIM_SUP 9.0
#define CENTER 5.444

using std::placeholders::_1;
using namespace std::chrono_literals;

struct pose_{
  float x,y,yaw;
};

pose_ pose, pose1;

float conv(float);
float P_ctrl(float,float);
float satura(float,float);

class collision_avoidance : public rclcpp::Node{
public:
  collision_avoidance() : Node("collision_avoidance")
  {
      auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
      publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/turtle/cmd_vel", default_qos); //Publica comandos de velocidade
      publisher1_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/turtle1/cmd_vel", default_qos); //Publica comandos de velocidade
      subscription_ = this->create_subscription<turtlesim::msg::Pose>(
      "turtle1/turtle/pose", default_qos, std::bind(&collision_avoidance::topic_callback, this, _1));                 //recebe informações da posição x,y,yaw da tataruga 
      subscription1_ = this->create_subscription<turtlesim::msg::Pose>(
      "turtle1/turtle1/pose", default_qos, std::bind(&collision_avoidance::topic_callback1, this, _1));               //recebe informações da posição x,y,yaw da tataruga
      timer_ = this->create_wall_timer(
         100ms, std::bind(&collision_avoidance::timer_callback, this));  //Tarefa aplicada criada para executar o algoritmo a cada 10ms
      timer1_ = this->create_wall_timer(
         100ms, std::bind(&collision_avoidance::timer_callback1, this));  //Tarefa aplicada criada para executar o algoritmo a cada 10ms
  }
private:

  void timer_callback();
  void timer_callback1();

  //POSIÇÕES PUBLICADAS PELAS TARTARUGAS
  void topic_callback(const turtlesim::msg::Pose::SharedPtr msg);
  void topic_callback1(const turtlesim::msg::Pose::SharedPtr msg);
  //POSIÇÕES PUBLICADAS PELAS TARTARUGAS

  //ESTRUTURAS DE DADOS
  float   varx = 0,vary = 0,var_ang = 0, cont = 0, cont1 = 0;
  float   varx1 = 0,vary1 = 0,var_ang1 = 0, px, py, px1, py1, offset_ang = 0, offset_ang1 = 0;
  uint8_t ang = 0,ang1 = 0, flg = 1, flg1 = 1, flg_offset = 0, flg_offset1 = 0; 
  
  rclcpp::TimerBase::SharedPtr timer_, timer1_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_, publisher1_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_,subscription1_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<collision_avoidance>());
  rclcpp::shutdown();
  return 0;
}


void collision_avoidance::timer_callback(){
     auto message = geometry_msgs::msg::Twist();

     /*Calculo do erro de posição x,y */
     float  err_lin = abs(trunc((pose1.x - pose.x)*1000)/1000);
     float  err_liny = abs(trunc((pose1.y - pose.y)*1000)/1000);
     /*Calculo do erro de posição x,y */

     float raio = sqrt(pow(err_lin,2) + pow(err_liny,2));  
     ang1 = (abs(pose.x) <= LIM_INF) + (abs(pose.y) <= LIM_INF) + (abs(pose.x) >= LIM_SUP) + (abs(pose.y) >= LIM_SUP); 

    if(raio <= RAIO){ 

          message.linear.x = LIM_VEL;
          
          if(!flg_offset1){ offset_ang1 = pose.yaw;  flg_offset1 = 1;}
          message.angular.z = LIM_VEL*P_ctrl(pose.yaw, M_PI + offset_ang1);


          publisher_->publish(message);
          
        } else if(ang1){
         
      /*Saturação velocidade linear */
        varx1 = LIM_VEL*err_lin ;  vary1 = LIM_VEL*err_liny;   
        message.linear.x = sqrt(pow(satura(varx1,2),LIM_VEL) + pow(satura(vary1,2),1.5*LIM_VEL));
      /*Saturação velocidade linear */

          var_ang1  = 6.5*P_ctrl(pose.yaw,atan2(CENTER - pose.y,CENTER - pose.x));
          message.angular.z =  satura(var_ang1,7);  
          publisher_->publish(message); 
              flg_offset1 = 0;
     } else if(!ang1){
              srand(time(0));
              int i = rand();
              message.linear.x = LIM_VEL;
              message.angular.z = 2.5*sin(100*atan(i%10));
              publisher_->publish(message);
              flg_offset1 = 0;
           }
}


  void collision_avoidance::timer_callback1(){ //controle teleoperado
    auto message = geometry_msgs::msg::Twist();
    /*Calculo do erro de posição x,y */
    float  err_lin = abs(trunc((pose1.x - pose.x)*1000)/1000);
    float  err_liny = abs(trunc((pose1.y - pose.y)*1000)/1000);
    float raio = sqrt(pow(err_lin,2) + pow(err_liny,2));
    /*Calculo do erro de posição x,y */

     ang = (abs(pose1.x) <= LIM_INF) + (abs(pose1.y) <= LIM_INF) + (abs(pose1.x) >= LIM_SUP) + (abs(pose1.y) >= LIM_SUP); 

      if((raio < RAIO*(1-0.65))) flg = 0;

      if(raio <= RAIO){ 

         message.linear.x = 2.0;

          flg_offset = flg1;
          if(!flg_offset){ offset_ang = pose1.yaw;  flg_offset = 1;}
          message.angular.z = 3.5*P_ctrl(pose1.yaw, M_PI + offset_ang);


         publisher1_->publish(message);


         if(!flg1){
           if(!(raio  <= RAIO*(1-0.65))) cont1++;
           if(!flg){cont ++; flg = 1;}

            flg1 = 1;
            system("clear");
            printf("N de colisões entre tartarugas:\n Colisões direta: %f \n Colisões evitada: %f\n Percentual de colisões [direta/evitada]: %f% \n" ,cont,cont1, 100*cont/cont1);
         }

      } else if(ang){

      /*Saturação velocidade linear */
        varx  = LIM_VEL*err_lin ;  vary =  LIM_VEL*err_liny;   
        message.linear.x = sqrt(pow(satura(varx,2),LIM_VEL) + pow(satura(vary,2),1.5*LIM_VEL));
      /*Saturação velocidade linear */

          var_ang  = 6.5*P_ctrl(pose1.yaw,atan2(CENTER - pose1.y,CENTER - pose1.x));          
          message.angular.z =  satura(var_ang,7);

          publisher1_->publish(message); 
          flg1 = 0;
     }else if(!ang){
             srand(time(0));
             int i = rand();
             message.linear.x = 0.75*LIM_VEL;
             message.angular.z = 3.5*cos(200*atan(i%10));
             publisher1_->publish(message);
             flg1 = 0;
     }
 
  }


  void collision_avoidance::topic_callback(const turtlesim::msg::Pose::SharedPtr msg) {pose.x = msg->x; pose.y = msg->y; pose.yaw = msg->theta;}
  void collision_avoidance::topic_callback1(const turtlesim::msg::Pose::SharedPtr msg) {pose1.x = msg->x; pose1.y = msg->y; pose1.yaw = msg->theta;}

float conv(float a){           
      if(a >= -M_PI && a < 0) 
        return (a + 2*M_PI);
      return a;
}
float P_ctrl(float a1, float b1){
     a1 = conv(a1); b1 = conv(b1);
     float  err_ang = trunc((b1 - a1)*1000)/1000;
     if(err_ang > 0 && abs(err_ang) > 0.6*2*M_PI ) err_ang = -0.4;
     if(err_ang < 0 && abs(err_ang) > 0.6*2*M_PI ) err_ang = +0.4;
  return err_ang; 
}

float satura(float val,float sat){
  if(val > sat) return sat; else if(val < -sat) return  -sat;
  return val;
}
