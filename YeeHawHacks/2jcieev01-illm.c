
/** @Includes ----------------------------------------------------------------*/
#include "opt3001.h"

#define RASPBERRY_PI_I2C    "/dev/i2c-1"
#define I2CDEV              RASPBERRY_PI_I2C

#define conv8s_u16_be(b, n) \
    (uint16_t)(((uint16_t)b[n] << 8) | (uint16_t)b[n + 1])


uint32_t i2c_write_reg8(uint8_t devAddr, uint8_t regAddr,
                        uint8_t* data , uint8_t length
) {
    uint8_t buf[128];
    if (length > 127) {
        fprintf(stderr, "Byte write count (%d) > 127\n", length);
        return 11;
    }

    int fd = open(I2CDEV, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return 12;
    }
    int err = 0;
    do {
        if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
            fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
            err = 13; break;
        }
        buf[0] = regAddr;
        if (length > 0) {
            memcpy(buf + 1, data, length);
        }
        length += 1;
        int count = write(fd, buf, length);
        if (count < 0) {
            fprintf(stderr, "Failed to write device(%d): %s\n",
                    count, strerror(errno));
            err = 14; break;
        } else if (count != length) {
            fprintf(stderr, "Short write to device, expected %d, got %d\n",
                    length, count);
            err = 15; break;
        }
    } while (false);
    close(fd);
    return err;
}

uint32_t i2c_read_reg8(uint8_t devAddr, uint8_t regAddr,
                       uint8_t *data, uint8_t length
) {
    int fd = open(I2CDEV, O_RDWR);

    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return 21;
    }
    int err = 0;
    do {
        if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
            fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
            err = 22; break;
        }
        if (write(fd, &regAddr, 1) != 1) {
            fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
            err = 23; break;
        }
        int count = read(fd, data, length);
        if (count < 0) {
            fprintf(stderr, "Failed to read device(%d): %s\n",
                    count, strerror(errno));
            err = 24; break;
        } else if (count != length) {
            fprintf(stderr, "Short read  from device, expected %d, got %d\n",
                    length, count);
            err = 25; break;
        }
    } while (false);
    close(fd);
    return err;
}

static uint32_t opt3001_read_triggered_value(uint16_t *value_data);
static uint32_t opt3001_convert_lux_value_x100(uint16_t value_raw);

/** <!-- opt3001_setup {{{1 --> start measurement.
 */
bool opt3001_setup(void) {
    uint32_t err_code = opt3001_trigger_measurement();
    if (err_code) {
        printf("opt3001 ng \n");
        return true;
    }
    return false;
}

/** <!-- opt3001_trigger_measurement {{{1 --> start to measurement
 */
uint32_t opt3001_trigger_measurement(void) {
    uint32_t err_code;
    uint8_t wbuf[2];

    wbuf[0] = OPT3001_CMD_CONFIG_MSB;
    wbuf[1] = OPT3001_CMD_CONFIG_LSB;

    err_code = i2c_write_reg8(OPT3001_SLAVE_ADDR, OPT3001_REG_CONFIG,
            wbuf, sizeof(wbuf));
    return err_code;
}

/** <!-- opt3001_read_data {{{1 --> read and convert the sensor value.
 */
uint32_t opt3001_read_data(uint16_t *light_x100) {
    uint32_t err_code = 0;
    uint16_t value_data;

    /* Measure the temperature value */
    err_code = opt3001_read_triggered_value(&value_data);
    if (err_code) {
        return err_code;
    }

    *light_x100 = (uint16_t)(opt3001_convert_lux_value_x100(value_data) / 100);
    return 0;
}

/** <!-- opt3001_read_triggered_value {{{1 --> read sensor value
 */
static uint32_t opt3001_read_triggered_value(uint16_t *value_data) {
    uint32_t err_code = 0;
    uint8_t read_buff[2];

    err_code = i2c_read_reg8(OPT3001_SLAVE_ADDR, OPT3001_REG_CONFIG,
            read_buff, sizeof(read_buff));
    if (err_code) {
        return err_code;
    }
    if ((read_buff[1] & 0x80) == 0) {
        return 2;  // sensor is working...
    }

    err_code = i2c_read_reg8(OPT3001_SLAVE_ADDR, OPT3001_REG_RESULT,
            read_buff, sizeof(read_buff));
    if (err_code) {
        return 100 + err_code;
    }

    *value_data = conv8s_u16_be(read_buff, 0);
    return 0;
}

/** <!-- opt3001_convert_lux_value_x100 {{{1 --> convert sensors
 * raw output digits to [100lx]
 */
uint32_t opt3001_convert_lux_value_x100(uint16_t value_raw) {
    uint32_t value_converted = 0;
    uint32_t exp;
    uint32_t data;

    /* Convert the value to centi-percent RH */
    exp = (value_raw >> 12) & 0x0F;
    exp = 1 << exp;
    data = value_raw & 0x0FFF;
    value_converted = (uint32_t)(exp * data);

    return value_converted;
}

/** <!-- main - illuminance sensor {{{1 -->
 * 1. setup sensor
 * 2. output results, format is: [lx]
 */
int main() {
    uint16_t illm;

    if (opt3001_setup()) {
        return 1;
    }
    delay(110);
    int ret = opt3001_read_data(&illm);
    printf("%10d, return code: %d\n", illm, ret);
    return 0;
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80
