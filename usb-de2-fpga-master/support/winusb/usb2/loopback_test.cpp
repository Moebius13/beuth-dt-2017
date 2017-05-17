#include "stdafx.h"

#include <lusb0_usb.h>
#include <stdio.h>
#include <stdint.h>

#include <Windows.h>
#include <process.h>         // needed for _beginthread()

#include <string>
#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// Device vendor and product id.
#define MY_VID 0x04CC
#define MY_PID 0x1A62

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint(s)
#define EP_IN 0x82
#define EP_OUT 0x01


//////////////////////////////////////////////////////////////////////////////
usb_dev_handle *open_dev(void);


#define BUF_SIZE (256*256)

char tbuf[BUF_SIZE];
volatile char usb_thread_done = 0;
void  usb_thread( void *arg )
{
	int ret;
	for (int i = 0; i < sizeof(tbuf); i++)
		tbuf[i] = i;
	
	usb_dev_handle *dev = (usb_dev_handle *) arg;
	ret = usb_bulk_write(dev, EP_OUT,  tbuf, sizeof(tbuf), 10000);
	 if (ret < 0)
        printf("error writing:\n%s\n", usb_strerror());
    else
		printf("Success! :Wrote %d bytes\n", ret);

	 usb_thread_done = 1;
}


int _tmain(int argc, _TCHAR* argv[])
{
	usb_dev_handle *dev = NULL; /* the device handle */
	setbuf(stdout, NULL);
    int ret;
    usb_init(); /* initialize the library */
    usb_find_busses(); /* find all busses */
    usb_find_devices(); /* find all connected devices */
	
	if (!(dev = open_dev()))
    {
        printf("error opening device: \n%s\n", usb_strerror());
        return 0;
    }
    else
        printf("success: device %04X:%04X opened\n", MY_VID, MY_PID);	
    if (usb_set_configuration(dev, MY_CONFIG) < 0)
    {
        printf("error setting config #%d: %s\n", MY_CONFIG, usb_strerror());
        usb_close(dev);
        return 0;
    }
    else
        printf("success: set configuration #%d\n", MY_CONFIG);
	
    if (usb_claim_interface(dev, 0) < 0)
    {
        printf("error claiming interface #%d:\n%s\n", MY_INTF, usb_strerror());
        usb_close(dev);
        return 0;
    }
    else
        printf("success: claim_interface #%d\n", MY_INTF);
	
	
	char rbuf[BUF_SIZE];
	memset(rbuf,0, sizeof(rbuf));


// At some point, get the Tick Count
	DWORD tickAtStart = GetTickCount();

	//start downloading the data
	_beginthread( usb_thread, 0, (void*)dev );	


	//wait for all the data to loopback to the device
    ret = usb_bulk_read(dev, EP_IN, rbuf, sizeof(rbuf), 10000);
	
	// At some other point get the change of Tick Count
	double elapsedTicks = (double)( GetTickCount() - tickAtStart);
    if (ret < 0)
    {
        printf("error reading:\n%s\n", usb_strerror());
    }
    else
		printf("success: bulk read %d bytes, TX+RX Bandwidth = %.2f KB/s\n", ret, ret * 2 / 1024/ (elapsedTicks / 1000.0));



	for (int i = 0; i < sizeof(rbuf); i++)
	{
		//printf("0x%02X\n",rbuf[0] & 0xFF);
		if ((rbuf[i] & 0xFF) != (tbuf[i] & 0xFF))
		{
			printf("Error data corrupt!%d\n",i);
			break;
		}
	}
	

	while(!usb_thread_done);
    usb_release_interface(dev, 0);
	if (dev)
        usb_close(dev);	 



	return 0;
}



usb_dev_handle *open_dev(void)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_get_busses(); bus; bus = bus->next)
    {
        for (dev = bus->devices; dev; dev = dev->next)
        {
            if (dev->descriptor.idVendor == MY_VID
                    && dev->descriptor.idProduct == MY_PID)
            {
                return usb_open(dev);
            }
        }
    }
    return NULL;
}