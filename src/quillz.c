#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include <wren.h>


#define RESTRICT restrict
#define NORETURN __attribute__ ((noreturn))


struct {
	WrenVM *vm;
	WrenHandle *sketch, *sketchclass;
	char *sketchfile, *sketchsrc;
	GLFWwindow *win;
} g = {0};

#include "quillz.wren.inc"


static void cleanup_gl();
static void init();
static int init_gl();
static char *load_file(const char *path, size_t *RESTRICT size_out);
static void parse_args(int argc, char **argv);
static void run();
static void quillzError(WrenVM *vm, WrenErrorType type, const char *module, int line, const char *message);
static WrenLoadModuleResult quillzLoadModule(WrenVM *vm, const char *name);
static void quillzWrite(WrenVM *vm, const char *text);

#include "logging.c"
#include "foreigns.c"


void
cleanup_gl()
{
	glfwTerminate();
}

void
init()
{
	WrenConfiguration cfg;
	wrenInitConfiguration(&cfg);
	cfg.loadModuleFn = quillzLoadModule;
	cfg.errorFn = quillzError;
	cfg.writeFn = quillzWrite;
	cfg.bindForeignMethodFn = quillzBindForeignMethod;
	g.vm = wrenNewVM(&cfg);
	
	g.sketchsrc = load_file(g.sketchfile, NULL);
	if (!g.sketchsrc) Fatal("Cannot load sketch file `%s`.", g.sketchfile);
	puts(g.sketchsrc);
	
	WrenInterpretResult ires = wrenInterpret(g.vm, "quillz:init", "import \"sketch\"");
	if (ires != WREN_RESULT_SUCCESS) Fatal("Error running module `sketch`.");
	
	ires = wrenInterpret(g.vm, "quillz:init",
		"import \"meta\" for Meta" "\n"
		"import \"quillz\" for Sketch" "\n"
		"var sketchclass" "\n"
		"var modvars = Meta.getModuleVariables(\"sketch\")" "\n"
		"class IsSketch {" "\n"
		"	static check(clazz) {" "\n"
		"		while (clazz != Object) {" "\n"
		"			if (clazz == Sketch) return true" "\n"
		"			clazz = clazz.supertype" "\n"
		"		}" "\n"
		"		return false" "\n"
		"	}" "\n"
		"}" "\n"
	);
	if (ires != WREN_RESULT_SUCCESS) Fatal("Error searching for the sketch class.");
	
	wrenEnsureSlots(g.vm, 1);
	wrenGetVariable(g.vm, "quillz:init", "modvars", 0);
	WrenHandle *handle = NULL;
	WrenHandle *list_handle = wrenGetSlotHandle(g.vm, 0);
	WrenHandle *attributes_handle = wrenMakeCallHandle(g.vm, "attributes");
	WrenHandle *self_handle = wrenMakeCallHandle(g.vm, "self");
	int nvars = wrenGetListCount(g.vm, 0);
	for (int i = 0; i < nvars; ++i) {
		wrenSetSlotHandle(g.vm, 0, list_handle);
		wrenGetListElement(g.vm, 0, i, 0);
		char *s = strdup(wrenGetSlotString(g.vm, 0));
		printf("modvar: %s\n", s);
		wrenGetVariable(g.vm, "sketch", s, 0);
		free(s);
		handle = wrenGetSlotHandle(g.vm, 0);
		if (wrenGetSlotType(g.vm, 0) == WREN_TYPE_UNKNOWN) {
			printf("modvar type unknown\n");
			wrenCall(g.vm, attributes_handle);
			wrenEnsureSlots(g.vm, 1);
			if (wrenGetSlotType(g.vm, 0) == WREN_TYPE_UNKNOWN) {
				printf("modvar.attributes type unknown\n");
				wrenCall(g.vm, self_handle);
				wrenEnsureSlots(g.vm, 2);
				if (wrenGetSlotType(g.vm, 0) == WREN_TYPE_MAP) {
					printf("modvar.attributes.self type map\n");
					wrenSetSlotNull(g.vm, 1);
					wrenGetMapValue(g.vm, 0, 1, 0);
					wrenSetSlotString(g.vm, 1, "sketch");
					if (wrenGetMapContainsKey(g.vm, 0, 1)) {
						printf("map[null] has key `sketch`\n");
						break;
					}
				}
			}
		}
		wrenReleaseHandle(g.vm, handle);
		handle = NULL;
	}
	wrenReleaseHandle(g.vm, list_handle);
	wrenReleaseHandle(g.vm, attributes_handle);
	wrenReleaseHandle(g.vm, self_handle);
	if (!(g.sketchclass = handle)) Fatal("No class found marked with `#!sketch`.");
	
	wrenEnsureSlots(g.vm, 2);
	handle = wrenMakeCallHandle(g.vm, "check(_)");
	wrenGetVariable(g.vm, "quillz:init", "IsSketch", 0);
	wrenSetSlotHandle(g.vm, 1, g.sketchclass);
	if (wrenCall(g.vm, handle) != WREN_RESULT_SUCCESS) Fatal("Error checking if the sketch class inherits from `Sketch`.");
	if (!wrenGetSlotBool(g.vm, 0)) Fatal("Sketch class doesn't inherit from `Sketch`.");
	wrenReleaseHandle(g.vm, handle);
	
	wrenEnsureSlots(g.vm, 1);
	handle = wrenMakeCallHandle(g.vm, "preinit()");
	wrenSetSlotHandle(g.vm, 0, g.sketchclass);
	if (wrenCall(g.vm, handle) != WREN_RESULT_SUCCESS) Fatal("Error calling `preinit()` constructor.");
	g.sketch = wrenGetSlotHandle(g.vm, 0);
	wrenReleaseHandle(g.vm, handle);
}

int
init_gl()
{
	if (!glfwInit()) return -1;
	
	g.win = glfwCreateWindow(400, 400, "Quillz sketch", NULL, NULL);
	if (!g.win) {
		glfwTerminate();
		return -2;
	}
	
	glfwMakeContextCurrent(g.win);
	
	return 0;
}

char *
load_file(const char *path, size_t *RESTRICT size_out)
{
	int fd = open(path, O_RDONLY);
	if (fd == -1) return NULL;
	
	struct stat stat;
	if (fstat(fd, &stat) < 0) {
		close(fd);
		return NULL;
	}
	
	void *mapped = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mapped == MAP_FAILED) {
		close(fd);
		return NULL;
	}
	
	char *to_return = malloc(stat.st_size + 1);
	if (!to_return) {
		munmap(mapped, stat.st_size);
		close(fd);
		return NULL;
	}
	memcpy(to_return, mapped, stat.st_size);
	to_return[stat.st_size] = '\0';
	
	munmap(mapped, stat.st_size);
	close(fd);
	if (size_out) *size_out = stat.st_size;
	return to_return;
}

void
parse_args(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i) {
		char *arg = argv[i];
		if (!strcmp(arg, "--help")) {
			
		} else if (arg[0] == '-') {
			
		} else {
			g.sketchfile = arg;
		}
	}
}

void
run()
{
	if (init_gl() < 0) {
		Fatal("Initializing GLFW/OpenGL failed.");
	}
	
	WrenHandle *handle = wrenMakeCallHandle(g.vm, "setup()");
	wrenEnsureSlots(g.vm, 1);
	wrenSetSlotHandle(g.vm, 0, g.sketch);
	wrenCall(g.vm, handle);
	wrenReleaseHandle(g.vm, handle);
	
	handle = wrenMakeCallHandle(g.vm, "draw()");
	while (!glfwWindowShouldClose(g.win)) {
		wrenEnsureSlots(g.vm, 1);
		wrenSetSlotHandle(g.vm, 0, g.sketch);
		wrenCall(g.vm, handle);
		
		glfwSwapBuffers(g.win);
		glfwPollEvents();
	}
	wrenReleaseHandle(g.vm, handle);
	
	cleanup_gl();
}

void
quillzError(WrenVM *vm, WrenErrorType type, const char *module, int line, const char *message)
{
	fprintf(stderr, "error type %d in module `%s` at line %d: %s\n", type, module, line, message);
}

WrenLoadModuleResult
quillzLoadModule(WrenVM *vm, const char *name)
{
	WrenLoadModuleResult res = {0};
	if (!strcmp(name, "sketch")) {
		res.source = g.sketchsrc;
	} else if (!strcmp(name, "quillz")) {
		res.source = quillz_module_src;
	}
	return res;
}

void
quillzWrite(WrenVM *vm, const char *text)
{
	fputs(text, stdout);
}


int
main(int argc, char **argv)
{
	parse_args(argc, argv);
	init();
	run();
	return 0;
}
