#
# Copyright (c) 2016 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(chron)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/gpio.c)
target_sources(app PRIVATE src/imu.c)
target_sources(app PRIVATE src/temp_sensor.c)
target_sources(app PRIVATE src/hr_sensor.c)
target_sources(app PRIVATE src/page.c)
target_sources(app PRIVATE src/pages/home.c)
target_sources(app PRIVATE src/pages/summary.c)
target_sources(app PRIVATE src/pages/settings.c)
target_sources(app PRIVATE src/pages/activity.c)
target_sources(app PRIVATE src/pages/climate.c)