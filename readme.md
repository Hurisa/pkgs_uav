Changelog

--- 1 ---

Changed uav_launch to include 

<node pkg="tf" type="static_transform_publisher" name="link1_broadcaster" args="0 0 0 0 0 0 $(arg robot_name)/laser0_frame $(arg robot_name)/hokuyo_link 100" />

inside the group tag

