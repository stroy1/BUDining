#include "pebble.h"

#define NUM_MENU_SECTIONS 3
#define NUM_FIRST_MENU_ITEMS 1
#define NUM_SECOND_MENU_ITEMS 1
#define NUM_THIRD_MENU_ITEMS 1
  
static void send_cmd(void);

enum{BREAKFAST, LUNCH, DINNER};

static Window *window;
static SimpleMenuLayer *simple_menu_layer;
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
// Each section is composed of a number of menu items
static SimpleMenuItem first_menu_items[NUM_FIRST_MENU_ITEMS];
static SimpleMenuItem second_menu_items[NUM_SECOND_MENU_ITEMS];
static SimpleMenuItem third_menu_items[NUM_THIRD_MENU_ITEMS];

static bool special_flag = false;
static int hit_count = 0; // cause global

static void menu_select_callback(int index, void *ctx) { // hit select on menu item
  first_menu_items[index].subtitle = "Oh you hungry though";
  layer_mark_dirty(simple_menu_layer_get_layer(simple_menu_layer));
}

static void special_select_callback(int index, void *ctx) {
  special_flag = !special_flag;
  SimpleMenuItem *menu_item = &second_menu_items[index];

  if (special_flag) {
    menu_item->subtitle = "Okay, it's not so special.";
  } else {
    menu_item->subtitle = "Well, maybe a little.";
  }

  if (++hit_count > 5) {
    menu_item->title = "Very Special Item";
  }
  layer_mark_dirty(simple_menu_layer_get_layer(simple_menu_layer));
}

// This initializes the menu upon window load
static void window_load(Window *window) {
  int num_a_items = 0;
  
  //app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      //sync_tuple_changed_callback, sync_error_callback, NULL);



  // This is an example of how you'd set a simple menu item
  first_menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "First Item", // needs update from API
    .callback = menu_select_callback,
  };

  // This initializes the second section
  second_menu_items[0] = (SimpleMenuItem){
    .title = "Special Item",
    .callback = special_select_callback,
  };
  
  third_menu_items[0] = (SimpleMenuItem){
    .title = "Food name goes here",
    .callback = special_select_callback,
  };

  // Bind the menu items to the corresponding menu sections
  menu_sections[0] = (SimpleMenuSection){
    .title = "Breakfast",
    .num_items = NUM_FIRST_MENU_ITEMS,
    .items = first_menu_items,
  };
  menu_sections[1] = (SimpleMenuSection){
    .title = "Lunch",
    .num_items = NUM_SECOND_MENU_ITEMS,
    .items = second_menu_items,
  };
  menu_sections[2] = (SimpleMenuSection){
    .title = "Dinner",
    .num_items = NUM_THIRD_MENU_ITEMS,
    .items = third_menu_items,
  };
  
  send_cmd();

  // Now we prepare to initialize the simple menu layer
  // We need the bounds to specify the simple menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Initialize the simple menu layer
  simple_menu_layer = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(simple_menu_layer));
}

// BEGIN CODE FOR SENDING MESSAGES
static void send_cmd(void) {
  Tuplet value = TupletInteger(1, 1);
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: I IS A MESSAGE");

  if (iter == NULL) return;
  dict_write_tuplet(iter, &value);
  dict_write_end(iter);
  app_message_outbox_send();
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *breakfast_tuple = dict_find(iter, BREAKFAST); // Check for fields you expect to receive
  Tuple *lunch_tuple = dict_find(iter, LUNCH);
  Tuple *dinner_tuple = dict_find(iter, DINNER);
  
  if (breakfast_tuple) { // Act on the found fields received
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", breakfast_tuple->value->cstring);
    first_menu_items[0].title = breakfast_tuple->value->cstring;
  }
  if (lunch_tuple) { // Act on the found fields received
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", lunch_tuple->value->cstring);
    second_menu_items[0].title = lunch_tuple->value->cstring;
  }
  if (dinner_tuple) { // Act on the found fields received
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", dinner_tuple->value->cstring);
    third_menu_items[0].title = dinner_tuple->value->cstring;
  }
  layer_mark_dirty(simple_menu_layer_get_layer(simple_menu_layer));
}


void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
  first_menu_items[0].callback = menu_select_callback;
  layer_mark_dirty(simple_menu_layer_get_layer(simple_menu_layer));
}
// END CODE FOR SENDING MESSAGES





// Deinitialize resources on window unload that were initialized on window load
void window_unload(Window *window) {
  simple_menu_layer_destroy(simple_menu_layer);
}

int main(void) {
  // START MESSAGE CODE
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  //app_message_register_outbox_sent(out_sent_handler);
  //app_message_register_outbox_failed(out_failed_handler);

  const uint32_t inbound_size = 128;
  const uint32_t outbound_size = 128;
  app_message_open(inbound_size, outbound_size);
  // END MESSAGE CODE
  
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  window_stack_push(window, true /* Animated */);
  app_event_loop();
  window_destroy(window);
}