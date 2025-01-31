#include "configuracao.h"
#include "chaves.h"
#include "pinosLoraWan.h"
#include "filas.h"
#include "sensores.h"
#include "temporizacao.h"
#include "Staks.h"


void os_getArtEui(u1_t *buf) {}
void os_getDevEui(u1_t *buf) {}
void os_getDevKey(u1_t *buf) {}

static osjob_t sendjob;

const lmic_pinmap lmic_pins = {
  .nss = LORA_NSS,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = RADIO_PORTA_RESET,
  .dio = { RADIO_DIO_0_PORTA, RADIO_DIO_2_PORTA, RADIO_DIO_1_PORTA },
};

/*Semaforos*/
SemaphoreHandle_t xI2C_semaforo;
SemaphoreHandle_t xSerial_semaforo;





void onEvent(ev_t ev) {
  if (xSemaphoreTake(xSerial_semaforo, TEMPO_PARA_OBTER_SEMAFORO) == pdTRUE) {
    Serial.print(os_getTime());
    Serial.print(": ");
    Serial.println(ev);
    xSemaphoreGive(xSerial_semaforo);
  }
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      break;
    case EV_BEACON_FOUND:
      break;
    case EV_BEACON_MISSED:
      break;
    case EV_BEACON_TRACKED:
      break;
    case EV_JOINING:
      break;
    case EV_JOINED:
      break;
    case EV_JOIN_FAILED:
      break;
    case EV_REJOIN_FAILED:
      break;
    case EV_TXCOMPLETE:
      if (xSemaphoreTake(xSerial_semaforo, TEMPO_PARA_OBTER_SEMAFORO) != pdTRUE) {
        break;
      }

      /* COntrole do semáforo serial obtido. Printa na serial as informações do evento. */
      Serial.println(millis());
      Serial.println(F("EV_TXCOMPLETE (incluindo espera pelas janelas de recepção)"));
      escreve_no_SD(dados_Enviados);


      /* Verifica se ack foi recebido do gateway */
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Ack recebido"));

      /* Verifica se foram recebidos dados do gateway */
      if (LMIC.dataLen > 0) {
        Serial.println(F("Recebidos "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes (payload) do gateway"));

        /* Como houve recepção de dados do gateway, os coloca
                   em um array para uso futuro. */
        uint8_t dados_recebidos = LMIC.frame[LMIC.dataBeg + 0];
        Serial.print(F("Dados recebidos: "));
        Serial.write(dados_recebidos);
      }

      /* Devolve o controle do semáforo da serial */
      xSemaphoreGive(xSerial_semaforo);

      break;

    case EV_LOST_TSYNC:
      break;
    case EV_RESET:
      break;
    case EV_RXCOMPLETE:
      break;
    case EV_LINK_DEAD:
      break;
    case EV_LINK_ALIVE:
      break;
    case EV_TXSTART:
      if (xSemaphoreTake(xSerial_semaforo, TEMPO_PARA_OBTER_SEMAFORO) == pdTRUE) {
        Serial.println(F("EV_TXSTART"));
        Serial.println(millis());
        Serial.print("Frequencia: ");
        Serial.println(LMIC.freq);
        xSemaphoreGive(xSerial_semaforo);
      }
      break;
    default:
      break;
  }
}

void envia(osjob_t *j) {
  float leitura_CO_MQ135;
  float leitura_Alcool_MQ135;
  float leitura_CO2_MQ135;
  float leitura_Toluen_MQ135;
  float leitura_MH4_MQ135;
  float leitura_Aceton_MQ135;
  float leitura_Ozone;
  float leitura_Temperatura;
  float leitura_Humidade;
  BaseType_t resultados_peek_fila;

  esp_task_wdt_reset();

  /*colocand nas variaveis cada leitura detectada nos sensores*/
    do {
    resultados_peek_fila = xQueuePeek(xFila_CO_MQ135, (void *)&leitura_CO_MQ135, TEMPO_PARA_LER_FILAS);
    if (resultados_peek_fila == pdTRUE) {
      Serial.print("CO: ");
      Serial.println(leitura_CO_MQ135);
    } else {
      Serial.println("saindo do laço");
    }
  } while (resultados_peek_fila != pdTRUE);
  esp_task_wdt_reset();
  do {
    resultados_peek_fila = xQueuePeek(xFila_CO2_MQ135, (void *)&leitura_CO2_MQ135, TEMPO_PARA_LER_FILAS);
    if (resultados_peek_fila == pdTRUE) {
      Serial.print("CO2: ");
      Serial.println(leitura_CO2_MQ135);
    } else {
      Serial.println("saindo do laço");
    }
  } while (resultados_peek_fila != pdTRUE);
  esp_task_wdt_reset();

  do {
    resultados_peek_fila = xQueuePeek(xFila_Alcool_MQ135, (void *)&leitura_Alcool_MQ135, TEMPO_PARA_LER_FILAS);
    if (resultados_peek_fila == pdTRUE) {
      Serial.print("Alcool: ");
      Serial.println(leitura_Alcool_MQ135);
    } else {
      Serial.println("saindo do laço");
    }
  } while (resultados_peek_fila != pdTRUE);
  esp_task_wdt_reset();

  do {
    resultados_peek_fila = xQueuePeek(xFila_Toluen_MQ135, (void *)&leitura_Toluen_MQ135, TEMPO_PARA_LER_FILAS);
    if (resultados_peek_fila == pdTRUE) {
      Serial.print("Tolueno: ");
      Serial.println(leitura_Toluen_MQ135);
    } else {
      Serial.println("saindo do laço Toluen MQ135");
    }
  } while (resultados_peek_fila != pdTRUE);
  esp_task_wdt_reset();

  do {
    resultados_peek_fila = xQueuePeek(xFila_MH4_MQ135, (void *)&leitura_MH4_MQ135, TEMPO_PARA_LER_FILAS);
    if (resultados_peek_fila == pdTRUE) {
      Serial.print("MHA: ");
      Serial.println(leitura_MH4_MQ135);
    } else {
      Serial.println("saindo do laço MH4");
    }
  } while (resultados_peek_fila != pdTRUE);
  esp_task_wdt_reset();

  do {
    resultados_peek_fila = xQueuePeek(xFila_Aceton_MQ135, (void *)&leitura_Aceton_MQ135, TEMPO_PARA_LER_FILAS);
    if (resultados_peek_fila == pdTRUE) {
      Serial.print("Acetona: ");
      Serial.println(leitura_Aceton_MQ135);
    } else {
      Serial.println("saindo do laço");
    }
  } while (resultados_peek_fila != pdTRUE);
  esp_task_wdt_reset();

  do {
    resultados_peek_fila = xQueuePeek(xFila_Ozone, (void *)&leitura_Ozone, TEMPO_PARA_LER_FILAS);
    if (resultados_peek_fila == pdTRUE) {
      Serial.print("Ozonio: ");
      Serial.println(leitura_Ozone);
    } else {
      Serial.println("saindo do laço");
    }
  } while (resultados_peek_fila != pdTRUE);
  esp_task_wdt_reset();

  //  do {
  //   resultados_peek_fila = xQueuePeek(xFila_Temperatura, (void *)&leitura_Temperatura, TEMPO_PARA_LER_FILAS);
  //   if (resultados_peek_fila == pdTRUE) {
  //     Serial.print("Temperatura: ");
  //     Serial.println(leitura_Temperatura);
  //   } else {
  //     Serial.println("saindo do laço");
  //   }
  // } while (resultados_peek_fila != pdTRUE);
  // esp_task_wdt_reset();

  //  do {
  //   resultados_peek_fila = xQueuePeek(xFila_Humidade, (void *)&leitura_Humidade, TEMPO_PARA_LER_FILAS);
  //   if (resultados_peek_fila == pdTRUE) {
  //     Serial.print("Humidade: ");
  //     Serial.println(leitura_Humidade);
  //   } else {
  //     Serial.println("saindo do laço");
  //   }
  // } while (resultados_peek_fila != pdTRUE);
  // esp_task_wdt_reset();

 
  xSemaphoreGive(xSerial_semaforo);

String mensagem_RTOS = String(",CO_MQ135|")     + String(leitura_CO_MQ135) +
                       String(",CO2_MQ135|")    + String(leitura_CO2_MQ135) + 
                       String(",Alcool_MQ135|") + String(leitura_Alcool_MQ135) +
                       String(",Aceton_MQ135|") + String(leitura_Aceton_MQ135) + 
                       String(",Toluen_MQ135|") + String(leitura_Toluen_MQ135) +
                       String(",MH4_MQ135|")    + String(leitura_MH4_MQ135)/* + 
                       String(",Temperatura|")  + String(leitura_Temperatura) +
                       String(",Humidade|")     + String(leitura_Humidade)*/ + 
                       String(",O3_MQ131|")    + String(leitura_Ozone); 
 
// String mensagem_RTOS = /*String(",C|") +*/ String(leitura_CO_MQ135) +
//                       /*String(",C2|") +*/ String(leitura_CO2_MQ135) +
//                        /*String(",A|") +*/ String(leitura_Alcool_MQ135) +
//                        /*String(",Ac|") +*/ String(leitura_Aceton_MQ135) +
//                        /*String(",T|") + */ String(leitura_Toluen_MQ135) +
//                        /*String(",M|") +*/ String(leitura_MH4_MQ135) +
//                        /*String(",T|") +*/ String(leitura_Temperatura) +
//                        /*String(",H|") +*/ String(leitura_Humidade);

  if(ntp.update()){dados_Enviados[0] =  ntp.getFormattedDate(0,1); dados_Enviados[1] = ntp.getFormattedTime();}else{dados_Enviados[0] =  ntp.getFormattedDate(0,1); dados_Enviados[1] = ntp.getFormattedTime();}
  dados_Enviados[2] =  String(leitura_CO_MQ135);
  dados_Enviados[3] =  String(leitura_Alcool_MQ135);
  dados_Enviados[4] =  String(leitura_CO2_MQ135);
  dados_Enviados[5] =  String(leitura_Toluen_MQ135);
  dados_Enviados[6] =  String(leitura_MH4_MQ135);
  dados_Enviados[7] =  String(leitura_Aceton_MQ135);
  dados_Enviados[8] =  String(leitura_Ozone);
  

  const char* mensagem_convertida = mensagem_RTOS.c_str();


  /*Verifica se tem envio sendo feito*/
  if (LMIC.opmode & OP_TXRXPEND) {
    if (xSemaphoreTake(xSerial_semaforo, TEMPO_PARA_OBTER_SEMAFORO) == pdTRUE) {
      Serial.println(F("OP_TXRXEND: há um envio pedente, portanto o envio atual não será feito"));
      xSemaphoreGive(xSerial_semaforo);
    }
  } else {
    LMIC_setTxData2(4, (uint8_t *)mensagem_convertida, strlen(mensagem_convertida), 0);
    dados_Enviados[9] = String(strlen(mensagem_convertida));
    if (xSemaphoreTake(xSerial_semaforo, TEMPO_PARA_OBTER_SEMAFORO) == pdTRUE) {
      Serial.println(F("Pacote na fila de Envio"));
      xSemaphoreGive(xSerial_semaforo);
    }
  }
  esp_task_wdt_reset();
}


void task_envio_LoRaWan(void *pvParameters) {
  Serial.println("Entrando na Tarefa envio_LoRaWan");
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t networkskey[sizeof(NETWORKSKEY)];

  SPI.begin(RADIO_SCLK_PORTA, RADIO_MISO_PORTA, RADIO_MOSI_PORTA, LORA_NSS);

  os_init();
  LMIC_reset();

  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(networkskey, NETWORKSKEY, sizeof(NETWORKSKEY));

  LMIC_setSession(0x1, DEVADDR, networkskey, appskey);

#if defined(CFG_us915) || defined(CFG_au915)
  LMIC_selectSubBand(0);
#else
#error Regiao nao suportada
#endif

  for (int b = 0; b < 8; b++) {
    LMIC_disableSubBand(b);
  }

  LMIC_enableChannel(0);  // 915.2 MHz
  LMIC_enableChannel(1);  // 915.4 MHz
  LMIC_enableChannel(2);  // 915.6 MHz
  LMIC_enableChannel(3);  // 915.8 MHz
  LMIC_enableChannel(4);  // 916.0 MHz
  LMIC_enableChannel(5);  // 916.2 MHz
  LMIC_enableChannel(6);  // 916.4 MHz
  LMIC_enableChannel(7);  // 916.6 MHz

  LMIC_setLinkCheckMode(0);
  LMIC_setAdrMode(0);

  LMIC.dn2Dr = DR_SF12;

  LMIC_setDrTxpow(DR_SF12, GANHO_LORA_DBM);
 ostime_t proximaTransmissao = os_getTime() + sec2osticks(TX_INTERVAL);

  while (1) {

    esp_task_wdt_reset();
    if (os_getTime() >= proximaTransmissao) { 
      envia(&sendjob);
      proximaTransmissao = os_getTime() + sec2osticks(TX_INTERVAL); 
      }
    
    os_runloop_once();
    vTaskDelay((TickType_t)1);
  }
}
void task_sensores(void *pvParameters) {
  Serial.println("Entrando na Tarefa envio_sensores");

  float CO_MQ135;
  float Alcool_MQ135;
  float CO2_MQ135;
  float Toluen_MQ135;
  float MH4_MQ135;
  float Aceton_MQ135;
  float Ozone;
  float temperatura;
  float humidade;

  esp_task_wdt_add(NULL);
  while (1) {
    esp_task_wdt_reset();

    
    MQ135.update();  // Update data, O arduino vai ler a voltagem do pino analogico do esp32
    MQ131.update();  //Update data, O arduino vai ler a voltagem do pino analogico do esp32

  
    CO_MQ135 = MQ135_CO();
    Alcool_MQ135 = MQ135_Alcool();
    CO2_MQ135 = MQ135_CO2();
    Toluen_MQ135 = MQ135_Toluen();
    Aceton_MQ135 = MQ135_Aceton();
    MH4_MQ135 = MQ135_MH4();
    Ozone = MQ131_Ozone();
    // temperatura = dhtTemperatura();
    // humidade = dhtHumidade();
    
    xQueueOverwrite(xFila_CO_MQ135, (void *)&CO_MQ135);
    xQueueOverwrite(xFila_CO2_MQ135, (void *)&CO2_MQ135);
    xQueueOverwrite(xFila_Alcool_MQ135, (void *)&Alcool_MQ135);
    xQueueOverwrite(xFila_Toluen_MQ135, (void *)&Toluen_MQ135);
    xQueueOverwrite(xFila_Aceton_MQ135, (void *)&Aceton_MQ135);
    xQueueOverwrite(xFila_MH4_MQ135, (void *)&MH4_MQ135);
    xQueueOverwrite(xFila_Ozone, (void *)&Ozone);
    // xQueueOverwrite(xFila_Temperatura, (void *)&temperatura);
    // xQueueOverwrite(xFila_Humidade, (void *)&humidade);



    vTaskDelay(TEMPO_DE_DETECAO / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(230400);
  
  calibacao();
  xSerial_semaforo = xSemaphoreCreateMutex();

  if ((xSerial_semaforo == NULL)) {
    Serial.println("Falha ao criar os semaforos");
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("Sucesso ao Criar os semaforos");
  }



  xFila_CO_MQ135 = xQueueCreate(1, sizeof(float));
  xFila_CO2_MQ135 = xQueueCreate(1, sizeof(float));
  xFila_Alcool_MQ135 = xQueueCreate(1, sizeof(float));
  xFila_Aceton_MQ135 = xQueueCreate(1, sizeof(float));
  xFila_Toluen_MQ135 = xQueueCreate(1, sizeof(float));
  xFila_MH4_MQ135 = xQueueCreate(1, sizeof(float));
  xFila_Ozone = xQueueCreate(1, sizeof(float));
  xFila_Temperatura = xQueueCreate(1, sizeof(float));
  xFila_Humidade = xQueueCreate(1, sizeof(float));


  if ((xFila_CO_MQ135 == NULL) || 
      (xFila_CO2_MQ135 == NULL) || 
      (xFila_Alcool_MQ135 == NULL) || 
      (xFila_Aceton_MQ135 == NULL) || 
      (xFila_Toluen_MQ135 == NULL) || 
      (xFila_MH4_MQ135 == NULL) || 
      (xFila_Ozone == NULL) ||
      (xFila_Humidade == NULL) ||
      (xFila_Temperatura == NULL)){
    Serial.println("Falha ao criar filas.");
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("Sucesso ao criar as filas");
  }

 

  esp_task_wdt_init(60, true);
  Serial.println("Filas iniciadas");
  xTaskCreate(task_envio_LoRaWan,
              "LoraWan_task",
              STACK_SIZE_LORAWAN,
              NULL,
              PRIO_LORAWAN,
              NULL);

  xTaskCreate(task_sensores,
              "Sensores_task",
              STACK_SIZE_SENSORES,
              NULL,
              PRIO_SENSORES,
              NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
