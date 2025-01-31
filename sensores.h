float MQ135_CO2(){
    MQ135.setA(110.47);
    MQ135.setB(-2.862);                    // Configura a equação para calcular a concentração de CO2
    float CO2_MQ135 = MQ135.readSensor();
    return CO2_MQ135;
}
float MQ135_Toluen(){
    MQ135.setA(44.947);
    MQ135.setB(-3.445);                       // Configura a equação para calcular a concentração de Tolueno
    float Toluen_MQ135 = MQ135.readSensor();
    return Toluen_MQ135;
}
float MQ135_MH4(){
    MQ135.setA(102.2);
    MQ135.setB(-2.473);                    // Configura a equação para calcular a concentração de MH4
    float MH4_MQ135 = MQ135.readSensor();
    return MH4_MQ135;
}
float MQ135_CO(){
    MQ135.setA(605.18);
    MQ135.setB(-3.937);                   // Configura a equação para calcular a concentração de CO
    float CO_MQ135 = MQ135.readSensor();
    return CO_MQ135;
}
float MQ135_Aceton(){
     MQ135.setA(34.668);
    MQ135.setB(-3.369);                       // Configura a equação para calcular a concentração de Acetona
    float Aceton_MQ135 = MQ135.readSensor();
    return Aceton_MQ135;
}
float MQ135_Alcool(){
    MQ135.setA(77.255);
    MQ135.setB(-3.18);                        // Configura a equação para calcular a concentração de Alcool
    float Alcool_MQ135 = MQ135.readSensor();
    return Alcool_MQ135;
}
float MQ131_Ozone(){
    MQ131.setA(23.943); 
    MQ131.setB(-1.11);
    float Ozone = MQ131.readSensor();
    return Ozone;
}
// float dhtTemperatura(){
//   float temperatura = dht.readTemperature();
//   return temperatura;
// }
// float dhtHumidade(){
//   float humidade = dht.readHumidity();
//   return humidade;
// }



