#include <Bluepad32.h>
#include "parametros.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {

  bool foundEmptySlot = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {

      Serial.println("AVISO: controle conectado.");
      myControllers[i] = ctl;
      foundEmptySlot = true;

      /* Caso queira realizar alguma tarefa assim que a conexão 
      com o contole for estabelecidada, coloque o código aqui */

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

      /* Caso queira realizar alguma tarefa assim 
      que o controle for desconectado, coloque o código aqui */

      break;
    }
  }
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData() && myController->isGamepad()) {

      // Coloque seu código aqui
    }
  }
}

void setup() {

  // Inicializando comunicação serial
  Serial.begin(115200);

  // Inicializa biblioteca Bluepad32
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false);

  // Desparea os controles que haviam sido conectados anteriormente
  BP32.forgetBluetoothKeys();
}

void loop() {
  // Checa se houve atualização nos dados do controle
  bool dataUpdated = BP32.update();
  
  // Se sim, chama a função processControllers() para processar os dados
  if (dataUpdated)
    processControllers();
}