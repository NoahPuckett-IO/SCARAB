// most of this is pieced together from examples on the site
// https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/

#ifndef HELPER_COM
#define HELPER_COM
//--------------------------------------------------------------------
#include <esp_now.h>
#include <WiFi.h>
#include <helper_DATA.h>
//--------------------------------------------------------------------
uint8_t receiver_MAC_Address[] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x00};
//--------------------------------------------------------------------
esp_now_peer_info_t peerInfo;

//--------------------------------------------------------------------
// callback when data is sent
//--------------------------------------------------------------------
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//--------------------------------------------------------------------
// setup our comm
//--------------------------------------------------------------------
void setupCOM(void) {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, receiver_MAC_Address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

//--------------------------------------------------------------------
// send data
//--------------------------------------------------------------------
void sendDataCOM(struct_message_t& myData) {
  esp_err_t result = esp_now_send(receiver_MAC_Address, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}
#endif //HELPER_COM
//--------------------------------------------------------------------