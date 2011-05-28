#include <ruby.h>
#include <hidapi/hidapi.h>
#include <stdlib.h>
#include <stdio.h>

static VALUE module_ruby_hidapi;
static VALUE class_hid_device_info;
static VALUE class_hid_device;

static int hid_utf8fromwc( const wchar_t* str, char* dst)
{
	size_t len = wcslen(str);
	unsigned int j = 0;
	if(len > 0)
	{
		unsigned count;
		unsigned int i = 0;
		unsigned ucs;
		while(i < len )
		{
			ucs = str[i];
			
			if (ucs > 0x10ffff) {
			     ucs = 0xfffd;
      }

			dst[j++] = ucs;
      dst[j++] = ucs >> 8;
      dst[j++] = ucs >> 16;
      dst[j++] = 0;

		i++;
		}
		dst[j++] = 0;
		dst[j++] = 0;
		dst[j++] = 0;
		dst[j++] = 0;
	}
	
  return j;
}

/******************************************************************************\
 *
 * HID Device 
 *
\******************************************************************************/
static void hidDeviceFree(void *ptr) {
	
}

static VALUE hidDeviceWrite( VALUE self, VALUE data_str )
{
  hid_device* ptr;
	const char* data = rb_string_value_ptr(&data_str);
  Data_Get_Struct( self, hid_device, ptr );
  return INT2NUM( hid_write(ptr, data, sizeof(data)) );
}

static VALUE hidDeviceRead( VALUE self )
{
  hid_device* ptr;
  Data_Get_Struct( self, hid_device, ptr );
	hid_set_nonblocking(ptr, 1);

	const int reportSize = 32;
	unsigned char buf[reportSize];
	//memset(buf, 0, reportSize);
	int res = hid_read(ptr, buf, reportSize);

	if (res < 0)
	{
	    //printf("failed to read");
	    return Qnil;
	}
	/*
	//printf("read: %s", buf);
	printf("c ints: [");
	int i;
	for (i=0; i<res; i++)
	    printf("%i, ",(int)buf[i]);
	printf("]\n");
	*/
	return rb_str_new(buf,res);
}

static VALUE hidDeviceReadInts( VALUE self )
{
  hid_device* ptr;
  Data_Get_Struct( self, hid_device, ptr );
	hid_set_nonblocking(ptr, 1);

	const int reportSize = 32;
	unsigned char buf[reportSize];
	//memset(buf, 0, reportSize);
	int res = hid_read(ptr, buf, reportSize);

	if (res < 0)
	{
	    //printf("failed to read");
	    return Qnil;
	}

	/*printf("read: ");
	int i;
	for (i=0; i<reportSize; i++)
	    printf("read: %s", (char*)&buf[i]);

	printf("buf: %i", buf);*/
	return rb_str_new(buf,res);
}

static VALUE hidDeviceFeatureReport( VALUE self, VALUE report_id )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
	unsigned char report[9];
	int res;
	unsigned char buf[65];
	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(ptr, buf, sizeof(buf));
		
	//report[0] = NUM2INT(report_id);
	//int e = hid_get_feature_report(ptr, report, 9);
	VALUE string = rb_str_new(buf, sizeof(buf));
  return string;
}


/******************************************************************************\
 *
 * HID Device Info 
 *
\******************************************************************************/
static void hidDeviceInfoFree(void *ptr) {
	printf("Free info\n");
	hid_free_enumeration(ptr);
}

static VALUE hidDeviceInfoNew(VALUE class, VALUE vendor_id, VALUE product_id)
{
  //VALUE *argv = malloc((2)*sizeof(VALUE));
  struct hid_device_info *ptr = hid_enumerate(NUM2INT(vendor_id), NUM2INT(product_id));
  VALUE tdata = Data_Wrap_Struct(class, 0, hidDeviceInfoFree, ptr);
  //argv[0] = vendor_id;
	//argv[1] = product_id;
  //rb_obj_call_init(tdata, 2, argv);
  return tdata;
}

static VALUE hidDeviceInfoHelloWorld( VALUE self )
{
  return rb_str_new2("Hello World");
}

static VALUE hidDeviceInfoVendorID( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
  return INT2NUM( ptr->vendor_id );
}

static VALUE hidDeviceInfoProductID( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
  return INT2NUM( ptr->product_id );
}

static VALUE hidDeviceInfoPath( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
	VALUE string = rb_str_new2(ptr->path);
  return string;
}

static VALUE hidDeviceInfoSerialNumber( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
	size_t len = (wcslen(ptr->serial_number)+1)*sizeof(wchar_t);
	char *dst[len];
	hid_utf8fromwc(ptr->serial_number, dst);
	VALUE string = rb_str_new(dst, len);
  return string;
}

static VALUE hidDeviceInfoReleaseNumber( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
  return INT2NUM( ptr->release_number );
}

static VALUE hidDeviceInfoManufacturerString( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
	size_t len = (wcslen(ptr->manufacturer_string)+1)*sizeof(wchar_t);
	char *dst[len];
	hid_utf8fromwc(ptr->manufacturer_string, dst);
	VALUE string = rb_str_new(dst, len);
  return string;
}

static VALUE hidDeviceInfoProductString( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
	//printf("prod:%i",wcslen(ptr->product_string) );
	size_t len = (wcslen(ptr->product_string)+1)*sizeof(wchar_t);
	char *dst[len];
	hid_utf8fromwc(ptr->product_string, dst);
	VALUE string = rb_str_new(dst, len);
	//int enc = rb_enc_find_index("UTF-8");
	//rb_enc_associate_index(string, enc);
  return string;
}

static VALUE hidDeviceInfoUsagePage( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
  return INT2NUM( ptr->usage_page );
}

static VALUE hidDeviceInfoUsage( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
  return INT2NUM( ptr->usage );
}

static VALUE hidDeviceInfoInterfaceNumber( VALUE self )
{
  struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
  return INT2NUM( ptr->interface_number );
}


static VALUE hidDeviceInfoNext( VALUE self )
{
	struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
	if(ptr->next)
  	return Data_Wrap_Struct( class_hid_device_info, 0, hidDeviceInfoFree, ptr->next );
	else
		return Qnil;
}

static VALUE hidDeviceInfoOpen( VALUE self )
{
	struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
	hid_device* dev = hid_open(ptr->vendor_id, ptr->product_id, ptr->serial_number);
	if(dev)
  	return Data_Wrap_Struct( class_hid_device, 0, hidDeviceFree, dev );
	else
		return Qnil;
}

static VALUE hidDeviceInfoClose( VALUE self )
{
	struct hid_device_info* ptr;
  Data_Get_Struct( self, struct hid_device_info, ptr );
	hid_close(ptr);
	return Qnil;
}

/******************************************************************************\
 *
 * Register, modules, classes, methods, etc
 *
\******************************************************************************/
void Init_ruby_hidapi()
{
	/* Module to act as namespace */
	module_ruby_hidapi  = rb_define_module( "RubyHidApi" );
	
  class_hid_device_info = rb_define_class_under(module_ruby_hidapi, "HidDeviceInfo", rb_cObject);
	rb_define_singleton_method( class_hid_device_info, "new", hidDeviceInfoNew, 2 );
	//rb_define_method( class_hid_device_info, "destroy", hidDeviceInfoFree, 0 );
	rb_define_method( class_hid_device_info, "hello", hidDeviceInfoHelloWorld, 0 );
	rb_define_method( class_hid_device_info, "path", hidDeviceInfoPath, 0 );
	rb_define_method( class_hid_device_info, "vendor_id", hidDeviceInfoVendorID, 0 );
	rb_define_method( class_hid_device_info, "product_id", hidDeviceInfoProductID, 0 );
	rb_define_method( class_hid_device_info, "serial_number", hidDeviceInfoSerialNumber, 0 );
	rb_define_method( class_hid_device_info, "release_number", hidDeviceInfoReleaseNumber, 0 );
	rb_define_method( class_hid_device_info, "manufacturer_string", hidDeviceInfoManufacturerString, 0 );
	rb_define_method( class_hid_device_info, "product_string", hidDeviceInfoProductString, 0 );
	rb_define_method( class_hid_device_info, "usage_page", hidDeviceInfoUsagePage, 0 );
	rb_define_method( class_hid_device_info, "usage", hidDeviceInfoUsage, 0 );
	rb_define_method( class_hid_device_info, "interface_number", hidDeviceInfoInterfaceNumber, 0 );
	rb_define_method( class_hid_device_info, "next", hidDeviceInfoNext, 0 );
	rb_define_method( class_hid_device_info, "open", hidDeviceInfoOpen, 0 );
	rb_define_method( class_hid_device_info, "close", hidDeviceInfoClose, 0 );
	
	class_hid_device = rb_define_class_under(module_ruby_hidapi, "HidDevice", rb_cObject);
	//rb_define_singleton_method( class_hid_device, "new", hidDeviceNew, 2 );
	rb_define_method( class_hid_device, "write", hidDeviceWrite, 1 );
	rb_define_method( class_hid_device, "read", hidDeviceRead, 0 );
	rb_define_method( class_hid_device, "feature_report", hidDeviceFeatureReport, 1 );
}