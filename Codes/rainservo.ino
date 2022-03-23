#include <Servo.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID "wifi-name" //change this accordingly
#define WIFI_PASSWORD "password" //change this accordingly
#define BOT_TOKEN "bot-token" //change this accordingly

const unsigned long BOT_MTBS = 1000;
Servo myservo;
#define rainAnalog 35
int analogVal;
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;

int servoStatus;

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/close")
    {
      myservo.write(100);    
      servoStatus = 1;
       bot.sendMessage(chat_id, "Recieved", "");
    }
    if (text == "/open")
    {
      myservo.write(5);
      servoStatus = 0;
      bot.sendMessage(chat_id,"Recieved", "");
    }
    if (text == "/report")
    {
      if(analogVal<2000)
      {
        bot.sendMessage(chat_id, "IT IS RAINING", "");
      }
      else
      {
        bot.sendMessage(chat_id,"IT IS NOT RAINING", "");
      }
    }
    if (text == "/start")
    {
      String welcome = "Welcome to the water sensing bot, " + from_name + ".\n";
      welcome += "This is your bot.\n\n";
      welcome += "/report : Returns current status of the weather\n";
      welcome += "/close : to close the motor\n";
      welcome += "/open : to open the motor\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
    
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(rainAnalog,INPUT);
  myservo.attach(13);
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop() {
  analogVal = analogRead(rainAnalog);
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
