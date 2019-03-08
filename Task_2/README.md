Hello!!
To use this complete program : 
1) In the directory moveturtle/src change the path of myfile to the path of points.txt in your system.
1) Move the "moveturtle" package to your catkin workspace/src directory.
2) Then use catkin_make after cd into your workspace.
3) cd into the this directory and use the command "make rrt-star"(without quotes)
7) Now use the command "./rrt-star" (without quotes)
 Voila! you have your path.
8) Start roscore in one terminal. (type roscore in your terminal)
9) Start turtlesim in another terminal. (type rosrun turtlesim turtlesim_node)
10) type the following in your terminal:
	cd (your catkin workspace)
	source ./devel/setup.bash
	rosrun moveturtle moveturtle


	Now the turtle will move along the path generated.
Thanks :)
