#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

WebServer server(80);

 

const char* ssid = "zjfyyx";
const char* password = "wenwolaopo";
const int LED=5;

int freq = 50;      // 频率(20ms周期)
int channel = 8;    // 通道(高速通道（0 ~ 7）由80MHz时钟驱动，低速通道（8 ~ 15）由 1MHz 时钟驱动。)
int resolution = 8; // 分辨率
const int led = 15;

int calculatePWM(int degree)
{ //0-180度
 //20ms周期，高电平0.5-2.5ms，对应0-180度角度
  const float deadZone = 6.4;//对应0.5ms（0.5ms/(20ms/256）) 舵机转动角度与占空比的关系：(角度/90+0.5)*1023/20
  const float max = 32;//对应2.5ms
  if (degree < 0)
    degree = 0;
  if (degree > 180)
    degree = 180;
  return (int)(((max - deadZone) / 180) * degree + deadZone);
}


void ARMswitch(){     
  String state = server.arg("ARM");
  if(state == "on"){
      Serial.println("1");
      ledcWrite(channel, calculatePWM(115)); // 输出PWM
      delay(300);
      ledcWrite(channel, calculatePWM(90)); // 输出PWM
      delay(250);
    }
    
    else if(state == "off"){

      Serial.println("0");
     
      ledcWrite(channel, calculatePWM(60)); // 输出PWM
      delay(300);
      ledcWrite(channel, calculatePWM(90)); // 输出PWM
      delay(250);
      ledcWrite(channel, calculatePWM(90)); // 输出PWM
      delay(500);

   server.send(200,"text/html","ARM IS <b>"+ state +"</b>.");
}
}

void handleroot()
{
  String HTML ="<!DOCTTYPE html>\
  <html>\
  <head><meta charset='utf-8'></head>\
  <body>\
  温馨小窝的灯控系统:\
  <script>var xhttp = new XMLHttpRequest(); \
          function ARMswitch(arg){\
             xhttp.open('GET' , '/arm?ARM=' + arg,true);\
             xhttp.send(); }\
  </script>\
  <button onmousedown=ARMswitch('on') >开  灯</button>\
  <button onmousedown=ARMswitch('off') >关  灯</button>\
  </html>";
  server.send(200,"text/html",HTML);
  }

  

void setup() {
  Serial.begin(9600);   //串口1初始化
  Serial2.begin(9600);  //串口2初始化


  pinMode(LED,OUTPUT);

  WiFi.begin(ssid,password);    //连接WiFi
  
  Serial.print("正在连接 Wi-Fi");

  while(WiFi.status()!=WL_CONNECTED)    //判断wifi是否连接成功
  {
    delay(500);
    //Serial.print(".");
  }
  Serial.println("连接成功");
  Serial.print("IP地址");
  Serial.println(WiFi.localIP()); //打印IP地址

  pinMode(LED,OUTPUT);       
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
  delay(500);
  digitalWrite(LED,HIGH);
  delay(1500);              //闪烁一次常亮1.5秒
  digitalWrite(LED,LOW);  //通过板载LED反馈WiFi连接状态
//wifi连接成功

    server.on("/",handleroot); //创建服务器，访问ip地址的根目录
    server.on("/arm",ARMswitch); //创建服务器，访问ip地址的根目录
    server.begin(); 


  ledcSetup(channel, freq, resolution); // 设置通道
  ledcAttachPin(led, channel);          // 将通道与对应的引脚连接
   ledcWrite(channel, calculatePWM(90)); // 输出PWM
}

void loop() {
  server.handleClient();//循环连接服务器
  }
 
