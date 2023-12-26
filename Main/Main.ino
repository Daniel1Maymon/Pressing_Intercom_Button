#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Servo.h>

#define SERVO_PIN 16

// Wifi network station credentials
#define WIFI_SSID "<ChangeItToYourWifiName>"
#define WIFI_PASSWORD "<ChangeItToYourWifiPassword>"

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "<ChangeIt>"
#define CHAT_ID "<ChangeIt>"

#define SERVO_ANGLE_OPEN 100
#define SERVO_ANGLE_CLOSED 0 
#define DELAY_TIME 1200

const unsigned long SCAN_MESSAGE_DELAY = 1000;  // mean time between scan messages

Servo my_servo;          // Create a Servo object
bool has_moved = false;  // Flag to track if the servo has already moved
unsigned long bot_last_time;  // last time messages' scan has been done

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void HandleNewMessages(int num_of_new_msgs) {

  for (int i = 0; i < num_of_new_msgs; i++) {

    String chat_id = String(bot.messages[i].chat_id);
    if (strcmp(chat_id.c_str(), String(CHAT_ID).c_str()) != 0) {
        bot.sendMessage(chat_id, "Unauthorized user", "");
    } else {
      String text = bot.messages[i].text;
      String from_name = bot.messages[i].from_name;


      if (strcmp(from_name.c_str(), "") == 0) {
        // Strings are equal, so set from_name to "Guest"
        from_name = "Guest";
      }

      if (strcmp(text.c_str(), "/click") == 0) {
        // Servo program:
        my_servo.write(SERVO_ANGLE_OPEN);
        delay(DELAY_TIME);
        my_servo.write(SERVO_ANGLE_CLOSED);

      }

      if (strcmp(text.c_str(), "/start") == 0) {
        String welcome_msg = "Hey " + from_name + ",Welcome to the Intercom Click Button Bot.\n";
        welcome_msg += "/click : to open the door\n";

        // Printing welcome_msg in the chat bot
        bot.sendMessage(chat_id, welcome_msg, "Markdown");
      }
    }
  }
}

const int INITIAL_SERVO_ANGLE = 0; 
const int SERIAL_BAUD_RATE = 115200;
const int LED_PIN = 2;  // Desired GPIO pin (e.g., D2)
const int PIN_SETUP_DELAY = 10; // Reflecting that it is a delay used during pin setup.
const int WIFI_CONNECT_DELAY = 500;

void setup() {
  my_servo.attach(SERVO_PIN);
  my_servo.write(INITIAL_SERVO_ANGLE);

  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println();


  pinMode(LED_PIN, OUTPUT);  // initialize digital LED_PIN as an output.
  delay(PIN_SETUP_DELAY);
  digitalWrite(LED_PIN, LOW);  // initialize pin as off (active LOW)

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setInsecure();  // Use insecure connection for Telegram
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(WIFI_CONNECT_DELAY);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - bot_last_time > SCAN_MESSAGE_DELAY) {
    int num_of_new_msgs = bot.getUpdates(bot.last_message_received + 1);

    while (num_of_new_msgs > 0) {
      Serial.println("got response");
      HandleNewMessages(num_of_new_msgs);
      num_of_new_msgs = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_last_time = millis();
  }
}
