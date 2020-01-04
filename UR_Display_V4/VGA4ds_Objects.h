#ifndef VGA4ds_Objects_h
#define VGA4ds_Objects_h

VGA4dsObject master_left_led_4ds;
VGA4dsObject master_right_led_4ds;
VGA4dsObject left_level_led_4ds;
VGA4dsObject right_level_led_4ds;

VGA4dsObject leveler_mode_4ds;
VGA4dsObject hours_4ds;
VGA4dsObject minutes_4ds;
VGA4dsObject date_4ds;
VGA4dsObject month_4ds;

VGA4dsObject ground_speed_4ds;
VGA4dsObject supply_voltage_4ds;
VGA4dsObject supply_current_4ds;

VGA4dsObject left_ram_position_4ds;
VGA4dsObject right_ram_position_4ds;
VGA4dsObject bucket_speed_4ds;
VGA4dsObject head_speed_4ds;

void Init4DSObjects(){
  
  left_level_led_4ds.Id = VGA4DS_LED;
  left_level_led_4ds.Index = 0;
  left_level_led_4ds.Value = 0;

  right_level_led_4ds.Id = VGA4DS_LED;
  right_level_led_4ds.Index = 1;
  right_level_led_4ds.Value = 0;

  master_left_led_4ds.Id = VGA4DS_LED;
  master_left_led_4ds.Index = 2;
  master_left_led_4ds.Value = 1;

  master_right_led_4ds.Id = VGA4DS_LED;
  master_right_led_4ds.Index = 3;
  master_right_led_4ds.Value = 0;
  
  leveler_mode_4ds.Id = VGA4DS_STRINGS;
  leveler_mode_4ds.Index = 1;
  leveler_mode_4ds.Value = 0;
  
  hours_4ds.Id = VGA4DS_STRINGS;
  hours_4ds.Index = 2;
  hours_4ds.Value = 0;  

  minutes_4ds.Id = VGA4DS_STRINGS;
  minutes_4ds.Index = 3;
  minutes_4ds.Value = 0;  

  date_4ds.Id = VGA4DS_STRINGS;
  date_4ds.Index = 4;
  date_4ds.Value = 0; 
  
  month_4ds.Id = VGA4DS_STRINGS;
  month_4ds.Index = 5;
  month_4ds.Value = 1;  
  
  ground_speed_4ds.Id = VGA4DS_LEDDIGITS;
  ground_speed_4ds.Index = 0;
  ground_speed_4ds.Value = 0;
  
  supply_voltage_4ds.Id = VGA4DS_LEDDIGITS;
  supply_voltage_4ds.Index = 1;
  supply_voltage_4ds.Value = 0;  
  
  supply_current_4ds.Id = VGA4DS_LEDDIGITS;
  supply_current_4ds.Index = 2;
  supply_current_4ds.Value = 0;
  
  bucket_speed_4ds.Id = VGA4DS_LEDDIGITS;
  bucket_speed_4ds.Index = 3;
  bucket_speed_4ds.Value = 0;
  
  head_speed_4ds.Id = VGA4DS_LEDDIGITS;
  head_speed_4ds.Index = 4;
  head_speed_4ds.Value = 0;  
  
  left_ram_position_4ds.Id = VGA4DS_GAUGE;
  left_ram_position_4ds.Index = 0;
  left_ram_position_4ds.Value = 0;

  right_ram_position_4ds.Id = VGA4DS_GAUGE;
  right_ram_position_4ds.Index = 1;
  right_ram_position_4ds.Value = 0;
}
  
#endif
