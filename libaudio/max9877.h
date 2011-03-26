#ifndef AMP_MAX9877_H
#define AMP_MAX9877_H
#include <linux/types.h>
#include <linux/ioctl.h>
#include <asm/sizes.h>

/*************************************************************
*	IOCTL define
*************************************************************/

#define MAX9877_I2C_IOCTL_MAGIC 'm'

#define MAX9877_I2C_IOCTL_W \
	        _IOW(MAX9877_I2C_IOCTL_MAGIC, 0, unsigned)

#define MAX9877_I2C_IOCTL_R \
	        _IOR(MAX9877_I2C_IOCTL_MAGIC, 1, unsigned)

#define MAX9877_I2C_IOCTL_SWITCH_DEVICE \
	        _IO(MAX9877_I2C_IOCTL_MAGIC, 2)

#define MAX9877_HPH_VOL_SET \
	        _IOW(MAX9877_I2C_IOCTL_MAGIC, 3, unsigned)

#define MAX9877_SPK_VOL_SET \
	        _IOW(MAX9877_I2C_IOCTL_MAGIC, 4, unsigned)

#define MAX9877_SPEAKER_ON \
	        _IO(MAX9877_I2C_IOCTL_MAGIC, 5)

#define MAX9877_SPEAKER_OFF \
	        _IO(MAX9877_I2C_IOCTL_MAGIC, 6)
    
#define MAX9877_HEADSET_ON \
               _IOW(MAX9877_I2C_IOCTL_MAGIC, 7, unsigned)

#define MAX9877_HEADSET_OFF \
               _IO(MAX9877_I2C_IOCTL_MAGIC, 8)

#define MAX9877_SPK_EAR_ON \
	        _IO(MAX9877_I2C_IOCTL_MAGIC, 9)

#define MAX9877_RCV_ON \
	        _IO(MAX9877_I2C_IOCTL_MAGIC, 10)

#define MAX9877_RCV_OFF \
	        _IO(MAX9877_I2C_IOCTL_MAGIC, 11)

    
#define MAX9877_I2C_IOCTL_INIT \
	_IO(MAX9877_I2C_IOCTL_MAGIC, 12) 

#define MAX9877_AMP_SUSPEND \
	_IO(MAX9877_I2C_IOCTL_MAGIC, 13)

#define MAX9877_AMP_RESUME \
	_IO(MAX9877_I2C_IOCTL_MAGIC, 14)

#define MAX9877_AMP_RECORDING_START \
	_IO(MAX9877_I2C_IOCTL_MAGIC, 15)

#define MAX9877_AMP_RECORDING_STOP \
	_IO(MAX9877_I2C_IOCTL_MAGIC, 16)

#endif

