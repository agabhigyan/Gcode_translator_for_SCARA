# Gcode_translator_for_SCARA
Code that translates a G-code targeted for a  cartesian CNC machine to a G-code that can be run in a SCARA CNC machine with a firmware for cartesian machine installed.


Fore translating the G-Codes the file format needs to be changed from ".g" or ".gcode" or ".gco" to ".txt" and save them in the same directory where main.cpp file is stored. In addition to that in the directory, there must be least two more files - "settings.txt" and "targets.txt". Save the G-Code files in the same directory
The file "settings.txt" contains the following parameters of the SCARA arm\
Upper_Arm_(L1)_(mm) ____\
Forearm_(L2)_(mm) _____\
Inner_Radius_limit_(mm) ____\
Quality_(mm) ____

The file "targets.txt" lists the G-Code files needed to be translated

Hardware:\
Upload the cartesian firmware Sprinter/Marlin to your CNC machine. Connect the motor controlling the movement of the lower arm, that is, controlling the angle theta1, angle made by the lower arm with respect to the x-axis; to the X-motor port of your 3D printing shield.Connect the motor controlling the movement of the forearm, that is, controlling the angle theta2, angle made by the forearm with respect to the lower arm; to the Y-motor port of your 3D printing shield. Connect the motor controlling the movement of the platform in the z-direction to the Z-motor port of your 3D printing shield. Connect the corresponding minimum limit switches as well.


Calibration:\
 Install interface software like pronterface/Repitier-Host in your PC. connect your CNC machine to your PC through any of the interface software. Increment the x coordinate (as denoted in the interfacing software) by 10 unit. Then theta1 must be incremented by 10 degree. If not, let's say it has incremented by x1. Now go to "congiguration.h" file in the firmware. There must be a line like-\
#define _AXIS_STEP_PER_UNIT {x2, y2, z2,e2} \
Now replace x2 by (x1/x2)*10. Repeat the same steps for Y-motor as well.\
Now increment the z coordinate (as denoted in the interfacing software) by 10 unit. The platform must move upward by 10mm. If not, let's say it has moved by z1 unit. Now replace z2 by (z1/z2)*10.\
If the machine is used for 3D printing perform the same steps for extruder as well. 

