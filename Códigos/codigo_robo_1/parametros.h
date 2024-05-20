// Arquivo de parâmetros utilizados no código
// Criado para deixar o código + organizado

#ifndef parametros_h

#define parametros_h

#include <Arduino.h>

#define SENTIDO_MOTOR_ESQUERDO 33
#define VELOCIDADE_MOTOR_ESQUERDO 32

#define SENTIDO_MOTOR_DIREITO 26
#define VELOCIDADE_MOTOR_DIREITO 25 

#define PINO_ARMA 27

// Comportamento natural do controle em ambos os analógicos
// Cima - Baixo +
// Direita + Esquerda -

const int32_t minAnalogR = -508, centerAnalogR = 0, maxAnalogR = 512;  // vide teste com controle. Valor negado
const int32_t minAnalogL = -512, centerAnalogL = 0, maxAnalogL = 508;  // vide teste com controle

const int32_t toleranciaAnalogico = 20;  // zona morta do controle

// Pino de controle em HIGH: 255 (menor velocidade) a 0 (maior velocidade)
// Pino de controle em LOW: 0(menor velocidade) a 255 (maior velocidade)

const int minPWM = 0, maxPWM = 255;

#endif