#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gptimer.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define CONTROL_PILOT_GPIO_NUM 33
#define RELAY_GPIO_PIN GPIO_NUM_33  // Change to your relay's GPIO pin
#define TAG "ADC"
#define AC1_0_EVRE (1)

gptimer_handle_t gptimer = NULL;
uint32_t time = 0;
uint32_t duty = 53;
uint32_t duty_old = 0;
uint32_t timeIncrement = 100;
uint32_t ADC_Result = 0;
uint32_t loopCount = 0;
volatile int32_t gs32EVSE_SMW_Control_pilot_Average = 0.0;
#if AC1_0_EVRE

#define EVSE_STATE_A_UPPER_THRESHOULD (4096)

#define EVSE_STATE_A_LOWER_THRESHOULD (3600)

#define EVSE_STATE_B_UPPER_THRESHOULD (3599)

#define EVSE_STATE_B_LOWER_THRESHOULD (3400) // previous 2800

#define EVSE_STATE_C_UPPER_THRESHOULD (1700)

#define EVSE_STATE_C_LOWER_THRESHOULD (1300)

#define EVSE_STATE_D_UPPER_THRESHOULD (1299)

#define EVSE_STATE_D_LOWER_THRESHOULD (850)

#define EVSE_STATE_E_THRESHOULD (850)

#define EVSE_STATE_SUS_UPPER_THRESHOULD (1900)

#define EVSE_STATE_SUS_LOWER_THRESHOULD (1800)

#define EVSE_STATE_E2_UPPER_THRESHOULD (2999) // previous 2800

#define EVSE_STATE_E2_LOWER_THRESHOULD (2900)

#define EVSE_STATE_DIS_UPPER_THRESHOULD (2200)

#define EVSE_STATE_DIS_LOWER_THRESHOULD (2050)

#endif
void ControlPilotPwmInit(void)
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 30000,
        .clk_cfg = LEDC_AUTO_CLK};

    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = CONTROL_PILOT_GPIO_NUM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 50,
        .hpoint = 0};
    ledc_channel_config(&channel);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void SetControlPilotDuty(uint32_t Percentage)
{
    uint32_t duty = 0;
    if ((Percentage <= 100) && (Percentage >= 0))
    {
        duty = ((Percentage * 1024) / 100);
    }
    else
    {
        duty = 1024;
    }
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void delayMicroSeconds(gptimer_handle_t gptimer, uint32_t time)
{
    uint64_t timer_count = 0;
    gptimer_set_raw_count(gptimer, 0);
    gptimer_get_raw_count(gptimer, &timer_count);
    while (timer_count < time)
    {
        gptimer_get_raw_count(gptimer, &timer_count);
    }
}

void ControlPilotAdcInit(void)
{

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
}

uint32_t    readAdcValue(void)
{
    uint32_t i = 0;
    int val = 0;
    for (i = 0; i < 300; i++)
    {
        val = val + adc1_get_raw(ADC1_CHANNEL_7);
        delayMicroSeconds(gptimer, 10);
    }
    val = val / 300;

    return (uint32_t)val;
}

#if 1



// Function to initialize the relay
void relay_init()
{
    // Configure the GPIO pin for relay as output
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY_GPIO_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

// Function to set the relay state (1 for ON, 0 for OFF)
void relay_set(int state)
{
    gpio_set_level(RELAY_GPIO_PIN, state);
}


#endif

void app_main(void)
{
    // relay_init();
    ControlPilotAdcInit();
    ControlPilotPwmInit();
    //  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    
    #if 0
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        time++;
        if (time > timeIncrement)
        {
            duty = 16;
        }
        if (time > (timeIncrement * 2))
        {
            duty = 26;
        }
        if (time > (timeIncrement * 3))
        {
            duty = 33;
        }
        if (time > (timeIncrement * 4))
        {
            duty = 41;
        }
        if (time > (timeIncrement * 5))
        {
            duty = 53;
        }
        if (time > (timeIncrement * 6))
        {
            duty = 66;
        }
        if (time > (timeIncrement * 7))
        {
            duty = 83;
        }
        if (time > (timeIncrement * 8))
        {
            duty = 89;
        }
        if (time > (timeIncrement * 9))
        {
            time = 0;
            duty = 100;
        }
        if (duty != duty_old)
        {
            ESP_LOGI(TAG, " ");
            ESP_LOGI(TAG, "**************************************");
            ESP_LOGI(TAG, "TESTING STARTING WITH DUTY : %ld", duty);
            ESP_LOGI(TAG, "**************************************");
            SetControlPilotDuty(duty);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "\tDuty : %ld\tADC : %ld", duty, readAdcValue());

        duty_old = duty;
    }
    #endif
    // duty = 100;
    SetControlPilotDuty(duty);
    // relay_set(1);  // Turn relay ON
    // relay_set(0);  // Turn relay OFF
    
    while(1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        // time++;
        // if(time >= timeIncrement)
        // {
        //      duty = 25;
        //      SetControlPilotDuty(duty);
        // }
        //  if(time >= timeIncrement* 2)
        // {
        //     duty = 100;
        //     time = 0;

        // }
        //  if (duty != duty_old)
        // {
        //     ESP_LOGI(TAG, " ");
        //     ESP_LOGI(TAG, "**************************************");
        //     ESP_LOGI(TAG, "TESTING STARTING WITH DUTY : %ld", duty);
        //     ESP_LOGI(TAG, "**************************************");
        //     SetControlPilotDuty(duty);
        //     vTaskDelay(100 / portTICK_PERIOD_MS);
        // }
        ADC_Result = readAdcValue();
        gs32EVSE_SMW_Control_pilot_Average = ADC_Result;
        loopCount++;
		// if (loopCount % 30 == 0)
		if (loopCount % 15 == 0)
		{
            // Serial.print("[ControlPilot] ADC Value : ");
            
            // Serial.println(ADC_Result);
        ESP_LOGI(TAG, "\tDuty : %ld\tADC : %ld", duty, ADC_Result );

			if ((gs32EVSE_SMW_Control_pilot_Average < EVSE_STATE_A_UPPER_THRESHOULD) &&
				(gs32EVSE_SMW_Control_pilot_Average > EVSE_STATE_A_LOWER_THRESHOULD))
			{
				ESP_LOGI(TAG, "STATE_A");
			}

			/* ADC result is between 2.8 and 2.33 volts */
			else if ((gs32EVSE_SMW_Control_pilot_Average < EVSE_STATE_B_UPPER_THRESHOULD) &&
				(gs32EVSE_SMW_Control_pilot_Average > EVSE_STATE_B_LOWER_THRESHOULD))
			{

                ESP_LOGI(TAG, "STATE_B");
			}

			/* ADC result is between 1.49 and 1.32 volts */ // implemented based on observation
			else if ((gs32EVSE_SMW_Control_pilot_Average < EVSE_STATE_SUS_UPPER_THRESHOULD) &&
				(gs32EVSE_SMW_Control_pilot_Average > EVSE_STATE_SUS_LOWER_THRESHOULD)) // 1750 working
			{

                ESP_LOGI(TAG, "STATE_SUS");
			}
			else if ((gs32EVSE_SMW_Control_pilot_Average < EVSE_STATE_E2_UPPER_THRESHOULD) &&
				(gs32EVSE_SMW_Control_pilot_Average > EVSE_STATE_E2_LOWER_THRESHOULD))
			{

                ESP_LOGI(TAG, "STATE_E2");
			}
			/* ADC result is between 1.73 and 1.49 volts */
			// DIS state changed from 2100 to 2700 by chandu observation
			else if ((gs32EVSE_SMW_Control_pilot_Average < EVSE_STATE_DIS_UPPER_THRESHOULD) &&
				(gs32EVSE_SMW_Control_pilot_Average > EVSE_STATE_DIS_LOWER_THRESHOULD)) // 1750 working
			{

                ESP_LOGI(TAG, "STATE_DIS");
			}

			/* ADC result is between 1.32 and 1.08 volts */
			else if ((gs32EVSE_SMW_Control_pilot_Average < EVSE_STATE_C_UPPER_THRESHOULD) &&
				(gs32EVSE_SMW_Control_pilot_Average > EVSE_STATE_C_LOWER_THRESHOULD))
			{

                ESP_LOGI(TAG, "STATE_C");
			}
			/* ADC result is between 1.08 and 0.60 volts */
			else if ((gs32EVSE_SMW_Control_pilot_Average < EVSE_STATE_D_UPPER_THRESHOULD) &&
				(gs32EVSE_SMW_Control_pilot_Average > EVSE_STATE_D_LOWER_THRESHOULD))
			{

                ESP_LOGI(TAG, "STATE_D");
			}

			else if (gs32EVSE_SMW_Control_pilot_Average < EVSE_STATE_E_THRESHOULD)
			{

                ESP_LOGI(TAG, "STATE_E");
			}

			else
			{
                // Serial.println("ADC values are not in range");
                ESP_LOGI(TAG, "ADC values are not in range");
            }

			// Serial.printf("CONTROL PILOT DUTY: %lu\n", duty);
		}


     
            // duty_old = duty;
    }



}
