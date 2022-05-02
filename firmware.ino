// WiFi Client
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

// ESP8266 WiFi
#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

// Utilities
#include <Hash.h>
#include <LittleFS.h>
#include <Ticker.h>

#include "Arduino_JSON.h"
#include <XMLWriter.h>  // For writing XML to the server

// Sensors
#include <DHT.h>

/////////////////////////////////////////////////////////////
// Defines

#define string String
#define nullstr (const char*)nullptr
#define strptr const char*
#define ulong unsigned long
#define uint unsigned int
#define byte unsigned char

/////////////////////////////////////////////////////////////
// Config

const char* server = "999.999.999.999";

/////////////////////////////////////////////////////////////
// Globals

// Should be DHT11, DHT12, DHT21, DHT22 or AM2301
DHT dht (2, DHT21);

/////////////////////////////////////////////////////////////

// Halts the CPU
void halt () {
    while (true) {
        delay(60000);
    }
}

 // Turns the error LED on
void error () {
    digitalWrite(5, HIGH);
    
    while (true) {
        digitalWrite(5, HIGH);
        delay(1000);
        digitalWrite(5, LOW);
        delay(1000);
    }
}

// Initialize LittleFS
void init_lfs () {
    Serial.print("Initializing LFS... ");

    bool ok = LittleFS.begin();
    if (!ok) {
        Serial.print("Formatting... ");
        LittleFS.format();
        ok = LittleFS.begin();
    }
    if (!ok) {
        Serial.println("Error");
        error();
    }
    Serial.println("Ok");
}

bool is_flash_pressed () {
    return (digitalRead(0) == LOW);
}

// Returns true if we should enter config mode
bool should_enter_config_mode () {
    for (int i = 0; i < 10; i++) {
        digitalWrite(5, HIGH);
        if (is_flash_pressed()) {
            return true;
        }
        delay(50);
        digitalWrite(5, LOW);
        delay(400);
    }
    return false;
}

namespace cfgmenu {
    void new_page () {
        //for (int i = 0; i < 50; i++) {
        //    Serial.println("");
        //}

        Serial.println("-------------------------------------------------");
        Serial.println("");
    }

    void enter_to_continue () {
        Serial.println(" Press ENTER to continue.");

        while (true) {
            if (Serial.available() > 0) {
                char read = Serial.read();
                if (read == '\n' || read == '\r') {
                    break;
                }
            } else delay(50);
        }
    }

    string prompt_string () {
        Serial.print(">");

        string buffer = "";
        while (true) {
            if (Serial.available() > 0) {
                char read = Serial.read();
                if (read == '\n' || read == '\r') {
                    break;
                } else if (read == 127) {
                    buffer = buffer.substring(0, buffer.length() - 1);
                    Serial.print((char)127);
                } else {
                    buffer += read;
                    Serial.print(read);
                }
            } else delay(50);
        }
        Serial.println();

        return buffer;
    }

    int get_option (int max) {
        while (true) {
            int res = prompt_string().toInt();
            if (res <= 0 || res > max) {
                Serial.println("Invalid option!");
                Serial.println("");
            } else {
                return res;
            }
        }
    }

    int main_menu () {
        new_page();
        Serial.println(" Configuration options:");
        Serial.println("");
        Serial.println("    (1): Change WiFi SSID");
        Serial.println("    (2): Change WiFi Password");
        Serial.println("    (3): View device info");
        Serial.println("");
        Serial.println("    (4): Restart");
        Serial.println("    (5): Save and Restart");
        Serial.println("    (6): Continue");
        Serial.println("");
        return get_option(6);
    }

    void device_info () {
        new_page();
        Serial.println(" Device information:");
        Serial.println("");
        Serial.println("     MAC Address: " + WiFi.macAddress());
        Serial.println("   Firmware Hash: " + string(ESP.getSketchMD5()));
        Serial.println("    Reset Reason: " + string(ESP.getResetReason()));
        Serial.println("       Boot Mode: " + string(ESP.getBootMode()));
        Serial.println("    Core Version: " + string(ESP.getCoreVersion()));
        Serial.println("     SDK Version: " + string(ESP.getSdkVersion()));
        Serial.println("");
        
        enter_to_continue();
    }

    string change_string (string name, string current) {
        new_page();
        Serial.println("Current " + name + " is: " + current);
        return prompt_string();
    }
}

void enter_config_mode () {
    Serial.println();
    Serial.println("Entering configuration mode...");

    File wifi_config_file = LittleFS.open("wifi_config.json", "r");
    if (!wifi_config_file) {
        Serial.println("Could not open WiFi config!");
        delay(1000);
    }

    // Read the config
    string json = wifi_config_file.readString();
    JSONVar wifi_config_body = JSON.parse(json);

    while (true) {
        int option = cfgmenu::main_menu();
        if (option == 1) {
            wifi_config_body["ssid"] = cfgmenu::change_string("WiFi SSID", (strptr)wifi_config_body["ssid"]);
        } else if (option == 2) {
            wifi_config_body["pass"] = cfgmenu::change_string("WiFi Password", (strptr)wifi_config_body["pass"]);
        } else if (option == 3) {
            cfgmenu::device_info();
        } else if (option == 4) {
            Serial.end();
            ESP.restart();
        } else if (option == 5) {

            wifi_config_file = LittleFS.open("wifi_config.json", "w+");
            if (!wifi_config_file) {
                Serial.println("Could not open WiFi config!");
            }

            // Write the config
            wifi_config_file.write(JSON.stringify(wifi_config_body).c_str());
            wifi_config_file.close();

            ESP.restart();
        } else if (option == 6) {
            return;
        }
    }
}

// Initialize WiFi
void init_wifi () {
    Serial.print("Initializing WiFi... ");

    File wifi_config_file = LittleFS.open("wifi_config.json", "r");
    if (!wifi_config_file) {
        Serial.print("Error (Config does not exist)");
        error();
    }

    // Read the config
    string json = wifi_config_file.readString();
    JSONVar wifi_config_body = JSON.parse(json);

    string ssid = (strptr)wifi_config_body["ssid"];
    string pass = (strptr)wifi_config_body["pass"];

    Serial.print("Connecting to \"" + ssid + "\"... ");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        if (WiFi.status() == WL_NO_SSID_AVAIL) {
            Serial.println("Error (Unreachable)");
            error();
        }

        if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("Error (Generic)");
            error();
        }

        delay(1000);
    }
    Serial.println("Ok");

    Serial.println("My MAC is " + WiFi.macAddress());
    Serial.println("My IP is " + WiFi.localIP().toString());
    Serial.println("");
}

string send_to_server (string content) {

    // Restart if the WiFI is unreachable
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi is unreachable! Restarting...");
        delay(1000);
        Serial.end();
        ESP.restart();
    }
    
    WiFiClient client;
    HTTPClient http;

    http.begin(client, "http://" + string(server) + ":16415/put");
    int response = http.POST(content);
    String payload = http.getString();

    Serial.print("Posted ");
    Serial.print(content.length());
    Serial.print(" bytes. (Status: ");
    Serial.print(response);
    Serial.print(", Response: ");
    Serial.print(payload.length());
    Serial.println(" bytes)");

    http.end();
    client.stop();

    return payload;
}

void status_report (string source) {
    File w_buffer = LittleFS.open("temp.xml", "w+");
    {
        XMLWriter body ((Print*)&w_buffer);

        body.header();
        body.tagStart("data");
        body.tagField("scheme", "weather-station-v1");
        body.tagField("version", "1");
        body.tagEnd(true, false);

            body.tagOpen("report");
                body.tagStart("identity");
                body.tagField("mac", WiFi.macAddress().c_str());
                body.tagEnd();

                #define status(KEY, VALUE) body.tagStart("value");body.tagField("id", KEY);body.tagField("value", string(VALUE).c_str());body.tagEnd();

                body.tagStart("status");
                body.tagField("source", source.c_str());
                body.tagEnd(true, false);
                    status("reset_reason", ESP.getResetReason());
                    status("reset_info", ESP.getResetInfo());
                    status("firmware_hash", ESP.getSketchMD5());
                    status("heap_free", ESP.getFreeHeap());
                    status("heap_fragmentation", ESP.getHeapFragmentation());
                body.print("</status>");

            body.tagClose();
        body.print("</data>");

        body.flush();
    }
    w_buffer.close();

    File r_buffer = LittleFS.open("temp.xml", "r");
    send_to_server(r_buffer.readString());
    r_buffer.close();
    LittleFS.remove("temp.xml");
}

// Called on startup
void setup () {

    // Initialize Serial
    Serial.begin(74880);
    Serial.println();

    // Make the flash() method be called when the flash button is pressed
    pinMode(0, INPUT_PULLUP);

    // Configure the LEDs
    pinMode(5, OUTPUT);

    // Test the error LED
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);

    // Initialize the filesystem
    init_lfs();

    // Check if we should enter config mode
    if (should_enter_config_mode()) {
        digitalWrite(5, LOW);
        enter_config_mode();
    }

    // Initialize WiFi
    init_wifi();
    
    // Make a status report
    status_report("startup");

    // Initialize the DHT sensor
    dht.begin();
}

// Keep track of how many times we've sent data to the server
int post_count = 1;

// Called continuously after startup
void loop () {
    
    float humidity = dht.readHumidity();
    while (isnan(humidity)) {
        Serial.println("Attempt to read humidity from DHT resulted in nan! Retrying..");
        delay(500);
        humidity = dht.readHumidity();
    } 

    float temperature = dht.readTemperature();
    while (isnan(temperature)) {
        Serial.println("Attempt to read temperature from DHT resulted in nan! Retrying..");
        delay(500);
        temperature = dht.readTemperature();
    }

    File w_buffer = LittleFS.open("temp.xml", "w+");
    {
        XMLWriter body ((Print*)&w_buffer);

        body.header();
        body.tagStart("data");
        body.tagField("scheme", "weather-station-v1");
        body.tagField("version", "1");
        body.tagEnd(true, false);

            body.tagOpen("report");
                body.tagStart("identity");
                body.tagField("mac", WiFi.macAddress().c_str());
                body.tagEnd();

                body.tagOpen("modules");
                    body.tagStart("module");
                    body.tagField("id", "temperature");
                    body.tagField("type", "number");
                    body.tagField("value", string(temperature).c_str());
                    body.tagEnd();

                    body.tagStart("module");
                    body.tagField("id", "humidity");
                    body.tagField("type", "number");
                    body.tagField("value", string(humidity).c_str());
                    body.tagEnd();
                body.tagClose();

            body.tagClose();
        body.print("</data>");

        body.flush();
    }
    w_buffer.close();

    File r_buffer = LittleFS.open("temp.xml", "r");
    send_to_server(r_buffer.readString());
    r_buffer.close();
    LittleFS.remove("temp.xml");

    // Reboot after a day of being online
    if (post_count > 24) {
        Serial.println("I've been online for a long time! Restarting...");
        delay(1000);
        Serial.end();
        ESP.restart();
    }
    
    // 60 minutes (Minutes * Seconds per Minute * Milliseconds per Second)
    delay((60 * 60) * 1000);
    post_count++;
}
