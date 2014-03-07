//
//  network.h
//  French for Future
//
//  Created by Julian Lepinski on 2014-02-12
//  Based on Futura Weather by Niknam (https://github.com/Niknam/futura-weather-sdk2.0)
//

#include <pebble.h>

// these are our dictionary keys – they all correspond to stuff in appinfo.json
#define KEY_TEMPERATURE 0
#define KEY_CONDITION 1
#define KEY_ERROR 2
#define KEY_COLOURSCHEME 3
#define KEY_BITCOIN 4
#define KEY_REQUEST_UPDATE 42

typedef enum {
  UPDATE_E_OK = 0,
  UPDATE_E_DISCONNECTED,
  UPDATE_E_PHONE,
  UPDATE_E_NETWORK
} UpdateError;

typedef struct {
  int temperature;
  int btc_price;
  int condition;
  time_t updated;
  int bitcoin;
  UpdateError error;
} UpdateData;

void (*cbf)(bool);
void init_network(UpdateData *update_data, void (*callbackFunction)(bool));
void close_network();
void request_update();
