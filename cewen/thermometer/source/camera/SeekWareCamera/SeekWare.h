
#ifndef _SeekWare_h
#define _SeekWare_h

#include <inttypes.h>
//#include "SeekUsb.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef SEEKWAREFUNCSDLL_EXPORTS
#define SEEKWAREFUNCSDLL_API __declspec(dllexport) 
#else
#define SEEKWAREFUNCSDLL_API __declspec(dllimport) 
#endif


typedef enum sw_settings {
	SETTING_ACTIVE_LUT,
	SETTING_TEMP_UNITS,
	SETTING_TIMEOUT,
	SETTING_SOFTKNEE,
	SETTING_AUTOSHUTTER,
	SETTING_TRIGGER_SHUTTER,
	SETTING_ENABLE_TIMESTAMP,
	SETTING_RESET_TIMESTAMP,
} sw_settings;

typedef enum sw_display_lut {
	SW_LUT_WHITE,
	SW_LUT_BLACK,
	SW_LUT_IRON,
	SW_LUT_COOL,
	SW_LUT_AMBER,
	SW_LUT_INDIGO,
	SW_LUT_TYRIAN,
	SW_LUT_GLORY,
	SW_LUT_ENVY,
   SW_LUT_NEWWHITE,
   SW_LUT_NEWBLACK,
   SW_LUT_SPECTRA,
   SW_LUT_PRISM,
   SW_LUT_NEWTYRIAN,
   SW_LUT_NEWIRON,
   SW_LUT_NEWAMBER,
   SW_LUT_HI,
   SW_LUT_HILO,
   SW_LUT_LAST,
} sw_display_lut;

typedef enum sw_temp_units {
	SW_TEMP_F, // Fahrenheit 
	SW_TEMP_C, // Celsius
	SW_TEMP_K // Kelvin
} sw_temp_units;


typedef enum sw_retcode {
	SW_RETCODE_NONE,           // No error has been detected. 
	SW_RETCODE_OPENEX,         // Device is already opened exclusively
	SW_RETCODE_BPARAM,         // Bad parameter.
   SW_RETCODE_DONOTPROCESS    // IP Do Not Process 
} sw_retcode;


typedef enum sw_exerrcode {
	SW_ERR_NONE, // No error has been detected. 
	SW_ERR_FIND_OFLOW // Found more objects than could be filled into the buffer.
} sw_exerrcode;


typedef struct sw_buffer_t {
	void *Buffer;
	uint32_t NumElements;
	uint32_t ElementSizeInBytes;
} sw_buffer_t;


typedef struct sw_process_t {
	// Inputs
	sw_buffer_t RawImageFrame;		// User-allocated buffer
	sw_buffer_t FilteredImageFrame;	// User-allocated buffer
	sw_buffer_t ThermImageFrame;	// User-allocated buffer
	sw_buffer_t ColorImageFrame;	// User-allocated buffer

	float ThermSpotTemp;

	float ThermMinTemperature, ThermMaxTemperature;
	uint32_t ThermMinTempX, ThermMinTempY;
	uint32_t ThermMaxTempX, ThermMaxTempY;
} sw_process_t;


// internal struct
typedef struct SeekWarePrivate {
	//PSEEK_USB_DEVICE seekUsbPtr;
	//CRITICAL_SECTION deviceLock;
	uint16_t isOpen;
	int timeout;
	sw_process_t processData;
	uint32_t rawFrameWidth;
	uint32_t rawFrameHeight;
	uint32_t rawFrameElementSizeInBytes;
	uint32_t thermFrameWidth;
	uint32_t thermFrameHeight;
	uint32_t thermFrameElementSizeInBytes;
	uint32_t filteredFrameWidth;
	uint32_t filteredFrameHeight;
	uint32_t filteredFrameElementSizeInBytes;
	uint32_t colorFrameWidth;
	uint32_t colorFrameHeight;
	uint32_t colorFrameElementSizeInBytes;
	sw_display_lut displayLut;
	sw_temp_units tempUnit;
	float softKnee;
} SeekWarePrivate;


typedef struct sw {
	// Device Information
	unsigned char model[16];
	unsigned char serialNumber[12];
	uint8_t fw_version_major;
	uint8_t fw_version_minor;
	uint8_t fw_build_major;
	uint8_t fw_build_minor;
	uint16_t frame_rows;
	uint16_t frame_cols;

#ifdef __linux__
	// Linux Fields
	libusb_device_handle * lusb_dev_handle;
	libusb_transfer_status * lusb_status;
#elif defined _WIN32 || defined _WIN64
	// Windows Fields
	char * win_dev_path;
	FILE * win_dev_handle;
#endif

	// Latest return code;
	sw_retcode retcode;

	// SDK Private Data
	void *ipPrivate;
	//SeekWarePrivate *swPrivate;
	void *swPrivate;
	uint16_t rawframe_rows;
	uint16_t rawframe_cols;
} sw, *psw;


typedef struct sw_sdk_info {
	uint8_t sdk_version_major; // SDK version number 
	uint8_t sdk_version_minor;
	uint8_t sdk_build_major;
	uint8_t sdk_build_minor;
	uint8_t lib_version_major; // Library version number 
	uint8_t lib_version_minor;
	uint8_t lib_build_major;
	uint8_t lib_build_minor;
} sw_sdk_info;


typedef struct COLOR_ARGB {
   union {
      uint32_t color;
      struct {
         uint8_t a;
         uint8_t r;
         uint8_t g;
         uint8_t b;
      };
   };
} COLOR_ARGB;

typedef COLOR_ARGB PALETTE[];

typedef enum PALETTE_INDEX {
   // Oldies but goodies
   PALETTE_grayscale,
   PALETTE_cool,
   PALETTE_fire,
   PALETTE_glow,
   PALETTE_ica,
   PALETTE_spectrum,
   PALETTE_smart,
   PALETTE_unionjack,
   PALETTE_yellowhot,
   PALETTE_coldest,
   PALETTE_hottest,
   PALETTE_redhot,
   PALETTE_sepia,
   PALETTE_royal,
   PALETTE_jet,
   PALETTE_gem,
   PALETTE_black,
   PALETTE_petcolor,
   PALETTE_hotmetalblue,
   PALETTE_hotiron,
   // New Crew
   PALETTE_white = 100,
   PALETTE_blacknew,
   PALETTE_spectra,
   PALETTE_prism,
   PALETTE_iron,
   PALETTE_tyrian,
   PALETTE_amber,
   PALETTE_hi,
   PALETTE_hilo
} PALETTE_INDEX;


// Function Prototypes
SEEKWAREFUNCSDLL_API sw_retcode Seekware_Find(psw pswlist[], int length, int *numfound);
SEEKWAREFUNCSDLL_API sw_retcode Seekware_Open(psw id);
SEEKWAREFUNCSDLL_API sw_retcode Seekware_Close(psw id);
SEEKWAREFUNCSDLL_API sw_retcode Seekware_GetSdkInfo(sw_sdk_info *info);
SEEKWAREFUNCSDLL_API sw_retcode Seekware_GetSetting(psw id, sw_settings index, int *value);
SEEKWAREFUNCSDLL_API sw_retcode Seekware_SetSetting(psw id, sw_settings index, int value);
SEEKWAREFUNCSDLL_API sw_retcode Seekware_UploadFirmware(psw id, char *filename);
SEEKWAREFUNCSDLL_API sw_retcode Seekware_GetImage(psw id, uint16_t *binaryImage, float *temperatureImage, uint32_t *display);
SEEKWAREFUNCSDLL_API sw_retcode Analysis_GetPalette(void *context, PALETTE_INDEX index, PALETTE *palette);
SEEKWAREFUNCSDLL_API sw_retcode Analysis_GetThermographyMinMax(void *context, float *thermographyData, float *min, float *max);


#ifdef __cplusplus
	}
#endif

#endif