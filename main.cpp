//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                             Gcode_translator_for_SCARA
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Fore translating the G-Codes the file format needs to be changed from ".g" or ".gcode" or ".gco" to ".txt". In the same directory where the main.cpp file is stored, there must be at least two more files - "settings.txt" and "targets.txt". Save the G-Code files in the same directory
The file "settings.txt" contains the following parameters of the SCARA arm
Upper_Arm_(L1)_(mm) ____
Forearm_(L2)_(mm) _____
Inner_Radius_limit_(mm) ____
Quality_(mm) ____
The file "targets.txt" lists the G-Code files needed to be translated
Hardware:
Upload the cartesian firmware Sprinter/Marlin to your CNC machine. Connect the motor controlling the movement of the lower arm, that is, controlling the angle theta1, angle made by the lower arm with respect to the x-axis; to the X-motor port of your 3D printing shield.Connect the motor controlling the movement of the forearm, that is, controlling the angle theta2, angle made by the forearm with respect to the lower arm; to the Y-motor port of your 3D printing shield. Connect the motor controlling the movement of the platform in the z-direction to the Z-motor port of your 3D printing shield. Connect the corresponding minimum limit switches as well.
Calibration:
 Install interface software like pronterface/Repitier-Host in your PC. connect your CNC machine to your PC through any of the interface software. 
*/

#include<iostream>
#include<math.h>
#include<fstream>
#include<stdlib.h>
#include<string>
#include <sstream>
float calTheta2(float x,float y, float l1, float l2)
{	float d1=(x*x+y*y-l1*l1-l2*l2)/(2*l1*l2);
	float th2=acos(d1);
        th2=th2*57.3;
return th2;
}
/* The function calculates the angle Theta2 (in degree), that is the angle made by the forearm with respect to the lower arm in the counter-clockwise direction. 
Angular movement of the forearm will be controlled by a motor attached to Y- motor of a cartesian CNC machine */
float calTheta1(float x,float y, float l1, float l2)
{	float d1=(x*x+y*y-l1*l1-l2*l2)/(2*l1*l2);
	float th2=acos(d1);
	float d2=(sin(th2)*l2)/(l1+l2*sin(th2));
        float beta=atan(d2);
        float d3=y/x;
        float gama=atan(d3);
        float th1=gama-beta;
       th1=th1*57.3;
	return th1;
}
/*The function calculates the angle Theta1 (in degree), that is the angle made by the lower arm with respect to the x-axis in the counter-clockwise direction. 
Angular movement of the forearm will be controlled by a motor attached to Y- motor of a cartesian CNC machine */
using namespace std;
int main()
{       float x[10]; 
	char ch;char ch_s;
	int gcde=0;int mcde=0;
        float x_value=0.0;float y_value=0.0;float z_value=0.0; float e_value=0.0; 
	float f_value=0.0;float s_value=0.0; 
        float x_mid=0.0;float y_mid=0.0;
        float x_value_new=0.0;float y_value_new=0.0;
	string comment;//for storing the comments of the G-code files
        bool got_X=false;
        bool got_Y=false;
        bool got_Z=false;
        bool got_E=false;
        bool got_F=false;
        bool got_only_Z=false;
        bool got_only_E=false;
        bool check_quality=false;
        bool got_G_M=false;
    	float distance=0.0;   
        float angl1;float angl2; 
        char data[80];
        string str[10];//name of the files will be stored
        string mod="modified_for_scara_";
        int NUMofFiles=0.0;
	int i=0;
	ifstream infile("settings.txt"); //opening file for reading the data
	if (! infile)
	{ cout<<"\n The file \"settings.txt\"  must be there in the current directory for the required settings of the specific configuration of SCARA."<<endl;
	exit(1);
	}
	while(!infile.eof())
	{ infile.getline(data,80);
          istringstream in(data);      //make a stream for the line itself
           std::string type;
           in >> type;
         in >> x[i];
         i=i+1;
    	}
        cout<<"Length of upper Arm (L1) in milimeter :"<<x[0]<<endl;
        cout<<"Length of forearm (L2) in milimeter :"<<x[1]<<endl;
        cout<<"Inner Radius limit in milimeter :"<< x[2]<<endl;
        cout<<"Quality in milimeter :"<< x[3]<<endl;
     	infile.close();
        ifstream infile2("targets.txt");
        if (! infile2)
	{ cout<<"\n The file \"targets.txt\"  must be there in the current directory for the required settings of the specific configuration of SCARA."<<endl;
	exit(1);
	} int j=0;
       while(!infile2.eof())
        {j++;infile2>>str[j];} int a=j;
         cout<<"The G-code files are : ";
         NUMofFiles=j-1;
         for(int i=1;i<=NUMofFiles;i++)
         cout<<str[i]<<"\t";
         cout<<"\nNumber of Gcode files to be processed :"<<NUMofFiles<<endl;
	for(int k=1;k<=NUMofFiles;k++) 
       {
//in the following lines variables are reinitialized for reading and writng the next G-Code file	 
	x_value=0.0;y_value=0.0;z_value=0.0;e_value=0.0;f_value=0.0;
        x_mid=0.0;y_mid=0.0;
        x_value_new=0.0;y_value_new=0.0;
	gcde=0;mcde=0;
	got_X=false;
        got_Y=false;        
        got_Z=false;
        got_E=false;
        got_F=false;
	got_G_M=true;
        got_only_Z=false;
        got_only_E=false;
        check_quality=false;
    	distance=0.0;   
        ifstream infile3(str[k].c_str()); //reads the file str1 for G-Code
        if (! infile3)
	{ cout<<"\n The target files do not exist"<<endl;
	exit(1);
	}
        str[k].insert(0, mod);
        str[k].erase(str[k].size()-3,3);
        str[k]=str[k]+"g";
       	ofstream outfile(str[k].c_str()); //writing G-code to a new file
        while(!infile3.eof())
        {infile3>>ch;
         exit2:got_G_M==false;
          if(infile3.eof()!=false) break;
         if(ch=='G')
          {infile3>>gcde;
          got_only_Z=false;got_F=false;got_only_E=false;got_E=false;
if(gcde==0||gcde==00||gcde==01||gcde==1)
              {
              infile3>>ch;
switch(ch)
{ 
//The following case reads the G0/G1 argument of type
//(X_ Y_ Z_),(X_ Y_ Z_ E_ F_),(X_ Y_ Z_ E_),(X_ Y_ Z_ F_),(X_ Y_ E_ F_),(X_ Y_ E_),
//(X_ Y_ F_),(X_ Y_),(X_ Z_),(X_ Z_ E_ F_),(X_ Z_ E_),(X_ Z_ F_),(X_ E_ F_),(X_ E_),(X_)
case 'X':  infile3>>x_value;//reads x value
           infile3>>ch;
           switch(ch)      		
           {   
           case 'Y':infile3>>y_value;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
                              //X_ Y_ Z_ 
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                              infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                               //X_ Y_ Z_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                //X_ Y_ Z_ E_
                                       }
                            if(ch=='F')
                               infile3>>f_value;
                               got_F=true;
                               //X_ Y_ Z_ F_
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                                 //X_ Y_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                 //X_ Y_ E_
                            break;
	   	   case 'F':infile3>>f_value; got_F=true;
                            //X_ Y_ F_
                            break;
	           case 'G':got_G_M=true;
			    //X_ Y_
                            break;
                    }
                        break;
           case 'Z':infile3>>z_value;
                    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
                              //X_ Z_   
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                              infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                               //X_ Z_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                //X_ Z_ E_
                                       }
                            if(ch=='F')
                               infile3>>f_value;
                               got_F=true;
                               //X_ Z_ F_
                     break;
	   case 'E':infile3>>e_value;
                     got_E=true;
                     infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                               //X_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                //X_ E_
		    break;
	   case 'F':infile3>>f_value;
                     //X_ F_
                     got_F=true;
		    break;
           case 'G':got_G_M=true;
                    //X_
		    break;
           }
	   break; 
//The following case reads the G0/G1 argument of type
//(Y_ Z_),(Y_ Z_ E_ F_),(Y_ Z_ E_),(Y_ Z_ F_),(Y_ E_ F_),(Y_ E_),(Y_ F_),(Y_)
case 'Y':  infile3>>y_value;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			      //Y_ Z_
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                              infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                                 //Y_ Z_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                 //Y_ Z_ E_
                                       }
                            if(ch=='F')
                               {infile3>>f_value;
			       //Y_ Z_ F_
                               got_F=true;}
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                                 //Y_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
		                 //Y_ E_
                            break;
	   	   case 'F':infile3>>f_value; 
                            //Y_ F_
                            got_F=true;
                            break;
                   case 'G':got_G_M=true;
                            //Y_ 
                            break;
                    }
         break;
//The following case reads the G0/G1 argument of type
//(Z_),(Z_E_ F_),(Z_E_),(Z_,F_)
case 'Z':  infile3>>z_value;
           got_only_Z=true;
           if(gcde==01||gcde==1)
                                 outfile<<"G1";
           if(gcde==00||gcde==0)
                                 outfile<<"G0";
           outfile<<" Z"<<z_value;
           infile3>>ch;
	   if(ch=='G')
              got_G_M=true;
              outfile<<"\n";
              //Z_
	   if(ch=='E'){
	      infile3>>e_value;
              outfile<<" E"<<e_value;
              infile3>>ch;
	      if(ch=='F')
                {
                infile3>>f_value;
                outfile<<" F"<<f_value<<endl;
		//Z_ E_ F_	        
		got_F=true;}
              if(ch=='G')
                got_G_M=true;
                outfile<<"\n";
                //Z_ E_
                      }
            if(ch=='F')
               {infile3>>f_value;
               outfile<<" F"<<f_value<<endl;}
               //Z_ F_
           break;
//The following case reads the G0/G1 argument of type
//(E_ F_),(E_)   
case 'E':  infile3>>e_value;
           got_only_E=true;
           if(gcde==01||gcde==1)
                                 outfile<<"G1";
           if(gcde==00||gcde==0)
                                 outfile<<"G0";
           outfile<<" E"<<e_value;
           infile3>>ch;
	   if(ch=='F')
             {
             infile3>>f_value;
             outfile<<" F"<<f_value<<endl;
	     got_F=true;}
             //E_ F_
           if(ch=='G')
               {got_G_M=true;
               outfile<<"\n";}
               //E_
         break; 
//The following case reads the G0/G1 argument of type
//(F_ X_ Y_ Z_),(F_ X_ Y_ Z_ E_),(F_ X_ Y_),(F_ X_ Z_),(F_ X_ Z_ E_),(F_ X_ E_),(F_ X_)
//(F_ Y_ Z_),(F_ Y_ Z_ E_),(F_ Y_ E_),(F_ Y_),(F_ Z_),(F_ Z_ E_)
case 'F':infile3>>f_value;
         got_F=true;
         infile3>>ch;
         switch(ch)
         {
         case 'X':  infile3>>x_value;//reads x value
           infile3>>ch;
           switch(ch)      		
              {   
              case 'Y':infile3>>y_value;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
                              //F_ X_ Y_ Z_
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                                       }
                               //F_ X_ Y_ Z_ E_
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            //F_ X_ Y_ E_
                            break;
                   case 'G':got_G_M=true;
                            //F_ X_ Y_
                            break;
                    }
                        break;
              case 'Z':infile3>>z_value;
                       infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			      //F_ X_ Z_
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;}
                              //F_ X_ Z_ E_ 
                       break;
	      case 'E':infile3>>e_value;
                     got_E=true;
                     //F_ X_ E_
		    break;
              case 'G':got_G_M=true;
                    //F_ X_
		    break;
               }
	         break;                                                
         case 'Y':  infile3>>y_value;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
                              //F_ Y_ Z_
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                                        }
                              //F_ Y_ Z_ E_
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            //F_ Y_ E_
                            break;
                   case 'G':got_G_M=true;
                            //F_ Y_
                            break;
                    }
               break;
       case 'Z':  infile3>>z_value;
                  got_only_Z=true;
                  if(gcde==01||gcde==1)
                                 outfile<<"G1";
                  if(gcde==00||gcde==0)
                                 outfile<<"G0";
                  outfile<<" F"<<f_value<<" Z"<<z_value;
                  infile3>>ch;
	   if(ch=='G')
              got_G_M=true;
              //F_ Z_
              outfile<<"\n";
	   if(ch=='E'){
	      infile3>>e_value;
              outfile<<" E"<<e_value;
              outfile<<"\n";
                      }
              //F_ Z_ E_
           break;
         }      
}
//end of switch statement

                 if(x_value!=0.0||y_value!=0.0)           
                {if(check_quality==false)
                {x_mid=x_value;y_mid=y_value;check_quality=true;goto exit1;}}
               distance=sqrt(pow((x_mid-x_value),2)+pow((y_mid-y_value),2));
               if(x_value!=0.0||y_value!=0.0)
               {
               while(distance>x[3])
               {  x_mid = x_mid + (x_value - x_mid)*x[3]/distance;
                  y_mid = y_mid + (y_value - y_mid)*x[3]/distance;
                  angl1=calTheta1(x_mid,y_mid, x[0],x[1]);
                  angl2=calTheta2(x_mid,y_mid, x[0],x[1]);
                  if(gcde==01||gcde==1)
                                 outfile<<"G1";
                  if(gcde==00||gcde==0)
                                 outfile<<"G0";
                  outfile<<" X"<<angl1<<" "<<"Y"<<angl2;
                  if(got_F==true) outfile<<" "<<"F"<<f_value;
                 outfile<<"\n";
                distance=sqrt(pow((x_mid-x_value),2)+pow((y_mid-y_value),2));
             }}//end of new points generation by interpolation.
                x_mid=x_value;y_mid=y_value;
             
               exit1:
               if(x_value!=0.0 && y_value!=0.0 && got_only_Z==false&&got_only_E==false)
               { 
//The following if statement warns the user about the points going out out of range of the current SCARA configuration
 	       if (sqrt(pow(x_value,2)+pow(y_value,2))>(x[0]+x[1])||sqrt(pow(x_value,2)+pow(y_value,2))<x[2])
//checking points lying outside of the circle of radius x[0]+x[1],and inside of the circle of radius x[2]
	        {cout<<"\nPoint ("<<x_value<<","<<y_value<<") is going out of range."<<endl;
                 goto exit3;}
	       x_value_new=calTheta1(x_value,y_value, x[0],x[1]);
               y_value_new=calTheta2(x_value,y_value, x[0],x[1]);
	       if(gcde==01||gcde==1)
                                 outfile<<"G1";
               if(gcde==00||gcde==0)
                                 outfile<<"G0";
                outfile<<" X"<<x_value_new<<" "<<"Y"<<y_value_new;
                if(got_E==true) outfile<<" "<<"E"<<e_value;
		if(got_F==true) outfile<<" "<<"F"<<f_value;
                 outfile<<" ;original point\n";
               }
              exit3:
              if(got_G_M==true) goto exit2;
} //end of if(gcde==0||gcde==00||gcde==01||gcde==1)
if(gcde==28) 
               {infile3>>ch;
               if(ch=='X') outfile<<"G28 X\n";
                  else {if (ch=='Y') outfile<<"G28 Y\n";
                   else {if (ch=='Z') outfile<<"G28 Z\n";
                    else {outfile<<"G28\n";
                     if(ch !='X'||ch !='Y'||ch !='Z')  
                      goto exit2;}}}
} // end of reading and writing G-code G28.        
if(gcde==90)  outfile<<"G90 ;use absolute coordinate"<<endl;
if(gcde==21)  outfile<<"G21 ; set units to millimeters"<<endl;
if(gcde==92) 
           {infile3>>ch;
switch(ch)
{ 
case 'X':  infile3>>x_value;
           got_X=true;
           infile3>>ch;
           switch(ch)      		
           {   
           case 'Y':infile3>>y_value;
                    got_Y=true;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
                            got_Z=true;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                                        }
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            break;
	           case 'G':got_G_M=true;
                            break;
                    }
                        break;
           case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			      if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                              infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                               if(ch=='G')
                                 got_G_M=true;
                                       }
                            if(ch=='F')
                               {infile3>>f_value;
			       got_F=true;}
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
			      if(ch=='G')
                                 got_G_M=true;
		            break;
           case 'G':got_G_M=true;
		    break;
           }
	   break;                                                
case 'Y':  infile3>>y_value;
		    got_Y=true;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
                            got_Z=true;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                                        }
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            break;
                   case 'G':got_G_M=true;
                            break;
                    }
         break;
case 'Z':  infile3>>z_value;
           got_Z=true;
           infile3>>ch;
	   if(ch=='G')
              got_G_M=true;
	   if(ch=='E')
	      {infile3>>e_value;got_E=true;}
           break;    
case 'E':  infile3>>e_value;
           got_E=true;
           break;
case 'G':  got_G_M=true;
           break;  
}
//end of switch statemen
}
outfile<<"G92";
if(got_X==true)
          outfile<<" X"<<x_value;
if(got_Y==true)
	  outfile<<" Y"<<y_value;
if(got_Z==true)
          outfile<<" Z"<<z_value;
if(got_E==true)
           outfile<<" E"<<e_value;
outfile<<"\n";
if(got_G_M==true) goto exit2;
//end of if(gcde==92)
}
//end of reading and writing G-codes
if(ch=='M')
          {infile3>>mcde;
switch(mcde)
{
	case 82 :outfile<<"M82 ; use absolute distances for extrusion"<<endl;
		 break;
	case 104:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M104 S"<<s_value;
                 outfile<<" ;set temperature"<<endl;
                 break;
	case 106:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M106 S"<<s_value<<endl;
                 break;
	case 107:outfile<<"M107"<<endl;
		 break;
	case 109:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M109 S"<<s_value<<endl;
                 break;
	case 140:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M140 S"<<s_value<<endl;
                 break;
	case 190:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M190 S"<<s_value;
		 outfile<<" ;wait for bed temperature to be reached"<<endl;
                 break;
}//end of switch(mcde) 
           }
//End of reading and writing M-codes
if(ch==';')
         {infile3>>comment;
          outfile<<";"<<comment<<endl;
         }
//end of reading and writing comments (starting with ';')
         }//end of while
}//end of for loop
return 0;
}
