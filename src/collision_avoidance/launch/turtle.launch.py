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

    anti_colision  = Node(
            package='collision_avoidance',
            executable='collision_avoidance'
           ) 
   
    spawn_turtle = ExecuteProcess(
        cmd=[[
            'ros2 service call ',
            turtlesim_ns,
            '/spawn ',
            'turtlesim/srv/Spawn ',
            '"{x: 3, y: 3, theta: 0.0, name: "turtle"}"'
        ]],
        shell=True
     )

    set_pen = ExecuteProcess(
        cmd=[[
            'ros2 service call ',
            turtlesim_ns,
            '/turtle/set_pen ',
            'turtlesim/srv/SetPen ',
            '"{r: 69, g: 86, b: 255, width: 4}"'
        ]],
        shell=True
    )
    
    set_pen1 = ExecuteProcess(
        cmd=[[
            'ros2 service call ',
            turtlesim_ns,
            '/turtle1/set_pen ',
            'turtlesim/srv/SetPen ',
            '"{r: 69, g: 86, b: 255, width: 4}"'
        ]],
        shell=True
    )
    
    return LaunchDescription([
         turtle,
         spawn_turtle,
         set_pen,
         set_pen1,
         anti_colision

    ])
