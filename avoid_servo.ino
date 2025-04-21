//============================智宇科技========================================
//  智能小车超声波避障实验(有舵机)
//  程序中电脑打印数值部分都被屏蔽了，打印会影响小车遇到障碍物的反应速度
//  调试时可以打开屏蔽内容Serial.print，打印测到的距离
//  本实验控制速度的pwm值和延时均有调节，但还是配合实际情况，实际电量调节数值
//=============================================================================
//#include <Servo.h> 
#include <LiquidCrystal.h> //申明1602液晶的函数库
//申明1602液晶的引脚所连接的Arduino数字端口，8线或4线数据模式，任选其一
//LiquidCrystal lcd(12,11,10,9,8,7,6,5,4,3,2);   //8数据口模式连线声明
LiquidCrystal lcd(13,12,7,6,5,4,3); //4数据口模式连线声明 P13--LCD 4脚  P12--LCD 5脚  
              //P7--LCD 6脚   P6--LCD 11脚  P5--LCD 12脚  P4--LCD 13脚  P3--LCD 14脚  

int Echo = A1;  // Echo回声脚(P2.0)
int Trig =A0;  //  Trig 触发脚(P2.1)

int Front_Distance = 0;//
int Left_Distance = 0;
int Right_Distance = 0;

int Left_motor_go=8;     //左电机前进(IN1)
int Left_motor_back=9;     //左电机后退(IN2)

int Right_motor_go=10;    // 右电机前进(IN3)
int Right_motor_back=11;    // 右电机后退(IN4)

int key=A2;//定义按键 A2 接口
int beep=A3;//定义蜂鸣器 A3 接口

//const int SensorRight = 3;   	//右循迹红外传感器(P3.2 OUT1)
//const int SensorLeft = 4;     	//左循迹红外传感器(P3.3 OUT2)

//const int SensorRight_2 = 6;   	//右红外传感器(P3.5 OUT4)
//const int SensorLeft_2 = 5;     //左红外传感器(P3.4 OUT3)

//int SL;    //左循迹红外传感器状态
//int SR;    //右循迹红外传感器状态
//int SL_2;    //左红外传感器状态
//int SR_2;    //右红外传感器状态

int servopin=2;//设置舵机驱动脚到数字口2
int myangle;//定义角度变量
int pulsewidth;//定义脉宽变量
int val;

void setup()
{
  Serial.begin(9600);     // 初始化串口
  //初始化电机驱动IO为输出方式
  pinMode(Left_motor_go,OUTPUT); // PIN 8 (PWM)
  pinMode(Left_motor_back,OUTPUT); // PIN 9 (PWM)
  pinMode(Right_motor_go,OUTPUT);// PIN 10 (PWM) 
  pinMode(Right_motor_back,OUTPUT);// PIN 11 (PWM)
  pinMode(key,INPUT);//定义按键接口为输入接口
  pinMode(beep,OUTPUT);
  // pinMode(SensorRight, INPUT); //定义右循迹红外传感器为输入
  // pinMode(SensorLeft, INPUT); //定义左循迹红外传感器为输入
  //pinMode(SensorRight_2, INPUT); //定义右红外传感器为输入
  //pinMode(SensorLeft_2, INPUT); //定义左红外传感器为输入
  //初始化超声波引脚
  pinMode(Echo, INPUT);    // 定义超声波输入脚
  pinMode(Trig, OUTPUT);   // 定义超声波输出脚
  lcd.begin(16,2);      //初始化1602液晶工作                       模式
  //定义1602液晶显示范围为2行16列字符  
  pinMode(servopin,OUTPUT);//设定舵机接口为输出接口
}
//=======================智能小车的基本动作=========================
//void run(int time)     // 前进
void run()     // 前进
{
  digitalWrite(Right_motor_go,HIGH);  // 右电机前进
  digitalWrite(Right_motor_back,LOW);     
  analogWrite(Right_motor_go,165);//PWM比例0~255调速，左右轮差异略增减
  analogWrite(Right_motor_back,0);
  digitalWrite(Left_motor_go,LOW);  // 左电机前进
  digitalWrite(Left_motor_back,HIGH);
  analogWrite(Left_motor_go,0);//PWM比例0~255调速，左右轮差异略增减
  analogWrite(Left_motor_back,160);
  //delay(time * 100);   //执行时间，可以调整  
}

void brake(int time)  //刹车，停车
{
  digitalWrite(Right_motor_go,LOW);
  digitalWrite(Right_motor_back,LOW);
  digitalWrite(Left_motor_go,LOW);
  digitalWrite(Left_motor_back,LOW);
  delay(time * 100);//执行时间，可以调整  
}

void left(int time)         //左转(左轮不动，右轮前进)
//void left()         //左转(左轮不动，右轮前进)
{
  digitalWrite(Right_motor_go,HIGH);	// 右电机前进
  digitalWrite(Right_motor_back,LOW);
  analogWrite(Right_motor_go,180); 
  analogWrite(Right_motor_back,0);//PWM比例0~255调速
  digitalWrite(Left_motor_go,LOW);   //左轮后退
  digitalWrite(Left_motor_back,LOW);
  analogWrite(Left_motor_go,0); 
  analogWrite(Left_motor_back,0);//PWM比例0~255调速
  delay(time * 100);	//执行时间，可以调整  
}

void spin_left(int time)         //左转(左轮后退，右轮前进)
{
  digitalWrite(Right_motor_go,HIGH);	// 右电机前进
  digitalWrite(Right_motor_back,LOW);
  analogWrite(Right_motor_go,200); 
  analogWrite(Right_motor_back,0);//PWM比例0~255调速
  digitalWrite(Left_motor_go,HIGH);   //左轮后退
  digitalWrite(Left_motor_back,LOW);
  analogWrite(Left_motor_go,200); 
  analogWrite(Left_motor_back,0);//PWM比例0~255调速
  delay(time * 100);	//执行时间，可以调整   
}

void right(int time)
//void right()        //右转(右轮不动，左轮前进)
{
  digitalWrite(Right_motor_go,LOW);   //右电机后退
  digitalWrite(Right_motor_back,LOW);
  analogWrite(Right_motor_go,0); 
  analogWrite(Right_motor_back,0);//PWM比例0~255调速
  digitalWrite(Left_motor_go,LOW);//左电机前进
  digitalWrite(Left_motor_back,HIGH);
  analogWrite(Left_motor_go,0); 
  analogWrite(Left_motor_back,200);//PWM比例0~255调速
  delay(time * 100);	//执行时间，可以调整  
}

void spin_right(int time)        //右转(右轮后退，左轮前进)
{
   digitalWrite(Right_motor_go,LOW);   //右电机后退
  digitalWrite(Right_motor_back,HIGH);
  analogWrite(Right_motor_go,0); 
  analogWrite(Right_motor_back,150);//PWM比例0~255调速
  digitalWrite(Left_motor_go,LOW);//左电机前进
  digitalWrite(Left_motor_back,HIGH);
  analogWrite(Left_motor_go,0); 
  analogWrite(Left_motor_back,150);//PWM比例0~255调速
  delay(time * 100);	//执行时间，可以调整   
}

void back(int time)          //后退
{
   digitalWrite(Right_motor_go,LOW);  //右轮后退
  digitalWrite(Right_motor_back,HIGH);
  analogWrite(Right_motor_go,0);
  analogWrite(Right_motor_back,150);//PWM比例0~255调速
  digitalWrite(Left_motor_go,HIGH);  //左轮后退
  digitalWrite(Left_motor_back,LOW);
  analogWrite(Left_motor_go,150);
  analogWrite(Left_motor_back,0);//PWM比例0~255调速
  delay(time * 100);     //执行时间，可以调整  
}
//==========================================================

void keysacn()//按键扫描
{
  int val;
  val=digitalRead(key);//读取数字7 口电平值赋给val
  while(!digitalRead(key))//当按键没被按下时，一直循环
  {
    val=digitalRead(key);//此句可省略，可让循环跑空
  }
  while(digitalRead(key))//当按键被按下时
  {
    delay(10);	//延时10ms
    val=digitalRead(key);//读取数字7 口电平值赋给val
    if(val==HIGH)  //第二次判断按键是否被按下
    {
      digitalWrite(beep,HIGH);		//蜂鸣器响
      while(!digitalRead(key))	//判断按键是否被松开
        digitalWrite(beep,LOW);		//蜂鸣器停止
    }
    else
      digitalWrite(beep,LOW);          //蜂鸣器停止
  }
}

float Distance_test()   // 量出前方距离 
{
  digitalWrite(Trig, LOW);   // 给触发脚低电平2μs
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  // 给触发脚高电平10μs，这里至少是10μs
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);    // 持续给触发脚低电
  float Fdistance = pulseIn(Echo, HIGH);  // 读取高电平时间(单位：微秒)
  Fdistance= Fdistance/58;       //为什么除以58等于厘米，  Y米=（X秒*344）/2
  // X秒=（ 2*Y米）/344 ==》X秒=0.0058*Y米 ==》厘米=微秒/58
  //Serial.print("Distance:");      //输出距离（单位：厘米）
  //Serial.println(Fdistance);         //显示距离
  //Distance = Fdistance;
  return Fdistance;
}  

void Distance_display(int Distance)//显示距离
{
  if((2<Distance)&(Distance<400))
  {
    lcd.home();        //把光标移回左上角，即从头开始输出   
    lcd.print("    Distance: ");       //显示
    lcd.setCursor(6,2);   //把光标定位在第2行，第6列
    lcd.print(Distance);       //显示距离
    lcd.print("cm");          //显示
  }
  else
  {
    lcd.home();        //把光标移回左上角，即从头开始输出  
    lcd.print("!!! Out of range");       //显示
  }
  delay(250);
  lcd.clear();
}

void servopulse(int servopin,int myangle)/*定义一个脉冲函数，用来模拟方式产生PWM值舵机的范围是0.5MS到2.5MS 1.5MS 占空比是居中周期是20MS*/ 
{
  pulsewidth=(myangle*11)+500;//将角度转化为500-2480 的脉宽值 这里的myangle就是0-180度  所以180*11+50=2480  11是为了换成90度的时候基本就是1.5MS
  digitalWrite(servopin,HIGH);//将舵机接口电平置高                                      90*11+50=1490uS  就是1.5ms
  delayMicroseconds(pulsewidth);//延时脉宽值的微秒数  这里调用的是微秒延时函数
  digitalWrite(servopin,LOW);//将舵机接口电平置低
 // delay(20-pulsewidth/1000);//延时周期内剩余时间  这里调用的是ms延时函数
  delay(20-(pulsewidth*0.001));//延时周期内剩余时间  这里调用的是ms延时函数
}

void front_detection()
{
  //此处循环次数减少，为了增加小车遇到障碍物的反应速度
  for(int i=0;i<=5;i++) //产生PWM个数，等效延时以保证能转到响应角度
  {
    servopulse(servopin,90);//模拟产生PWM
  }
  Front_Distance = Distance_test();
  //Serial.print("Front_Distance:");      //输出距离（单位：厘米）
 // Serial.println(Front_Distance);         //显示距离
 //Distance_display(Front_Distance);
}

void left_detection()
{
  for(int i=0;i<=15;i++) //产生PWM个数，等效延时以保证能转到响应角度
  {
    servopulse(servopin,175);//模拟产生PWM
  }
  Left_Distance = Distance_test();
  //Serial.print("Left_Distance:");      //输出距离（单位：厘米）
  //Serial.println(Left_Distance);         //显示距离
}

void right_detection()
{
  for(int i=0;i<=15;i++) //产生PWM个数，等效延时以保证能转到响应角度
  {
    servopulse(servopin,5);//模拟产生PWM
  }
  Right_Distance = Distance_test();
  //Serial.print("Right_Distance:");      //输出距离（单位：厘米）
  //Serial.println(Right_Distance);         //显示距离
}
//===========================================================
void loop()
{
  keysacn();	   //调用按键扫描函数
  while(1)
  {
    front_detection();//测量前方距离
    if(Front_Distance < 30)//当遇到障碍物时
    {
      brake(2);//先刹车
      back(2);//后退减速
      brake(2);//停下来做测距
      left_detection();//测量左边距障碍物距离
      Distance_display(Left_Distance);//液晶屏显示距离
      right_detection();//测量右边距障碍物距离
      Distance_display(Right_Distance);//液晶屏显示距离
      if((Left_Distance < 30 ) &&( Right_Distance < 30 ))//当左右两侧均有障碍物靠得比较近
        spin_left(0.7);//旋转掉头
      else if(Left_Distance > Right_Distance)//左边比右边空旷
      {      
        left(3);//左转
        brake(1);//刹车，稳定方向
      }
      else//右边比左边空旷
      {
        right(3);//右转
        brake(1);//刹车，稳定方向
      }
    }
    else
    {
      run(); //无障碍物，直行     
    }
  } 
}











