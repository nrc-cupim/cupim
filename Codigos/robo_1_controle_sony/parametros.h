
#ifndef parametros_h
#define parametros_h

#include <Arduino.h>

#define PINO_1_MOTOR_ESQUERDO 33
#define PINO_2_MOTOR_ESQUERDO 32

#define PINO_1_MOTOR_DIREITO 26
#define PINO_2_MOTOR_DIREITO 25

#define PINO_ARMA 27

// Comportamento natural do controle em ambos os analógicos
// Cima - Baixo +
// Direita + Esquerda -

const int32_t minAnalogR_Y = -512, centerAnalogR_Y = -4, maxAnalogR_Y = 508;  // Valores reais * -1
const int32_t minAnalogL_X = -508, centerAnalogL_X = 4, maxAnalogL_X = 512;

const int32_t toleranciaAnalogico = 10;  // zona morta do controle

// Pino de controle em HIGH: 255 (menor velocidade) a 0 (maior velocidade)
// Pino de controle em LOW: 0 (menor velocidade) a 255 (maior velocidade)

const int minPWM = 0, maxPWM = 200;

#endif