#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"

//INICIO- Definição das teclas

#define KEY_W 0x77 
#define KEY_X 0x78 
#define KEY_A 0x61
#define KEY_D 0x64

#define KEY_Q 0x71
#define KEY_Z 0x7A
#define KEY_E 0x65
#define KEY_C 0x63

#define KEY_S 0x73

#define KEY_1 0x31 
#define KEY_2 0x32
#define KEY_3 0x33
#define KEY_4 0x34

#define KEY_P 0x70

//FIM - Definição das teclas

//INICIO - Definição das velocidades iniciais [m/s]

#define VEL_LINEAR  1.0
#define VEL_ANGULAR 1.0

//FIM - Definição das velocidades iniciais [m/s]

//INICIO - Definição do protótipo das funções 

void print_cmd(void);                 //MENU DE COMANDOS 
void clock_(float,float);             //rEALIZA A IMPRESÃO DA DATA E CHAMA O MENU 
void config_key(void);                //REALIZA AS CONFIGURAÇÕES PARA O ACESSO AOS CRACTERES DO TECLADO
float* read_and_write_cmd(bool* flg); //REALIZA A LEITURA DAS TECLAS E ENVIA AS VELOCIDADES QUE SERÃO ESCRITAS OU PUBLICADAS  

//FIM - Definição do protótipo das funções 

//INICIO - VARIAVEIS GLOBAIS

int kfd = 0; char h = '0';
float vel_[2]={0,0};
struct termios cooked, raw;
float inc_lin_= 1, inc_ang_= 1, exit_ = 0;

//FIM - VARIAVEIS GLOBAIS


int main(int argc,char **argv){  

//REALOZA A CONFIGURAÇÃO DO ACESSO AO TECLADO
//CRIA O NÓ E O TOPICO 
  
  config_key();
  rclcpp::init(argc, argv);
  geometry_msgs::msg::Twist message;
  rclcpp::Node::SharedPtr nh = rclcpp::Node::make_shared("teleop");
  auto pub = nh->create_publisher<geometry_msgs::msg::Twist>("turtle1/turtle1/cmd_vel", 10);
  //RCLCPP_INFO(nh->get_logger(), "Olá mundo!!!");

  //REALIZA A CHAMADA DAS FUNÇÕES DE ESCRITA , LEITURA E IMPRESSÃO DA INTERFACE
  
  float *c = NULL;
  bool flg = false;
  clock_(0,0); 
  while (rclcpp::ok() && !exit_) {
        c = read_and_write_cmd(&flg);
        clock_(c[1],c[0]); 

        if(flg ==true){               //ACESSO A PUBLICAÇÃO DA MENSAGEM SE A TECLA PRESSIONADO CORRESPONTE A TABELA DE COMANDO 
           message.linear.x  = c[1];   ///MOVIMENTO LINEAR 
           message.angular.z = c[0];   //MOVIMENTO ANGULAR
           pub->publish(message);      //PUBLICA A MENSAGEM
           flg = false;
        }
        usleep(100000);   //DELAY DE 100ms
  }
  system("clear");
  puts("Bye! \n");
  tcsetattr(kfd, TCSANOW, &cooked);
  rclcpp::shutdown();

  return 0;
}



float* read_and_write_cmd(bool* flg){
   char c = '0';
   if(::read(kfd, &c, 1) < 0){
      perror("read():");
      exit(-1);
    }   
    switch(c){
   // vel_[0] = velocidade angular; vel_[1] = velocidade linear;
      case KEY_W:
        vel_[1] = inc_lin_*VEL_LINEAR; vel_[0] = 0.0*VEL_ANGULAR;
        *flg = true;
        break;
      case KEY_X:
        vel_[1] = -inc_lin_*VEL_LINEAR; vel_[0] = 0.0*VEL_ANGULAR;
        *flg = true;
        break;
      case KEY_A:
        vel_[0] = inc_ang_*VEL_ANGULAR; vel_[1] = 0.0*VEL_LINEAR;
        *flg = true;
        break;
      case KEY_D:
        vel_[0] = -inc_ang_*VEL_ANGULAR; vel_[1] = 0.0*VEL_LINEAR;
        *flg = true;
        break;
      //INCREMENTOS
      case KEY_1:
        inc_lin_ += 0.1;
        *flg = false;
        break;
      case KEY_2:
        inc_lin_ -= 0.1;
        *flg = false;
        break;
      case KEY_3:
        inc_ang_ += 0.1;
        *flg = false;
        break;
      case KEY_4:
        inc_ang_ -= 0.1;
        *flg = false;
        break;
      //DESVIOS
      case KEY_Q:
        vel_[0] = inc_ang_*VEL_ANGULAR; vel_[1] = inc_lin_*VEL_LINEAR;
        *flg = true;
        break;
      case KEY_Z:
         vel_[0] = -inc_ang_*VEL_ANGULAR; vel_[1] = -inc_lin_*VEL_LINEAR;
        *flg = true;
        break;
      case KEY_E:
        vel_[0] = -inc_ang_*VEL_ANGULAR; vel_[1] = inc_lin_*VEL_LINEAR;
        *flg = true;
        break;
      case KEY_C:
        vel_[0] = inc_ang_*VEL_ANGULAR; vel_[1] = -inc_lin_*VEL_LINEAR;
        *flg = true;
        break;
      case KEY_S:
        vel_[0] = 0.0; vel_[1] = 0.0;
        *flg = true;
        break;
      //SAÍDA  
      case KEY_P:
        exit_ = 1;
        break;
    }

   return vel_;
}

void print_cmd(){
      system("clear");
      puts("|*****************ROS2*****************|\n");  
      puts("|********Unifei*Campus*Itabira*********|\n");  
      puts("|****************ECAi21****************|\n");
      puts("|-Digite a tecla de comando:-----------|\n");
      puts("|-Setas:-W-(^)-X-(v)-A-(>)-D-(<)-------|\n");
      puts("|-Desvios:-Q-(^<)-Z-(v<)-E-(^>)-C-(v<)-|\n");
      puts("|-Parar:-S-----------------------------|\n");
      puts("|-Incremento:-1-(Linear)-3-(ANGULAR)---|\n");
      puts("|-Decremento:-2-(Linear)-4-(ANGULAR)---|\n");
      puts("|-SAIR:-P------------------------------|\n");  
      puts("|**************************************|\n");  
}

void config_key(){
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);                      
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);
}

void clock_(float lin, float ang){
    print_cmd();
    struct tm* ptr;
    time_t t;
    t = time(NULL);
    ptr = localtime(&t);
    printf("\n%s\n", asctime(ptr));
    printf("Velocidades: \n\n       Linear: %f Angular: %f\n",lin,ang);
}