#include <LPC21xx.h>
#include "types.h"
#include "spi.h"
#include "mcp3204.h"
#include "lcd.c"
#include "gsm.h"

#define IR_SENSOR (1<<17)
#define BUZZER (1<<16)   /* P1.16 */

/* -------- MCP3204 channel map -------- */
#define CH_TEMP  0    /* LM35 */
#define CH_SOIL  2    /* Soil moisture sensor */
#define CH_LDR   3    /* LDR */

/* -------- Thresholds (calibrate on actual hardware) -------- */
#define TEMP_THRESHOLD_C     40.0    /* alert if temp > 40C */
#define SOIL_THRESHOLD_V      1.0    /* alert if soil voltage < 1.0V (dry) */
#define LDR_NIGHT_THRESHOLD_V 1.5    /* below = night */

u8 PHONE_NUMBER[] = "+919489559453";

/* ---- append integer as decimal text into a buffer, return new write pointer ---- */
u8 *append_int(u8 *p, s32 val)
{
	u8 temp[8];
	s8 i = 0;

	if (val == 0) { *p++ = '0'; return p; }
	if (val < 0) { *p++ = '-'; val = -val; }

	while (val > 0) { temp[i++] = (val % 10) + '0'; val /= 10; }
	while (i > 0) *p++ = temp[--i];

	return p;
}

u8 *append_str(u8 *p, u8 *s)
{
	while (*s) *p++ = *s++;
	return p;
}

int main(void)
{
	f32 temp_v, soil_v, ldr_v;
	u8 ir_detected;
	f32 temperature, soil_percent;
	u8 alert;
	u8 sms_msg[120];
	u8 *p;

	Init_SPI0();          /* SPI0 + CS pin (P0.7) init - must run before lcd_init */
	lcd_init();
	GSM_Init();
	IODIR1 |= BUZZER;
	IODIR0 &= ~IR_SENSOR;
	IOCLR1 = BUZZER;

	str("WEATHER MONITOR");
	lcd_cmd(0xc0);
	str("Initializing...");
	delay_milliseconds(2000);
	lcd_cmd(0x01);

	while (1)
	{
		/* ---- Read all sensors ---- */
		temp_v = Read_ADC_MCP3204(CH_TEMP);
		ir_detected = (IOPIN0 & IR_SENSOR) ? 0 : 1;
		soil_v = Read_ADC_MCP3204(CH_SOIL);
		ldr_v  = Read_ADC_MCP3204(CH_LDR);

		temperature = temp_v * 10.0;   /* LM35: 10mV/C -> V*100 = degrees C */

		/* ---- Screen 1: Temp + Rain ---- */
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		str("Temp:");
		Float_display(temperature);
		str("C");

		lcd_cmd(0xc0);
		str("IR:");
		str((u8 *)(ir_detected ? "DETECTED" : "CLEAR"));

		delay_milliseconds(2000);

		/* ---- Screen 2: Soil + Light ---- */
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		str("Soil:");
		soil_percent=((3.3-soil_v)/3.3)*100.0;
		Float_display(soil_percent);
		str("%");

		lcd_cmd(0xc0);
		str("Light:");
		str((u8 *)((ldr_v > LDR_NIGHT_THRESHOLD_V) ? "NIGHT" : "DAY"));

		delay_milliseconds(2000);

		/* ---- Threshold check ---- */
		alert = 0;
		if (temperature > TEMP_THRESHOLD_C) alert = 1;
		if (ir_detected)        alert = 1;
		if (soil_v < SOIL_THRESHOLD_V)       alert = 1;

		if (alert)
		{
			IOSET1 = BUZZER;

			lcd_cmd(0x01);
			lcd_cmd(0x80);
			str("** ALERT **");
			lcd_cmd(0xc0);
			str("Sending SMS...");

			/* ---- Build SMS text ---- */
			p = sms_msg;
			p = append_str(p, "WEATHER ALERT\r\nTemp:");
			p = append_int(p, (s32)temperature);
			p = append_str(p, "C\r\nIR:");
			p = append_str(p, (u8 *)(ir_detected ? "DETECTED" : "CLEAR"));
			p = append_str(p, "\r\nSoil:");
			p = append_int(p, (s32)soil_percent);
			p = append_str(p, "\r\nTake Action.");
			*p = '\0';

			GSM_SendSMS(PHONE_NUMBER, sms_msg);

			IOCLR1 = BUZZER;
		}
	}
}
               





