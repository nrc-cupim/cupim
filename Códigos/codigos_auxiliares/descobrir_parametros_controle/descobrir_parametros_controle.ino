#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {

  bool foundEmptySlot = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.println("AVISO: controle conectado.");
      myControllers[i] = ctl;
      foundEmptySlot = true;

      uint8_t addr[6];
      for (int i = 0; i < 6; i++)
        addr[i] = ctl->getProperties().btaddr[i];
      Serial.printf("Endereco MAC: %x:%x:%x:%x:%x:%x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

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
        Serial.printf(
          "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
          "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
          myController->index(),        // Controller Index
          myController->dpad(),         // D-pad
          myController->buttons(),      // bitmask of pressed buttons
          myController->axisX(),        // (-511 - 512) left X Axis
          myController->axisY(),        // (-511 - 512) left Y axis
          myController->axisRX(),       // (-511 - 512) right X axis
          myController->axisRY(),       // (-511 - 512) right Y axis
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