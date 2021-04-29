#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void Info(const char *fmt, ...);
static void Warn(const char *fmt, ...);
static void Error(const char *fmt, ...);
static void Fatal(const char *fmt, ...) NORETURN;

void
Info(const char *fmt, ...)
{
	va_list v;
	fputs(" [INFO] ", stdout);
	va_start(v, fmt);
	vprintf(fmt, v);
	va_end(v);
	putchar('\n');
}

void
Warn(const char *fmt, ...)
{
	va_list v;
	fputs(" [WARN] ", stdout);
	va_start(v, fmt);
	vprintf(fmt, v);
	va_end(v);
	putchar('\n');
}

void
Error(const char *fmt, ...)
{
	va_list v;
	fputs("[ERROR] ", stdout);
	va_start(v, fmt);
	vprintf(fmt, v);
	va_end(v);
	putchar('\n');
}

void
Fatal(const char *fmt, ...)
{
	va_list v;
	fputs("[FATAL] ", stdout);
	va_start(v, fmt);
	vprintf(fmt, v);
	va_end(v);
	putchar('\n');
	exit(1);
}
