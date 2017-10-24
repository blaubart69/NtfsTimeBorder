#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdio.h>

#define _SECOND (10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)

char* sprintSytemtime(const SYSTEMTIME sysTime, char* buffer, const size_t BufferCount);
char* sprintFiletimeUTC(const FILETIME fileTime, char* buffer, const size_t bufferCount);

int main(int argc, char* argv[])
{
	char buf[255];
	
	_declspec(align(64)) FILETIME ft;
	
	ULARGE_INTEGER* ui;
	ui = &ft;
	//ui->QuadPart= 0x8000000000000000;
	ui->QuadPart =  0x7FFFFFFFFFFFFFFF;

	printf("biggest filetime as system time UTC: %s (0x%I64X)\n", sprintFiletimeUTC(ft, buf, sizeof(buf)), *ui );

	SYSTEMTIME stUTC;
	SYSTEMTIME stLocal;

	stUTC.wYear = 30827;
	stUTC.wMonth = 12;
	stUTC.wDay = 31;
	stUTC.wHour = 23;
	stUTC.wMinute = 59;
	stUTC.wSecond = 59;
	stUTC.wMilliseconds = 999;

	if (!SystemTimeToFileTime(&stUTC, &ft)) {
		printf("E-SystemTimeToFileTime: rc=%d\n", GetLastError());
	}

	ui->QuadPart += 100000;
	printf("setting FILETIME to: %s\n", sprintFiletimeUTC(ft, buf, sizeof(buf)));

	ULONG64 errorsConvToLocalTime = -1;
	do {
		//ui->QuadPart -= _SECOND;
		ui->QuadPart -= 1;

		if (!FileTimeToSystemTime(&ft, &stUTC)) {
			printf("E-FileTimeToSystemTime: rc=%d\n", GetLastError());
			return 1;
		}
		errorsConvToLocalTime += 1;
	} while ( ! SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal) );

	printf("number errors (100ns steps) converting to local time: %I64d\n", errorsConvToLocalTime);
	printf("first filetime converted successfully to local time:\n");

	sprintSytemtime(stUTC, buf, sizeof(buf));
	printf("UTC:   %s (0x%I64X)\n", buf, ui->QuadPart);

	sprintSytemtime(stLocal, buf, sizeof(buf));
	printf("local: %s\n", buf);


}

char* sprintFiletimeUTC(const FILETIME fileTime, char* buffer, const size_t bufferCount) {
	SYSTEMTIME stUTC;

	if (!FileTimeToSystemTime(&fileTime, &stUTC)) {
		buffer[0] = 0;
		printf("E-sprintFiletimeUTC: FileTimeToSystemTime rc=%d\n", GetLastError());
	}
	else {
		char tmpBuf[255];
		sprintf_s(buffer, bufferCount, "%s", sprintSytemtime(stUTC, tmpBuf, sizeof(tmpBuf)) );
	}

	return buffer;
}

char* sprintSytemtime(const SYSTEMTIME sysTime, char* buffer, const size_t bufferCount) {
	sprintf_s(
		buffer, 
		bufferCount,
		"%d.%02d.%02d %02d:%02d:%02d.%03d",
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

	return buffer;
}

