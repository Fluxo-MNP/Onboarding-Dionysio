#include<LiquidCrystal.h> //inclusão da biblioteca
LiquidCrystal LCD(12, 11, 10, 9, 8, 7);

//Define os pinos dos sensore
#define sensor_temp_pino A0 //Define o pino analógico A0 para o sensor de temperatura
#define sensor_pres_pino 2 //Define o pino digital 2 para o sensor de presença
#define sensor_umi_pino A1 //Define o pino anológico A1 para o sensor de umidade
#define sensor_fum_pino A2//define o pino analógico A2 para o sensor de fumaça

//variáveis para armazenar valores lido
int valor_sensor_pres = 0; //aramazena o estado lido do sensor de presença
int valor_sensor_fum = 0; //armazena o valor lido no sensor de fumaça

//pino de controle de funcionamento e alerta
#define funcionando_sensor 5 //led para indicar o funcionamento do sistema de temperatura
#define alerta_sensor 4      //led para alertas de temperatura e presença
#define led_umi 3            //led para indicar o funcionamento do sensor de umidade
#define led_red_umi 6        //led para indicar alertas de umidade e fumaça
#define som 13               //defino o pino 13 para o buzzer

//limites de temperatura
#define temp_normal 28  //Define que atemperatura do ambiente está normal
#define temp_anormal 35 //Define temperatura anormal

//temporizadores
unsigned long previousMillisFum = 0;
unsigned long previousMillisLCD = 0;
unsigned long previousMillisBeep = 0;
const long intervaloBeep = 500;//intervalo de 500ms para o buzzer

bool beepState = false;

//variáveis do tempo
unsigned long previousMillis = 0;//último valor de millis
const long intervalo = 1000; //intervalo de 1 seg

int segundos = 0;
int minutos = 0;
int horas = 0;

void setup(){
  Serial.begin(9600);
  
  //configurações dos pinos de led
  pinMode(funcionando_sensor, OUTPUT);//Define o pino de alerta de temperatura normal como saída
  pinMode(alerta_sensor, OUTPUT); //Define o pino de alerta de temperatura anormal como saída
  
  //configuração dos sensores
  pinMode(sensor_pres_pino, INPUT);//configuração do sensor de presença
  pinMode(sensor_fum_pino, INPUT);//configuração do sensor de fumaça
  
  //INICIALIZAÇÃO DO LCD
  LCD.begin(16, 2);
  LCD.setCursor(0, 0);
  //imprime a mensagem no LCD
  LCD.print("Temp: ");
  LCD.print("   C");  
  
  //configura a comunicação serial
  Serial.println("Digite o comando:");
  Serial.println("HXX - Ajusta horas (ex: H15)");
  Serial.println("MXX - Ajusta minutos (ex: M30)");
  Serial.println("SXX - Ajusta segundos (ex: S45)");  
}

void loop(){
  //visualização no monitor serial
  //Serial.println(sensor_umi_pino);
  //Serial.println(sensor_pres_pino);
  //Serial.println(analogRead(sensor_temp_pino));
  //Serial.println(sensor_fum_pino);

//Relógio
//atualiza o relógio
atualizarelogio();
  
//verifica se há comandos na interface serial
ajustahorarioserial();
  
// exibe o horrário no LCD
LCD.setCursor(8, 1);
exibehora();

sensordeumidade();
sensordetemperatura();  
sensordepresenca();  
sensordefumaca();  
  
}


//função para ligar o led
   void ligar_LED(){
   digitalWrite(alerta_sensor, HIGH); //define o pino do led em nível alto(liga o led)
}
//função para desligar o led
  void desligar_LED(){
   digitalWrite(alerta_sensor, LOW);//define o pino do led em nível baixo(desliga o led)
  }

//função para atualizar o relógio 
void atualizarelogio(){
 unsigned long currentMillis = millis();
  
 //incrementa os segundos a cada 1 segundo
  if(currentMillis - previousMillis >= intervalo){
    previousMillis = currentMillis;
    segundos++;
    
    if(segundos >= 60){
    segundos = 0;
    minutos++;
    }
    if(minutos >= 60){
      minutos = 0;
      horas++;
    }
    if(horas >= 24){
      horas = 0;
    }
  }
}

//função para ajustar o horário pela interface serial
void ajustahorarioserial(){
  if(Serial.available()){
    String comando = Serial.readStringUntil('\n');
    comando.trim();//remove esoaços em branco
    
    if(comando.startsWith("H")){
      horas = comando.substring(1).toInt();
      if(horas < 0 || horas >= 24) horas = 0;//validação do horário    
    } else if(comando.startsWith("M")){
      minutos = comando.substring(1).toInt();
      if(minutos < 0 || minutos >= 60) minutos = 0;//validação dos minutos
    }else if(comando.startsWith("S")){
     segundos = comando.substring(1).toInt();
     if(segundos < 0 || segundos >= 60) segundos = 0;//validação dos segundos 
    }else{
     Serial.println("Comando invalido! Use HXX, MXX, SXX.");
    }
  }
}

//função para exibir o horário no LCD
void exibehora(){
  if(horas < 10) LCD.print("0");
  LCD.print(horas);
  LCD.print(":");
  if(minutos < 10) LCD.print("0");
  LCD.print(minutos);
  LCD.print(":");
  if(segundos < 10) LCD.print("0");
  LCD.print(segundos);
}

void sensordeumidade(){
//SENSOR DE UMIDADE
  if(valor_sensor_fum < 250 && valor_sensor_pres == 0){     
//define a entrada analogica na porta A1
    int sensor_umi_valor = analogRead(sensor_umi_pino);
  
//posição que ira imprimir na tela
  LCD.setCursor(0, 1);//seta a posição do texto
  if (sensor_umi_valor >= 512){//se for maior ou igual a 512
  LCD.print(" Fresco");
  //acende o led verde
  digitalWrite(led_umi,HIGH);
  digitalWrite(led_red_umi, LOW);
  }
  else if(sensor_umi_valor< 512){
  LCD.print("Abafado");
  //acende o led vermelho
  digitalWrite(led_red_umi, HIGH); 
  digitalWrite(led_umi, LOW);
  } 
  }
}

void sensordetemperatura(){
//SENSOR DE TEMPERATURA  
// faz a leitura da tensão no sensor de temperatura
  int sensor_temp_tensao = analogRead(sensor_temp_pino);   
  
//converte o valor análogico para uma faixa de temperatura
int temperaturaC = map(sensor_temp_tensao, 20 ,358 , -40,125 );
  
  if(temperaturaC <= 100){
   // Muda o cursor para a sexta coluna e primeira linha no LCD
  LCD.setCursor(8, 0);

  // Limpa o valor anterior (independente de ser um número de 2 ou 3 dígitos)
  LCD.print(" ");  // Limpa 1 espaço
  }
  if( temperaturaC < 0){
     LCD.setCursor(7, 0);
     LCD.print(" ")  ;
  }
  //muda o cursor para a setima coluna e primeira linha
  LCD.setCursor(6,0);
  
  //imprime a temperatura em graus celsius 
  LCD.print(temperaturaC);
  
  //Acende ou apaga os leds de acordo com a temperatura
  if (temperaturaC >= temp_anormal){
     digitalWrite(funcionando_sensor, LOW);
     digitalWrite(alerta_sensor, HIGH);
  }
  else if(temperaturaC <= temp_normal){
     digitalWrite(funcionando_sensor, HIGH);
     digitalWrite(alerta_sensor, LOW);
  }
  
}




void sensordepresenca(){
//SENSOR DE PRESENÇA 
//Lê o valor do sensor de presença (0 ou 1 dependendo da detectação do movimento)
 valor_sensor_pres = digitalRead(sensor_pres_pino);  
//verifica o valor do sensor pres
  if(valor_sensor_pres == 1){//se o sensor detectar movimento(valor 1)
  ligar_LED(); //chama a função para ligar o LED
  LCD.setCursor(0,1);
  LCD.print(" Alerta");
  }
  else{//caso contrário, se não detectar moimento(valor 0)
  desligar_LED();
  }
}

void sensordefumaca(){
//SENSOR DE FUMAÇA
 valor_sensor_fum = analogRead(sensor_fum_pino);//le o valor analógico do sensor de gás
 
 unsigned long currentMillis = millis();
  
  if(valor_sensor_fum > 250){
    if(currentMillis - previousMillisBeep >= intervaloBeep){
      previousMillisBeep = currentMillis;
      beepState = !beepState;
      
      if(beepState){
       tone(som, 1500);//emite um som com uma frequência de 1500 hz
       digitalWrite(led_red_umi, HIGH);//faz o led piscar
      } else {
        noTone(som);//desliga o som
        digitalWrite(led_red_umi, LOW);//se o valor for menor ou igual a 250 ele apaga
  }
   
}
LCD.setCursor(0, 1);
LCD.print(" Perigo ");
}else{
    noTone(som);
    digitalWrite(led_red_umi, LOW);
  }
}