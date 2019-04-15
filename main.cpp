/*Fore translating the G-Codes the file format needs to be changed from ".g" or ".gcode" or ".gco" to ".txt". In the same directory where the main.cpp file is stored, there must be at least two more files - "settings.txt" and "targets.txt". Save the G-Code files in the same directory
Calibration:
Upload cartesian firmware Sprinter/Marlin to your CNC machine. Connect your 
*/
#include<iostream>
#include<math.h>
#include<fstream>
#include<stdlib.h>
#include<string>
#include <sstream>
float calTheta2(float x,float y, float l1, float l2)
{float d1=(x*x+y*y-l1*l1-l2*l2)/(2*l1*l2);
	float th2=acos(d1);
        th2=th2*57.3;
return th2;
}
/*The function calculates the angle Theta2, that is the angle made by the forearm with respect to the lower arm. 
Angular movement of the forearm will be controlled by a motor attached to Y- motor of a cartesian CNC machine */
float calTheta1(float x,float y, float l1, float l2)
{float d1=(x*x+y*y-l1*l1-l2*l2)/(2*l1*l2);
	float th2=acos(d1);
	float d2=(sin(th2)*l2)/(l1+l2*sin(th2));
        float beta=atan(d2);
        float d3=y/x;
        float gama=atan(d3);
        float th1=gama-beta;
       th1=th1*57.3;
	return th1;
}
/*The function calculates the angle Theta2, that is the angle made by the forearm with respect to the lower arm. 
Angular movement of the forearm will be controlled by a motor attached to Y- motor of a cartesian CNC machine */
using namespace std;
int main()
{       float x[10]; 
	char ch_x;char ch_y;char ch_z; char ch_f;char g28;
        float x_value=0.0;float y_value=0.0;float z_value=0.0; float f_value=0.0; 
        float x_mid=0.0;float y_mid=0.0;
        float x_value_new=0.0;float y_value_new=0.0;float z_value_new=0.0; 
	char ch;int gcde=0;
        bool got_values=false;
        bool got_F=false;
        bool got_only_z=false;
        bool check_quality=false;
    	float movement=0.0;   
        float angl1;float angl2; 
	char buffer[80];  
        char data[80];char files[80];string str1="square.txt";
        string str[10];//name of the files will be stored
        string strNew[10];
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
        cout<<"Length of upper Arm (L1) in milimeter :"<<x[1]<<endl;
        cout<<"Length of forearm (L2) in milimeter :"<<x[2]<<endl;
        cout<<"Inner Radius limit in milimeter :"<< x[3]<<endl;
        cout<<"Quality in milimeter :"<< x[4]<<endl;
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
	for(int k=1;k<=NUMofFiles;k++) /////////starting of for loop
       { 
	x_value=0.0;y_value=0.0;z_value=0.0;f_value=0.0; 
        x_mid=0.0;y_mid=0.0;
        x_value_new=0.0;y_value_new=0.0;z_value_new=0.0; 
	gcde=0;
        got_values=false;
        got_F=false;
        got_only_z=false;
        check_quality=false;
    	movement=0.0;   
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
         exit2: if(infile3.eof()!=false) break;
         if(ch=='G')
          {infile3>>gcde;got_only_z=false;got_values=false;got_F=false;
            if(gcde==0)
              {
                infile3>>ch_x;
                if(ch_x=='X')  {infile3>>x_value;//reads x value
                               infile3>>ch_y;
                 		if(ch_y=='Y')  {infile3>>y_value; got_values=true;
                 		 infile3>>ch_z;
                		if(ch_z=='Z')  {infile3>>z_value;}
				if(ch_z=='F')  {infile3>>f_value; got_F=true;}
                                                        }}  
		 if(ch_x=='Y')  {infile3>>y_value;
                 		 infile3>>ch_z;
                		 if(ch_y=='Z')  {infile3>>y_value;} }
                 if(ch_x=='Z')  {infile3>>z_value;got_only_z=true;}
                
               } //done reading x,y and z values...........G0 code
                 if(got_values==true)            
                {if(check_quality==false)
                {x_mid=x_value;y_mid=y_value;check_quality=true;goto exit1;}}
                movement=sqrt((x_mid-x_value)*(x_mid-x_value)+(y_mid-y_value)*(y_mid-y_value));
                            //int n=0;
               if(got_values==true)
               {
               while(movement>x[4])
               {  x_mid = x_mid + (x_value - x_mid)*x[4]/movement;
                  y_mid = y_mid + (y_value - y_mid)*x[4]/movement;
                  angl1=calTheta1(x_mid,y_mid, x[1],x[2]);
                  angl2=calTheta2(x_mid,y_mid, x[1],x[2]);
                  outfile<<"G0"<<" "<<"X"<<angl1<<" "<<"Y"<<angl2;
                  if(got_F==true) outfile<<" "<<"F"<<f_value;
                 outfile<<"\n";
                movement=sqrt((x_mid-x_value)*(x_mid-x_value)+(y_mid-y_value)*(y_mid-y_value));
                
               }}//end of quality point generation
                
              if(gcde==28) 
               {infile3>>ch;
               if(ch=='X') outfile<<"G28 X\n";
                           else {if (ch=='Y') outfile<<"G28 Y\n";
                              else {if (ch=='Z') outfile<<"G28 Z\n";else {outfile<<"G28\n";if(ch !='X'||ch !='Y'||ch !='Z')     goto exit2;}}}
               } //////////////////////G28
               exit1:
               if(got_values==true)
               { 
 	       if (sqrt(x_value*x_value+y_value*y_value)>(x[1]+x[2]))
	        {cout<<"\nPoint ("<<x_value<<","<<y_value<<") is going out of range."<<endl;goto exit3;}
	       x_value_new=calTheta1(x_value,y_value, x[1],x[2]);
               y_value_new=calTheta2(x_value,y_value, x[1],x[2]);
                outfile<<"G0"<<" "<<"X"<<x_value_new<<" "<<"Y"<<y_value_new;
                if(got_F==true) outfile<<" "<<"F"<<f_value;
                 outfile<<" ;original point\n";
               }
             exit3:
             if(got_only_z==true)outfile<<"G0"<<" "<<"Z"<<z_value<<endl;
          }//end of reading G-value
         }//end of while
          }//end of for loop

       
     	
	return 0;
}