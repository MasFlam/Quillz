#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <wren.h>

static WrenForeignMethodFn quillzBindForeignMethod(WrenVM* vm, const char* module, const char* clazz, bool is_static, const char* signature);
static void quillz_background(WrenVM *vm);
static void quillz_begin(WrenVM *vm);
static void quillz_end(WrenVM *vm);
static void quillz_height(WrenVM *vm);
static void quillz_size(WrenVM *vm);
static void quillz_sleep(WrenVM *vm);
static void quillz_title(WrenVM *vm);
static void quillz_vertex(WrenVM *vm);
static void quillz_width(WrenVM *vm);


WrenForeignMethodFn
quillzBindForeignMethod(WrenVM* vm, const char* module, const char* clazz, bool is_static, const char* sign) {
	if (strcmp(module, "quillz")) return NULL;
	
	if (!strcmp(clazz, "Sketch")) {
		if (!strcmp(sign, "background_(_,_,_,_)")) {
			return quillz_background;
		} else if (!strcmp(sign, "begin_(_)")) {
			return quillz_begin;
		} else if (!strcmp(sign, "end()")) {
			return quillz_end;
		} else if (!strcmp(sign, "height")) {
			return quillz_height;
		} else if (!strcmp(sign, "size_(_,_)")) {
			return quillz_size;
		} else if(!strcmp(sign, "sleep_(_)")) {
			return quillz_sleep;
		} else if(!strcmp(sign, "title_(_)")) {
			return quillz_title;
		} else if (!strcmp(sign, "vertex_(_,_,_)")) {
			return quillz_vertex;
		} else if (!strcmp(sign, "width")) {
			return quillz_width;
		}
	}
	// should be unreachable
	return NULL;
}


void
quillz_background(WrenVM *vm)
{
	float r = wrenGetSlotDouble(vm, 1);
	float g = wrenGetSlotDouble(vm, 2);
	float b = wrenGetSlotDouble(vm, 3);
	float a = wrenGetSlotDouble(vm, 4);
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
	wrenSetSlotNull(vm, 0);
}

void
quillz_begin(WrenVM *vm)
{
	unsigned int mode = wrenGetSlotDouble(vm, 1);
	glBegin(mode);
	wrenSetSlotNull(vm, 0);
}

void
quillz_end(WrenVM *vm)
{
	glEnd();
	wrenSetSlotNull(vm, 0);
}

void
quillz_height(WrenVM *vm)
{
	wrenSetSlotDouble(vm, 0, g.winh);
}

void
quillz_size(WrenVM *vm)
{
	g.winw = wrenGetSlotDouble(vm, 1);
	g.winh = wrenGetSlotDouble(vm, 2);
	glfwSetWindowSize(g.win, g.winw, g.winh);
	wrenSetSlotNull(vm, 0);
}

void
quillz_sleep(WrenVM *vm)
{
	double sec = wrenGetSlotDouble(vm, 1);
	struct timespec tm = {
		.tv_nsec = (sec - floor(sec)) * 1000000000,
		.tv_sec = sec
	};
	while (nanosleep(&tm, &tm) < 0) {
		switch (errno) {
		case EINVAL:
		case EFAULT: {
			wrenSetSlotString(vm, 0, "Unexpected error occured when sleeping");
			wrenAbortFiber(vm, 0);
			return;
		}}
	}
	wrenSetSlotNull(vm, 0);
}

void
quillz_title(WrenVM *vm)
{
	int len;
	const char *str = wrenGetSlotBytes(vm, 1, &len);
	char *title = strndup(str, len);
	glfwSetWindowTitle(g.win, title);
	free(title);
	wrenSetSlotNull(vm, 0);
}

void
quillz_vertex(WrenVM *vm)
{
	float x = wrenGetSlotDouble(vm, 1);
	float y = wrenGetSlotDouble(vm, 2);
	float z = wrenGetSlotDouble(vm, 3);
	glVertex3f(x, y, z);
	wrenSetSlotNull(vm, 0);
}

void
quillz_width(WrenVM *vm)
{
	wrenSetSlotDouble(vm, 0, g.winw);
}
