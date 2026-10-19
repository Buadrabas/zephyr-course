#include "led_sensor.h"
#include "zephyr/toolchain.h"
#include <cerrno>
#include <cstddef>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static const struct device *get_led_sensor_device(const struct shell *sh) {
  const struct device *const dev = DEVICE_DT_GET_ANY(custom_led_sensor);

  if (dev == NULL) {
    /* No such node, or the node does not have status "okay". */
    shell_print(sh, "\nError: no device found.\n");
    return NULL;
  }

  if (!device_is_ready(dev)) {
    shell_print(sh,
                "\nError: Device \"%s\" is not ready; "
                "check the driver initialization logs for errors.\n",
                dev->name);
    return NULL;
  }

  shell_print(sh, "Found device \"%s\", getting sensor data\n", dev->name);

  return dev;
}

static int cmd_fetch(const struct shell *sh, size_t argc, char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  const struct device *dev = get_led_sensor_device(sh);

  int ret = sensor_sample_fetch(dev);

  if (ret < 0) {
    shell_error(sh, "Fetching new Data from \"%s\" failed");
    return ret;
  }

  shell_print(sh, "Fetching new Data successful (LED on)");

  return 0;
}

static int cmd_read(const struct shell *sh, size_t argc, char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  const struct device *dev = get_led_sensor_device(sh);

  struct sensor_value value;

  int ret = sensor_channel_get(dev, SENSOR_CHAN_POWER, &value);

  if (ret < 0) {
    shell_error(sh, "Fetching new Data from \"%s\" failed");
    return ret;
  }

  shell_print(sh, "Reading new Data successful (LED off)");

  return 0;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  const struct device *dev = get_led_sensor_device(sh);

  struct sensor_value value;

  shell_print(sh, "Device Name: %s", dev->name);
  shell_print(sh, "Device State: %s",
              device_is_ready(dev) ? "Ready" : "Not Ready");

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_sensor, SHELL_CMD(fetch, NULL, "Fetch new Sensor Data", cmd_fetch),
    SHELL_CMD(read, NULL, "Read new Sensor Data", cmd_read),
    SHELL_CMD(info, NULL, "Get Device Name and State", cmd_info),
    SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(led_sensor, &sub_sensor, "LED Sensor Commands", NULL);

int main(void) {
  return 0;
}
