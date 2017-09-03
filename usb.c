//libdev.h
//mntent.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>
#include <errno.h>
#include <mntent.h>	/* for getmntent(), et al. */
#include <unistd.h>	/* for getopt() */

#define BLOCK_SIZE 512

void process(const char *filename);
void print_mount(const struct mntent *fs);

char *myname;

/* main --- process options */

void getMounts(char *devNode, char *devName, char *idVendor, char *idProduct){
	char *filename = "/etc/mtab";	/* default file to read */
	char *nameUsb = "";
	char *mountAdr;

	FILE *fp;
	struct mntent *fs;

	fp = setmntent(filename, "r");	/* read only */
	if (fp == NULL) {
		fprintf(stderr, "%s: %s: could not open: %s\n",
			myname, filename, strerror(errno));
		exit(1);
	}

	while ((fs = getmntent(fp)) != NULL){
		if(strstr(fs->mnt_fsname, devName) != NULL && strstr(fs->mnt_fsname, "sda") == NULL) {
    		printf("Nodo: %s\nPunto montaje: %s\nIdVendor: %s\nIdProduct: %s\nNombre: %s\n\n",
				devNode,
				//fs->mnt_fsname,
				fs->mnt_dir,
				idVendor,
				idProduct,
				nameUsb);
		}
		
	}

	endmntent(fp);
}

int main(){
	struct udev *udev;
	struct udev_device *dev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device* usb;
	char *idVendor;
	char *idProduct;
	char *devNode;
	char *devName;
	
	while(1){
		/* create udev object */
		udev = udev_new();
		if (!udev) {
			fprintf(stderr, "Cannot create udev context.\n");
			return 1;
		}

		/* create enumerate object */
		enumerate = udev_enumerate_new(udev);
		if (!enumerate) {
			fprintf(stderr, "Cannot create enumerate context.\n");
			return 1;
		}

		udev_enumerate_add_match_subsystem(enumerate, "block");
		udev_enumerate_scan_devices(enumerate);

		/* fillup device list */
		devices = udev_enumerate_get_list_entry(enumerate);
		if (!devices) {
			fprintf(stderr, "Failed to get device list.\n");
			return 1;
		}

		udev_list_entry_foreach(dev_list_entry, devices) {
			const char *path;//, *tmp;
			//unsigned long long disk_size = 0;
			//unsigned short int block_size = BLOCK_SIZE;

			path = udev_list_entry_get_name(dev_list_entry);
			dev = udev_device_new_from_syspath(udev, path);
			usb = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");

			/* skip if device/disk is a partition or loop device */
			if (strncmp(udev_device_get_devtype(dev), "partition", 9) != 0 &&
			    strncmp(udev_device_get_sysname(dev), "loop", 4) != 0) {
				devNode = udev_device_get_devnode(dev);
				devName = udev_device_get_sysname(dev);
				idVendor = udev_device_get_sysattr_value(usb, "idVendor");
				idProduct = udev_device_get_sysattr_value(usb, "idProduct");
				getMounts(devNode, devName, idVendor, idProduct);
			}

			/* free dev */
			udev_device_unref(dev);
		}
		/* free enumerate */
		udev_enumerate_unref(enumerate);
		/* free udev */
		udev_unref(udev);

		
		sleep(1);
	}
	return 0;
}