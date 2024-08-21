
#ifndef parametros_h
#define parametros_h

#include <Arduino.h>

#define SENTIDO_MOTOR_ESQUERDO 26
#define VELOCIDADE_MOTOR_ESQUERDO 25

#define SENTIDO_MOTOR_DIREITO 33
#define VELOCIDADE_MOTOR_DIREITO 32

#define PINO_ARMA1 27
#define PINO_ARMA2 14

// Comportamento natural do controle em ambos os analógicos
// Cima - Baixo +
// Direita + Esquerda -

const int32_t minAnalogR_Y = -508, centerAnalogR_Y = 0, maxAnalogR_Y = 512;  // Valores reais * -1
const int32_t minAnalogL_X = -512, centerAnalogL_X = 0, maxAnalogL_X = 508;

const int32_t toleranciaAnalogico = 10;  // zona morta do controle

// Pino de controle em HIGH: 255 (menor velocidade) a 0 (maior velocidade)
// Pino de controle em LOW: 0 (menor velocidade) a 255 (maior velocidade)

const int minPWM = 0, maxPWM = 255;

#endif