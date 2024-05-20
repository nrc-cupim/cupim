// Codigo utilizado para habilitar a lista de permissão de conexão e para adicionar dispositivos a ela.
// Dispositivos que não estão na lista de permissão não conseguirão conectar-se à ESP32.
// O endereço adicionado será armazenado na memória não volátil das ESP (Non-volatile-storage - NVS), assim como a configuração da lista de permissões.
// Dessa forma, se o dispositivo reiniciar ou se outro coódigo for carregado, tanto o endereço quanto a configuração da lista de permissões ainda estarão salvos.
// Para desabilitar a lista ou remover endereços, basta seguir os passos descritos no void setup()
// Ao adicionar um endereço duplicado nada acontece.

// Link do tutorial: https://bluepad32.readthedocs.io/en/latest/FAQ/#how-to-pair-just-one-controller-to-one-particular-board
// Link da biblioteca: https://github.com/ricardoquesada/bluepad32/blob/main/src/components/bluepad32/include/bt/uni_bt_allowlist.h

#include <uni.h> // biblioteca que implementa o filtro

// Substitua o texto entre aspas pelo endereço MAC do seu controle.
static const char* endereco_mac_controle_1 = "41:42:99:41:CB:7F";

void setup() {
  // Reescreve endereço MAC
  bd_addr_t endereco_1;
  sscanf_bd_addr(endereco_mac_controle_1, endereco_1);

  // Adiciona endereço à lista de permissão.
  uni_bt_allowlist_add_addr(endereco_1);

  // Remove endereço bluetooth da lista de permissão
  // uni_bt_allowlist_remove_addr(endereco_rm);

  // Habilita lista de permissão.
  uni_bt_allowlist_set_enabled(true);

  // Desabilita lista de permissão.
  // uni_bt_allowlist_set_enabled(false);
}

void loop() {
  // Deve estar vazio.
}