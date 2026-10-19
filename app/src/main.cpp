#include "led_sensor.h"
#include "zephyr/shell/shell_string_conv.h"
#include "zephyr/toolchain.h"
#include <cerrno>
#include <cstddef>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

#define BLINK_PERIOD_MAX 10000
#define BLINK_PERIOD_MIN 10

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

static int cmd_set_blink_period(const struct shell *sh, size_t argc,
                                char **argv) {
  uint32_t value = 0;
  if (argc < 2) {
    shell_error(sh,
                "Blink Period Value is missing. Usage: led_sensor setPeriod "
                "<period_ms> (%d..%d)",
                BLINK_PERIOD_MIN, BLINK_PERIOD_MAX);
    return -EINVAL;
  }

  if (argv[1][0] == '-') {
    shell_error(
        sh, "Illegal Value recognized: \"%s\", Negative Values aren't allowed",
        argv[1]);
    return -EINVAL;
  }

  int err = 0;
  value = shell_strtoul(argv[1], 10, &err);
  if (err != 0) {
    shell_error(sh, "Illegal Value recognized: \"%s\", Decimal value only",
                argv[1]);
    return -EINVAL;
  }

  if (value < BLINK_PERIOD_MIN || value > BLINK_PERIOD_MAX) {
    shell_error(sh, "Value is out of bounds");
    return -ERANGE;
  }

  const struct device *dev = get_led_sensor_device(sh);

  int ret = led_sensor_set_blink_period(dev, value);
  if (ret < 0) {
    shell_error(sh, "Setting new Blink period value failed: %d", ret);
    return ret;
  }

  uint32_t storedValue = 0;

  ret = led_sensor_get_blink_period(dev, &storedValue);
  if (ret < 0) {
    shell_error(sh, "Getting stored Blink period value failed: %d", ret);
    return ret;
  }

  shell_print(sh, "Value stored: %d", storedValue);

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_sensor, SHELL_CMD(fetch, NULL, "Fetch new Sensor Data", cmd_fetch),
    SHELL_CMD(read, NULL, "Read new Sensor Data", cmd_read),
    SHELL_CMD(info, NULL, "Get Device Name and State", cmd_info),
    SHELL_CMD_ARG(setBlink, NULL,
                  "Setting a new blink period value (10..10000)",
                  cmd_set_blink_period, 1, 1),
    SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(led_sensor, &sub_sensor, "LED Sensor Commands", NULL);

int main(void) { return 0; }
