// Link para a biblioteca: https://github.com/aed3/PS4-esp32

#include <PS4Controller.h>
#include "parametros.h"

// Substituir o que está entre "" pelo endereço MAC salvo no controle
// Utilize a ferramenta sixaxispairer para descobrir o endereço
char* macAddress = "dc:97:ba:e7:d9:38";

bool roboLigado, configsTravadas;

/* Foi necessário utilizar essas variáveis para permitir a 
   inversão do sentido de giro de cada motor de locomoção */

int sentidoMotorEsquerdo, velocidadeMotorEsquerdo;
int sentidoMotorDireito, velocidadeMotorDireito;

bool direitoVesquerdoH, direitoHesquerdoV;

void desligaRobo() {
  analogWrite(sentidoMotorDireito, 0);
  analogWrite(velocidadeMotorDireito, 0);

  analogWrite(sentidoMotorEsquerdo, 0);
  analogWrite(velocidadeMotorEsquerdo, 0);

  roboLigado = false;
}

void processControllers() {

  if (PS4.Share()) {
    roboLigado = false;
    Serial.println("Robo desligado.");
  }

  else if (PS4.Options()) {
    roboLigado = true;
    Serial.println("Robo ligado.");
  }

  if (roboLigado) {

    /* --------------------- Lógica de funcionamento da arma --------------------- */

    // Se BOLINHA for pressionado, roda arma para um lado.
    if (PS4.Circle()) {
      Serial.print("Arma Sentido 1\n");
      analogWrite(PINO_1_ARMA1, 0);
      analogWrite(PINO_2_ARMA1, MAX_PWM);
      analogWrite(PINO_1_ARMA2, MAX_PWM);
      analogWrite(PINO_2_ARMA2, 0);
    }

    // Se QUADRADO for pressionado, roda arma para o outro lado.
    if (PS4.Square()) {
      Serial.print("Arma Sentido 2\n");
      analogWrite(PINO_1_ARMA1, MAX_PWM);
      analogWrite(PINO_2_ARMA1, 0);
      analogWrite(PINO_1_ARMA2, 0);
      analogWrite(PINO_2_ARMA2, MAX_PWM);
    }

    // Se TRIÂNGULO for presionado, desliga motores da arma.
    if (PS4.Triangle()) {
      Serial.print("Arma desligada\n");
      analogWrite(PINO_1_ARMA1, 0);
      analogWrite(PINO_2_ARMA1, 0);
      analogWrite(PINO_1_ARMA2, 0);
      analogWrite(PINO_2_ARMA2, 0);
    }

    /* ----------------- Lógica de trava das configurações ----------------- */

    if (PS4.L3() && PS4.R3()) {
      configsTravadas = !configsTravadas;
      digitalWrite(PINO_LED_INTERNO, configsTravadas);
      Serial.println(configsTravadas);
    }

    if (!configsTravadas) {  // Trava pra evitar de o público ficar alterando as configurações

      /* ----------------- Lógica de inversão dos analógicos de movimentação ----------------- */

      if (PS4.L1()) {
        direitoVesquerdoH = true, direitoHesquerdoV = false;
      }

      if (PS4.R1()) {
        direitoVesquerdoH = false, direitoHesquerdoV = true;
      }

      /* ----------------- Lógica de inversão de giro da movimentação ----------------- */

      if (PS4.Right()) {
        sentidoMotorEsquerdo = PINO_1_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_2_MOTOR_ESQUERDO;
        sentidoMotorDireito = PINO_1_MOTOR_DIREITO, velocidadeMotorDireito = PINO_2_MOTOR_DIREITO;
      }

      if (PS4.Down()) {
        sentidoMotorEsquerdo = PINO_1_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_2_MOTOR_ESQUERDO;
        sentidoMotorDireito = PINO_2_MOTOR_DIREITO, velocidadeMotorDireito = PINO_1_MOTOR_DIREITO;
      }

      if (PS4.Left()) {
        sentidoMotorEsquerdo = PINO_2_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_1_MOTOR_ESQUERDO;
        sentidoMotorDireito = PINO_1_MOTOR_DIREITO, velocidadeMotorDireito = PINO_2_MOTOR_DIREITO;
      }

      if (PS4.Up()) {
        sentidoMotorEsquerdo = PINO_2_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_1_MOTOR_ESQUERDO;
        sentidoMotorDireito = PINO_2_MOTOR_DIREITO, velocidadeMotorDireito = PINO_1_MOTOR_DIREITO;
      }
    }

    /* ----------------- Lógica de funcionamento da movimentação ----------------- */

    int8_t valorAnalogicoV, valorAnalogicoH;

    if (direitoVesquerdoH) {
      // Lê valor em Y do analógico direito (R-right).
      valorAnalogicoV = PS4.RStickY();

      // Lê valor em X do analógico esquerdo (L-left).
      valorAnalogicoH = PS4.LStickX();
    }

    if (direitoHesquerdoV) {
      // Lê valor em X do analógico direito (R-right).
      valorAnalogicoH = PS4.RStickX();

      // Lê valor em Y do analógico esquerdo (L-left).
      valorAnalogicoV = PS4.LStickY();
    }

    // Exibe valores no monitor serial.
    Serial.print("Y analogico: ");
    Serial.println(valorAnalogicoV);

    Serial.print("X analogico: ");
    Serial.println(valorAnalogicoH);

    int pwmMotorDireito1, pwmMotorDireito2, pwmMotorEsquerdo1, pwmMotorEsquerdo2;

    if (valorAnalogicoV > (CENTER_ANALOG_Y + TOLERANCIA_ANALOGICO)) {

      pwmMotorDireito1 = MAX_PWM;
      pwmMotorEsquerdo1 = MAX_PWM;

      if (valorAnalogicoH > (CENTER_ANALOG_X + TOLERANCIA_ANALOGICO)) {

        pwmMotorDireito2 = map(valorAnalogicoV - valorAnalogicoH,
                               CENTER_ANALOG_Y - MAX_ANALOG_X,
                               MAX_ANALOG_Y - CENTER_ANALOG_X,
                               MAX_PWM,
                               MIN_PWM);

        pwmMotorEsquerdo2 = map(valorAnalogicoV + valorAnalogicoH,
                                CENTER_ANALOG_Y + CENTER_ANALOG_X,
                                MAX_ANALOG_Y + MAX_ANALOG_X,
                                MAX_PWM,
                                MIN_PWM);
      }

      else if (valorAnalogicoH < (CENTER_ANALOG_X - TOLERANCIA_ANALOGICO)) {

        pwmMotorDireito2 = map(valorAnalogicoV - valorAnalogicoH,
                               CENTER_ANALOG_Y - CENTER_ANALOG_X,
                               MAX_ANALOG_Y - MIN_ANALOG_X,
                               MAX_PWM,
                               MIN_PWM);

        pwmMotorEsquerdo2 = map(valorAnalogicoV + valorAnalogicoH,
                                CENTER_ANALOG_Y + MIN_ANALOG_X,
                                MAX_ANALOG_Y + CENTER_ANALOG_X,
                                MAX_PWM,
                                MIN_PWM);
      }

      else {
        pwmMotorDireito2 = map(valorAnalogicoV, CENTER_ANALOG_Y, MAX_ANALOG_Y, MAX_PWM, MIN_PWM);
        pwmMotorEsquerdo2 = map(valorAnalogicoV, CENTER_ANALOG_Y, MAX_ANALOG_Y, MAX_PWM, MIN_PWM);
      }
    }

    else if (valorAnalogicoV < (CENTER_ANALOG_Y - TOLERANCIA_ANALOGICO)) {

      pwmMotorDireito1 = MIN_PWM;
      pwmMotorEsquerdo1 = MIN_PWM;

      if (valorAnalogicoH > (CENTER_ANALOG_X + TOLERANCIA_ANALOGICO)) {
        pwmMotorDireito2 = map(valorAnalogicoH + valorAnalogicoV,
                               CENTER_ANALOG_X + MIN_ANALOG_X,
                               MAX_ANALOG_X + CENTER_ANALOG_Y,
                               MIN_PWM,
                               MAX_PWM);

        pwmMotorEsquerdo2 = map(valorAnalogicoH - valorAnalogicoV,
                                CENTER_ANALOG_X - CENTER_ANALOG_Y,
                                MAX_ANALOG_X - MIN_ANALOG_Y,
                                MIN_PWM,
                                MAX_PWM);
      }

      else if (valorAnalogicoH < (CENTER_ANALOG_X - TOLERANCIA_ANALOGICO)) {
        pwmMotorDireito2 = map(valorAnalogicoH + valorAnalogicoV,
                               CENTER_ANALOG_X + CENTER_ANALOG_Y,
                               MIN_ANALOG_X + MIN_ANALOG_Y,
                               MIN_PWM,
                               MAX_PWM);

        pwmMotorEsquerdo2 = map(valorAnalogicoH - valorAnalogicoV,
                                MIN_ANALOG_X - CENTER_ANALOG_Y,
                                CENTER_ANALOG_X - MIN_ANALOG_Y,
                                MIN_PWM,
                                MAX_PWM);
      }

      else {
        pwmMotorDireito2 = map(valorAnalogicoV, CENTER_ANALOG_Y, MIN_ANALOG_Y, MIN_PWM, MAX_PWM);
        pwmMotorEsquerdo2 = map(valorAnalogicoV, CENTER_ANALOG_Y, MIN_ANALOG_Y, MIN_PWM, MAX_PWM);
      }
    }

    else {

      if (valorAnalogicoH > (CENTER_ANALOG_X + TOLERANCIA_ANALOGICO)) {
        pwmMotorDireito1 = MIN_PWM;
        pwmMotorDireito2 = map(valorAnalogicoH, CENTER_ANALOG_X, MAX_ANALOG_X, MIN_PWM, MAX_PWM);
        pwmMotorEsquerdo1 = map(valorAnalogicoH, CENTER_ANALOG_X, MAX_ANALOG_X, MIN_PWM, MAX_PWM);
        pwmMotorEsquerdo2 = MIN_PWM;
      }

      else if (valorAnalogicoH < (CENTER_ANALOG_X - TOLERANCIA_ANALOGICO)) {
        pwmMotorDireito1 = map(valorAnalogicoH, CENTER_ANALOG_X, MIN_ANALOG_X, MIN_PWM, MAX_PWM);
        pwmMotorDireito2 = MIN_PWM;
        pwmMotorEsquerdo1 = MIN_PWM;
        pwmMotorEsquerdo2 = map(valorAnalogicoH, CENTER_ANALOG_X, MIN_ANALOG_X, MIN_PWM, MAX_PWM);
      }

      else {
        pwmMotorDireito1 = MIN_PWM;
        pwmMotorEsquerdo1 = MIN_PWM;
        pwmMotorDireito2 = MIN_PWM;
        pwmMotorEsquerdo2 = MIN_PWM;
      }
    }

    analogWrite(sentidoMotorDireito, pwmMotorDireito1);
    analogWrite(velocidadeMotorDireito, pwmMotorDireito2);

    analogWrite(sentidoMotorEsquerdo, pwmMotorEsquerdo1);
    analogWrite(velocidadeMotorEsquerdo, pwmMotorEsquerdo2);

    Serial.print("PWM direito: ");
    Serial.print(pwmMotorDireito1);
    Serial.print(" ");
    Serial.println(pwmMotorDireito2);

    Serial.print("PWM esquerdo:");
    Serial.print(pwmMotorEsquerdo1);
    Serial.print(" ");
    Serial.println(pwmMotorEsquerdo2);
  }

  else
    desligaRobo();
}

void setup() {
  Serial.begin(115200);
  PS4.begin(macAddress);  // MAC salvo no controle
  Serial.printf("Battery Level : %d\n", PS4.Battery());

  pinMode(PINO_LED_INTERNO, OUTPUT);
  digitalWrite(PINO_LED_INTERNO, LOW);

  pinMode(PINO_1_MOTOR_ESQUERDO, OUTPUT);
  pinMode(PINO_2_MOTOR_ESQUERDO, OUTPUT);

  pinMode(PINO_1_MOTOR_DIREITO, OUTPUT);
  pinMode(PINO_2_MOTOR_DIREITO, OUTPUT);

  sentidoMotorEsquerdo = PINO_1_MOTOR_ESQUERDO, velocidadeMotorEsquerdo = PINO_2_MOTOR_ESQUERDO;
  sentidoMotorDireito = PINO_1_MOTOR_DIREITO, velocidadeMotorDireito = PINO_2_MOTOR_DIREITO;

  desligaRobo();

  direitoVesquerdoH = true, direitoHesquerdoV = false;
  configsTravadas = false;
}

void loop() {
  if (PS4.isConnected())
    processControllers();
  else
    desligaRobo();
}