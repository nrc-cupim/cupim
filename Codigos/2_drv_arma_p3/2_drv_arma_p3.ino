#include <Bluepad32.h>
#include "parametros.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
bool roboLigado;

/* Foi necessário utilizar essas variáveis para permitir a 
   inversão do sentido de giro de cada motor de locomoção */

int sentidoMotorEsquerdo, velocidadeMotorEsquerdo;
int sentidoMotorDireito, velocidadeMotorDireito;

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

      uint16_t estadoMiscButtons = myController->miscButtons();

      // Se SELECT for presionado, desliga robô.
      if (estadoMiscButtons == 0x02) {
        roboLigado = false;
        Serial.println("Robo desligado");
      }

      // Se START for presionado, liga robô.
      else if (estadoMiscButtons == 0x04) {
        roboLigado = true;
        Serial.println("Robo iniciado");
      }

      if (roboLigado) {

        /* --------------------- Lógica de funcionamento e inversão de giro da arma --------------------- */

        uint16_t botoesPressionados = myController->buttons();

        // Se TRIÂNGULO for presionado, desliga motores da arma.
        if (botoesPressionados == 0x0008) {
          Serial.print("Arma desligada\n");

          analogWrite(PINO_1_ARMA1, 0);
          analogWrite(PINO_2_ARMA1, 0);

          analogWrite(PINO_1_ARMA2, 0);
          analogWrite(PINO_2_ARMA2, 0);
        }

        //Se BOLINHA for pressionado, roda arma para um lado
        if (botoesPressionados == 0x0002) {
          Serial.print("Arma Sentido 1\n");

          analogWrite(PINO_1_ARMA1, 0);
          analogWrite(PINO_2_ARMA1, maxPWM);

          analogWrite(PINO_1_ARMA2, maxPWM);  // para rodar pro outro lado
          analogWrite(PINO_2_ARMA2, 0);
        }

        // Se QUADRADO for pressionado, roda arma para o outro lado
        else if (botoesPressionados == 0x0004) {
          Serial.print("Arma Sentido 2\n");

          analogWrite(PINO_1_ARMA1, maxPWM);
          analogWrite(PINO_2_ARMA1, 0);

          analogWrite(PINO_1_ARMA2, 0);  // para rodar pro outro lado
          analogWrite(PINO_2_ARMA2, maxPWM);
        }

        /* ----------------- Lógica de funcionamento da movimentação ----------------- */

        // Lê valor em Y do analógico direito (R-right).
        int32_t valorAnalogicoDireito = -myController->axisRY();

        // Lê valor em X do analógico esquerdo (L-left).
        int32_t valorAnalogicoEsquerdo = myController->axisX();

        // Exibe valores no monitor serial.
        Serial.print("Y analogico R: ");
        Serial.println(valorAnalogicoDireito);

        Serial.print("X analogico L: ");
        Serial.println(valorAnalogicoEsquerdo);

        int pwmMotorDireito1, pwmMotorDireito2, pwmMotorEsquerdo1, pwmMotorEsquerdo2;

        if (valorAnalogicoDireito > (centerAnalogR_Y + toleranciaAnalogico)) {

          pwmMotorDireito1 = maxPWM;
          pwmMotorEsquerdo1 = maxPWM;

          if (valorAnalogicoEsquerdo > (centerAnalogL_X + toleranciaAnalogico)) {

            pwmMotorDireito2 = map(valorAnalogicoDireito - valorAnalogicoEsquerdo,
                                   centerAnalogR_Y - maxAnalogL_X,
                                   maxAnalogR_Y - centerAnalogL_X,
                                   maxPWM,
                                   minPWM);

            pwmMotorEsquerdo2 = map(valorAnalogicoDireito + valorAnalogicoEsquerdo,
                                    centerAnalogR_Y + centerAnalogL_X,
                                    maxAnalogR_Y + maxAnalogL_X,
                                    maxPWM,
                                    minPWM);
          }

          else if (valorAnalogicoEsquerdo < (centerAnalogL_X - toleranciaAnalogico)) {

            pwmMotorDireito2 = map(valorAnalogicoDireito - valorAnalogicoEsquerdo,
                                   centerAnalogR_Y - centerAnalogL_X,
                                   maxAnalogR_Y - minAnalogL_X,
                                   maxPWM,
                                   minPWM);

            pwmMotorEsquerdo2 = map(valorAnalogicoDireito + valorAnalogicoEsquerdo,
                                    centerAnalogR_Y + minAnalogL_X,
                                    maxAnalogR_Y + centerAnalogL_X,
                                    maxPWM,
                                    minPWM);
          }

          else {
            pwmMotorDireito2 = map(valorAnalogicoDireito, centerAnalogR_Y, maxAnalogR_Y, maxPWM, minPWM);
            pwmMotorEsquerdo2 = map(valorAnalogicoDireito, centerAnalogR_Y, maxAnalogR_Y, maxPWM, minPWM);
          }
        }

        else if (valorAnalogicoDireito < (centerAnalogR_Y - toleranciaAnalogico)) {

          pwmMotorDireito1 = minPWM;
          pwmMotorEsquerdo1 = minPWM;

          if (valorAnalogicoEsquerdo > (centerAnalogL_X + toleranciaAnalogico)) {
            pwmMotorDireito2 = map(valorAnalogicoEsquerdo + valorAnalogicoDireito,
                                   centerAnalogL_X + minAnalogL_X,
                                   maxAnalogL_X + centerAnalogR_Y,
                                   minPWM,
                                   maxPWM);

            pwmMotorEsquerdo2 = map(valorAnalogicoEsquerdo - valorAnalogicoDireito,
                                    centerAnalogL_X - centerAnalogR_Y,
                                    maxAnalogL_X - minAnalogR_Y,
                                    minPWM,
                                    maxPWM);
          }

          else if (valorAnalogicoEsquerdo < (centerAnalogL_X - toleranciaAnalogico)) {
            pwmMotorDireito2 = map(valorAnalogicoEsquerdo + valorAnalogicoDireito,
                                   centerAnalogL_X + centerAnalogR_Y,
                                   minAnalogL_X + minAnalogR_Y,
                                   minPWM,
                                   maxPWM);

            pwmMotorEsquerdo2 = map(valorAnalogicoEsquerdo - valorAnalogicoDireito,
                                    minAnalogL_X - centerAnalogR_Y,
                                    centerAnalogL_X - minAnalogR_Y,
                                    minPWM,
                                    maxPWM);
          }

          else {
            pwmMotorDireito2 = map(valorAnalogicoDireito, centerAnalogR_Y, minAnalogR_Y, minPWM, maxPWM);
            pwmMotorEsquerdo2 = map(valorAnalogicoDireito, centerAnalogR_Y, minAnalogR_Y, minPWM, maxPWM);
          }
        }

        else {

          if (valorAnalogicoEsquerdo > (centerAnalogL_X + toleranciaAnalogico)) {
            pwmMotorDireito1 = minPWM;
            pwmMotorEsquerdo1 = maxPWM;
            pwmMotorDireito2 = map(valorAnalogicoEsquerdo, centerAnalogL_X, maxAnalogL_X, minPWM, maxPWM);
            pwmMotorEsquerdo2 = map(valorAnalogicoEsquerdo, centerAnalogL_X, maxAnalogL_X, maxPWM, minPWM);
          }

          else if (valorAnalogicoEsquerdo < (centerAnalogL_X - toleranciaAnalogico)) {
            pwmMotorDireito1 = maxPWM;
            pwmMotorEsquerdo1 = minPWM;
            pwmMotorDireito2 = map(valorAnalogicoEsquerdo, centerAnalogL_X, minAnalogL_X, maxPWM, minPWM);
            pwmMotorEsquerdo2 = map(valorAnalogicoEsquerdo, centerAnalogL_X, minAnalogL_X, minPWM, maxPWM);
          }

          else {
            pwmMotorDireito1 = minPWM;
            pwmMotorEsquerdo1 = minPWM;
            pwmMotorDireito2 = minPWM;
            pwmMotorEsquerdo2 = minPWM;
          }
        }

        /* ----------------- Lógica de inversão de giro da movimentação ----------------- */

        uint8_t leituraSetinhas = myController->dpad();

        // Se seta cima ou seta baixo forem pressionadas, inverte sentido motor direito
        if (leituraSetinhas == 0x01)
          sentidoMotorDireito = PINO_1_MOTOR_DIREITO, velocidadeMotorDireito = PINO_2_MOTOR_DIREITO;
        else if (leituraSetinhas == 0x02)
          sentidoMotorDireito = PINO_2_MOTOR_DIREITO, velocidadeMotorDireito = PINO_1_MOTOR_DIREITO;

        // Se seta esquerda ou seta direita forem pressionadas, inverte sentido motor esquerdo
        if (leituraSetinhas == 0x04)
          sentidoMotorEsquerdo = PINO_1_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_2_MOTOR_ESQUERDO;
        else if (leituraSetinhas == 0x08)
          sentidoMotorEsquerdo = PINO_2_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_1_MOTOR_ESQUERDO;

        Serial.print("PWM Direito 1: ");
        Serial.println(pwmMotorDireito1);
        Serial.print("PWM Direito 2: ");
        Serial.println(pwmMotorDireito2);
        analogWrite(sentidoMotorDireito, pwmMotorDireito1);
        analogWrite(velocidadeMotorDireito, pwmMotorDireito2);

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

  sentidoMotorEsquerdo = PINO_1_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_2_MOTOR_ESQUERDO;
  sentidoMotorDireito = PINO_1_MOTOR_DIREITO, velocidadeMotorDireito = PINO_2_MOTOR_DIREITO;

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