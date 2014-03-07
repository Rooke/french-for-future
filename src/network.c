//
//  network.c
//  French for Future
//
//  Created by Julian Lepinski on 2014-02-12
//  Based on Futura Weather by Niknam (https://github.com/Niknam/futura-weather-sdk2.0)
//

#include <pebble.h>
#include "network.h"

static void appmsg_in_received(DictionaryIterator *received, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "appmsg_in_received.");

  UpdateData *update_data = (UpdateData*) context;

  Tuple *temperature_tuple = dict_find(received, KEY_TEMPERATURE);
  Tuple *btc_tuple = dict_find(received, KEY_BTC);
  Tuple *condition_tuple = dict_find(received, KEY_CONDITION);
  Tuple *error_tuple = dict_find(received, KEY_ERROR);
  Tuple *colourscheme_tuple = dict_find(received, KEY_COLOURSCHEME);
  Tuple *bitcoin_tuple = dict_find(received, KEY_BITCOIN);

  if (temperature_tuple && condition_tuple) {
    update_data->temperature = temperature_tuple->value->int32;
    update_data->condition = condition_tuple->value->int32;
    update_data->error = WEATHER_E_OK;
    update_data->updated = time(NULL);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got temperature %i and condition %i", weather->temperature, weather->condition);
  } else if (bitcoin_tuple){
    update_data->bitcoin = bitcoin_tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got bitcoin %i", weather->bitcoin);
  } else if (error_tuple) {
    update_data->error = WEATHER_E_NETWORK;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got error %s", error_tuple->value->cstring);
  } else if (colourscheme_tuple){
    const char *colourScheme = colourscheme_tuple->value->cstring;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got colour scheme %s", colourscheme_tuple->value->cstring);
    if (strcmp(colourScheme, "light") == 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "parsed that as light");
      cbf(false);
    } else if (strcmp(colourScheme, "dark") == 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "parsed that as dark");
      cbf(true);
    } else {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "failed to parse the colour scheme");
    }
  } else {
    update_data->error = WEATHER_E_PHONE;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got message with unknown keys... temperature=%p condition=%p error=%p",
      temperature_tuple, condition_tuple, error_tuple);
  }
}

static void appmsg_in_dropped(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "In dropped: %i", reason);
  // Request a new update...
  request_weather();
}

static void appmsg_out_sent(DictionaryIterator *sent, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Out sent.");
}

static void appmsg_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  WeatherData *weather = (WeatherData*) context;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Out failed: %i", reason);

  switch (reason) {
    case APP_MSG_NOT_CONNECTED:
      weather->error = UPDATE_E_DISCONNECTED;
      request_weather();
      break;
    case APP_MSG_SEND_REJECTED:
    case APP_MSG_SEND_TIMEOUT:
      weather->error = UPDATE_E_PHONE;
      request_weather();
      break;
    default:
      weather->error = UPDATE_E_PHONE;
      request_weather();
      break;
  }
}

void init_network(UpdateData *update_data, void (*callbackFunction)(bool)) {
  cbf = callbackFunction;
  app_message_register_inbox_received(appmsg_in_received);
  app_message_register_inbox_dropped(appmsg_in_dropped);
  app_message_register_outbox_sent(appmsg_out_sent);
  app_message_register_outbox_failed(appmsg_out_failed);
  app_message_set_context(update_data);
  app_message_open(124, 256);

  update_data->error = UPDATE_E_OK;
  update_data->updated = 0;

}

void close_network() {
  app_message_deregister_callbacks();
}

void request_update()
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  dict_write_uint8(iter, KEY_REQUEST_UPDATE, 42);

  app_message_outbox_send();
}
