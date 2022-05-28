#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include<HTTPClient.h>
#include<ArduinoJson.h>
 
// Create the lcd object address 0x3F and 16 columns x 2 rows 
LiquidCrystal_I2C lcd (0x27, 16,2);  //

//WIFI NAME/PASSWORD
const char* ssid = "NOTECONECTES";
const char* pass = "PULPINYCHIQUITA2020";
 
void  setup(){
  Serial.begin(115200);
   // Initialize the LCD connected 
  lcd.begin();
  
  // Turn on the backlight on LCD. 
  lcd.backlight();

  WiFi.begin(ssid, pass);
  delay(2000);

  Serial.print("connecting to the Wi-Fi network ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    
    }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.print("Connected");
  delay(1000);
}
 
void  loop(){
  if ((WiFi.status() == WL_CONNECTED)){
    HTTPClient ClienteHTTP;
    ClienteHTTP.useHTTP10(true);
    ClienteHTTP.begin("https://api.bitso.com/v3/ticker/");
    int CodigoDeRespuesta = ClienteHTTP.GET();
    
    Serial.println(CodigoDeRespuesta);
    if (CodigoDeRespuesta == 200){
      DynamicJsonDocument Documento(4096);

      StaticJsonDocument<200> filter;
      filter["payload"][0]["book"] = true;
      filter["payload"][0]["last"] = true;

      DeserializationError Error = deserializeJson(Documento, ClienteHTTP.getStream(), DeserializationOption::Filter(filter));
      serializeJson(Documento, Serial);
      Serial.println();
      if(Error){
        Serial.print(F("Fallo la deserializacion Json: "));
        Serial.println(Error.f_str());
        return;
      }
      //float PrecioBitcoin = Documento["payload"][0]["last"];
      float BTCUSD = Documento["payload"][18]["last"];
      //Serial.println("Precio Bitcoin MXN = " + String(PrecioBitcoin));
      Serial.println("BTC/USDT: " + String(BTCUSD));

      //pantallalcd
      lcd.clear();
      lcd.print("BTC/USDT: ");

      //Here cursor is placed on first position (col: 0) of the second line (row: 1) 
      lcd.setCursor(0, 1);
      lcd.print(BTCUSD);
      
    }
    else{
      Serial.println("Error en la solicitud HTTP");
    }

    ClienteHTTP.end();
  }
}
