# tdk-icm42688-driver

Repo for out-of-tree driver development work on Zephyr icm42688 inertial
measurement unit driver.  Team needs I2C support for this IMU.

## Overview

The ICM42688 IMU driver in-tree Zephyr driver as of Zephyr 4.2.0 implements
support for a SPI bus connection to the sensor but not an I2C bus use case.
A driver for the similar ICM45686 IMU supports that sensor's SPI, I2C and I3C
capabilities, and as such its code provides some useful code and code
organization choices, when the developer's goal is to modify and extend the
ICM42688 driver code.

There are multiple elements or types of files involved in a Zephyr RTOS driver.
These can be considered to begin with device bindings files and Kconfig files.
Zephyr's build system uses cmake as a major part of its project build system,
so drivers involve cmake scripts.  Lastly there are source files, typically
written in C.  Part of that C code further involves the use of Zephyr device
tree macros.

The effort underway to amend driver code for TDK's ICM42688 is following these
steps:

- [x] align 42688 device bindings files (yaml files) to match those found in
      45686 driver
- [ ] amend 42688 device instantiation macros to account for sensors found on
       an I2C bus (1) _in progress_
- [x] identify the ways in which I2C and SPI bus transactions connect to Zephyr
      sensor APIs (2) _in progress_
- [ ] route 42688 spi bus APIs through generic bus read and write wrapper
      functions
- [ ] bring the new code to a point of successful compilation
- [ ] write tests for the modified driver
- [ ] set up hardware and modify driver until tests pass

Notes

1.  Yet to-do is the review preprocessor output of device instantiation macros,
    as a sanity check.  Compare original ICM42688 device instantiation macro
    expanded code with modified driver macro expansion.

2.  I3C is not a hardware feature of the ICM42688, so it would make no sense to
    support this protocol in a driver for the 42688.

