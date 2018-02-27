#include "DHT.h"
#include "Ethernet.h"

#define PinA_A0 A0 //Entrada Analógica do Sensor do SOLO
#define PinA_A1 A1 //Entrada Analógica do Sensor do AR
#define PinL_R7 7
#define PinL_Y6 6
#define PinL_G5 5
#define DHTTYPE DHT11 //Modelo do Sensor de AR

byte mac[]= {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,40,179);
IPAddress gateway(192,168,40,2);
IPAddress subnet(255,255,255,0);
EthernetServer server(80);

DHT dht(PinA_A1, DHTTYPE);

int ValueA0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(PinA_A0, INPUT);
  pinMode(PinL_R7, OUTPUT);
  pinMode(PinL_Y6, OUTPUT);
  pinMode(PinL_G5, OUTPUT);
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
}

void loop() {
  ValueA0 = analogRead(PinA_A0);
  float UmidAr = dht.readHumidity();
  float Temp = dht.readTemperature();
  Serial.print("Porta analogica: ");
  Serial.println(ValueA0);
  EthernetClient client = server.available();

  if (isnan(Temp) || isnan(UmidAr))
  {
    Serial.println("Falha ao ler a Umidade/Temperatura!");
  }
  else
  {
    Serial.print("Umidade: ");
    Serial.print(UmidAr);
    Serial.print(" Temperatura: ");
    Serial.print(Temp);
    Serial.println(" *C");
  }
  if (ValueA0 > 0 && ValueA0 < 400)
  {
    Serial.println("Status: Solo umido");
    apagaLeds();
    digitalWrite(PinL_G5, HIGH);
  }
  if (ValueA0 > 400 && ValueA0 < 800)
  {
    Serial.println("Status: Umidade moderado");
    apagaLeds();
    digitalWrite(PinL_Y6, HIGH);
  }
  if (ValueA0 > 800 && ValueA0 < 1024)
  {
    Serial.println("Status: Solo seco");
    apagaLeds();
    digitalWrite(PinL_R7, HIGH);
  }
  
  if (client)
  {
    Serial.println("New client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == 'n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 2");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.print("<font color=#FF0000><b><u>");
          client.print("Envio de informacoes pela rede utilizando Arduino");
          client.print("</u></b></font>");
          client.println("<br /><br />");
          int porta_digital = digitalRead(3);
          client.print("Porta Digital 3: ");
          client.print("<b>");
          client.print(porta_digital);
          client.println("</b>");
          client.print("  (0 = Desligada, 1 = Ligada)");
          client.println("<br /><br />");
          //Mostra as informacoes lidas pelo sensor do AR
          client.print("<font color=#1E90FF><b><u>");
          client.print("Informacoes do Ar");
          client.print("</u></b></font><br />");
          client.print("Temperatura: ");
          client.print("<b>");
          client.print(Temp);
          client.print(" *C");
          client.println("</b><br />");
          client.print("Umidade: ");
          client.print("<b>");
          client.print(UmidAr);
          client.println("</b><br /><br />");
          client.print("<font color=#1E90FF><b><u>");
          client.print("Informacoes do Solo");
          client.print("</u></b></font><br />");
          client.print("Umidade do Solo: ");
          client.print("<b>");
          client.print(ValueA0);
          client.println("<br />800 ate 1024 = Solo Seco<br />400 ate 800 = Solo Moderado<br />0 ate 400 = Solo Umido</b></html>");
          break;
        }
        if (c == 'n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != 'r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    delay(1000);
    client.stop();
  }
}
  
void apagaLeds() {
  digitalWrite(PinL_G5, LOW);
  digitalWrite(PinL_Y6, LOW); 
  digitalWrite(PinL_R7, LOW);
}

