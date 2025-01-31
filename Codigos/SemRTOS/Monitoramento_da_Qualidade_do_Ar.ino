/*
  MQUnifiedsensor Library - Leitura do MQ - 135, MQ - 131
  Projeto com base da biblioteca MQUnifiedSensor, biblioteca criada por
  Miguel A Califa, Yersson Carrillo, Ghiordy Contreras, Mario Rodriguez
  Projeto adaptado por Noedson Romario Vieira da Silve, Nandson e Alife
 
  Projeto com finalidade de enviar dados para um gateway
  
  Modificado em 09 de Fevereiro de 2024
  por Noedson Romario Vieira da Silva

*/

//Inclusão de Bibliotecas
#include <MQUnifiedsensor.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <WiFi.h>
#include <NTPClient.h>

/************************Hardware Macros************************************/
#define Board ("Heltec Wifi LoRa Kit 32 (V2)")
#define Pin_MQ135 (32)  //Analog input 3 do Esp32
#define Pin_MQ131 (33)   //Analog input 4 do Esp32 
/***********************Software Macros************************************/
#define Type_MQ135 ("MQ-135")  //MQ135
#define Type_MQ131 ("MQ-131")  //MQ131
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)   // Para Esp32
#define RatioMQ135CleanAir (3.6)  //RS / R0 = 3.6 ppm
#define RatioMQ131CleanAir (15)   //RS / R0 = 15 ppm


/*Atualização... implementando cartão de memoria*/
#define SD_CS 12
#define SD_SCK 16
#define SD_MOSI 17
#define SD_MISO 23
#define Micro_SD_CS 12

#define nomeArquivo "/AvaliacaoDoDiaSemRTOS"
#define numeroMaximo 5000
#define numero_de_colunas 10
#define sufixoArquivo "csv"
/*Definição do WiFi*/
#define EAP_IDENTIDADE "erroso@unifesspa.edu.br"
#define EAP_USUARIO    "erroso@unifesspa.edu.br"
#define EAP_SENHA      "n1o9e9d4Q!"
const char *wifi = "UNIFESSPA";
/*****************************Globals***********************************************/
//Declaração dos sensores e configuração do LoraWan

#ifdef COMPILE_REGRESSION_TEST0
#define FILLMEIN 0
#else
#warning "Você deve substituir os valores marcados como FILLMEIN por valores reais obtidos no painel de controle do TTN!"
#define FILLMEIN (#Não edite isso, edite as linhas que usam FILLMEIN)
#endif


MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ135, Type_MQ135);
MQUnifiedsensor MQ131(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ131, Type_MQ131);
SPIClass sd_spi(HSPI);
File logArquivo;
WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP);

char nomeDoArquivo[300];
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
                          "O3MQ131"};

String dados_Enviados[numero_de_colunas];
/* Chave de sessão de rede LoraWan */
static const PROGMEM u1_t NETWORKSKEY[16] = {0xCC, 0x19, 0xBD, 0xBC, 0x0E, 0xEC, 0xF2, 0x01, 0x2A, 0xC4, 0xC4, 0x6C, 0x3E, 0xFF, 0xE7, 0x2C };

/*Chave de Aplicação LoraWan*/
static const u1_t PROGMEM APPSKEY[16] = {0xB6, 0x3B, 0x24, 0x99, 0xED, 0x7E, 0x11, 0x5A, 0xF0, 0x76, 0x3D, 0x67, 0x7A, 0x67, 0x0E, 0x56};
/*Endereço do Gateway */
static const u4_t DEVADDR = 0x01278d0b;
/*
Esses retornos de chamada são usados ​​apenas na ativação over-the-air, portanto, são
deixados em branco aqui (não podemos deixá-los completamente de fora, a menos que
DISABLE_JOIN está definido em arduino-lmic/project_config/lmic_project_config.h,
caso contrário, o compilador lançará um warning).
*/

void os_getArtEui(u1_t* buf) {}
void os_getDevEui(u1_t* buf) {}
void os_getDevKey(u1_t* buf) {}


static osjob_t sendjob;

/* Agenda o TX a cada um segundo (Tempo viavel para ver a transmisão dos dados) */

const unsigned TX_INTERVAL = 5;

/*Pinos usados para Heltec Wifi LoRa Kit 32 (V2) */

const lmic_pinmap lmic_pins = {
  .nss = 18,  // LoRa_MISO
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,  // Pino de Reset
  .dio = { 26 /*LORA DIO0*/, 34 /*LORA DIO2*/, 35 /*LORA DIO1 */ },
};
/*função para conectar ao WiFi*/



void conectaWiFi(){
 WiFi.disconnect(true);
 WiFi.mode(WIFI_STA);

 WiFi.begin(wifi,WPA2_AUTH_PEAP,EAP_IDENTIDADE,EAP_USUARIO,EAP_SENHA);

 while(WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print(".");
  contador++;
  if(contador >= 30){
    Serial.println(" ");
    ESP.restart();
  }
 }
Serial.println("");
Serial.println("Wifi Conectado com Sucesso");
Serial.println("Com o IP: ");
Serial.println(WiFi.localIP());
ntp.begin();
ntp.setTimeOffset(-10800);
}
/*função que vai verificar os arquivos do cartão de memória*/



void criaArquivo(){
  String data;
  if(ntp.update()){data = ntp.getFormattedDate(0,2);}else{data = ntp.getFormattedDate(0,2);}
  for(int i = 0;i<numeroMaximo;i++){
    memset(nomeDoArquivo,0,sizeof(nomeDoArquivo));
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
    Serial.println("Não conseguiu escrever no SD, o arquivo não foi aberto");
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

/*Função que vai avaliar o Evento do Lora*/
void onEvent(ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    /*
        || Este evento está definido, mas não é utilizado no código. Não
        || faz sentido desperdiçar espaço de código com isso.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (Inclui aguardar pelas janelas de RX.)"));
      digitalWrite(LED, LOW);
      escreve_no_SD(dados_Enviados);
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Pacote recebido"));
      if (LMIC.dataLen) {
        Serial.println(F("Recebido "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes  de carga útil"));
      }
      // Agenda uma nova Transmissão
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // Dados recebidos no ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    /*
        || Este evento está definido, mas não é utilizado no código. Não faz sentido desperdiçar espaço de código com isso.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      //digitalWrite(LED, HIGH);
      break;
    case EV_TXCANCELED:
      Serial.println(F("EV_TXCANCELED"));
      break;
    case EV_RXSTART:
      /* Não imprime nada - isso prejudica o cronometragem. */
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE: Não foi Aceito"));
      break;
    default:
      Serial.print(F("Evento Desconhecido: "));
      Serial.println((unsigned)ev);
      break;
  }
}

void do_send(osjob_t* j) {
  // Verifica se não há um trabalho de TX/RX em andamento no momento.
  Serial.println("Entrando na função do_send()");
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, Não esta enviando"));
  } else {
    
    // Prepara a transmissão de dados upstream na próxima oportunidade possível.

    /*-------------------------Aqui será rodado o codigo dos sensores----------------------------------------*/

    MQ135.update();  // Update data, O arduino vai ler a voltagem do pino analogico do esp32
    MQ131.update();  // Update data, O arduino vai ler a voltagem do pino analogico do esp32


    /*--Sensibilidade do Sensor MQ135--
    Exponential regression:
    GAS     | a      | b
    CO      | 605.18 | -3.937
    CO2     | 110.47 | -2.862
    Alcool  | 77.255 | -3.18
    Toluen  | 44.947 | -2.862
    NH4     | 102.2  | -2.473
    Aceton  | 34.668  | -3.369
    ---------------------------------*/

   
    MQ135.setA(605.18);
    MQ135.setB(-3.937);                   // Configura a equação para calcular a concentração de CO
    float CO_MQ135 = MQ135.readSensor();  // O sensor lerá a concentração de PPM usando o modelo, valores a e b definidos anteriormente ou na configuração


    MQ135.setA(77.255);
    MQ135.setB(-3.18);                        // Configura a equação para calcular a concentração de Alcool
    float Alcool_MQ135 = MQ135.readSensor();  // O sensor lerá a concentração de PPM usando o modelo, valores a e b definidos anteriormente ou na configuração


    MQ135.setA(110.47);
    MQ135.setB(-2.862);                    // Configura a equação para calcular a concentração de CO2
    float CO2_MQ135 = MQ135.readSensor();  // O sensor lerá a concentração de PPM usando o modelo, valores a e b definidos anteriormente ou na configuração


    MQ135.setA(44.947);
    MQ135.setB(-3.445);                       // Configura a equação para calcular a concentração de Tolueno
    float Toluen_MQ135 = MQ135.readSensor();  // O sensor lerá a concentração de PPM usando o modelo, valores a e b definidos anteriormente ou na configuração



    MQ135.setA(102.2);
    MQ135.setB(-2.473);                    // Configura a equação para calcular a concentração de MH4
    float MH4_MQ135 = MQ135.readSensor();  // O sensor lerá a concentração de PPM usando o modelo, valores a e b definidos anteriormente ou na configuração


    MQ135.setA(34.668);
    MQ135.setB(-3.369);                       // Configura a equação para calcular a concentração de Acetona
    float Aceton_MQ135 = MQ135.readSensor();  // O sensor lerá a concentração de PPM usando o modelo, valores a e b definidos anteriormente ou na configuração

    MQ131.setA(23.943); 
    MQ131.setB(-1.11);
    float Ozone = MQ131.readSensor();

    Serial.println("    |");

   
                                                       /*--------------------------------------------------------------------------------------------------------*/

    String mq_messagem = String("CO_MQ135|")     + String(CO_MQ135) + 
                         String(",Alcool_MQ135|") + String(Alcool_MQ135) +
                         String(",CO2_MQ135|")    + String(CO2_MQ135)   + 
                         String(",Toluen_MQ135|") + String(Toluen_MQ135)   + 
                         String(",MH4_MQ135|")    + String(MH4_MQ135)   + 
                         String(",Aceton_MQ135|") + String(Aceton_MQ135)   + 
                         String(",O3_MQ131|")     + String(Ozone);
                           
    
    if(ntp.update()){ dados_Enviados[0] =  ntp.getFormattedDate(0,1); dados_Enviados[1] = ntp.getFormattedTimeMillis();
    }else{ dados_Enviados[0] =  ntp.getFormattedDate(0,1); dados_Enviados[1] = ntp.getFormattedTimeMillis();}
    dados_Enviados[2] =  String(CO_MQ135);
    dados_Enviados[3] =  String(Alcool_MQ135);
    dados_Enviados[4] =  String(CO2_MQ135);
    dados_Enviados[5] =  String(Toluen_MQ135);
    dados_Enviados[6] =  String(MH4_MQ135);
    dados_Enviados[7] =  String(Aceton_MQ135);
    dados_Enviados[8] = String(Ozone);
    

    const char* mensagem_do_convertido_sensor = mq_messagem.c_str();

    // Instrução responsavel para o envio dos dados do sensor ao GateWay
    LMIC_setTxData2(1, (uint8_t*)mensagem_do_convertido_sensor, strlen(mensagem_do_convertido_sensor), 0);
    Serial.println(F("Pacote Enfileirado"));
    Serial.print(F("Mandando na Frequencia: "));
    Serial.println(LMIC.freq);
    Serial.println(" ");
    Serial.println(mensagem_do_convertido_sensor);
    Serial.println(strlen(mensagem_do_convertido_sensor));
    
    dados_Enviados[9] = String(strlen(mensagem_do_convertido_sensor));

    

    digitalWrite(LED, HIGH);
  }
  // O próximo TX é agendado após o evento TX_COMPLETE.
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

void setup() {
  //Init the serial port communication - to debug the library
  Serial.begin(115200);  //Init serial port
  pinMode(LED, OUTPUT);
  conectaWiFi();
  SPI.begin(5, 19, 27, 18);
  sd_spi.begin(SD_SCK, SD_MISO, SD_MOSI,SD_CS);

  // tela.begin();
  // tela.setFont(u8g2_font_NokiaSmallPlain_te );

  // LMIC init
  os_init();
  LMIC_reset();
#ifdef PROGMEM
  // Em AVR, esses valores são armazenados na memória flash e só são copiados para a RAM
  // uma vez. Copie-os para um buffer temporário aqui, o LMIC_setSession
  // os copiará novamente para um buffer próprio.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NETWORKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NETWORKSKEY, sizeof(NETWORKSKEY));
  LMIC_setSession(0x1, DEVADDR, nwkskey, appskey);
#else
  // Se não estiver executando em um AVR com PROGMEM, basta usar os arrays diretamente.
  LMIC_setSession(0x1, DEVADDR, NWKSKEY, APPSKEY);
#endif
  //Defina o modelo matemático para calcular a concentração de PPM e o valor das constantes.


  /*Como não usaremos qualquer outra banda, colocaremos somente em uma frequencia*/

#if defined(CFG_us915) || defined(CFG_au915)
  LMIC_selectSubBand(0);
#else
#error Regiao nao suportada
#endif

  //Desabilida a validação de link

  LMIC_setLinkCheckMode(0);
  LMIC_setAdrMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF12;

  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(DR_SF12, 10);

  // Inicia a Transferencia




  MQ135.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ131.setRegressionMethod(1);  //_PPM =  a*ratio^b
  

  /*****************************  MQ Init ********************************************/
  //Remarks: Configure the pin of arduino as input.
  /************************************************************************************/

  MQ135.init();
  MQ131.init();


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

  if ((isinf(calcR0MQ135) || isinf(calcR0MQ131))){


    Serial.print("RO calcR0MQ135: ");
    Serial.print(calcR0MQ135);

    Serial.print("RO calcR0MQ131: ");
    Serial.print(calcR0MQ131);
    delay(1000);
    ESP.restart();
  }
  if (calcR0MQ135 == 0 || calcR0MQ131 == 0) {

    Serial.print("RO calcR0MQ135: ");
    Serial.print(calcR0MQ135);

    Serial.print("RO calcR0MQ131: ");
    Serial.print(calcR0MQ131);

    delay(1000);
    ESP.restart();
    
  }
  iniciar_cartao();
  
  delay(20000);
  
 do_send(&sendjob);
 
}

void loop() {

  os_runloop_once();
}