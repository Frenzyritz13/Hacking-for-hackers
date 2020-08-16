
#if !defined(__OPT3001_H__)
#define __OPT3001_H__

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>

#include <wiringPi.h>

#define OPT3001_SLAVE_ADDR                 ((uint8_t)0x45)  /* Slave address */

/* Commands */
/* Register values */
#define OPT3001_REG_RESULT                   (uint8_t)0x00
#define OPT3001_REG_CONFIG                   (uint8_t)0x01
#define OPT3001_REG_MANUFACTUREID            (uint8_t)0x7E
#define OPT3001_DEVICEID                     (uint8_t)0x7F
/* Settings */
#define OPT3001_CMD_CONFIG_MSB                   (uint8_t)0xC6
#define OPT3001_CMD_CONFIG_LSB                   (uint8_t)0x10

uint32_t opt3001_trigger_measurement(void);
uint32_t opt3001_read_data(uint16_t *light_x100);
#endif  // OPT3001_H__

