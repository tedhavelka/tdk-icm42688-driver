Overview

In Zephyr 4.2.0 a driver exists for TDK/Invensense ICM45686 which supports SPI,
I2C and I3C bus protocols.  The source files for this driver are:

```
-rw-rw-r-- 1 ted ted   431 jun  4 05:26 CMakeLists.txt
-rw-rw-r-- 1 ted ted  2578 jun  4 05:26 icm45686_bus.h
-rw-rw-r-- 1 ted ted 14651 jun  4 05:26 icm45686.c
-rw-rw-r-- 1 ted ted 15756 jun  4 05:26 icm45686_decoder.c
-rw-rw-r-- 1 ted ted   649 jun  4 05:26 icm45686_decoder.h
-rw-rw-r-- 1 ted ted  6554 jun  4 05:26 icm45686.h
-rw-rw-r-- 1 ted ted  4882 jun  4 05:26 icm45686_reg.h
-rw-rw-r-- 1 ted ted 21627 jun  4 05:26 icm45686_stream.c
-rw-rw-r-- 1 ted ted   431 jun  4 05:26 icm45686_stream.h
-rw-rw-r-- 1 ted ted  4861 jun  4 05:26 icm45686_trigger.c
-rw-rw-r-- 1 ted ted   703 jun  4 05:26 icm45686_trigger.h
-rw-rw-r-- 1 ted ted  2120 jun  4 05:26 Kconfig
```

Some of the filenames give us clues right away about the features which they implement.  i  File cm45686_reg.h contains IMU register addresses and bit-wise flags related to the fields within certain registers.  File icm45686_trigger.c contains code related to hardware interrupts support, as Zephyr sensor model normally refers to interrupts as "triggers".  File icm45686_bus.h implements two general purpose data bus APIs, a "bus read" and a "bus write" pair of functions.

There are very few references specifically to I2C and to SPI.  These refs appear mostly in conditionally compiled code.

The references to 'spi' in these sources are:

```
$ grep -n spi ./*

./icm45686.c:12:#include <zephyr/drivers/spi.h>
./icm45686.c:256:	if ((data->rtio.type == ICM45686_BUS_SPI) && !spi_is_ready_iodev(data->rtio.iodev)) {
./icm45686.c:417:	COND_CODE_1(DT_INST_ON_BUS(inst, spi),							   \
./icm45686.c:456:			COND_CODE_1(DT_INST_ON_BUS(inst, spi),					   \
./Kconfig:9:	select SPI if $(dt_compat_on_bus,$(DT_COMPAT_INVENSENSE_ICM45686),spi)
./Kconfig:10:	select SPI_RTIO if $(dt_compat_on_bus,$(DT_COMPAT_INVENSENSE_ICM45686),spi)
```

The references to 'i2c' in these sources are:

```
$ grep -n i2c ./*

./icm45686.c:13:#include <zephyr/drivers/i2c.h>
./icm45686.c:262:	if ((data->rtio.type == ICM45686_BUS_I2C) && !i2c_is_ready_iodev(data->rtio.iodev)) {
./icm45686.c:413:	(COND_CODE_1(DT_INST_ON_BUS(inst, i2c),							   \
./icm45686.c:454:			(COND_CODE_1(DT_INST_ON_BUS(inst, i2c),					   \
./Kconfig:11:	select I2C if $(dt_compat_on_bus,$(DT_COMPAT_INVENSENSE_ICM45686),i2c)
./Kconfig:12:	select I2C_RTIO if $(dt_compat_on_bus,$(DT_COMPAT_INVENSENSE_ICM45686),i2c)
```

File icm45686.h defines eight structures.  These are just the structure names . . . some of the definitions are fairly complex:

```
21:struct icm45686_encoded_payload {
41:struct icm45686_encoded_fifo_payload {
70:struct icm45686_encoded_header {
79:struct icm45686_encoded_data {
87:struct icm45686_triggers {
104:struct icm45686_stream {
142:struct icm45686_data {
164:struct icm45686_config {
```

In [icm45686.c](https://github.com/zephyrproject-rtos/zephyr/blob/413b789deb391d3a37d06b463288a5fe765ee57e/drivers/sensor/tdk/icm45686/icm45686.c "ICM45686 driver source icm45686.c") the functions ``static inline int reg_read()`` and ``static inline int reg_write()`` are wrappers to ``icm45686_bus_read()`` and ``icm45686_bus_read()``.  Overall icm45686.c contains the functions:

- static inline int reg_write(const struct device *dev, uint8_t reg, uint8_t val)
- static inline int reg_read(const struct device *dev, uint8_t reg, uint8_t *val)
- static int icm45686_sample_fetch(const struct device *dev, enum sensor_channel chan)
- static int icm45686_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val)
- static void icm45686_complete_result(struct rtio *ctx, const struct rtio_sqe *sqe, void *arg)
- static inline void icm45686_submit_one_shot(const struct device *dev, struct rtio_iodev_sqe *iodev_sqe)
- static void icm45686_submit(const struct device *dev, struct rtio_iodev_sqe *iodev_sqe)
- static int icm45686_init(const struct device *dev)
