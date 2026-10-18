#include <stdint.h>
#define DT_DRV_COMPAT custom_led_sensor

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include "led_sensor.h"

LOG_MODULE_REGISTER(led_sensor, CONFIG_SENSOR_LOG_LEVEL);

#define LED_SENSOR_DEFAULT_PERIOD_MS 1000

struct led_sensor_config {
  struct gpio_dt_spec gpio;
};

struct led_sensor_data {
  const struct device *dev;
  bool ledStatus;
  uint32_t blinkPeriodMs;
};

static int led_sensor_sample_fetch(const struct device *dev,
                                   enum sensor_channel chan) {

  const struct led_sensor_config *config = dev->config;
  struct led_sensor_data *data = dev->data;
  int ret;

  ARG_UNUSED(chan);

  ret = gpio_pin_set_dt(&config->gpio, 1);
  if (ret < 0) {
    printk("Turn on failed: %d", ret);
    return ret;
  }

  data->ledStatus = true;
  printk("Turn On the Led\n");
  return 0;
}

static int led_sensor_channel_get(const struct device *dev,
                                  enum sensor_channel chan,
                                  struct sensor_value *val) {
  const struct led_sensor_config *config = dev->config;
  struct led_sensor_data *data = dev->data;
  int ret;

  ARG_UNUSED(chan);

  ret = gpio_pin_set_dt(&config->gpio, 0);
  if (ret < 0) {
    printk("Turn off failed: %d", ret);
    return ret;
  }

  data->ledStatus = false;
  if (val != NULL) {
    val->val1 = 0;
    val->val2 = 0;
  }

  printk("Turn Off the Led\n");
  return 0;
}

static int led_sensor_set_blinking_period(const struct device *dev,
                                       uint32_t period_ms) {
  struct led_sensor_data *data = dev->data;

  if (period_ms == 0) {
    return -EINVAL;
  }

  data->blinkPeriodMs = period_ms;
  LOG_DBG("Blink period set to %u ms", period_ms);
  return 0;
}

static int led_sensor_get_blinking_period(const struct device *dev,
                                       uint32_t *period_ms) {
  struct led_sensor_data *data = dev->data;

  if (period_ms == NULL) {
    return -EINVAL;
  }

  *period_ms = data->blinkPeriodMs;
  return 0;
}

static const struct led_sensor_driver_api led_sensor_api = {
    .base = {.channel_get = led_sensor_channel_get,
             .sample_fetch = led_sensor_sample_fetch},
.set_blinking_period = led_sensor_set_blinking_period,
    .get_blinking_period = led_sensor_get_blinking_period
}
;

static int led_sensor_init(const struct device *dev) {
  const struct led_sensor_config *config = dev->config;
  struct led_sensor_data *data = dev->data;
  int ret;

  if (!gpio_is_ready_dt(&config->gpio)) {
    printk("GPIO is not ready");
    return -ENODEV;
  }

  ret = gpio_pin_configure_dt(&config->gpio, GPIO_OUTPUT_INACTIVE);
  if (ret < 0) {
    printk("Configure gpio failed: %d", ret);
    return ret;
  }

  data->dev = dev;
  data->ledStatus = false;

  return 0;
}

#define LED_SENSOR_DEFINE(inst)                                                \
  static struct led_sensor_data data_##inst;                                   \
  static const struct led_sensor_config config_##inst = {                      \
      .gpio = GPIO_DT_SPEC_INST_GET(inst, led_gpios),                          \
  };                                                                           \
  SENSOR_DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL, &data_##inst,      \
                               &config_##inst, POST_KERNEL,                    \
                               LED_SENSOR_INIT_PRIORITY, &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)
