#include "led_sensor.h"
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

static const struct device *get_led_sensor_device(void) {
  const struct device *const dev = DEVICE_DT_GET_ANY(custom_led_sensor);

  if (dev == NULL) {
    /* No such node, or the node does not have status "okay". */
    printk("\nError: no device found.\n");
    return NULL;
  }

  if (!device_is_ready(dev)) {
    printk("\nError: Device \"%s\" is not ready; "
           "check the driver initialization logs for errors.\n",
           dev->name);
    return NULL;
  }

  printk("Found device \"%s\", getting sensor data\n", dev->name);
  
  return dev;
}

int main(void) {
  const struct device *dev = get_led_sensor_device();

  if (dev == NULL) {
    printk("No device found");
    return 0;
  }

  uint32_t period = 0;

  int ret = led_sensor_set_blink_period(dev, 500);

  if (ret < 0) {
    printk("led_sensor_set_blink_period failed: %d\n", ret);
    return 0;
  }

  ret = led_sensor_get_blink_period(dev, &period);
  if (ret < 0) {
    printk("led_sensor_get_blink_period failed: %d\n", ret);
    return 0;
  }

  printk("period value is now : %u\n", (uint16_t)period);

  struct sensor_value value;

  while (true) {

    sensor_sample_fetch(dev);
    k_msleep(period);
    sensor_channel_get(dev, SENSOR_CHAN_POWER, &value);
    k_msleep(period);
  }
}
