#ifndef ORESAT_DRIVER_SENSOR_ICM42688_BUS_H
#define ORESAT_DRIVER_SENSOR_ICM42688_BUS_H

#include "icm42688.h"
#include "icm42688_reg.h"

#include <zephyr/rtio/rtio.h>

// - DEV 0625 BEGIN -
// #include <zephyr/logging/log.h>
// LOG_MODULE_DECLARE(ICM42688, CONFIG_SENSOR_LOG_LEVEL);
// - DEV 0625 END -

#define BYTE_COUNT_ONE 1

int icm42688_bus_read(const struct device *dev,
				    uint8_t reg,
				    uint8_t *buf,
				    uint16_t len);

int icm42688_bus_write(const struct device *dev,
				     uint8_t reg,
				     const uint8_t *buf,
				     uint16_t len);

#endif // ORESAT_DRIVER_SENSOR_ICM42688_BUS_H
