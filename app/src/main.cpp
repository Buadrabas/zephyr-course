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

  struct sensor_value temp;

  while (true) {

    sensor_sample_fetch(dev);
    k_msleep(200);
    sensor_channel_get(dev, SENSOR_CHAN_POWER, &temp);
    k_msleep(200);
  }
}
