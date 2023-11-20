#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"

// CGI handler which is run when a request for /led.cgi is detected
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if an request for LED has been made (/led.cgi?led=x)
    if (strcmp(pcParam[0] , "led") == 0){
        // Look at the argument to check if LED is to be turned on (x=1) or off (x=0)
        if(strcmp(pcValue[0], "0") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        else if(strcmp(pcValue[0], "1") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    
    // Send the index page back to the user
    return "/index.shtml";
}

// CGI handler which is run when a request for /led.cgi is detected
const char * cgi_direction_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if (strcmp(pcParam[0] , "direction") == 0){
        if(strcmp(pcValue[0], "0") == 0){
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            motor_stop();
            sleep_ms(500);    
            //motor_forward(1);
        }
        else if(strcmp(pcValue[0], "1") == 0){
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            motor_stop();
            sleep_ms(500);    
            //motor_reverse(1);
        }
        else if(strcmp(pcValue[0], "2") == 0){
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            motor_stop();
            sleep_ms(500);    
            //motor_forward_left(1);
        }
        else if(strcmp(pcValue[0], "3") == 0){
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            motor_stop();
            sleep_ms(500);    
            //motor_forward_right(1);
        }
        else if(strcmp(pcValue[0], "4") == 0){
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            motor_stop();
            sleep_ms(500);    
        }
    }
    
    // Send the index page back to the user
    return "/index.shtml";
}

// tCGI Struct
// Fill this with all of the CGI requests and their respective handlers
static const tCGI cgi_handlers[] = {
    {
        // Html request for "/led.cgi" triggers cgi_led_handler
        "/led.cgi", cgi_led_handler
    },
    {
        // Html request for "/direction.cgi" triggers cgi_direction_handler
        "/direction.cgi", cgi_direction_handler
    },
};


void cgi_init(void)
{
    http_set_cgi_handlers(cgi_handlers, 2);
}