#ifndef HELPER_COM_rec
#define HELPER_COM_rec
/*
  Rui Santos & Sara Santos, Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files.
*/
//--------------------------------------------------------------------
#include <esp_now.h>
#include <WiFi.h>
#include <helper_DATA.h>
//--------------------------------------------------------------------
uint8_t receiver_MAC_Address[] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x00}; //force our mac address
//--------------------------------------------------------------------
struct_message_t myData;

//--------------------------------------------------------------------
// receive data
//--------------------------------------------------------------------
static void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");
  memcpy(&myData, data, sizeof(myData));
  printData(myData);
}

//--------------------------------------------------------------------
// setup our comm
//--------------------------------------------------------------------
void setupCOMrecv(void) {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry or Simply Restart
    ESP.restart();
  }

  Serial.print("\nOLD ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
  esp_wifi_set_mac(WIFI_IF_STA, receiver_MAC_Address);
  Serial.print("NEW ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());

  //setup callback function
  esp_now_register_recv_cb(OnDataRecv);
}
//--------------------------------------------------------------------
#endif //HELPER_COM