#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_dayOfWeek_layer;
static TextLayer *s_percent_layer;
static TextLayer *s_step_layer;


static GFont s_time_font;
static GFont s_date_font;
static GFont s_dayOfTheWeek_font;
static GFont s_percent_font;
static GFont s_step_font;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char date_buffer[16];
  static char day_buffer[16];
  static char percent_buffer[8];
  static char step_buffer[16];

  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  strftime(date_buffer, sizeof(date_buffer), 
                                          "%B %e", tick_time);
  
  strftime(day_buffer, sizeof(day_buffer), "%A", tick_time);
  
    

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
  text_layer_set_text(s_dayOfWeek_layer, day_buffer);
  
  BatteryChargeState charge_state = battery_state_service_peek();
  snprintf (percent_buffer, sizeof(percent_buffer), "%d%%", charge_state.charge_percent);
  text_layer_set_text(s_percent_layer, percent_buffer);   
  
  HealthMetric metric = HealthMetricStepCount;
  time_t start = time_start_of_today();
  time_t end = time(NULL);

  // Check the metric has data available for today
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, 
    start, end);

  if(mask & HealthServiceAccessibilityMaskAvailable) {
    // Data is available!
     snprintf (step_buffer, sizeof(step_buffer), "%d", (int)health_service_sum_today(metric));
    text_layer_set_text(s_step_layer, step_buffer);   

  } 
 
 if (charge_state.is_charging) {
   text_layer_set_text_color(s_time_layer, GColorVividCerulean);
 } else if (charge_state.charge_percent < 11) {
  text_layer_set_text_color(s_time_layer, GColorMelon);
 } else if (charge_state.charge_percent < 31) {
    text_layer_set_text_color(s_time_layer, GColorRajah);
 } else if (charge_state.charge_percent < 101) {
    text_layer_set_text_color(s_time_layer, GColorWhite);
 } else {
    text_layer_set_text_color(s_time_layer, GColorLavenderIndigo);

 }
 text_layer_set_text_color(s_step_layer, GColorClear);
 text_layer_set_text_color(s_percent_layer, GColorClear); 
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Create Date Layer
  s_date_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(125, 100), bounds.size.w - 20, 30));

  // Style te text
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorPictonBlue);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  text_layer_set_text(s_date_layer, "Loading...");
  
  //Date Of Week
  s_dayOfWeek_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(40, 40), bounds.size.w - 20, 30));
   // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_dayOfWeek_layer, GColorClear);
  text_layer_set_text_color(s_dayOfWeek_layer, GColorPictonBlue);
  text_layer_set_text(s_dayOfWeek_layer, "Someday");
  text_layer_set_text_alignment(s_dayOfWeek_layer, GTextAlignmentRight);

  s_percent_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(150, 150), bounds.size.w - 20, 30));

  // Style te text
  text_layer_set_background_color(s_percent_layer, GColorClear);
  text_layer_set_text_color(s_percent_layer, GColorPictonBlue);
  text_layer_set_text_alignment(s_percent_layer, GTextAlignmentLeft);
  text_layer_set_text(s_percent_layer, "0%");
  
  s_step_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(0, 0), bounds.size.w - 20, 30));

  // STEPs
  text_layer_set_background_color(s_step_layer, GColorClear);
  text_layer_set_text_color(s_step_layer, GColorWhite);
  text_layer_set_text_alignment(s_step_layer, GTextAlignmentLeft);
  text_layer_set_text(s_step_layer, "0");

  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLACIALINDIFFERENCE_REGULAR_48));
  //fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLACIALINDIFFERENCE_REGULAR_22));
  s_dayOfTheWeek_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLACIALINDIFFERENCE_REGULAR_18));
  s_percent_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLACIALINDIFFERENCE_REGULAR_12));
  s_step_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLACIALINDIFFERENCE_REGULAR_18));



  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  
  // Apply to DateLayer
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_font(s_percent_layer, s_percent_font);
  text_layer_set_font(s_step_layer, s_step_font);

  // Apply DateOfTheWeek
  text_layer_set_font(s_dayOfWeek_layer, s_dayOfTheWeek_font);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
    // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_dayOfWeek_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_percent_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_step_layer));

  
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_dayOfWeek_layer);
  text_layer_destroy(s_percent_layer);
  text_layer_destroy(s_step_layer);


 // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);  
  fonts_unload_custom_font(s_dayOfTheWeek_font);
  fonts_unload_custom_font(s_percent_font);

}

static void showExtras() {
 text_layer_set_text_color(s_step_layer, GColorWhite);
 text_layer_set_text_color(s_percent_layer, GColorWhite); 
}


static void tap_handler(AccelAxisType axis, int32_t direction) {
 showExtras();
}





static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set the background color
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Tap handler
  accel_tap_service_subscribe(tap_handler);
  //window_set_click_config_provider(s_main_window, (ClickConfigProvider) config_provider);

}



static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  accel_tap_service_unsubscribe();
  tick_timer_service_unsubscribe();

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}