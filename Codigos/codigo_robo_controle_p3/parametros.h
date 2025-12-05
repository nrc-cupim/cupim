
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

// Ambos os analógicos (E e D) tem o mesmo comportamento, retornam os mesmos valores para os mesmos sentidos / direções
// Ambas as direções retornam a mesma faixa de valores (-512 a 508, tanto na vertical quanto horizontal)
// Os valores retornados para cada um dos sentidos em uma mesma direção (esquerda/direita ou cima/baixo) não são simétricos

const int32_t MIN_JOYSTICK_Y = -512, PARADO_JOYSTICK_Y = 0, MAX_JOYSTICK_Y = 508;
const int32_t MIN_JOYSTICK_X = -512, PARADO_JOYSTICK_X = 0, MAX_JOYSTICK_X = 508;

const int32_t TOLERANCIA_JOYSTICK = 5;  // zona morta do controle

const int MIN_PWM = 0, MAX_PWM = 255;  // valores limite para o PWM da ESP32

#endif