#include <fcntl.h>
#include <linux/soundcard.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include "../headers/logic.h"
#include "../headers/system_logic.h"
#include "../headers/globals.h"
#include "../headers/utils.h"
#if defined TARGET_OD || defined TARGET_OD_BETA
#include <shake.h>
#endif

#if defined TARGET_OD_BETA
#define SYSFS_CPUFREQ_DIR "/sys/devices/system/cpu/cpu0/cpufreq"
#define SYSFS_CPUFREQ_LIST SYSFS_CPUFREQ_DIR "/scaling_available_frequencies"
#define SYSFS_CPUFREQ_SET SYSFS_CPUFREQ_DIR "/scaling_setspeed"
#define SYSFS_CPUFREQ_CUR SYSFS_CPUFREQ_DIR "/scaling_cur_freq"
#endif

volatile uint32_t *memregs;
int32_t memdev = 0;
int oldCPU;


#ifdef MIYOOMINI
typedef struct {
    int channel_value;
    int adc_value;
} SAR_ADC_CONFIG_READ;
 
#define SARADC_IOC_MAGIC                     'a'
#define IOCTL_SAR_INIT                       _IO(SARADC_IOC_MAGIC, 0)
#define IOCTL_SAR_SET_CHANNEL_READ_VALUE     _IO(SARADC_IOC_MAGIC, 1)

static SAR_ADC_CONFIG_READ  adcCfg = {0,0};
static int sar_fd = 0;

static void initADC(void) {
    sar_fd = open("/dev/sar", O_WRONLY);
    ioctl(sar_fd, IOCTL_SAR_INIT, NULL);
}

static int is_charging = 0;
void checkCharging(void) {
    int i = 0;
    FILE *file = fopen("/sys/devices/gpiochip0/gpio/gpio59/value", "r");
    if (file!=NULL) {
        fscanf(file, "%i", &i);
        fclose(file);
    }
    is_charging = i;
}

int percBat = 0;
int firstLaunch = 1; 
int countChecks = 0;

static int checkADC(void) {  
    ioctl(sar_fd, IOCTL_SAR_SET_CHANNEL_READ_VALUE, &adcCfg);

	
	int old_is_charging = is_charging;
	checkCharging();
	
        char val[3];
                
  		int percBatTemp = 0;
  		if (is_charging == 0){
			if (adcCfg.adc_value >= 528){
  				percBatTemp = adcCfg.adc_value-478;
  			}
  			else if ((adcCfg.adc_value >= 512) && (adcCfg.adc_value < 528)){
  				percBatTemp = (int)(adcCfg.adc_value*2.125-1068);		
  			}
  			else if ((adcCfg.adc_value >= 480) && (adcCfg.adc_value < 512)){
  				percBatTemp = (int)(adcCfg.adc_value* 0.51613 - 243.742);		
  			}
  			
  			if ((firstLaunch == 1) || (old_is_charging == 1)){
        		// Calibration needed at first launch or when the 
        		// user just unplugged his charger
        		firstLaunch = 0;
        		percBat =  percBatTemp;
        	}
        	else {
        		if (percBat>percBatTemp){
  					percBat--;
  				}
  				else if (percBat < percBatTemp){
  					percBat++;
  				}        
        	}
        	if (percBat<0){
 				percBat=0;
 			}	
  			if (percBat>100){
 				percBat=100;
 			}	

  		}
  		else {
  		// The handheld is currently charging
  		percBat = 500 ;
  		}
  
  		

		
  		//countChecks ++;
    //}
	return percBat;
}
#endif

void to_string(char str[], int num)
{
    int i, rem, len = 0, n;

    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

void setCPU(uint32_t mhz)
{
	currentCPU = mhz;
	#if defined TARGET_OD_BETA
		char strMhz[10];
		int fd = open(SYSFS_CPUFREQ_SET, O_RDWR);
		to_string(strMhz, (mhz * 1000));
		ssize_t ret = write(fd, strMhz, strlen(strMhz));
		if (ret==-1) {
			logMessage("ERROR", "setCPU", "Error writting to file");
		}
		close(fd);
		char temp[300];
		snprintf(temp,sizeof(temp),"CPU speed set: %d",currentCPU);
		logMessage("INFO","setCPU",temp);
	#endif
}

void turnScreenOnOrOff(int state) {
	const char *path = "/sys/class/graphics/fb0/blank";
	const char *blank = state ? "0" : "1";
	int fd = open(path, O_RDWR);
	int ret = write(fd, blank, strlen(blank));
	if (ret==-1) {
		generateError("FATAL ERROR", 1);
	}
	close(fd);
}

void clearTimer() {
	if (timeoutTimer != NULL) {
		SDL_RemoveTimer(timeoutTimer);
	}
	timeoutTimer = NULL;
}

uint32_t suspend() {
	if(timeoutValue!=0&&hdmiEnabled==0) {
		clearTimer();
		oldCPU=currentCPU;
		turnScreenOnOrOff(0);
		isSuspended=1;
	}
	return 0;
};

void resetScreenOffTimer() {
#ifndef TARGET_PC
	if(isSuspended) {
		turnScreenOnOrOff(1);
		currentCPU=oldCPU;
		isSuspended=0;
	}
	clearTimer();
	timeoutTimer=SDL_AddTimer(timeoutValue * 1e3, suspend, NULL);
#endif
}

void initSuspendTimer() {
	timeoutTimer=SDL_AddTimer(timeoutValue * 1e3, suspend, NULL);
	isSuspended=0;
	logMessage("INFO","initSuspendTimer","Suspend timer initialized");
}

void HW_Init()
{
	#if defined TARGET_OD || defined TARGET_OD_BETA
	Shake_Init();
	device = Shake_Open(0);
	Shake_SimplePeriodic(&effect, SHAKE_PERIODIC_SQUARE, 0.5, 0.1, 0.05, 0.1);
	Shake_SimplePeriodic(&effect1, SHAKE_PERIODIC_SQUARE, 0.5, 0.1, 0.05, 0.1);
	effect_id=Shake_UploadEffect(device, &effect);
	effect_id1=Shake_UploadEffect(device, &effect1);
	#endif
	
	#ifdef MIYOOMINI
	initADC();
	#endif
	logMessage("INFO","HW_Init","HW Initialized");
}

void rumble() {

}

int getBatteryLevel() {
	int max_voltage;
	int voltage_now;
	int total;
#if defined (TARGET_OD_BETA)
	int charging=0;
	int min_voltage;
	FILE *f = fopen("/sys/class/power_supply/jz-battery/voltage_max_design", "r");
	fscanf(f, "%i", &max_voltage);
	fclose(f);

	f = fopen("/sys/class/power_supply/jz-battery/voltage_min_design", "r");
	fscanf(f, "%i", &min_voltage);
	fclose(f);

	f = fopen("/sys/class/power_supply/jz-battery/voltage_now", "r");
	fscanf(f, "%i", &voltage_now);
	fclose(f);

	f = fopen("/sys/class/power_supply/usb-charger/online", "r");
	fscanf(f, "%i", &charging);
	fclose(f);

	total = (voltage_now - min_voltage) * 6 / (max_voltage - min_voltage);
	if (charging==1) {
		return 6;
	}
	if (total>5) {
		return 5;
	}
	return total;
#elif defined (TARGET_OD)
	int min_voltage;
	int charging=0;
	FILE *f = fopen("/sys/class/power_supply/battery/voltage_max_design", "r");
	fscanf(f, "%i", &max_voltage);
	fclose(f);

	f = fopen("/sys/class/power_supply/battery/voltage_min_design", "r");
	fscanf(f, "%i", &min_voltage);
	fclose(f);

	f = fopen("/sys/class/power_supply/battery/voltage_now", "r");
	fscanf(f, "%i", &voltage_now);
	fclose(f);

	total = (voltage_now - min_voltage) * 6 / (max_voltage - min_voltage);
	if (charging==1) {
		return 6;
	}
	if (total>5) {
		return 5;
	}
	return total;
#elif defined (MIYOOMINI)	
	int charge = checkADC();
	if (charge<=20)		return 1;
	else if (charge<=40) return 2;
	else if (charge<=60) return 3;
	else if (charge<=80) return 4;
	else if (charge<=100) return 5;
	else return 6;	
#elif defined (RGNANO) || defined (FUNKEY)
	unsigned long voltage_min = 0, voltage_max = 1;
	voltage_now = 1;
	FILE *handle;

	handle = fopen("/sys/class/power_supply/AC0/status", "r");
	if (handle) {
		char buf[16];

		buf[sizeof(buf) - 1] = '\0';

		fread(buf, 1, 16, handle);
		fclose(handle);
		if (strcmp(buf, "Charging") == 0)
			return 6;
	}

	handle = fopen("/sys/class/power_supply/AC0/online", "r");
	if (handle) {
		int usbval = 0;
		fscanf(handle, "%d", &usbval);
		fclose(handle);
		if (usbval == 1)
			return 6;
	}

	handle = fopen("/sys/class/power_supply/AC0/capacity", "r");
	if (handle) {
		int battval = 0;
		fscanf(handle, "%d", &battval);
		fclose(handle);

		if (battval>90) return 5;
		if (battval>70) return 4;
		if (battval>50) return 3;
		if (battval>30) return 2;
		if (battval>10) return 1;

		return 0;
	}

	/*
	 * No 'capacity' file in sysfs - Do a dumb approximation of the capacity
	 * using the current voltage reported and the min/max voltages of the
	 * battery.
	 */

	handle = fopen("/sys/class/power_supply/BAT0/voltage_max_design", "r");
	if (handle) {
		fscanf(handle, "%lu", &voltage_max);
		fclose(handle);
	}

	handle = fopen("/sys/class/power_supply/BAT0/voltage_min_design", "r");
	if (handle) {
		fscanf(handle, "%lu", &voltage_min);
		fclose(handle);
	}

	handle = fopen("/sys/class/power_supply/BAT0/voltage_now", "r");
	if (handle) {
		fscanf(handle, "%lu", &voltage_now);
		fclose(handle);
	}

	return (voltage_now - voltage_min) * 6 / (voltage_max - voltage_min);	
#else
/*
	FILE *f = fopen("/sys/class/power_supply/BAT0/charge_full", "r");
	int ret = fscanf(f, "%i", &max_voltage);
	if (ret==-1) {
		logMessage("INFO","getBatteryLevel","Error");
	}
	fclose(f);
	f = fopen("/sys/class/power_supply/BAT0/charge_now", "r");
	ret = fscanf(f, "%i", &voltage_now);
	if (ret==-1) {
		logMessage("INFO","getBatteryLevel","Error");
	}
	fclose(f);

	total = (voltage_now*6)/(max_voltage);
	return total;
*/
	return 2;
#endif
}

int getCurrentBrightness() {
	int level;
	FILE *f = fopen("/sys/class/backlight/backlight/brightness", "r");
	if (f==NULL) {
		logMessage("INFO","getCurrentBrightness","Error, file not found");
		return 12;
	} else {
		int ret = fscanf(f, "%i", &level);
		if(ret==-1) {
			logMessage("INFO","getCurrentBrightness","Error reading file");
			return 12;
		}
		fclose(f);
	}
	return level;
}

int getMaxBrightness() {
	int level;
	FILE *f = fopen("/sys/class/backlight/backlight/max_brightness", "r");
	if (f==NULL) {
		logMessage("INFO","getCurrentBrightness","Error, file not found");
		return 12;
	} else {
		int ret = fscanf(f, "%i", &level);
		if(ret==-1) {
			logMessage("INFO","getCurrentBrightness","Error reading file");
			return 12;
		}
		fclose(f);
	}
	return level;
}

void setBrightness(int value) {
	FILE *f = fopen("/sys/class/backlight/backlight/brightness", "w");
	if (f!=NULL) {
		fprintf(f, "%d", value);
		fclose(f);
	}
}

