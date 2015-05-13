// DateTime.h - Date and time management.

#ifndef __DATETIME_H__
#define __DATETIME_H__

struct DateTime
{
	signed short Year;
	unsigned char Month;
	unsigned char Day;
	unsigned char Hour;
	unsigned char Minute;
	unsigned char Second;
} PACKED;

#endif
