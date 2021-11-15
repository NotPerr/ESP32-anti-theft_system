/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-esp32-motion-detection-arduino/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>


// Replace with your network credentials
const char* ssid = "Kohaku";
const char* password = "21618132";

// Initialize Telegram BOT
#define BOTtoken "2073238009:AAGdpMo3vAd1g9Jq9dpLty5RKQ_RkvKLYC4"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "2090418821"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int motionSensor = 27; // PIR Motion Sensor
bool motionDetected = false;

// Indicates when motion is detected
void IRAM_ATTR detectsMovement() {
  //Serial.println("MOTION DETECTED!!!");
  motionDetected = true;
}

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;    //每1秒檢查一次新的電報消息


const int BUZZZER_PIN = 17;
bool buzzerState = LOW;

// Handle what happens when you receive new messages 
//利用handleNewMessages（） 函數處理收到的訊息。
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

//檢查是否為可用訊息
  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    //檢查聊天室ID是否正確。
    //如果 聊天室ID與您的聊天室ID不同，則表示某人（不是您）已向您的漫遊器發送了一條消息。如果是這種情況，請忽略該消息，然後等待下一條消息。
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;   //發送的訊息
    Serial.println(text);

    String from_name = bot.messages[i].from_name;  //發送者的名字
 

 //如果收到/start消息，我們將發送有效命令來控制ESP32。如果您碰巧忘記了控制主板的命令，這將很有用。
    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/buzzer_on to turn buzzer ON \n";
      welcome += "/buzzer_off to turn buzzer OFF \n";
      welcome += "/state to request current buzzer state \n";
      bot.sendMessage(chat_id, welcome, "");  //發送消息回Telegram
    }

    if (text == "/buzzer_on") {
      bot.sendMessage(chat_id, "警鈴開啟", "");
      buzzerState = HIGH;
      digitalWrite(BUZZZER_PIN, buzzerState);
      delay(2000);
      
    }
    
    if (text == "/buzzer_off") {
      bot.sendMessage(chat_id, "警鈴關閉", "");
      buzzerState = LOW;
      digitalWrite(BUZZZER_PIN, buzzerState);
    }
    
    if (text == "/state") {
      if (digitalRead(BUZZZER_PIN)){
        bot.sendMessage(chat_id, "警鈴開啟", "");
      }
      else{
        bot.sendMessage(chat_id, "警鈴關閉", "");
      }
    }
  }
}
void setup() {
  Serial.begin(115200);

  pinMode(BUZZZER_PIN, OUTPUT);
  digitalWrite(BUZZZER_PIN, buzzerState);

  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}



void loop() {
  if(motionDetected){
    bot.sendMessage(CHAT_ID, "偵測到動作!!", "");
    Serial.println("Motion Detected");
    motionDetected = false;
  }

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  
}
