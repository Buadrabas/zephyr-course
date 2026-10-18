#ifndef LED_SENSOR_H_
#define LED_SENSOR_H_

#include <asm-generic/errno.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*led_sensor_set_blink_period_t)(const struct device *dev,
                                             uint32_t period_ms);
typedef int (*led_sensor_get_blink_period_t)(const struct device *dev,
                                             uint32_t *period_ms);

struct led_sensor_driver_api {
  struct sensor_driver_api base;
  led_sensor_set_blink_period_t set_blinking_period;
  led_sensor_get_blink_period_t get_blinking_period;
};

static inline int led_sensor_set_blink_period(const struct device *dev,
                                              uint32_t period_ms) {
  const struct led_sensor_driver_api *api =
      (const struct led_sensor_driver_api *)dev->api;

  if (api->set_blinking_period == NULL) {
    return -ENOSYS;
  }

  return api->set_blinking_period(dev, period_ms);
}

static inline int led_sensor_get_blink_period(const struct device *dev,
                                              uint32_t *period_ms) {
  const struct led_sensor_driver_api *api =
      (const struct led_sensor_driver_api *)dev->api;

  if (api->set_blinking_period == NULL) {
    return -ENOSYS;
  }

  return api->get_blinking_period(dev, period_ms);
}

#ifdef __cplusplus
}
#endif

#endif /*LED_SENSOR_H_*/