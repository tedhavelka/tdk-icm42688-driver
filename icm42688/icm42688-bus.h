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

static inline int icm42688_bus_read(const struct device *dev,
				    uint8_t reg,
				    uint8_t *buf,
				    uint16_t len)
{
	struct icm42688_dev_data *data = dev->data;
	// TODO [x] check 'struct icm42688_dev_data' has a member named 'rtio':
	struct rtio *ctx = data->ctx;
	struct rtio_iodev *iodev = data->iodev;
	// RTIO submission and completion queue constructs:
	struct rtio_sqe *write_sqe = rtio_sqe_acquire(ctx);
	struct rtio_sqe *read_sqe = rtio_sqe_acquire(ctx);
	struct rtio_cqe *cqe;

	int rc = 0;

	if (!write_sqe || !read_sqe) {
		return -ENOMEM;
	}

	// TODO [ ] Check whether REG_READ_BIT defined:
	reg = reg | REG_READ_BIT;

	// TODO [ ] Learn what the parameters 5 and 6 of API rtio_sqe_prep_write() represents:
	rtio_sqe_prep_write(write_sqe, iodev, RTIO_PRIO_HIGH, &reg, 1, NULL);
	write_sqe->flags |= RTIO_SQE_TRANSACTION;
	rtio_sqe_prep_read(read_sqe, iodev, RTIO_PRIO_HIGH, buf, len, NULL);
	if (data->type == ICM42688_BUS_I2C) {
		read_sqe->iodev_flags |= RTIO_IODEV_I2C_STOP | RTIO_IODEV_I2C_RESTART;
	}

	// TODO [ ] Learn what the second parameter of API rtio_submit() represents:
	rc = rtio_submit(ctx, 2); 
	if (rc) {
		return rc;
	}

	do {
		cqe = rtio_cqe_consume(ctx);
		if (cqe != NULL) {
			rc = cqe->result;
			rtio_cqe_release(ctx, cqe);
		}
	} while (cqe != NULL);

	return rc;
}

static inline int icm42688_bus_write(const struct device *dev,
				     uint8_t reg,
				     const uint8_t *buf,
				     uint16_t len)
{
	struct icm42688_dev_data *data = dev->data;
	struct rtio *ctx = data->ctx;
	struct rtio_iodev *iodev = data->iodev;
	// RTIO submission and completion queue constructs:
	struct rtio_sqe *write_reg_sqe = rtio_sqe_acquire(ctx);
	struct rtio_sqe *write_buf_sqe = rtio_sqe_acquire(ctx);
	struct rtio_cqe *cqe;

	int rc;
	LOG_INF("W1");

	if (!write_reg_sqe || !write_buf_sqe) {
		return -ENOMEM;
	}
	LOG_INF("W2");

	rtio_sqe_prep_write(write_reg_sqe, iodev, RTIO_PRIO_HIGH, &reg, 1, NULL);
	write_reg_sqe->flags |= RTIO_SQE_TRANSACTION;
	rtio_sqe_prep_write(write_buf_sqe, iodev, RTIO_PRIO_HIGH, buf, len, NULL);
	if (data->type == ICM42688_BUS_I2C) {
		write_buf_sqe->iodev_flags |= RTIO_IODEV_I2C_STOP;
	}
	LOG_INF("W3");

	rc = rtio_submit(ctx, 2);
	if (rc) {
		return rc;
	}
	LOG_INF("W4");

	do {
		cqe = rtio_cqe_consume(ctx);
		if (cqe != NULL) {
			rc = cqe->result;
			rtio_cqe_release(ctx, cqe);
		}
	} while (cqe != NULL);
	LOG_INF("W5");

	return rc;
}

#endif // ORESAT_DRIVER_SENSOR_ICM42688_BUS_H
