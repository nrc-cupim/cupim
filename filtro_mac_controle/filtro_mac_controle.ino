// Codigo utilizado para habilitar a lista de permissão de conexão e para adicionar dispositivos a ela.
// Dispositivos que não estão na lista de premissão não conseguirão conectar-se à ESP32.
// O endereço adicionado será armazenado na memória não volátil das ESP (Non-volatile-storage - NVS), assim como a configuração da lista de permissões.
// Dessa forma, se o dispositivo reiniciar ou se outro coódigo for carregado, tanto o endereço quanto a configuração da lista de permissões ainda estarão salvos.
// Para desabilitar a lista ou remover endereços é necessário carregar um código específico
// Ao adicionar um endereço duplicado nada acontece.

// Link do tutorial: https://bluepad32.readthedocs.io/en/latest/FAQ/#how-to-pair-just-one-controller-to-one-particular-board
// Link da biblioteca: https://github.com/ricardoquesada/bluepad32/blob/main/src/components/bluepad32/include/bt/uni_bt_allowlist.h

#include <uni.h>

// Substitua o texto entre aspas pelo endereço MAC do seu controle
static const char* endereco_mac_controle_1 = "41:42:99:41:CB:7F";
//static const char* endereco_mac_controle_2 = "98:B6:A7:7E:3B:F9";

static const char* endereco_mac_remover = "98:BC:19:6F:FC:A3";

void setup() {
  bd_addr_t endereco_1, endereco_2, endereco_rm;

  // Analisa endereço Bluetooth.
  sscanf_bd_addr(endereco_mac_controle_1, endereco_1);
  //sscanf_bd_addr(endereco_mac_controle_2, endereco_2);
  sscanf_bd_addr(endereco_mac_remover, endereco_rm);

  // Adiciona endereço à lista de permissão
  uni_bt_allowlist_add_addr(endereco_1);
  //uni_bt_allowlist_add_addr(endereco_2);

  // Remove endereço bluetooth da lista de permissão
  uni_bt_allowlist_remove_addr(endereco_rm);

  // Habilita lista de permissão
  uni_bt_allowlist_set_enabled(true);

  // Para desabilitar a lista de permissão, basta trocar true para false no comando acima
}

void loop() {
  // Deve estar vazio
}