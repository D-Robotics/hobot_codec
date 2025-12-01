# Copyright (c) 2024，D-Robotics.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch_ros.actions import Node
from launch.substitutions import TextSubstitution
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import LoadComposableNodes
from ament_index_python.packages import get_package_share_directory, get_package_prefix

from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.descriptions import ComposableNode, ParameterFile
from launch.actions import GroupAction

from ament_index_python import get_package_share_directory
from ament_index_python.packages import get_package_prefix


def generate_launch_description():
    # args that can be set from the command line or a default will be used
    image_width_launch_arg = DeclareLaunchArgument(
        "image_width", default_value=TextSubstitution(text="960")
    )
    image_height_launch_arg = DeclareLaunchArgument(
        "image_height", default_value=TextSubstitution(text="544")
    )
    declare_container_name_cmd = DeclareLaunchArgument(
        'container_name', 
        default_value='tros_container',
        description='the name of the container that launches all nodes')
    declare_log_level_cmd = DeclareLaunchArgument(
        'log_level', 
        default_value='warn',
        description='log level')
    # mipi cam图片发布pkg
    mipi_cam_device_arg = DeclareLaunchArgument(
        'device',
        default_value='',
        description='mipi camera device')
        
    load_composable_nodes = LoadComposableNodes(
        target_container=LaunchConfiguration('container_name'),
        composable_node_descriptions=[
            ComposableNode(
                package='mipi_cam',
                plugin='mipi_cam::MipiCamNode',
                name='mipi_cam_node',
                parameters=[
                    {"out_format": 'nv12'},
                    {"image_width": LaunchConfiguration('image_width')},
                    {"image_height": LaunchConfiguration('image_height')},
                    {"io_method": 'ros'},
                    {"video_device": LaunchConfiguration('device')},
                    {"frame_ts_type": 'realtime'}
                ],
                extra_arguments=[{'use_intra_process_comms': True}],
            ),
            
            # 图片编码&发布pkg
            ComposableNode(
                package='hobot_codec',
                plugin='HobotCodecNode',
                name='image_encoder_node',
                parameters=[
                    {"in_mode": LaunchConfiguration('codec_in_mode')},
                    {"in_format": LaunchConfiguration('codec_in_format')},
                    {"out_mode": LaunchConfiguration('codec_out_mode')},
                    {"out_format": LaunchConfiguration('codec_out_format')},
                    {"sub_topic": LaunchConfiguration('codec_sub_topic')},
                    {"pub_topic": LaunchConfiguration('codec_pub_topic')},
                    {"jpg_quality": LaunchConfiguration('codec_jpg_quality')},
                    {"input_framerate": LaunchConfiguration('codec_input_framerate')},
                    {"output_framerate": LaunchConfiguration('codec_output_framerate')},
                    {"dump_output": LaunchConfiguration('codec_dump_output')},
                    {"dump_frame_count": LaunchConfiguration('codec_dump_frame_count')}
                ],
                extra_arguments=[{'use_intra_process_comms': True}],
            )
        ]
    )

    # component container
    bringup_cmd_group = GroupAction([
        Node(
            name='tros_container',
            package='rclcpp_components',
            executable='component_container_isolated',
            exec_name='tros_container',
            parameters=[
                {'autostart': 'True'}],
            arguments=['--ros-args', '--log-level', LaunchConfiguration('log_level')],
            # prefix=['xterm -e gdb -ex run --args'],
            output='screen')
    ])

    return LaunchDescription([
        mipi_cam_device_arg,
        image_width_launch_arg,
        image_height_launch_arg,
        DeclareLaunchArgument(
            'codec_channel',
            default_value='1',
            description='hobot codec channel'),
        DeclareLaunchArgument(
            'codec_in_mode',
            default_value='ros',
            description='image input mode'),
        DeclareLaunchArgument(
            'codec_in_format',
            default_value='nv12',
            description='image input format'),
        DeclareLaunchArgument(
            'codec_out_mode',
            default_value='ros',
            description='image output mode'),
        DeclareLaunchArgument(
            'codec_out_format',
            default_value='h264',
            description='image ouput format'),
        DeclareLaunchArgument(
            'codec_sub_topic',
            default_value='/image_raw',
            description='subscribe topic name'),
        DeclareLaunchArgument(
            'codec_pub_topic',
            default_value='/image_encoder',
            description='publish topic name'),
        DeclareLaunchArgument(
            'codec_jpg_quality',
            default_value='60.0',
            description='mjpeg encoding quality, 0-100'),
        DeclareLaunchArgument(
            'codec_input_framerate',
            default_value='30',
            description='image input framerate'),
        DeclareLaunchArgument(
            'codec_output_framerate',
            default_value='-1',
            description='image output framerate'),
        DeclareLaunchArgument(
            'codec_dump_output',
            default_value='False',
            description='Dump codec output configuration'),
        DeclareLaunchArgument(
            'codec_dump_frame_count',
            default_value='0',
            description='Dump codec output frame count, <=0 means unlimited'),
        DeclareLaunchArgument(
            'log_level',
            default_value='warn',
            description='Log level'),
        declare_container_name_cmd,
        declare_log_level_cmd, 
        bringup_cmd_group,
        load_composable_nodes
    ])
