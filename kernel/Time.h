// Time.h - Basic time unit.

#ifndef __TIME_H__
#define __TIME_H__

#include <cstdint>

namespace Kernel
{
	union Time
	{
	public:
		struct
		{
			uint64_t fraction : 24;
			int64_t seconds : 40;
		} time;
		int64_t raw;

		inline Time(int64_t t = 0) {raw = t;};
		inline Time(int64_t sec, uint64_t frac) {time.seconds = sec; time.fraction = frac;};

		inline static const Time Microsecond(int n = 1) {return Time((n * (1LL << 24)) / 1000000);};
		inline static const Time Millisecond(int n = 1) {return Time((n * (1LL << 24)) / 1000);};
		inline static const Time Second(int n = 1) {return Time(n, 0);};
		inline static const Time Minute(int n = 1) {return Time(60 * n, 0);};
		inline static const Time Hour(int n = 1) {return Time(3600 * n, 0);};
		inline static const Time Day(int n = 1) {return Time(86400 * n, 0);};

		inline Time& operator+=(Time t) {raw += t.raw; return *this;};
		inline Time& operator-=(Time t) {raw -= t.raw; return *this;};
		inline Time& operator*=(int64_t x) {raw *= x; return *this;};
		inline Time& operator/=(int64_t x) {raw /= x; return *this;};
	} PACKED;

	inline Time operator+(Time t) {return t;}
	inline Time operator-(Time t) {return Time(-t.raw);}

	inline Time operator+(Time t1, Time t2) {return Time(t1.raw + t2.raw);}
	inline Time operator-(Time t1, Time t2) {return Time(t1.raw - t2.raw);}
	inline Time operator*(Time t, int64_t x) {return Time(t.raw * x);}
	inline Time operator/(Time t, int64_t x) {return Time(t.raw / x);}

	inline bool operator<(Time t1, Time t2) {return(t1.raw < t2.raw);}
	inline bool operator>(Time t1, Time t2) {return(t1.raw > t2.raw);}
	inline bool operator<=(Time t1, Time t2) {return(t1.raw <= t2.raw);}
	inline bool operator>=(Time t1, Time t2) {return(t1.raw >= t2.raw);}
	inline bool operator==(Time t1, Time t2) {return(t1.raw == t2.raw);}
	inline bool operator!=(Time t1, Time t2) {return(t1.raw != t2.raw);}
}

#endif
