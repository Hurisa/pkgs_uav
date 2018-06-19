Changelog

--- 1 ---

Changed uav_launch to include 

    <node pkg="tf" type="static_transform_publisher" name="link1_broadcaster" args="0 0 0 0 0 0 $(arg robot_name)/laser0_frame $(arg robot_name)/hokuyo_link 100" />

inside the group tag


--- 2 ---

in file launch_uav.launch delete the following code 


<include file="$(find hector_mapping)/launch/mapping_default.launch">
  <arg name="robot_name" value="$(arg robot_name)"/>
  <arg name="scan_topic" value="$(arg robot_name)/scan"/>
</include> 


and replace with the following WITHIN THE NAMESPACE

<node name="slam_gmapping" pkg="gmapping" type="slam_gmapping" respawn="false" output="screen" args="scan:=/$(arg robot_name)/scan map:=/$(arg robot_name)/map">
    <param name="map_frame" value="/map" />
    <param name="odom_frame" value="/$(arg robot_name)/ground_truth_state"/>
    <param name="base_frame" value="/$(arg robot_name)/base_link"/>
</node>

H
