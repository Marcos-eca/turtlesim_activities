from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
from launch.substitutions import LaunchConfiguration

def generate_launch_description():

    turtlesim_ns = LaunchConfiguration('turtlesim_ns', default='turtle1')
        
    turtle = Node(
            package='turtlesim',
            namespace = turtlesim_ns,
            executable='turtlesim_node',
            name = turtlesim_ns,
        )

    pega_pega  = Node(
            package='pega_pega',
            executable='two_turtle'
           ) 
        
    teleop = Node(
            package='teleop',
            executable='teleop'
           ) 
   
    spawn_turtle = ExecuteProcess(
        cmd=[[
            'ros2 service call ',
            turtlesim_ns,
            '/spawn ',
            'turtlesim/srv/Spawn ',
            '"{x: 0, y: 0, theta: 0.0, name: "turtle"}"'
        ]],
        shell=True
     )

    set_pen = ExecuteProcess(
        cmd=[[
            'ros2 service call ',
            turtlesim_ns,
            '/turtle/set_pen ',
            'turtlesim/srv/SetPen ',
            '"{r: 255, width: 4}"'
        ]],
        shell=True
    )
    
    
    return LaunchDescription([
     #    teleop,
         turtle,
         spawn_turtle,
         set_pen,
         pega_pega,

    ])
