def to_c(varname, srclines):
	c = f'const char *{varname} =\n'
	for line in srclines:
		c += '"' + line.replace('"', r'\"').replace('\n', r'\n') + '"\n'
	return c + ';\n'

def main():
	wrenlines = None
	with open('src/quillz.wren', 'r') as wrenfile:
		wrenlines = wrenfile.readlines()
	c = to_c('quillz_module_src', wrenlines)
	with open('src/quillz.wren.inc', 'w') as cfile:
		cfile.write(c)

main()
