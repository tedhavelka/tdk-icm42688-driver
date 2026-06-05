# tdk-icm42688-driver

Repo for out-of-tree driver development work on Zephyr icm42688 inertial measurement unit driver.  Team needs I2C support for this IMU.

This driver includes:

- zephyr/module.yml
  - registers module, in this case driver with 'west'
  - tells Zephyr build system where to find cmake, Kconfig and DTS bindings
