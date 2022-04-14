
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h> 
#include "HTTPSRedirect.h" 
#include <Adafruit_MLX90614.h>


char auth[] = "Aqjwx65fYTlkphc97yWgUTB41Rc_qtke";

char ssid[] = "DXTP";
char pass[] = "PPPP1111";

const char* host = "script.google.com";
const int httpsPort = 443;
const char *GScriptId = "AKfycbwhcnmC6cSE_P5MKice-T6GX8pfe4rFFNiKUnY9-vuwE9hwzCA";


constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
int count = 0;

String url = String("/macros/s/") + GScriptId + "/exec?tag=adc_A0&value=";

String payload = "";



HTTPSRedirect* client = nullptr;


SimpleTimer timer;




WidgetLCD lcd(V3);

void CardScan(){ 
  String tag;
  int rfidf1 = 0;
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }


    if (tag !=  "169164174213") {

    count += 1;
    lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(1, 0, "ID:");
    lcd.print(4, 0, tag); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(0, 1, "Total contact: ");
    lcd.print(14, 1, count);
    Serial.println("Access Granted!");
    digitalWrite(D8, HIGH);  
    delay(500);  
    digitalWrite(D8, LOW);   
    delay(500);     
    digitalWrite(D8, HIGH);  
    delay(500);  
    digitalWrite(D8, LOW);   
    delay(500);                 

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
//    delay(1000);

    rfidf1 = 1;

    

  }
}

 Blynk.virtualWrite(V5, rfidf1);
 Blynk.virtualWrite(V6, count);

}



void setup()
{
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  pinMode(D8,OUTPUT);
  client = new HTTPSRedirect(httpsPort);
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  payload = "tag=aaaa&value=122";
  client->POST(url, host, payload, false);
  client->GET(url, host); 

  
  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, CardScan);
  Blynk.virtualWrite(V6, count);
}


void loop()
{
  String InputData = "tagvalue";
  String FinalStringToSend = url + InputData;

  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
      payload= "";
       Serial.println("POST Data to Sheet");
//      FinalStringToSend = url + myString;
  Serial.println("POST url :" + FinalStringToSend);
  client->POST(FinalStringToSend, host, payload);
    }
  }
  else{
  Serial.println(" >> Failed to POST data");
  }

  Serial.println("GET url :"+FinalStringToSend);
  client->GET(FinalStringToSend, host);
  timer.run();
  Blynk.run();
}
