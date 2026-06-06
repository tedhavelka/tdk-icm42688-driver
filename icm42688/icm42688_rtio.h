/*
 * Copyright (c) 2023 Google LLC
 * Copyright (c) 2026 Portland State University Oresat Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ORESAT_DRIVERS_SENSOR_ICM42688_RTIO_H_
#define ORESAT_DRIVERS_SENSOR_ICM42688_RTIO_H_

#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>

void icm42688_submit(const struct device *sensor, struct rtio_iodev_sqe *iodev_sqe);

void icm42688_submit_stream(const struct device *sensor, struct rtio_iodev_sqe *iodev_sqe);

void icm42688_fifo_event(const struct device *dev);

#endif /* ORESAT_DRIVERS_SENSOR_ICM42688_RTIO_H_ */
