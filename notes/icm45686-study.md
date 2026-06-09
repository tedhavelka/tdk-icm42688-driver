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

ted@zakia:~/projects/oresat/oresat-firmware/zephyr/drivers/sensor/tdk/icm45686$
