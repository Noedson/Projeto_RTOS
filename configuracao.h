#include <driver/adc.h>
#include <MQUnifiedsensor.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_task_wdt.h>
#include <SD.h>
#include <Wire.h>
#include <WiFi.h>
#include <NTPClient.h>
#include "DHT.h"

/************************Hardware Macros************************************/
#define Board ("Heltec Wifi LoRa Kit 32 (V2)")

#define Pin_MQ135 (32)  //Analog input 3 do Esp32
#define Pin_MQ131 (33)   //Analog input 4 do Esp32 
// #define DHTPINO (13)
/***********************Software Macros************************************/

#define Type_MQ135 ("MQ-135")  //MQ135
#define Type_MQ131 ("MQ-131")  //MQ131
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)   // Para Esp32
#define RatioMQ135CleanAir (3.6)  //RS / R0 = 3.6 ppm
#define RatioMQ131CleanAir (15) //RS / R0 = 15 ppm
#define DHTTYPE DHT11

/********************Confiuração dos pinos SD***************************************/
#define SD_CS 12
#define SD_SCK 16
#define SD_MOSI 17
#define SD_MISO 23
#define Micro_SD_CS 12

#define nomeArquivo "/AvaliacaoDoDiaComRTOS "
#define numeroMaximo 5000
#define numero_de_colunas 10
#define sufixoArquivo "csv"
/*Definição WiFi*/
#define EAP_IDENTIDADE "erroso@unifesspa.edu.br"
#define EAP_USUARIO    "erroso@unifesspa.edu.br"
#define EAP_SENHA      "n1o9e9d4Q!"
#define EAP_SENHA2      "chicobento"
const char *wifi = "UNIFESSPA";
const char *wifi2 = "ACCESS-POINT-TEST";
/*****************************Globals***********************************************/
//Declaração dos sensores e configuração do LoraWan

#ifdef COMPILE_REGRESSION_TEST
#define FILLMEIN 0
#else
#warning "Você deve substituir os valores marcados como FILLMEIN por valores reais obtidos no painel de controle do TTN!"
#define FILLMEIN (#Não edite isso, edite as linhas que usam FILLMEIN)
#endif

MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ135, Type_MQ135);
MQUnifiedsensor MQ131(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ131, Type_MQ131);
// DHT dht(DHTPINO, DHTTYPE);
SPIClass sd_spi(HSPI);
File logArquivo;
WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP);

char nomeDoArquivo[100];
int contador = 0;
String cabecalho[numero_de_colunas] = 
                        { "Data",
                          "Horas",
                          "COMQ135",       
                          "AlcoolMQ135",  
                          "CO2MQ135",     
                          "ToluenMQ135",   
                          "MH4MQ135",      
                          "AcetonMQ135",   
                          "O3MQ131" ,
                          "bytes" };
String dados_Enviados[numero_de_colunas];

/*função que vai verificar os arquivos do cartão de memória*/
void criaArquivo(){

  for(int i = 0;i<numeroMaximo;i++){
    memset(nomeDoArquivo,0,sizeof(nomeDoArquivo));
    String data;
    if(ntp.update()){data = ntp.getFormattedDate(0,2);}else{data = ntp.getFormattedDate(0,2);}
    sprintf(nomeDoArquivo, "%s_%s_%d.%s",nomeArquivo , data , i, sufixoArquivo );
     for (int j = 0; j < strlen(nomeDoArquivo); j++) {
      if (nomeDoArquivo[j] == ' ') {
        nomeDoArquivo[j] = '-';
      }
    }
    Serial.println(nomeDoArquivo);
    if(!SD.exists(nomeDoArquivo)){
   
      Serial.println("ponto 1");
      break;
    }else{
      Serial.println(nomeDoArquivo);
      Serial.println("Já Existe");
      Serial.println("ponto 2");
  }
}
   
  Serial.print("Nome do arquivo: ");
  Serial.println(nomeDoArquivo);

}

/*Função que vai abrir e escrever no cartão*/
void escreve_no_SD(String dados[numero_de_colunas]){
  logArquivo = SD.open(nomeDoArquivo, FILE_APPEND);
  if(!logArquivo){
    Serial.println("Tentando Novamente");
    escreve_no_SD(dados_Enviados);
  }else{
    Serial.println("Escreveu no micro");
    for(int i = 0; i < numero_de_colunas; i++){
      Serial.println("entrando no laço para gravar");
      Serial.println(i);
      logArquivo.print(dados[i]);
      if(i < numero_de_colunas-1){
        logArquivo.print(";");
      }else{
        logArquivo.println();
      }
      
    }
  }logArquivo.close();
 
}

void iniciar_cartao(){
  if(!SD.begin(SD_CS,sd_spi)){
    Serial.println("Erro ao iniciar Cartão SD");
    iniciar_cartao();
  }else{
  Serial.println("SD iniciado");
  criaArquivo();
  escreve_no_SD(cabecalho);
  }
}

void conectaWiFi(){
 WiFi.disconnect(true);
 WiFi.mode(WIFI_STA);

 WiFi.begin(wifi,WPA2_AUTH_PEAP,EAP_IDENTIDADE,EAP_USUARIO,EAP_SENHA);
//  WiFi.begin(wifi2,EAP_SENHA2);

 while(WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print(".");
  contador++;
  if(contador >= 30){
    Serial.println(" ");
    ESP.restart();
    contador++; 
  }
 }
Serial.println("");
Serial.println("Wifi Conectado com Sucesso");
Serial.println("Com o IP: ");
Serial.println(WiFi.localIP());
ntp.begin();
ntp.setTimeOffset(-10800);
}

void calibacao(){
  conectaWiFi();
  sd_spi.begin(SD_SCK, SD_MISO, SD_MOSI,SD_CS);
  
  MQ135.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ131.setRegressionMethod(1);  //_PPM =  a*ratio^b
  

  /*****************************  MQ Init ********************************************/
  //Remarks: Configure the pin of arduino as input.
  /************************************************************************************/
 
  MQ135.init();
  MQ131.init();
  // dht.begin();
  
  /* 
    //Se o valor RL for diferente de 10K, atribua seu valor RL com o seguinte método:
    MQ9.setRL(10);
  */
  /*****************************  MQ CAlibration ********************************************/
  // Explicação:
  // Nesta rotina o sensor medirá a resistência do sensor supostamente antes de ser pré-aquecido
  // e em ar limpo (condições de calibração), configurando o valor R0.
  // Recomendamos executar esta rotina apenas na configuração em condições de laboratório.
  // Esta rotina não precisa ser executada a cada reinicialização, você pode carregar seu valor R0 do eeprom.
  // Reconhecimentos: https://jayconsystems.com/blog/understanding-a-gas-sensor
  Serial.print("Calibrating please wait.");
  
  float calcR0MQ135 = 0;
  float calcR0MQ131 = 0;

  for (int i = 1; i <= 10; i++) {
    
    MQ135.update();
    MQ131.update();
    
    
    calcR0MQ135 += MQ135.calibrate(RatioMQ135CleanAir);
    calcR0MQ131 += MQ131.calibrate(RatioMQ131CleanAir);
    
    Serial.print(".");
  }
 
  MQ135.setR0(calcR0MQ135 / 10);
  MQ131.setR0(calcR0MQ131 / 10);
  Serial.println("Feito!");

  if (isinf(calcR0MQ135) || isinf(calcR0MQ131)) {
    
    Serial.print("RO calcR0MQ135: ");
    Serial.print(calcR0MQ135);

    Serial.print("RO calcR0MQ131: ");
    Serial.print(calcR0MQ131);
    
    ESP.restart();
  }
  if (calcR0MQ135 == 0 || calcR0MQ131 == 0) {


    Serial.print("RO calcR0MQ135: ");
    Serial.print(calcR0MQ135);

    Serial.print("RO calcR0MQ131: ");
    Serial.print(calcR0MQ131);

    ESP.restart();
  }
    iniciar_cartao();
}

