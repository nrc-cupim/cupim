// Código utilizado para habilitar a lista de permissão de conexão e para adicionar dispositivos a ela.
// Dispositivos que não estão na lista de permissão não conseguirão se conectar à ESP32.
// O endereço adicionado será armazenado na memória não volátil das ESP (Non-volatile-storage - NVS), assim como a configuração da lista de permissões.
// Dessa forma, se o dispositivo reiniciar ou se outro coódigo for carregado, tanto o endereço quanto a configuração da lista de permissões ainda estarão salvos.
// É possível armazenar até quatro endereços MAC na memória. Se a memória estiver cheia, é necessário remover um endereço antes de adicionar outro.
// Ao adicionar um endereço duplicado nada acontece.

// Link do tutorial: https://bluepad32.readthedocs.io/en/latest/FAQ/#how-to-pair-just-one-controller-to-one-particular-board
// Link da biblioteca: https://github.com/ricardoquesada/bluepad32/blob/main/src/components/bluepad32/include/bt/uni_bt_allowlist.h

#include <uni.h>  // inclusão da biblioteca que implementa o filtro

// Substitua o texto entre aspas pelo endereço MAC do seu controle.
static const char* endereco_mac_controle_1 = "98:B6:24:15:0E:6F";

void setup() {

  // Converte endereço MAC e salva na variável endereco_1
  bd_addr_t endereco_1;
  sscanf_bd_addr(endereco_mac_controle_1, endereco_1);

  // Remove endereço bluetooth da lista de permissão
  // uni_bt_allowlist_remove_addr(endereco_1);

  // Adiciona endereço à lista de permissão.
  uni_bt_allowlist_add_addr(endereco_1);

  // Desabilita lista de permissão.
  //uni_bt_allowlist_set_enabled(false);

  // Habilita lista de permissão.
  uni_bt_allowlist_set_enabled(true);
}

void loop() {
  // Deve estar vazio.
}