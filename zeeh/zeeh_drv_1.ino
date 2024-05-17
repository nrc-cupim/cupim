
#include <Bluepad32.h>
#include "parametros.h"

ControllerPtr myControllers[1];
bool roboLigado;

void desligaRobo() {
  digitalWrite(PINO_ARMA_1, LOW);
  digitalWrite(PINO_ARMA_2, LOW);

  digitalWrite(SENTIDO_MOTOR_DIREITO, LOW);
  digitalWrite(SENTIDO_MOTOR_ESQUERDO, LOW);

  analogWrite(VELOCIDADE_MOTOR_ESQUERDO, 0);
  analogWrite(VELOCIDADE_MOTOR_DIREITO, 0);

  roboLigado = false;
}

// Função para conctar o controle e garantir que somente um controle se conecte
void onConnectedController(ControllerPtr ctl) {

  bool foundEmptySlot = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.println("AVISO: controle conectado.");
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }

  if (!foundEmptySlot) {
    Serial.println("AVISO: Nao foi possivel conectar o controle.");
    Serial.println("AVISO: Reinicie a ESP32 e tente novamente.");
  }
}

//Função para Disconectar o controle
void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

  Serial.printf("AVISO: controle desconectado");
  desligaRobo();
}

//Inputs do controle
void processControllers() {
  for (auto myController : myControllers) {

    if (myController && myController->isConnected() && myController->hasData() && myController->isGamepad()) {

      uint16_t botoesMisc = myController->miscButtons();

      if (botoesMisc == 0x04) {  //Start do controle para ligar o robo
        roboLigado = true;
        Serial.println("Robo iniciado");
      }

      else if (botoesMisc == 0x02) {  // Select do controle para desligar o robo
        roboLigado = false;
        Serial.println("Robo desligado");
      }

      if (roboLigado) {

        uint16_t botoesPressionados = myController->buttons();

        if (botoesPressionados == 0x0001) {  // se pressionou apenas X
          digitalWrite(PINO_ARMA_1, LOW);    // liga arma
          digitalWrite(PINO_ARMA_2, LOW);
          Serial.print("Arma desligada ");
        }

        else if (botoesPressionados == 0x0008) {  // se pressionou apenas TRIÂNGULO
          digitalWrite(PINO_ARMA_1, HIGH);        // liga arma
          digitalWrite(PINO_ARMA_2, HIGH);
          Serial.print("Arma ligada ");
        }

        int32_t valorAnalogicoDireito = -myController->axisRY();  // menos pra ficar com o mesmo sinal do plano cartesiano
        int32_t valorAnalogicoEsquerdo = myController->axisX();

        Serial.println(valorAnalogicoDireito);
        Serial.println(valorAnalogicoEsquerdo);

        int pwmMotorDireito = 0, pwmMotorEsquerdo = 0;

        if (valorAnalogicoDireito > toleranciaAnalogico) {  // Ir para frente

          digitalWrite(SENTIDO_MOTOR_DIREITO, HIGH);
          digitalWrite(SENTIDO_MOTOR_ESQUERDO, HIGH);

          if (valorAnalogicoEsquerdo > toleranciaAnalogico) {
            pwmMotorDireito = map(valorAnalogicoDireito - valorAnalogicoEsquerdo, centerAnalogR - maxAnalogL, maxAnalogR - centerAnalogL, maxPWM, minPWM);
            pwmMotorEsquerdo = map(valorAnalogicoDireito + valorAnalogicoEsquerdo, centerAnalogR + centerAnalogL, maxAnalogR + maxAnalogL, maxPWM, minPWM);
          }

          else if (valorAnalogicoEsquerdo < -toleranciaAnalogico) {
            pwmMotorDireito = map(valorAnalogicoDireito - valorAnalogicoEsquerdo, centerAnalogR - centerAnalogL, maxAnalogR - minAnalogL, maxPWM, minPWM);
            pwmMotorEsquerdo = map(valorAnalogicoDireito + valorAnalogicoEsquerdo, centerAnalogR + minAnalogL, maxAnalogR + centerAnalogL, maxPWM, minPWM);
          }

          else {
            pwmMotorDireito = map(valorAnalogicoDireito, centerAnalogR, maxAnalogR, maxPWM, minPWM);
            pwmMotorEsquerdo = map(valorAnalogicoDireito, centerAnalogR, maxAnalogR, maxPWM, minPWM);
          }
        }

        else if (valorAnalogicoDireito < -toleranciaAnalogico) {  // Ir para tras

          digitalWrite(SENTIDO_MOTOR_DIREITO, LOW);
          digitalWrite(SENTIDO_MOTOR_ESQUERDO, LOW);

          if (valorAnalogicoEsquerdo > toleranciaAnalogico) {
            pwmMotorDireito = map(valorAnalogicoEsquerdo + valorAnalogicoDireito, centerAnalogL + minAnalogL, maxAnalogL + centerAnalogR, minPWM, maxPWM);
            pwmMotorEsquerdo = map(valorAnalogicoEsquerdo - valorAnalogicoDireito, centerAnalogL - centerAnalogR, maxAnalogL - minAnalogR, minPWM, maxPWM);
          }

          else if (valorAnalogicoEsquerdo < -toleranciaAnalogico) {
            pwmMotorDireito = map(valorAnalogicoEsquerdo + valorAnalogicoDireito, centerAnalogL + centerAnalogR, minAnalogL + minAnalogR, minPWM, maxPWM);
            pwmMotorEsquerdo = map(valorAnalogicoEsquerdo - valorAnalogicoDireito, minAnalogL - centerAnalogR, centerAnalogL - minAnalogR, minPWM, maxPWM);
          }

          else {
            pwmMotorDireito = map(valorAnalogicoDireito, centerAnalogR, minAnalogR, minPWM, maxPWM);
            pwmMotorEsquerdo = map(valorAnalogicoDireito, centerAnalogR, minAnalogR, minPWM, maxPWM);
          }
        }

        else {

          if (valorAnalogicoEsquerdo > toleranciaAnalogico) {
            digitalWrite(SENTIDO_MOTOR_DIREITO, LOW);
            digitalWrite(SENTIDO_MOTOR_ESQUERDO, HIGH);
            pwmMotorDireito = map(valorAnalogicoEsquerdo, centerAnalogL, maxAnalogL, minPWM, maxPWM);
            pwmMotorEsquerdo = map(valorAnalogicoEsquerdo, centerAnalogL, maxAnalogL, maxPWM, minPWM);
          }

          else if (valorAnalogicoEsquerdo < -toleranciaAnalogico) {
            digitalWrite(SENTIDO_MOTOR_DIREITO, HIGH);
            digitalWrite(SENTIDO_MOTOR_ESQUERDO, LOW);
            pwmMotorDireito = map(valorAnalogicoEsquerdo, centerAnalogL, minAnalogL, maxPWM, minPWM);
            pwmMotorEsquerdo = map(valorAnalogicoEsquerdo, centerAnalogL, minAnalogL, minPWM, maxPWM);
          }

          else {
            digitalWrite(SENTIDO_MOTOR_ESQUERDO, LOW);
            digitalWrite(SENTIDO_MOTOR_DIREITO, LOW);
          }
        }

        Serial.print("PWM Direito: ");
        Serial.println(pwmMotorDireito);
        Serial.print("PWM Esquerdo: ");
        Serial.println(pwmMotorEsquerdo);

        analogWrite(VELOCIDADE_MOTOR_DIREITO, pwmMotorDireito);
        analogWrite(VELOCIDADE_MOTOR_ESQUERDO, pwmMotorEsquerdo);
      }

      else
        desligaRobo();
    }
  }
}

void setup() {

  Serial.begin(115200);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forget
  BP32.enableVirtualDevice(false);

  pinMode(SENTIDO_MOTOR_ESQUERDO, OUTPUT);
  pinMode(VELOCIDADE_MOTOR_ESQUERDO, OUTPUT);

  pinMode(SENTIDO_MOTOR_DIREITO, OUTPUT);
  pinMode(VELOCIDADE_MOTOR_DIREITO, OUTPUT);

  pinMode(PINO_ARMA_1, OUTPUT);
  pinMode(PINO_ARMA_2, OUTPUT);

  desligaRobo();
}

void loop() {
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();
}