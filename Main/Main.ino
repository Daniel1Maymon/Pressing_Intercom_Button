#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <Servo.h>

Servo myservo;          // Create a Servo object
bool hasMoved = false;  // Flag to track if the servo has already moved
int servoPin = 16;

// Wifi network station credentials
#define WIFI_SSID "<ChangeItToYourWifiName>"
#define WIFI_PASSWORD "<ChangeItToYourWifiPassword>"

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "<ChangeIt>"
#define CHAT_ID "<ChangeIt>"

const unsigned long BOT_MTBS = 1000;  // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;  // last time messages' scan has been done

const int ledPin = 2;  // Desired GPIO pin (e.g., D2)
// int ledStatus = 0;

void handleNewMessages(int numNewMessages) {

  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
    } else {
      String text = bot.messages[i].text;

      // Printing for debugging purposes
      Serial.println("text = ");
      Serial.println(text);

      String from_name = bot.messages[i].from_name;

      // Printing for debugging purposes
      Serial.println("from_name = ");
      Serial.println(from_name);

      if (from_name == "")
        from_name = "Guest";

      if (text == "/click") {
        // Servo program:
        myservo.write(50 * 2);
        delay(1200);
        myservo.write(0);

      }

      if (text == "/start") {
        String welcome_msg = "Welcome to the Intercom Click Button Bot., " + from_name + ".\n";
        welcome_msg += "/click : to open the door\n";

        // Printing welcome_msg in the chat bot
        bot.sendMessage(chat_id, welcome_msg, "Markdown");
      }
    }
  }
}

void setup() {
  myservo.attach(servoPin);
  myservo.write(0);

  Serial.begin(115200);
  Serial.println();

  pinMode(ledPin, OUTPUT);  // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, LOW);  // initialize pin as off (active LOW)

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setInsecure();  // Use insecure connection for Telegram
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
