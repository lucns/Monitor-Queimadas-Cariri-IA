#include <WiFi.h>
#include <HTTPClient.h>
#include <Json.h>
#include <Rtc_Pcf8563.h>
#include <SSD1306AsciiWire.h>
#include <Delay.h>
#include <SdFat.h>

#define LED_BUILTIN 2

String ssid = "D-Link";
String password =  "01234567";
Rtc_Pcf8563 rtc;
SSD1306AsciiWire oled;
Delay oneSecond(INTERVAL_1S);
SdFat sd;
int nextThrigger = -1;

typedef struct {
  String city;
  String coordinates;
} cities;

cities coordinates[30] = {
  {"Salitre", "-7.285748,-40.457514"}, {"Campo Sales", "-7.075103,-40.372339"}, {"Araripe", "-7.211309,-40.138323"},
  {"Potengi", "-7.091934,-40.027603"}, {"Assaré", "-6.870889,-39.871030"}, {"Antonia do Norte", "-6.775348,-39.988188"},
  {"Tarrafas", "-6.684036,-39.758108"}, {"Altaneira", "-6.998939,-39.738878"}, {"Nova Olinda", "-7.092372,-39.678686"},
  {"Santana do Cariri", "-7.185914,-39.737159"}, {"Farias Brito", "-6.928198,-39.571438"}, {"Crato", "-7.231216,-39.410477"},
  {"Juazeiro do Norte", "-7.228166,-39.312093"}, {"Barbalha", "-7.288738,-39.299320"}, {"Jardim", "-7.586031,-39.279563"},
  {"Porteiras", "-7.534501,-39.116856"}, {"Penaforte", "-7.830278,-39.072340"}, {"Jati", "-7.688990,-39.005227"},
  {"Brejo Santo", "-7.488500,-38.987459"}, {"Abaiara", "-7.349389,-39.033383"}, {"Milagres", "-7.310940,-38.938627"},
  {"Mauriti", "-7.382958,-38.771900"}, {"Barro", "-7.174146,-38.779534"}, {"Caririaçu", "-7.042127,-39.285435"},
  {"Granjeiro", "-6.887292,-39.220469"}, {"Aurora", "-6.943031,-38.969761"}, {"Lavras da Mangabeira", "-6.752719,-38.965939"},
  {"Ipaumirim", "-6.789527,-38.718022"}, {"Baixio", "-6.730631,-38.716898"}, {"Umari", "-6.644247,-38.699599"}
};

void freezes() {
  while (1) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}

void request() {
  oled.setCursor(0, 4);
  oled.print("Requesting...");
  oled.clearToEOL();

  JsonArray jsonArray;
  for (int i = 0; i < 30; i++) {
    oled.setCursor(0, 5);
    oled.print(coordinates[i].city);
    oled.clearToEOL();

    String url = "https://api.weatherapi.com/v1/current.json?key=a8856d705d3b4b17b25151225240605&q=" + coordinates[i].coordinates + "&aqi=yes";
    int responseCode;
    String content;
    int retries = 0;
    while (1) {
      retries++;
      if (retries == 5) break;
      Serial.print("Requesting ");
      Serial.print(coordinates[i].city);
      Serial.print(" data...");

      HTTPClient http;
      http.begin(url);
      responseCode = http.GET();
      content = http.getString();
      http.end();

      if (responseCode == 200) break;
      if (WiFi.status() != WL_CONNECTED) return;

      Serial.println();
      Serial.print("Error: ");
      Serial.print(responseCode);
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);
    }

    JsonObject json(content);
    if (!json.contains("location")) {
      Serial.println("Invalid server response->" + content);
      return;
    }
    Serial.println(" Sucessful.");
    //Serial.println("data->" + content);

    String fileName = coordinates[i].city + ".json";
    JsonObject current = json.getJsonObject("current");
    int daysWithoutRain;
    boolean hadPrecipitation;
    if (current.getDouble("precip_mm") > 0) {
      daysWithoutRain = 0;
      hadPrecipitation = true;
    } else {
      if (sd.exists(fileName)) {
        File file = sd.open(fileName, O_READ);
        String s;
        while (file.available()) s += (char) file.read();
        file.close();
        delay(100);
        
        JsonObject jsonCity(s);
        daysWithoutRain = jsonCity.getInt("days_without_rain");
        hadPrecipitation = jsonCity.getBoolean("had_precipitation");
      } else {
        daysWithoutRain = 0;
        hadPrecipitation = false;
      }
    }

    JsonObject jsonItem;
    jsonItem.put("city", coordinates[i].city);
    jsonItem.put("timestamp", current.getLong("last_updated_epoch"));
    jsonItem.put("date_time", current.getString("last_updated"));
    jsonItem.put("temperature", current.getDouble("temp_c"));
    jsonItem.put("humidity", current.getInt("humidity"));
    jsonItem.put("precipitation", current.getDouble("precip_mm"));
    jsonItem.put("had_precipitation", hadPrecipitation);
    jsonItem.put("cloud", current.getInt("cloud"));
    jsonItem.put("carbon_monoxide", current.getJsonObject("air_quality").getDouble("co"));
    jsonItem.put("days_without_rain", daysWithoutRain);
    jsonArray.put(jsonItem);

    File file = sd.open(fileName, O_WRITE | O_CREAT);
    file.write(jsonItem.toString().c_str());
    file.flush();
    file.close();

    if (i < 30) delay(1000);
  }
  if (!jsonArray.length()) {
    Serial.println("Error: Nothing to send!");
    oled.setCursor(0, 4);
    oled.print("Error: Nothing to send!");
    oled.clearToEOL();
    oled.setCursor(0, 5);
    oled.clearToEOL();
    return;
  }
  //Serial.println("data->" + jsonArray.toString());

  String url = "https://secretamensagem.000webhostapp.com/tests/post_receiver.php";
  int retries = 0;
  while (1) {
    retries++;
    if (retries == 5) {
      Serial.print("Error: Busy back-end!");
      oled.setCursor(0, 3);
      oled.print("Error: Busy back-end!");
      oled.clearToEOL();
      oled.setCursor(0, 4);
      oled.clearToEOL();
      return;
    }
    Serial.println("Sending data to back-end...");
    oled.setCursor(0, 3);
    oled.print("Sending data to back-end...");
    oled.clearToEOL();
    oled.setCursor(0, 4);
    oled.clearToEOL();

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.67 Safari/537.36");
    int responseCode = http.POST(jsonArray.toString());
    //int responseCode = http.POST("{\"data\":\"abc123\"}");
    if (responseCode > 199 && responseCode < 300) {
      Serial.print("Sucessful. Response code->");
      Serial.println(responseCode);
      if (responseCode != 204) {
        Serial.print("Content->");
        Serial.println(http.getString());
      }
      http.end();
      break;
    } else {
      http.end();
      Serial.print("Failure. Response code->");
      Serial.println(responseCode);
      if (WiFi.status() != WL_CONNECTED) return;

      Serial.println();
      Serial.print("Error: ");
      Serial.print(responseCode);
      Serial.println(". Retrying in 10 seconds...");
      delay(10000);
    }
  }

  oled.setCursor(0, 3);
  oled.clearToEOL();
  oled.setCursor(0, 4);
  oled.print("OK");
  oled.clearToEOL();
  oled.setCursor(0, 5);
  oled.clearToEOL();
}

String getTime() {
  int year = rtc.getYear();
  int month = rtc.getMonth();
  int day = rtc.getDay();
  int hour = rtc.getHour();
  int minute = rtc.getMinute();
  int second = rtc.getSecond();

  String s;
  if (day < 10) s += "0";
  s += String(day);
  s += "/";
  if (month < 10) s += "0";
  s += String(month);
  s += "/20";
  s += String(year);
  s += " ";
  if (hour < 10) s += "0";
  s += String(hour);
  s += ":";
  if (minute < 10) s += "0";
  s += String(minute);
  s += " ";
  if (second < 10) s += "0";
  s += String(second);
  return s;
}

void setup() {
  Serial.begin(115200);
  while (!Serial || millis() < 1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // rtc.initClock(); // This call reset  module function
  String s = getTime();

  //oled.begin(&Adafruit128x32, 0x3C);
  oled.begin(&SH1106_128x64, 0x3C);
  oled.setFont(System5x7);
  //oled.setFont(Verdana12);
  oled.clear();
  oled.setCursor(0, 0);
  oled.print(s);

  Serial.println(s);

  if (sd.begin(5, SPI_HALF_SPEED) == 0) {
    //sd.initErrorHalt(&Serial);
    Serial.println("Couldn't find SD Card!");
    oled.setCursor(0, 1);
    oled.print("Couldn't find SD Card!");
    freezes();
  }

  int minute = rtc.getMinute();
  if (minute < 5) nextThrigger = 5;
  else if (minute < 20) nextThrigger = 20;
  else if (minute < 35) nextThrigger = 35;
  else if (minute < 50) nextThrigger = 50;
  else nextThrigger = 5;
  Serial.println("Next thrigger: " + String(rtc.getHour()) + ":" + (nextThrigger > 9 ? String(nextThrigger) : "05"));
  oled.setCursor(0, 1);
  oled.print("Next thrigger: " + String(rtc.getHour()) + ":" + (nextThrigger > 9 ? String(nextThrigger) : "05"));
  oled.clearToEOL();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("WiFi connecting...");
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      Serial.print(".");
    }
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println(" Connected.");
    request();
  } else {
    if (oneSecond.gate()) {
      oneSecond.reset();

      oled.home();
      oled.print(getTime());
      oled.clearToEOL();
      if (rtc.getMinute() == nextThrigger) {
        nextThrigger += 15;
        if (nextThrigger == 65) nextThrigger = 5;
        Serial.println("Next thrigger: " + String(rtc.getHour()) + ":" + (nextThrigger > 9 ? String(nextThrigger) : "05"));
        oled.setCursor(0, 1);
        oled.print("Next thrigger: " + String(rtc.getHour()) + ":" + (nextThrigger > 9 ? String(nextThrigger) : "05"));
        oled.clearToEOL();

        request();
      }
    } else {
      //readSerial();
    }
  }
}
