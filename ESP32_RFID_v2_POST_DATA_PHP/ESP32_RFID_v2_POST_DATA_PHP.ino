#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> //6.17.3
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>


char* ssid         = "delameta"; //isi dengan SSID WIFI
char* pass         = "bilano1407"; //isi dengan PASSWORD WIFI
//String HOST_NAME = "192.168.1.23"; // change to your PC's IP address
String HOST_NAME   = "toledp.000webhostapp.com"; // change to your PC's IP address
String PATH_NAME   = "/data-api.php?rfid=";
String NAMA_TOL    = "cibubur";
String getDataTol;

int buzzer=15;
#define SS_PIN 5
#define RST_PIN 27
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* id_dibaca; 
const char* rfid_dibaca; 
const char* nama_dibaca; 
const char* alamat_dibaca; 
const char* telepon_dibaca; 
const char* tanggal_dibaca;
const char* saldo_dibaca;
const char* namatol_dibaca;
const char* ceksaldo_dibaca;

int pinsensor=32;
int jeda=0;
void setup () {
  Serial.begin(115200);
  pinMode(pinsensor,INPUT);
  lcd.begin();
  WiFi.begin(ssid, pass);
  pinMode(buzzer,OUTPUT);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting..");
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("   SYSTEM TOL   ");
    lcd.setCursor(0,1); lcd.print("Menghubungkan...");
    delay(100);
  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connected!!!");
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("   SYSTEM TOL   ");
    lcd.setCursor(0,1); lcd.print("System Terhubung");
    buzzeroke();
    delay(1000);
  }
  else{
    Serial.println("Connected Failed!!!");
  }
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("   SYSTEM TOL   ");
    lcd.setCursor(0,1); lcd.print("TEMPELKAN KARTU");
    delay(1000);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    //START  JEDA 0 ke 1
    if(jeda==0){ //Jika keadaan Jeda masih 0 atau keadaan belum di Tap 
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
          //Serial.println(uidString);
          buzzer1x();
          HTTPClient http;  
          http.begin("http://" + String(HOST_NAME) + String(PATH_NAME) + String(uidString) +  "&&namatol=" + String(NAMA_TOL)); 
          int httpCode = http.GET();
      
          if (httpCode > 0) {
              //AMBIL DATA JSON
              String payload = http.getString();
              payload.trim();
              Serial.println(payload);
              const size_t capacity = JSON_OBJECT_SIZE(9) + 180; //cari dulu nilainya pakai Arduino Json 5 Asisten
              DynamicJsonDocument doc(capacity);
              //StaticJsonDocument<192> doc;
              DeserializationError error = deserializeJson(doc, payload);
              id_dibaca       = doc["id"]; 
              rfid_dibaca     = doc["rfid"]; 
              nama_dibaca     = doc["nama"]; 
              alamat_dibaca   = doc["alamat"]; 
              telepon_dibaca  = doc["telepon"]; 
              tanggal_dibaca  = doc["tanggal"];
              saldo_dibaca    = doc["saldo"];
              namatol_dibaca  = doc["namatol"];
              ceksaldo_dibaca = doc["ceksaldo"];
        
              //POST TO SERIAL
              //Serial.print("Waktu    = ");Serial.println(tanggal_dibaca);
              //Serial.print("ID       = ");Serial.println(id_dibaca);
              //Serial.print("RFID     = ");Serial.println(rfid_dibaca);
              //Serial.print("Nama     = ");Serial.println(nama_dibaca);
              //Serial.print("Alamat   = ");Serial.println(alamat_dibaca);
              //Serial.print("Telepon  = ");Serial.println(telepon_dibaca);
              //Serial.print("Saldo    = ");Serial.println(saldo_dibaca);
              //Serial.print("Cek Saldo= ");Serial.println(ceksaldo_dibaca);
              //Serial.print("Nama Tol = ");Serial.println(namatol_dibaca);
              //Serial.println();
              //delay(1000); 
            }
            if(String(id_dibaca)!="0" && String(ceksaldo_dibaca)=="cukup"){
              Serial.println("------------------------------------------------------------");
              Serial.println("PALANG DIBUKA!");
              Serial.println("------------------------------------------------------------");
              Serial.println("Selamat Jalan, Selamat Sampai Tujuan!");
              Serial.print("ID       = ");Serial.println(id_dibaca);
              Serial.print("RFID     = ");Serial.println(rfid_dibaca);
              Serial.print("Nama     = ");Serial.println(nama_dibaca);
              Serial.print("Saldo    = ");Serial.println(saldo_dibaca);
              Serial.print("Nama Tol = ");Serial.println(namatol_dibaca);
              Serial.println("------------------------------------------------------------");
              buzzeroke();
              jeda=1;
           }else if(String(id_dibaca)!="0" && String(ceksaldo_dibaca)=="kurang"){
              Serial.println("------------------------------------------------------------");
              Serial.println("Mohon Maaf Saldo Anda kurang, Segera Melakukan Top Up!");
              Serial.print("ID       = ");Serial.println(id_dibaca);
              Serial.print("RFID     = ");Serial.println(rfid_dibaca);
              Serial.print("Nama     = ");Serial.println(nama_dibaca);
              Serial.print("Saldo    = ");Serial.println(saldo_dibaca);
              Serial.print("Nama Tol = ");Serial.println(namatol_dibaca);
              Serial.println("------------------------------------------------------------");
              buzzeroke();
              buzzergagal();
           }else{
              Serial.println("------------------------------------------------------------");
              Serial.println("Mohon Maaf Kartu Anda Tidak Terdaftar!");
              Serial.print("RFID = "); Serial.println(rfid_dibaca);
              Serial.print("Nama Tol = ");Serial.println(namatol_dibaca);
              Serial.println("------------------------------------------------------------");
              buzzergagal();
           }
           //http.end();
    } 
    //END JEDA 0 ke 1

    //START MERUBAH JEDA 1 ke 0
    //LOGIKA UNTUK INFORMASI DAN MEMBUKA / MENUTUP PALANG MERUBAH JEDA
    int sensor=digitalRead(pinsensor); //BACA SENSOR INFRARED
      //Serial.print("Sensor =");Serial.println(sensor);
      //Serial.print("Jeda =");Serial.println(jeda);
    if(sensor==1 && jeda==1){ //Jika Sensor Infrared mendeteksi dan keadaan Jeda Sudah menjadi 1
        buzzer1x();
        Serial.println("------------------------------------------------------------");
        Serial.println("PALANG DITUTUP!");
        Serial.println("------------------------------------------------------------");
        jeda=0; //Merubah Kondisi jeda
        delay(1000);
    } 
    //END MERUBAH JEDA 0
    
 } //Tutup WL_CONNECTED
} //Untuk Loop

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
