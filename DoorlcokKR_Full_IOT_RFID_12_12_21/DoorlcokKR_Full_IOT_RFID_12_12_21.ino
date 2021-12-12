#include<KRwifi.h>
#include <ArduinoJson.h> //6.17.3
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

char* ssid          = "www.interactiverobotics.club";            // your network SSID (name)
char* pass           = "cilibur2019";        // your network password
int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char*   HOST_NAME   = "192.168.1.8"; // change to your PC's IP address
char* PATH_NAME   = "/ircabsensi/data-api.php?rfid=";
char* PATH_SENSOR = "/ircabsensi/data-apisensor.php?sensor=";
//String getData;
//String getData1;
//int status = WL_IDLE_STATUS;     // the Wifi radio's status

// Initialize the Ethernet client object
//WiFiEspClient client;
int buzzer=22;
#define SS_PIN 9
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* id_dibaca; 
const char* rfid_dibaca; 
const char* nama_dibaca; 
const char* alamat_dibaca; 
const char* telepon_dibaca; 
const char* tanggal_dibaca;
String statusapi;

void setup()
{
  Serial.begin(115200);
  //Serial3.begin(115200);
  lcd.begin();
  //WiFi.init(&Serial3);
  pinMode(buzzer,OUTPUT);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  // check for the presence of the shield
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("SYSTEM DOORLOCK");
  lcd.setCursor(0,1); lcd.print("Menghubungkan...");
  delay(100);
  setWifi(ssid, pass);

  // you're connected now, so print out the data
  Serial.println("Terhubung ke jaringan!");
  Serial.println(WiFi.localIP());
 
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("SYSTEM DOORLOCK");
    lcd.setCursor(0,1); lcd.print("TEMPELKAN KARTU");
    delay(1000);
}

void loop(){
rfidpush();
delay(100);
terima();
delay(100);
}

void rfidpush(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}
          // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}
          
  //Show UID on serial monitor
  //Serial.print("UID tag :");
  byte letter;
  String uidString="";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     uidString.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "" : ""));
     uidString.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //Serial.print("Message : ");
  uidString.toUpperCase();
  Serial.println(uidString);
  buzzer1x();
  httpGet(HOST_NAME, String(PATH_NAME)+ String(uidString)   , 80);  
  char json[500];
  String response = getData; 
  response.toCharArray(json, 500);
  StaticJsonDocument <200> doc1;//StaticJsonDocument<200> doc;
  deserializeJson(doc1, json); // Deserialize the JSON document 
  Serial.print("STRING RFID : ");Serial.println(getData);           
        id_dibaca       = doc1["id"]; 
        rfid_dibaca     = doc1["rfid"]; 
        nama_dibaca     = doc1["nama"]; 
        alamat_dibaca   = doc1["alamat"]; 
        telepon_dibaca  = doc1["telepon"]; 
        tanggal_dibaca  = doc1["tanggal"];
        
       //POST TO SERIAL
        //Serial.print("Waktu    = ");Serial.println(tanggal_dibaca);
        //Serial.print("ID       = ");Serial.println(id_dibaca);
        //Serial.print("RFID     = ");Serial.println(rfid_dibaca);
        //Serial.print("Nama     = ");Serial.println(nama_dibaca);
        //Serial.print("Alamat   = ");Serial.println(alamat_dibaca);
        //Serial.print("Telepon  = ");Serial.println(telepon_dibaca);
              
            if(String(id_dibaca)!="0"){
              Serial.println("------------------------------------------------------------");
              Serial.println("PINTU DIBUKA!");
              Serial.println("------------------------------------------------------------");
              lcd.clear();
              lcd.setCursor(0,0); lcd.print("SELAMAT DATANG!");
              lcd.setCursor(0,1); lcd.print(nama_dibaca);      
              Serial.println("Silahkan Masuk!");
              Serial.print("ID       = ");Serial.println(id_dibaca);
              Serial.print("RFID     = ");Serial.println(rfid_dibaca);
              Serial.print("Nama     = ");Serial.println(nama_dibaca);
              Serial.println("------------------------------------------------------------");
              buzzeroke();
              delay(1000);
              lcd.clear();
              lcd.setCursor(0,0); lcd.print("SYSTEM DOORLOCK");
              lcd.setCursor(0,1); lcd.print("TEMPELKAN KARTU");
           }else{
              Serial.println("------------------------------------------------------------");
              lcd.clear();
              lcd.setCursor(0,0); lcd.print("RFID:"); lcd.print(rfid_dibaca); 
              lcd.setCursor(0,1); lcd.print("TIDAK TERDAFTAR!");   
              Serial.println("Mohon Maaf Kartu Anda Tidak Terdaftar!");
              Serial.print("RFID = "); Serial.println(rfid_dibaca);
              Serial.println("------------------------------------------------------------");
              buzzergagal();
              delay(1000);
              lcd.clear();
              lcd.setCursor(0,0); lcd.print("SYSTEM DOORLOCK");
              lcd.setCursor(0,1); lcd.print("TEMPELKAN KARTU");
            }
}

void terima()
{
  //Baca data
  float sensorapi=random(0,50);
  if(sensorapi>=20){
    statusapi="MENDETEKSI";
  }else{
    statusapi="TIDAK MENDETEKSI";
  }
  
  httpGet(HOST_NAME, String(PATH_SENSOR)+ String(statusapi)   , 80);  
  char json[500];
  String response2 = getData; 
  response2.toCharArray(json, 500);
  StaticJsonDocument <200> doc;//StaticJsonDocument<200> doc;
  deserializeJson(doc, json); // Deserialize the JSON document 
  Serial.print("STRING SENSOR : ");Serial.println(getData);    
              
  const char* sensor_dibaca       = doc["sensor"]; // "2021-10-12 09:18:55"
  const char* lampu1_dibaca       = doc["lampu1"]; // "iwancilibur"
  const char* lampu2_dibaca       = doc["lampu2"]; // "44"
  const char* lampu3_dibaca       = doc["lampu3"]; // "40"
  const char* lampu4_dibaca       = doc["lampu4"]; // "0"
  const char* lampu5_dibaca       = doc["lampu5"]; // "0"
  const char* lampu6_dibaca       = doc["lampu6"]; // "0"
  const char* lampu7_dibaca       = doc["lampu7"]; // "0"
  
 //POST TO SERIAL
 Serial.print("sensor= ");Serial.println(sensor_dibaca);
 Serial.print("lampu1= ");Serial.println(lampu1_dibaca);
 Serial.print("lampu2= ");Serial.println(lampu2_dibaca);
 Serial.print("lampu3= ");Serial.println(lampu3_dibaca);
 Serial.print("lampu4= ");Serial.println(lampu4_dibaca);
 Serial.print("lampu5= ");Serial.println(lampu5_dibaca);
 Serial.print("lampu6= ");Serial.println(lampu6_dibaca);
 Serial.print("lampu7= ");Serial.println(lampu7_dibaca);
 Serial.println();
      
}


void buzzeroke(){
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  delay(100);
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  delay(100);
}

void buzzer1x(){
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
}

void buzzergagal(){
  digitalWrite(buzzer,HIGH);
  delay(1000);
  digitalWrite(buzzer,LOW);
}
