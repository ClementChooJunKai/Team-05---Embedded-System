#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"

// A global struct used to send a series of data across to SSI. Chose this method over a queue as all the tasks do not modify
// similar variables, race condition is not introduced, more memory effecient compared to constantly overwriting a queue.
struct SSI_CarData_Struct {
    int32_t compass_degree; // Stores compass bearing derived from xyz data from magnetometer
    double ultrasonic_distance; // Stores ultrasonic sensor readings
    xyz_struct_float accelerometer_readings_in_Gs; // Stores accelerometer readings of xyz acceleration (difference in Gs)
};

// Declare the struct instance globally
extern struct SSI_CarData_Struct ssi_car_data;

// SSI tags - tag length limited to 8 bytes by default
const char *ssi_tags[] = {"volt", "temp", "led", "deg", "u_dist"};

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen)
{
  size_t printed;
  switch (iIndex)
  {
  case 0: // volt
  {
    const float voltage = adc_read() * 3.3f / (1 << 12);
    printed = snprintf(pcInsert, iInsertLen, "%f", voltage);
  }
  break;
  case 1: // temp
  {
    const float voltage = adc_read() * 3.3f / (1 << 12);
    const float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
    printed = snprintf(pcInsert, iInsertLen, "%f", tempC);
  }
  break;
  case 2: // led
  {
    bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
    if (led_status == true)
    {
      printed = snprintf(pcInsert, iInsertLen, "ON");
    }
    else
    {
      printed = snprintf(pcInsert, iInsertLen, "OFF");
    }
  }
  break;
  case 3: // magnetnometer bearing
  {
    printed = snprintf(pcInsert, iInsertLen, "%d", ssi_car_data.compass_degree);
    printf("%.2d",ssi_car_data.compass_degree);
  }
  break;
  case 4: // ultrasonic_distance
  {
    printed = snprintf(pcInsert, iInsertLen, "%.2f", ssi_car_data.ultrasonic_distance);
    printf("%.2f",ssi_car_data.ultrasonic_distance);
  }
  break;
  case 5: // accelerometer
  {
    
  }
  break;
  
    break;

  default:
    printed = 0;
    break;
  }

  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init()
{
  // Initialise ADC (internal pin)
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}
