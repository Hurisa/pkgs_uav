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

Also within the namespace add

    <node pkg="tf" type="static_transform_publisher" name="broadcaster_laser_frame_to_hokyuo" args="0 0 0 0 0 0 $(arg robot_name)/laser0_frame $(arg robot_name)/hokuyo_link 100" />
    <node pkg="tf" type="static_transform_publisher" name="world_to_map" args="0 0 0 0 0 0 /world /map 100" />


Ignore other changes in the push but do catkin_make.


---3---

in file launch_uav.launch substitute the gmapping node in point 2 to 

	<node name="slam_gmapping" pkg="gmapping" type="slam_gmapping" respawn="false" output="screen" args="scan:=/$(arg robot_name)/scan map:=/$(arg robot_name)/map">
      <param name="map_frame" value="/world" />
      <param name="odom_frame" value="/$(arg robot_name)/ground_truth/state"/>
      <param name="base_frame" value="/$(arg robot_name)/base_link"/>
    </node>
     
and add

	<node name="map_to_image_node_mod" pkg="map_to_jpeg" type="map_to_image_node_mod" respawn="false" output="screen" args="map:=/$(arg robot_name)/map map_image/full:=/$(arg robot_name)/map_image"/>

all within the namespace.
Also comment the tf static transform in point 2 as everything has been resolved to frame /world.

    <node pkg="tf" type="static_transform_publisher" name="world_to_map" args="0 0 0 0 0 0 /world /map 100" />

Besides, whatever the ground robot (base station) is add this to the namespace

    <node name="map_merge" type="map_merge" pkg="multirobot_map_merge" respawn="false" output="screen">
     <param name="known_init_poses" value="false"/>
     <param name="world_frame" value="/world"/>
     <param name="robot_topic_name" value=""/>
     <param name="confidence" value="$(arg conf)"/>
    </node>

    <node name="map_to_image_node_mod" pkg="map_to_jpeg" type="map_to_image_node_mod" respawn="false" output="screen" args="map:=/$(arg robot_name)/map map_image/full:=/$(arg robot_name)/map_image"/>















