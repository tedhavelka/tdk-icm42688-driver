# ICM42688P Zephyr Driver Work

Out-of-tree driver work on Zephyr 4.2.0 ICM42688 driver code base, to add I2C
support.

## Overview

In Zephyr 4.2.0 a driver exists for TDK/Invensense ICM42688 inertial measurement
unit, but its bus support extends only to SPI bus interface and not I2C.  In the
same Zephyr release there is also a driver for the ICM45686 which supports SPI,
I2C and I3C bus protocols.  This driver looks like a good place to find working
example code to extend the driver for the similar 42688.

There is a second challenge to this driver development in that both drivers use
the relatively newer Zephyr RTIO subsystem.  This system is more effecient at
sensor communications than the long standing sensor fetch and get model.  It is
also more complex.  How the ICM45686 driver conveys I2C device structure and
data to the RTIO subsystme is a significant piece of the coding implementation.

Yet a third area in complete driver work is the way in which user applications
are able and or must call the newer, RTIO based drivers in recent 4.x releases
of Zephyr RTOS.

In summary the three coding areas with which project initiator and contributor
Ted H begins this work are:

- I2C device struct, data buffers and driver internal APIs
- connection of I2C code to RTIO APIs
- application calls to driver to elicit readings and change sensor settings

## ICM45686 As Example

The source files for the ICM45686 driver are:

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

Just prior to struct icm45686_data an enum is defined to provide symbols which are used to select one of the three data bus types this sensor supports:

```
enum icm45686_bus_type {
        ICM45686_BUS_SPI,
        ICM45686_BUS_I2C,
        ICM45686_BUS_I3C,
};
```

## Comparison Of Driver Structure ICM45686 Versus ICM42688

The ICM45686 driver defines eight structures, while the ICM42688 defines five.
Table 1 

_Table 1 - driver structs for the 45686 and 42688 IMU devices_

```
|:---------------------------------:|:---------------------------------:|:----------------------------------:|:--------------------------------:|
|        ICM45686 structures        |        ICM42688 Structures        |      45686 struct content          |   42688 struct content           |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
| icm45686_encoded_payload { }      |                                   | sensor data                        |                                  |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
| icm45686_encoded_fifo_payload { } |                                   | sensor data                        |                                  |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
| icm45686_encoded_header {         |                                   | sensor data                        |                                  |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
| icm45686_encoded_data { }         |                                   | summary of first three             |                                  |
|                                   |                                   | sensor data struct values          |                                  |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
| icm45686_triggers { }             | struct icm42688_trigger_entry { } | struct {                           | struct sensor_trigger trigger;   |
|                                   |                                   |   struct gpio_callback;            | sensor_trigger_handler_t cb;     |
|                                   |                                   |   const struct device *;           |                                  |
|                                   |                                   |   struct k_mutex;                  |                                  |
|                                   |                                   |   struct {                         |                                  |
|                                   |                                   |     struct sensor_trigger trigger; |                                  |
|                                   |                                   |     sensor_trigger_handler_t cb;   |                                  |
|                                   |                                   |   } entry;                         |                                  |
|                                   |                                   |  [cond]                            |                                  |
|                                   |                                   |   OWN_THREAD                       |                                  |
|                                   |                                   |  [cond]                            |                                  |
|                                   |                                   |   GLOBAL_THREAD                    |                                  |
|                                   |                                   | }                                  |                                  |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
| icm45686_stream { }               |                                   | struct {                           |                                  |
|                                   |                                   |   struct gpio_callback;            |                                  |
|                                   |                                   |   const struct device *;           |                                  |
|                                   |                                   |   struct rtio_iodev_sqe *;         |                                  |
|                                   |                                   |   struct { ... } settings;         |                                  |
|                                   |                                   |   struct { ... } data;             |                                  |
|                                   |                                   | }                                  |                                  |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
| icm45686_data { }                 | icm42688_dev_data { }             | struct icm45686_data {             | struct icm42688_dev_data {       |
|                                   |                                   |   struct {                         |   struct icm42688_cfg            |
|                                   |                                   |     struct rtio_iodev *iodev;      |  [cond]                          |
|                                   |                                   |     struct rtio *ctx;              |   OWN_THREAD                     |
|                                   |                                   |     enum icm45686_bus_type type;   |  [cond]                          |
|                                   |                                   |    [cond]                          |   GLOBAL_THREAD                  |
|                                   |                                   |     struct { ... } i3c;            |  [cond]                          |
|                                   |                                   |   } rtio;                          |   struct rtio_iodev_sqe *stream; |
|                                   |                                   |   struct icm45686_encoded_data;    |   struct rtio *r;                |
|                                   |                                   |  [cond]                            |   struct rtio_iodev *spi_dev;    |
|                                   |                                   |   struct icm45686_triggers;        |   uint8_t int_status;            |
|                                   |                                   |  [cond]                            |   uint16_t fifo_count;           |
|                                   |                                   |   struct icm45686_stream;          |   uint64_t timestamp;            |
|                                   |                                   | }                                  |   atomic_t reading_fifo;         |
|                                   |                                   |                                    | }                                |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
| icm45686_config { }               | icm42688_cfg { }                  | sensor config, gpio interrupt pin  | sensor config                    |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
|                                   | icm42688_dev_cfg { }              |                                    | spi_dt_spec,                     |
|                                   |                                   |                                    | gpio interrupts                  |
|:----------------------------------|:----------------------------------|:-----------------------------------|:---------------------------------|
|                                   | alignment { }                     |                                    | part of sensor                   |
|                                   |                                   |                                    | config                           |
```

Some take-aways from the table of 45686 and 42688 driver structs:

- both drivers declare
  1. struct rtio_iodev_sqe *
  2. struct rtio *
  3. struct rtio_iodev *
- both drivers declare and define sensor trigger (interrupt) structs and trigger callbacks.
- ICM45686 factors RTIO details in two structs, ICM42688 factors RTIO into one struct

There is no I2C support in the ICM42688 driver code base.

## Bus Read and Write Functions, ICM45686

In [icm45686.c](https://github.com/zephyrproject-rtos/zephyr/blob/413b789deb391d3a37d06b463288a5fe765ee57e/drivers/sensor/tdk/icm45686/icm45686.c "ICM45686 driver source icm45686.c") the functions ``static inline int reg_read()`` and ``static inline int reg_write()`` are wrappers to ``icm45686_bus_read()`` and ``icm45686_bus_write()``.  Overall icm45686.c contains the functions:

- static inline int reg_write(const struct device *dev, uint8_t reg, uint8_t val)
- static inline int reg_read(const struct device *dev, uint8_t reg, uint8_t *val)
- static int icm45686_sample_fetch(const struct device *dev, enum sensor_channel chan)
- static int icm45686_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val)
- static void icm45686_complete_result(struct rtio *ctx, const struct rtio_sqe *sqe, void *arg)
- static inline void icm45686_submit_one_shot(const struct device *dev, struct rtio_iodev_sqe *iodev_sqe)
- static void icm45686_submit(const struct device *dev, struct rtio_iodev_sqe *iodev_sqe)
- static int icm45686_init(const struct device *dev)

So a question to answer here is how many places call reg_read() and reg_write()?  There is a possibility that routines which call these functions will reveal how the driver configures and queries the sensor for data.  There are a modest number of calls to these two driver functions:

```
ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm45686$ grep -n reg_read ./*
./icm45686.c:37:static inline int reg_read(const struct device *dev, uint8_t reg, uint8_t *val)
./icm45686.c:281:		err = reg_read(dev, REG_MISC2, &read_val);
./icm45686.c:308:	err = reg_read(dev, REG_WHO_AM_I, &read_val);
ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm45686$ grep -n reg_write ./*
./icm45686.c:32:static inline int reg_write(const struct device *dev, uint8_t reg, uint8_t val)
./icm45686.c:272:		err = reg_write(dev, REG_MISC2, REG_MISC2_SOFT_RST(1));
./icm45686.c:294:	err = reg_write(dev, REG_DRIVE_CONFIG0, REG_DRIVE_CONFIG0_SPI_SLEW(2));
./icm45686.c:299:	err = reg_write(dev, REG_DRIVE_CONFIG1, REG_DRIVE_CONFIG1_I3C_SLEW(3));
./icm45686.c:323:	err = reg_write(dev, REG_PWR_MGMT0, val);
./icm45686.c:331:	err = reg_write(dev, REG_ACCEL_CONFIG0, val);
./icm45686.c:339:	err = reg_write(dev, REG_GYRO_CONFIG0, val);
```

Here are all the driver's calls to its internal icm45686_bus_read() and icm45686_bus_write() functions:

```
ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm45686$ grep -n icm45686_bus_read ./*
./icm45686_bus.h:17:static inline int icm45686_bus_read(const struct device *dev,
./icm45686.c:39:	return icm45686_bus_read(dev, reg, val, 1);
./icm45686.c:53:	err = icm45686_bus_read(dev,
./icm45686_stream.c:543:		err = icm45686_bus_read(dev, REG_INT1_STATUS0, &val, 1);
./icm45686_trigger.c:85:	err = icm45686_bus_read(dev, REG_INT1_CONFIG0, &val, 1);

ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm45686$ grep -n icm45686_bus_write ./*
./icm45686_bus.h:61:static inline int icm45686_bus_write(const struct device *dev,
./icm45686.c:34:	return icm45686_bus_write(dev, reg, &val, 1);
./icm45686.c:352:	err = icm45686_bus_write(dev, REG_IREG_ADDR_15_8, gyro_lpf_write_array,
./icm45686.c:370:	err = icm45686_bus_write(dev, REG_IREG_ADDR_15_8, accel_lpf_write_array,
./icm45686_stream.c:534:		err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_stream.c:555:		err = icm45686_bus_write(dev, REG_FIFO_CONFIG3, &val, 1);
./icm45686_stream.c:566:		err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_stream.c:576:		err = icm45686_bus_write(dev, REG_FIFO_CONFIG0, &val, 1);
./icm45686_stream.c:591:			err = icm45686_bus_write(dev, REG_FIFO_CONFIG2, &val, 1);
./icm45686_stream.c:599:			err = icm45686_bus_write(dev, REG_FIFO_CONFIG1_0, (uint8_t *)&fifo_ths, 2);
./icm45686_stream.c:609:			err = icm45686_bus_write(dev, REG_FIFO_CONFIG0, &val, 1);
./icm45686_stream.c:621:			err = icm45686_bus_write(dev, REG_FIFO_CONFIG3, &val, 1);
./icm45686_stream.c:672:		err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_stream.c:680:		err = icm45686_bus_write(dev, REG_INT1_CONFIG2, &val, 1);
./icm45686_trigger.c:91:	err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_trigger.c:100:	return icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_trigger.c:200:	err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_trigger.c:208:	err = icm45686_bus_write(dev, REG_INT1_CONFIG2, &val, 1);
```

## How ICM42688 Driver Transacts On Bus

Now it is worth looking at how the ICM42688 driver communicates, which is to say
prepares its messages to send and to transact for write-read message via the
RTIO subsystem.  This driver has a SPI bus API source file with five functions:

```
- static inline int spi_write_register(const struct spi_dt_spec *bus, uint8_t reg, uint8_t data);
- static inline int spi_read_register(const struct spi_dt_spec *bus, uint8_t reg, uint8_t *data, len);
- int icm42688_spi_read(const struct spi_dt_spec *bus, uint16_t reg, uint8_t *data, size_t len)
- int icm42688_spi_update_register(const struct spi_dt_spec *bus, uint16_t reg, uint8_t mask,
- int icm42688_spi_single_write(const struct spi_dt_spec *bus, uint16_t reg, uint8_t data)
```

There are a lot of places where various ICM42688 driver sources call some of
these SPI APIs:

```
ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm42688$ grep -n '^int ' icm42688_spi.c
66:int icm42688_spi_read(const struct spi_dt_spec *bus, uint16_t reg, uint8_t *data, size_t len)
76:int icm42688_spi_update_register(const struct spi_dt_spec *bus, uint16_t reg, uint8_t mask,
92:int icm42688_spi_single_write(const struct spi_dt_spec *bus, uint16_t reg, uint8_t data)
ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm42688$ vi icm42688_spi.c
ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm42688$ grep -n icm42688_spi_ ./*.*
./icm42688.c:111:	int res = icm42688_spi_read(&cfg->spi, REG_INT_STATUS, &status, 1);
./icm42688.c:339:	SPI_DT_IODEV_DEFINE(icm42688_spi_iodev_##inst, DT_DRV_INST(inst), ICM42688_SPI_CFG, 0U);   \
./icm42688.c:372:						    .spi_iodev = &icm42688_spi_iodev_##inst,))     \
./icm42688_common.c:31:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_DEVICE_CONFIG, BIT_SOFT_RESET_CONFIG);
./icm42688_common.c:42:	res = icm42688_spi_read(&dev_cfg->spi, REG_INT_STATUS, &value, 1);
./icm42688_common.c:53:	res = icm42688_spi_read(&dev_cfg->spi, REG_WHO_AM_I, &value, 1);
./icm42688_common.c:129:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_INT_SOURCE0, 0);
./icm42688_common.c:133:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_FIFO_CONFIG,
./icm42688_common.c:141:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_SIGNAL_PATH_RESET,
./icm42688_common.c:153:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_BANK_SEL, BIT_BANK1);
./icm42688_common.c:164:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_INTF_CONFIG5, intf_config5);
./icm42688_common.c:171:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_BANK_SEL, BIT_BANK0);
./icm42688_common.c:181:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_INTF_CONFIG1, intf_config1);
./icm42688_common.c:193:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_PWR_MGMT0, pwr_mgmt0);
./icm42688_common.c:209:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_ACCEL_CONFIG0, accel_config0);
./icm42688_common.c:219:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_GYRO_CONFIG0, gyro_config0);
./icm42688_common.c:235:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_FIFO_CONFIG, fifo_config_bypass);
./icm42688_common.c:244:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_FSYNC_CONFIG, 0);
./icm42688_common.c:249:	res = icm42688_spi_read(&dev_cfg->spi, REG_TMST_CONFIG, &tmst_config, 1);
./icm42688_common.c:254:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_TMST_CONFIG, tmst_config & ~BIT(1));
./icm42688_common.c:264:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_INT_CONFIG,
./icm42688_common.c:280:	res = icm42688_spi_single_write(&dev_cfg->spi, REG_INT_CONFIG1, int_config1);
./icm42688_common.c:303:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_FIFO_CONFIG1, fifo_cfg1);
./icm42688_common.c:314:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_FIFO_CONFIG2, fifo_wml);
./icm42688_common.c:323:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_FIFO_CONFIG3, fifo_wmh);
./icm42688_common.c:333:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_FIFO_CONFIG, fifo_config);
./icm42688_common.c:339:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_INT_SOURCE0, int_source0);
./icm42688_common.c:350:		res = icm42688_spi_single_write(&dev_cfg->spi, REG_INT_SOURCE0, int_source0);
./icm42688_common.c:378:	res = icm42688_spi_read(&dev_cfg->spi, REG_TEMP_DATA1, data, 14);
./icm42688_rtio.c:25:	int res = icm42688_spi_read(&cfg->spi, REG_INT_STATUS, &status, 1);
./icm42688_spi.c:66:int icm42688_spi_read(const struct spi_dt_spec *bus, uint16_t reg, uint8_t *data, size_t len)
./icm42688_spi.c:76:int icm42688_spi_update_register(const struct spi_dt_spec *bus, uint16_t reg, uint8_t mask,
./icm42688_spi.c:80:	int res = icm42688_spi_read(bus, reg, &temp, 1);
./icm42688_spi.c:89:	return icm42688_spi_single_write(bus, reg, temp);
./icm42688_spi.c:92:int icm42688_spi_single_write(const struct spi_dt_spec *bus, uint16_t reg, uint8_t data)
./icm42688_spi.h:24:int icm42688_spi_single_write(const struct spi_dt_spec *bus, uint16_t reg, uint8_t data);
./icm42688_spi.h:38:int icm42688_spi_update_register(const struct spi_dt_spec *bus, uint16_t reg, uint8_t mask,
./icm42688_spi.h:53:int icm42688_spi_read(const struct spi_dt_spec *bus, uint16_t reg, uint8_t *data, size_t len);
./icm42688_trigger.c:102:		res = icm42688_spi_read(&cfg->spi, REG_INT_STATUS, &status, 1);
./icm42688_trigger.c:159:	res = icm42688_spi_single_write(&cfg->spi, REG_INT_CONFIG,
./icm42688_trigger.c:166:	res = icm42688_spi_single_write(&cfg->spi, REG_INT_CONFIG1, 0);
./icm42688_trigger.c:183:	return icm42688_spi_single_write(&cfg->spi, REG_INT_SOURCE0, value);
```

Most likely these calls to SPI read and write APIs will need to be redirected to
a more general bus read and write API pair, as in the ICM42688 driver.

Driver ICM45686 bus read and write functions are found in [icm45686_bus.h](https://github.com/zephyrproject-rtos/zephyr/blob/main/drivers/sensor/tdk/icm45686/icm45686_bus.h).  These functions are:

```
 17 static inline int icm45686_bus_read(const struct device *dev,
 18                                     uint8_t reg,
 19                                     uint8_t *buf,
 20                                     uint16_t len)
 21 { . . .

 61 static inline int icm45686_bus_write(const struct device *dev,
 62                                      uint8_t reg,
 63                                      const uint8_t *buf,
 64                                      uint16_t len)
 65 {
```

These two bus communication APIs are called from icm45686.c, icm45686_stream.c and ism45686_trigger.c:

```c
ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm45686$ grep -n icm45686_bus_read ./*
./icm45686_bus.h:17:static inline int icm45686_bus_read(const struct device *dev,
./icm45686.c:39:	return icm45686_bus_read(dev, reg, val, 1);
./icm45686.c:53:	err = icm45686_bus_read(dev,
./icm45686_stream.c:543:		err = icm45686_bus_read(dev, REG_INT1_STATUS0, &val, 1);
./icm45686_trigger.c:85:	err = icm45686_bus_read(dev, REG_INT1_CONFIG0, &val, 1);

ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm45686$ grep -n icm45686_bus_write ./*
./icm45686_bus.h:61:static inline int icm45686_bus_write(const struct device *dev,
./icm45686.c:34:	return icm45686_bus_write(dev, reg, &val, 1);
./icm45686.c:352:	err = icm45686_bus_write(dev, REG_IREG_ADDR_15_8, gyro_lpf_write_array,
./icm45686.c:370:	err = icm45686_bus_write(dev, REG_IREG_ADDR_15_8, accel_lpf_write_array,
./icm45686_stream.c:534:		err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_stream.c:555:		err = icm45686_bus_write(dev, REG_FIFO_CONFIG3, &val, 1);
./icm45686_stream.c:566:		err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_stream.c:576:		err = icm45686_bus_write(dev, REG_FIFO_CONFIG0, &val, 1);
./icm45686_stream.c:591:			err = icm45686_bus_write(dev, REG_FIFO_CONFIG2, &val, 1);
./icm45686_stream.c:599:			err = icm45686_bus_write(dev, REG_FIFO_CONFIG1_0, (uint8_t *)&fifo_ths, 2);
./icm45686_stream.c:609:			err = icm45686_bus_write(dev, REG_FIFO_CONFIG0, &val, 1);
./icm45686_stream.c:621:			err = icm45686_bus_write(dev, REG_FIFO_CONFIG3, &val, 1);
./icm45686_stream.c:672:		err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_stream.c:680:		err = icm45686_bus_write(dev, REG_INT1_CONFIG2, &val, 1);
./icm45686_trigger.c:91:	err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_trigger.c:100:	return icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_trigger.c:200:	err = icm45686_bus_write(dev, REG_INT1_CONFIG0, &val, 1);
./icm45686_trigger.c:208:	err = icm45686_bus_write(dev, REG_INT1_CONFIG2, &val, 1);
```



## RTIO Constructs

Driver ICM42688 makes use of an RTIO construct defined in ``zephyr/include/zephyr/drivers/sensor.``:

```
/*
 * Internal data structure used to store information about the IODevice for async reading and
 * streaming sensor data.
 */
struct sensor_read_config {
        const struct device *sensor;
        const bool is_streaming;
        union {
                struct sensor_chan_spec *const channels;
                struct sensor_stream_trigger *const triggers;
        };
        size_t count;
        const size_t max; 
};
```

