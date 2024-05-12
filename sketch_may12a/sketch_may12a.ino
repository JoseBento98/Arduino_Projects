#include <ESP8266WiFi.h>
#include <OneWire.h>

const char* ssid = "********";
const char* password = "********";

const int pinDS18X20 = D4;
OneWire ds(pinDS18X20);

unsigned long previousMillis = 0;
const long interval = 1000;  // Intervalo de 1 segundo

void setup() {
  Serial.begin(115200);
  
  // Conectar-se à rede Wi-Fi
  Serial.printf("Conectando-se à rede %s ", ssid);
  WiFi.begin(ssid, password);

  // Esperar até que o dispositivo esteja conectado à rede Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Mostrar o endereço IP atribuído ao dispositivo
  Serial.println("");
  Serial.println("Conectado à rede Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    byte data[12];
    byte addr[8];

    if (!ds.search(addr)) {
      ds.reset_search();
      return;
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
      return;
    }

    if (addr[0] != 0x10 && addr[0] != 0x28 && addr[0] != 0x22) {
      return;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);  // Inicia a conversão de temperatura

    delay(1000);  // Aguarda a conversão ser concluída

    ds.reset();
    ds.select(addr);
    ds.write(0xBE);  // Envia comando de leitura do scratchpad

    // Lê os dados do scratchpad
    for (byte i = 0; i < 9; i++) {
      data[i] = ds.read();
    }

    int16_t raw = (data[1] << 8) | data[0];
    float celsius = (float)raw / 16.0;

    Serial.print(celsius);
    Serial.println("°C");
  }
}
