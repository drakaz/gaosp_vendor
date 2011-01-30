#ifndef PROXIMITY_H
#define PROXIMITY_H

#define GP2AP_IOCTL_MAGIC 'c'
#define GP2AP_IOCTL_GET_ENABLED \
		_IOR(GP2AP_IOCTL_MAGIC, 1, int *)
#define GP2AP_IOCTL_ENABLE \
		_IOW(GP2AP_IOCTL_MAGIC, 2, int *)

int is_proxi_open(void);

#endif
