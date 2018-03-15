 // Include the Servo library
#include <Servo.h> 
#include <Wire.h>
#include <Math.h>

bool grflag = 0;    // gripper release flag

// one= Length of first part of arm and two is length of second part in cm. and oneS and twoS are their square values.

int a1=15,a2=14, ba=0, sa=0, ea=0,hs, i, BAngle = 0, SAngle = 0,EAngle = 0, GAngle , angSH, angEL;
long int ReceivedByte = 0;

const float pi = 3.14, L1 = 9.6, L1s = 92.16, L2 = 14.5, L2s = 210.25;
float x,y,d;
double xs, ys, ds, c_ang1, rad1, c_angSH, radSH, AdjAngSH, c_ang2, rad2,AdjAngEL; 

// Declare the Servo & Switch pin
int servoPin1 = 3;
int servoPin2 = 4;
int servoPin3 = 5;
int servoPin4 = 6;

int s1State = 0;
const int s1Pin = 8;

int pos1 = 0;    // variable to store the servo1 position
int pos2 = 0;    // variable to store the servo2 position
int pos3 = 0;    // variable to store the servo3 position
int pos4 = 0;    // variable to store the servo4 position

// Create a servo object
Servo Servo1;
Servo Servo2;
Servo Servo3;
Servo Servo4;

void gripper()
{
  if(grflag)
  {
    GAngle = 90;  // close gripper
    Serial.println("griper opn");
    grflag = 0;
  }
  else
  {
    GAngle = 60;   // open angle
    Serial.println("griper cls");
    grflag = 1;
  }
  Servo4.write(GAngle);
}

/*
int angle_base(float d2,int x2,int y2,float denb)
{
float base, base_a;

base=(((x2+d2)-y2)/denb);
base_a= ((acos(base)) * 57296 / 1000);  //deg = rad * 57296 / 1000. ie 1 Radian = 57.2957795 degrees
return base_a;
}
*/

int angle_shoulder()
{
c_ang1  = (x/d);
rad1    = acos(c_ang1);

c_angSH = ((L1s + (xs + ys) - L2s)/(2*L1*d));
    if(c_angSH > 1.00)
            c_angSH = 1.00;
radSH   = (acos(c_angSH))+rad1;

AdjAngSH   = (radSH * (57296 / 1000));          //deg = rad * 57296 / 1000. ie 1 Radian = 57.2957795 degrees.
angSH     = (90-AdjAngSH);                       // 90 is to get adjuscend angle bcz we r interested in it  
          Serial.print("c_ang1: "); 
          Serial.println(c_ang1);
          Serial.print("rad1: "); 
          Serial.println(rad1); 
          Serial.print("cos shoulder: "); 
          Serial.println(c_angSH);
          Serial.print("shoulder angle_init: ");
          Serial.println(AdjAngSH); 
          Serial.print("shoulder angle: ");
          Serial.println(angSH);
          Serial.println(" ");
           
return angSH;       
}

int angle_elbow()
{
c_ang2  = ((L1s + L2s -ds)/(2*L1*L2));
rad2   = acos(c_ang2);
AdjAngEL  = (rad2 * (57296 / 1000));               //deg = rad * 57296 / 1000. ie 1 Radian = 57.2957795 degrees. 
angEL   = (180 - AdjAngEL);         
          Serial.print("cos elbow: "); 
          Serial.println(c_ang2); 
          Serial.print("elbow angle: ");
          Serial.println(angEL);
          Serial.println(" ");
          
return angEL;
}

void setup() {
    // initialize the pushbutton pin as an input:
  pinMode(s1Pin, INPUT);
  
  Serial.begin(9600);
  Wire.begin();
  // We need to attach the servo to the used pin number
  Servo1.attach(servoPin1);
  Servo2.attach(servoPin2);
  Servo3.attach(servoPin3);
  Servo4.attach(servoPin4);

    // Make servo go to initial position 

    Servo1.write(86);       // mechanical initial is 86 degree for motor 1
    delay(500);

    Servo2.write(0);        // mechanical initial is 4 degree for motor 2
    delay(500);

 //   Servo3.write(90);
 //   delay(500);

     Servo4.write(90);     // 90 degree is the mechanical initial for motor 3(zero degree)
    delay(500);
}

void loop(){
  
if(Serial.available())
{
          ReceivedByte = Serial.read();
        Serial.println(ReceivedByte);
         if(ReceivedByte < 1)
         {
          sa=0;
          ea=0;
         }
         else
         {
          if(ReceivedByte == 112)     // pick 
          {
            grflag = 1;
            gripper();
          
          }
          else if(ReceivedByte == 114)     // release 
          {
            grflag = 0;
            gripper();
          }
          else
          {
          x=(((ReceivedByte/10)+1)*2.678); //Scale value into  1 : 2.678 ( because arm length is 24.1 and max v=grid value is 9).
          y=((ReceivedByte%10)*2.678);
             
          xs= square(x);     
          ys= square(y);
          ds=(xs+ys);               
          d= sqrt( ds);   // find distance using pythagorus eqn
          Serial.println("x&y: "); 
          Serial.println(x); 
          Serial.println(y);
          Serial.print("d: "); 
          Serial.println(d); 
               
        //  ba=angle_base();        // get angle corresponding to place of object
          sa=angle_shoulder();
          ea=angle_elbow();
          }
         }
         
     /*     
          Serial.print(" Base angle: "); 
          Serial.println(ba);
          Serial.print(" Shoulder angle: "); 
          Serial.println(sa);
          Serial.print(" Elbow angle: "); 
          Serial.println(ea); */

          // Rotate Elbow motor
              EAngle = (ea);          // offset adgustment
             // Servo2.write(EAngle);//(EAngle);
          Serial.print("EA: "); 
          Serial.println(ea);
        
            pos2 = Servo2.read();
              if(EAngle>pos2)
              {
                for (i = pos2; i <= EAngle; i++)   
                {
                  Servo2.write(i); 
                  delay(15);
                } 
              }
              else if(EAngle<pos2)
              {
                for (i = pos2; i >= EAngle; i--)  
                {
                  Servo2.write(i); 
                  delay(15);
                }      
              }
        
        // Rotate Shoulder motor
             SAngle = ( 86 - sa );        // offset adgustment
             // Servo1.write(SAngle);
          Serial.print("SA: "); 
          Serial.println(sa); 
          
            pos1 = Servo1.read();
              if(SAngle>pos1)
              {
                for (i = pos1; i <= SAngle; i++)  
                {
                  Servo1.write(i); 
                  delay(15);
                } 
              }
              else if(SAngle<pos1)
              {
                for (i = pos1; i >= SAngle; i--)  
                {
                  Servo1.write(i); 
                  delay(15);
                }      
              }
        
        /*
             // read the state of the pushbutton value:
            s1State = digitalRead(s1Pin);
            while(s1State != LOW )
            {
            }
            */
}
}

