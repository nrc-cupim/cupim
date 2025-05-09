
#ifndef parametros_h
#define parametros_h

#include <Arduino.h>

#define PINO_1_ARMA2 13
#define PINO_2_ARMA2 12

#define PINO_1_MOTOR_ESQUERDO 14
#define PINO_2_MOTOR_ESQUERDO 27

#define PINO_1_MOTOR_DIREITO 25
#define PINO_2_MOTOR_DIREITO 26

#define PINO_1_ARMA1 33
#define PINO_2_ARMA1 32

// Comportamento natural do controle em ambos os analógicos
// Cima - Baixo +
// Direita + Esquerda -

const int32_t minAnalogR_Y = -508, centerAnalogR_Y = 0, maxAnalogR_Y = 512;  // Valores reais * -1
const int32_t minAnalogL_X = -512, centerAnalogL_X = 0, maxAnalogL_X = 508;

const int32_t toleranciaAnalogico = 10;  // zona morta do controle

// Pino de controle em HIGH: 255 (menor velocidade) a 0 (maior velocidade)
// Pino de controle em LOW: 0 (menor velocidade) a 255 (maior velocidade)

const int minPWM = 0, maxPWM = 255;  // valores limite para o PWM da ESP32

#endif