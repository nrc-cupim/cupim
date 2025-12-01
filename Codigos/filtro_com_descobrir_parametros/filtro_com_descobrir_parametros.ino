// Para mais informações sobre o filtro MAC, consulte os código filtro_mac_controle

#include <Bluepad32.h>
#include <uni.h>

static const char* endereco_mac_controle_1 = "98:B6:76:66:9D:AE";
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {

  bool foundEmptySlot = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.println("\nAVISO: controle conectado.");
      myControllers[i] = ctl;
      foundEmptySlot = true;

      // Mostra o endereço MAC do controle na Serial
      uint8_t addr[6];
      for (int i = 0; i < 6; i++)
        addr[i] = ctl->getProperties().btaddr[i];
      Serial.printf("Endereco MAC do controle: %x:%x:%x:%x:%x:%x\n\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
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
      break;
    }
  }
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {

        // Exibe leitura dos valores do controle na Serial
        Serial.printf(
          "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
          "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
          myController->index(),        // Controller Index
          myController->dpad(),         // D-pad
          myController->buttons(),      // bitmask of pressed buttons
          myController->axisX(),        // left X Axis
          myController->axisY(),        // left Y axis
          myController->axisRX(),       // right X axis
          myController->axisRY(),       // right Y axis
          myController->brake(),        // (0 - 1023): brake button
          myController->throttle(),     // (0 - 1023): throttle (AKA gas) button
          myController->miscButtons(),  // bitmask of pressed "misc" buttons
          myController->gyroX(),        // Gyro X
          myController->gyroY(),        // Gyro Y
          myController->gyroZ(),        // Gyro Z
          myController->accelX(),       // Accelerometer X
          myController->accelY(),       // Accelerometer Y
          myController->accelZ()        // Accelerometer Z
        );
      }
    }
  }
}

void setup() {

  /* ------------------------------ Filtro MAC ------------------------------ */

  // Converte endereço MAC e salva na variável endereco_1
  bd_addr_t endereco_1;
  sscanf_bd_addr(endereco_mac_controle_1, endereco_1);

  // Remove endereço bluetooth da lista de permissão
  // uni_bt_allowlist_remove_addr(endereco_1);

  // Adiciona endereço à lista de permissão.
  uni_bt_allowlist_add_addr(endereco_1);

  // Desabilita lista de permissão.
  // uni_bt_allowlist_set_enabled(false);

  // Habilita lista de permissão.
  uni_bt_allowlist_set_enabled(true);

  /* ------------------------------ Descobrir parâmetros ------------------------------ */

  Serial.begin(115200);
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
}

void loop() {
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();
}