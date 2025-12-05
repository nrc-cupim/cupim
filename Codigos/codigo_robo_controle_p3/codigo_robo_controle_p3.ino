#include <Bluepad32.h>
#include "parametros.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
bool roboLigado;

/* Foi necessário utilizar essas variáveis para permitir a 
   inversão do sentido de giro de cada motor de locomoção */

int sentidoMotorEsquerdo, velocidadeMotorEsquerdo;
int sentidoMotorDireito, velocidadeMotorDireito;

// Variáveis para inversão dos analógicos de movimentação
bool direitoVesquerdoH, direitoHesquerdoV;

void desligaRobo() {
  analogWrite(PINO_1_ARMA1, 0);
  analogWrite(PINO_2_ARMA1, 0);

  analogWrite(PINO_1_ARMA2, 0);
  analogWrite(PINO_2_ARMA2, 0);

  analogWrite(sentidoMotorDireito, 0);
  analogWrite(velocidadeMotorDireito, 0);

  analogWrite(sentidoMotorEsquerdo, 0);
  analogWrite(velocidadeMotorEsquerdo, 0);

  roboLigado = false;
}

void onConnectedController(ControllerPtr ctl) {

  bool foundEmptySlot = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.println("AVISO: controle conectado.");
      myControllers[i] = ctl;
      foundEmptySlot = true;

      /* Caso deseje realizar alguma tarefa assim que a conexão 
         com o contole for estabelecidada, coloque o código aqui. */

      break;
    }
  }

  if (!foundEmptySlot) {
    Serial.println("AVISO: Nao foi possivel conectar o controle.");
    Serial.println("AVISO: Reinicie a ESP32 e tente novamente.");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("AVISO: controle desconectado");
      myControllers[i] = nullptr;
      desligaRobo();

      /* Caso deseje realizar alguma tarefa assim que o 
         controle for desconectado, coloque o código aqui */

      break;
    }
  }
}

void processControllers() {
  for (auto myController : myControllers) {

    if (myController && myController->isConnected()
        && myController->hasData() && myController->isGamepad()) {

      /* A partir daqui inicia-se a lógica de funcionamento do robô.
         Qualquer alteração / nova implementação deve ser feita aqui. */

      // Se SELECT for presionado, desliga robô.
      if (myController->miscSelect()) {
        roboLigado = false;
        Serial.println("Robo desligado.");
      }

      // Se START for presionado, liga robô.
      else if (myController->miscStart()) {
        roboLigado = true;
        Serial.println("Robo ligado.");
      }

      if (roboLigado) {

        /* --------------------- Lógica de funcionamento da arma --------------------- */

        // Se BOLINHA for pressionado, roda arma para um lado.
        if (myController->b()) {
          Serial.print("Arma Sentido 1\n");
          analogWrite(PINO_1_ARMA1, 0);
          analogWrite(PINO_2_ARMA1, MAX_PWM);
          analogWrite(PINO_1_ARMA2, MAX_PWM);
          analogWrite(PINO_2_ARMA2, 0);
        }

        // Se QUADRADO for pressionado, roda arma para o outro lado.
        if (myController->x()) {
          Serial.print("Arma Sentido 2\n");
          analogWrite(PINO_1_ARMA1, MAX_PWM);
          analogWrite(PINO_2_ARMA1, 0);
          analogWrite(PINO_1_ARMA2, 0);
          analogWrite(PINO_2_ARMA2, MAX_PWM);
        }

        // Se TRIÂNGULO for presionado, desliga motores da arma.
        if (myController->y()) {
          Serial.print("Arma desligada\n");
          analogWrite(PINO_1_ARMA1, 0);
          analogWrite(PINO_2_ARMA1, 0);
          analogWrite(PINO_1_ARMA2, 0);
          analogWrite(PINO_2_ARMA2, 0);
        }

        /* ----------------- Lógica de inversão dos analógicos de movimentação ----------------- */

        // Se L1 for pressionado, locomoção Direito V - Esquerdo H
        if (myController->l1()) {
          direitoVesquerdoH = true, direitoHesquerdoV = false;
        }

        // Se R1 for pressionado, locomoção Direito H - Esquerdo V
        if (myController->r1()) {
          direitoVesquerdoH = false, direitoHesquerdoV = true;
        }

        /* ----------------- Lógica de inversão de giro da movimentação ----------------- */

        uint8_t leituraSetinhas = myController->dpad();

        // Cada SETINHA representa uma configuração de pinos para os motores
        switch (leituraSetinhas) {
          case 0x01: // cima
            sentidoMotorEsquerdo = PINO_1_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_2_MOTOR_ESQUERDO;
            sentidoMotorDireito = PINO_1_MOTOR_DIREITO, velocidadeMotorDireito = PINO_2_MOTOR_DIREITO;
            break;
          case 0x02: // baixo
            sentidoMotorEsquerdo = PINO_1_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_2_MOTOR_ESQUERDO;
            sentidoMotorDireito = PINO_2_MOTOR_DIREITO, velocidadeMotorDireito = PINO_1_MOTOR_DIREITO;
            break;
          case 0x04: // direita
            sentidoMotorEsquerdo = PINO_2_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_1_MOTOR_ESQUERDO;
            sentidoMotorDireito = PINO_1_MOTOR_DIREITO, velocidadeMotorDireito = PINO_2_MOTOR_DIREITO;
            break;
          case 0x08: // esquerda
            sentidoMotorEsquerdo = PINO_2_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_1_MOTOR_ESQUERDO;
            sentidoMotorDireito = PINO_2_MOTOR_DIREITO, velocidadeMotorDireito = PINO_1_MOTOR_DIREITO;
            break;
        }

        /* ----------------- Lógica de funcionamento da movimentação ----------------- */

        int32_t valorAnalogicoV, valorAnalogicoH;

        if (direitoVesquerdoH) {
          // Lê valor em Y do analógico direito (R-right).
          valorAnalogicoV = myController->axisRY();

          // Lê valor em X do analógico esquerdo (L-left).
          valorAnalogicoH = myController->axisX();
        }

        if (direitoHesquerdoV) {
          // Lê valor em X do analógico direito (R-right).
          valorAnalogicoH = myController->axisRX();

          // Lê valor em Y do analógico esquerdo (L-left).
          valorAnalogicoV = myController->axisY();
        }

        // Exibe valores no monitor serial.
        // Serial.print("Y analogico R: ");
        // Serial.println(valorAnalogicoV);

        // Serial.print("X analogico L: ");
        // Serial.println(valorAnalogicoH);

        int pwmMotorDireito1, pwmMotorDireito2, pwmMotorEsquerdo1, pwmMotorEsquerdo2;

        // Analógico Y movimentado para trás
        if (valorAnalogicoV > (PARADO_JOYSTICK_Y + TOLERANCIA_JOYSTICK)) {
          pwmMotorDireito1 = MIN_PWM;
          pwmMotorEsquerdo2 = MIN_PWM;

          // Analógico X movimentado para direita
          if (valorAnalogicoH > (PARADO_JOYSTICK_X + TOLERANCIA_JOYSTICK)) {
            pwmMotorDireito2 = map(valorAnalogicoV - valorAnalogicoH,
                                   PARADO_JOYSTICK_Y - MAX_JOYSTICK_X,
                                   MAX_JOYSTICK_Y - PARADO_JOYSTICK_X,
                                   MIN_PWM, MAX_PWM);
            pwmMotorEsquerdo1 = map(valorAnalogicoV, PARADO_JOYSTICK_Y, MAX_JOYSTICK_Y, MIN_PWM, MAX_PWM);
          }

          // Analógico X movimentado para esquerda
          else if (valorAnalogicoH < (PARADO_JOYSTICK_X - TOLERANCIA_JOYSTICK)) {
            pwmMotorDireito2 = map(valorAnalogicoV, PARADO_JOYSTICK_Y, MAX_JOYSTICK_Y, MIN_PWM, MAX_PWM);
            pwmMotorEsquerdo1 = map(valorAnalogicoV + valorAnalogicoH,
                                    PARADO_JOYSTICK_Y + MIN_JOYSTICK_X,
                                    MAX_JOYSTICK_Y + PARADO_JOYSTICK_X,
                                    MIN_PWM, MAX_PWM);
          }

          // Analógico X não movimentado
          else {
            pwmMotorDireito2 = map(valorAnalogicoV, PARADO_JOYSTICK_Y, MAX_JOYSTICK_Y, MIN_PWM, MAX_PWM);
            pwmMotorEsquerdo1 = map(valorAnalogicoV, PARADO_JOYSTICK_Y, MAX_JOYSTICK_Y, MIN_PWM, MAX_PWM);
          }
        }

        // Analógico Y movimentado para frente
        else if (valorAnalogicoV < (PARADO_JOYSTICK_Y - TOLERANCIA_JOYSTICK)) {
          pwmMotorDireito2 = MIN_PWM;
          pwmMotorEsquerdo1 = MIN_PWM;

          // Analógico X movimentado para direita
          if (valorAnalogicoH > (PARADO_JOYSTICK_X + TOLERANCIA_JOYSTICK)) {
            pwmMotorDireito1 = map(valorAnalogicoV + valorAnalogicoH,
                                   MIN_JOYSTICK_Y + PARADO_JOYSTICK_X,
                                   PARADO_JOYSTICK_Y + MAX_JOYSTICK_X,
                                   MAX_PWM, MIN_PWM);
            pwmMotorEsquerdo2 = map(valorAnalogicoV, MIN_JOYSTICK_Y, PARADO_JOYSTICK_Y, MAX_PWM, MIN_PWM);
          }

          // Analógico X movimentado para esquerda
          else if (valorAnalogicoH < (PARADO_JOYSTICK_X - TOLERANCIA_JOYSTICK)) {
            pwmMotorDireito1 = map(valorAnalogicoV, MIN_JOYSTICK_Y, PARADO_JOYSTICK_Y, MAX_PWM, MIN_PWM);
            pwmMotorEsquerdo2 = map(valorAnalogicoV - valorAnalogicoH,
                                    MIN_JOYSTICK_Y - PARADO_JOYSTICK_X,
                                    PARADO_JOYSTICK_Y - MIN_JOYSTICK_X,
                                    MIN_PWM, MAX_PWM);
          }

          // Analógico X não movimentado
          else {
            pwmMotorDireito1 = map(valorAnalogicoV, MIN_JOYSTICK_Y, PARADO_JOYSTICK_Y, MAX_PWM, MIN_PWM);
            pwmMotorEsquerdo2 = map(valorAnalogicoV, MIN_JOYSTICK_Y, PARADO_JOYSTICK_Y, MAX_PWM, MIN_PWM);
          }
        }

        // Analógico Y não movimentado
        else {

          // Analógico X movimentado para direita
          if (valorAnalogicoH > (PARADO_JOYSTICK_X + TOLERANCIA_JOYSTICK)) {
            pwmMotorDireito1 = MIN_PWM;
            pwmMotorEsquerdo1 = MIN_PWM;
            pwmMotorDireito2 = map(valorAnalogicoH, PARADO_JOYSTICK_X, MAX_JOYSTICK_X, MIN_PWM, MAX_PWM);
            pwmMotorEsquerdo2 = map(valorAnalogicoH, PARADO_JOYSTICK_X, MAX_JOYSTICK_X, MIN_PWM, MAX_PWM);
          }

          // Analógico X movimentado para esquerda
          else if (valorAnalogicoH < (PARADO_JOYSTICK_X - TOLERANCIA_JOYSTICK)) {
            pwmMotorDireito1 = map(valorAnalogicoH, MIN_JOYSTICK_X, PARADO_JOYSTICK_X, MAX_PWM, MIN_PWM);
            pwmMotorEsquerdo1 = map(valorAnalogicoH, MIN_JOYSTICK_X, PARADO_JOYSTICK_X, MAX_PWM, MIN_PWM);
            pwmMotorDireito2 = MIN_PWM;
            pwmMotorEsquerdo2 = MIN_PWM;
          }

          // Analógico X não movimentado
          else {
            pwmMotorDireito1 = MIN_PWM;
            pwmMotorEsquerdo1 = MIN_PWM;
            pwmMotorDireito2 = MIN_PWM;
            pwmMotorEsquerdo2 = MIN_PWM;
          }
        }

        Serial.print("PWM Direito 1: ");
        Serial.println(pwmMotorDireito1);
        Serial.print("PWM Direito 2: ");
        Serial.println(pwmMotorDireito2);
        analogWrite(sentidoMotorDireito, pwmMotorDireito1);
        analogWrite(velocidadeMotorDireito, pwmMotorDireito2);

        Serial.println();

        Serial.print("PWM Esquerdo 1: ");
        Serial.println(pwmMotorEsquerdo1);
        Serial.print("PWM Esquerdo 2: ");
        Serial.println(pwmMotorEsquerdo2);
        analogWrite(sentidoMotorEsquerdo, pwmMotorEsquerdo1);
        analogWrite(velocidadeMotorEsquerdo, pwmMotorEsquerdo2);
      }

      else
        desligaRobo();
    }
  }
}

void setup() {

  Serial.begin(115200);

  // Inicia comunicação Bluetooth que permite a conexão com controle.
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false);

  // Desparea os controles que haviam sido conectados anteriormente.
  BP32.forgetBluetoothKeys();

  // Configura pinos da ESP32 para controle dos motores de arma.
  pinMode(PINO_1_ARMA1, OUTPUT);
  pinMode(PINO_2_ARMA1, OUTPUT);

  pinMode(PINO_1_ARMA2, OUTPUT);
  pinMode(PINO_2_ARMA2, OUTPUT);

  // Configura pinos da ESP32 para controle dos motores de locomoção.
  pinMode(PINO_1_MOTOR_ESQUERDO, OUTPUT);
  pinMode(PINO_2_MOTOR_ESQUERDO, OUTPUT);

  pinMode(PINO_1_MOTOR_DIREITO, OUTPUT);
  pinMode(PINO_2_MOTOR_DIREITO, OUTPUT);

  sentidoMotorEsquerdo = PINO_2_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_1_MOTOR_ESQUERDO;
  sentidoMotorDireito = PINO_2_MOTOR_DIREITO, velocidadeMotorDireito = PINO_1_MOTOR_DIREITO;

  direitoVesquerdoH = true, direitoHesquerdoV = false;

  // Desliga movimentação e arma do robô.
  desligaRobo();
}

void loop() {
  // Checa se houve atualização nos dados do controle
  bool dataUpdated = BP32.update();

  // Se sim, chama a função processControllers() para processar os dados
  if (dataUpdated)
    processControllers();
}

