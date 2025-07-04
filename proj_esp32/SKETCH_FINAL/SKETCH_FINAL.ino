#include <WiFi.h>                     
#include <HTTPClient.h>
#include <SD.h>
#include <SPI.h>


//constantes da rede
const char* ssid = ""; //SSID da rede
const char* password = ""; //senha da rede

//link do webscript
const char* serverURL = "https://script.google.com/macros/s/AKfycbwzCLYZIODmRuY7YKJ2FEv1OL0mrttC8k4_EFFTNMjRR-3f62by_gT2Ndy9syv4Kc8/exec"; 

const int SD_CS_PIN = 5; //const pro cs pin

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  //setup pra escanear as redes wifi
  Serial.println("Escaneando as Redes: ");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    Serial.printf("%d: %s, RSSI: %d, %s\n", i + 1,
                  WiFi.SSID(i).c_str(),
                  WiFi.RSSI(i),
                  (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Encrypted");
  }

//se conecta a rede wifi definida nas constantes
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFalha na conexão");
  } else {
    Serial.println("\nConectado.");
  }


//iniciando o cartãoSD
  Serial.println("Inicializando SDCard");
  
  SPI.begin(18, 19, 23); 

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Falha ao inicializar.");
    return; 
  }
  
  Serial.println("Modulo SD Conectado.");

  uploadFilesFromSD("/"); //utilizando a função abaixo para definir o path como todo o cartãoSD, podemos especificar para dar maior precisão ao programa.
}

void loop() {
//loop vazio
}

//função para definir o caminho do cartãoSD utilizando uma constante que pode ser alocada dinamicamente como visto acima
void uploadFilesFromSD(const char* path) {
  File root = SD.open(path);
  if (!root) {
    Serial.println("Falha ao abrir diretorio.");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Não e um diretorio.");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      Serial.printf("Uploading %s (%d bytes)\n", file.name(), file.size());
      uploadFile(file);
    }
    file = root.openNextFile();
  }
}

//chamada da função anterior, utilizando conexão http para dar upload nos arquivos com a constante do webscript
//chunk de 4kb para estabilidade
void uploadFile(File file) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    String uploadURL = String(serverURL) + "?filename=" + String(file.name());
    http.begin(uploadURL);
    http.addHeader("Content-Type", "application/octet-stream"); 

    WiFiClient *stream = http.getStreamPtr();

    const size_t chunkSize = 4096;
    uint8_t buffer[chunkSize];
    
    while (file.available()) {
      size_t bytesRead = file.read(buffer, chunkSize);
      stream->write(buffer, bytesRead);
    }

    int httpResponseCode = http.POST((uint8_t *)NULL, 0); 
    if (httpResponseCode > 0) {
      Serial.printf("Upload terminado. HTTP Code: %d\n", httpResponseCode);
      Serial.println(http.getString());
    } else {
      Serial.printf("Upload falhou. Error code: %d\n", httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi desconectado, não e possivel dar upload.");
  }
  file.close();
}
