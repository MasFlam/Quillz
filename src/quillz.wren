
class GL {
	static POINTS { 0x0000 }
	static LINES { 0x0001 }
	static LINE_STRIP { 0x0002 }
	static LINE_LOOP { 0x0003 }
	static TRIANGLES { 0x0004 }
	static TRIANGLE_STRIP { 0x0005 }
	static TRIANGLE_FAN { 0x0006 }
	static QUADS { 0x0007 }
	static QUAD_STRIP { 0x0008 }
	static POLYGON { 0x0009 }
}

class Sketch {
	setup() {}
	draw() {}
	
	map(x, start, stop, newStart, newStop) { (x-start)/(stop-start) * (newStop-newStart) + newStart }
	normalize(x, start, stop) { (x-start)/(stop-start) }
	
	foreign background_(r, g, b, a)
	background(r, g, b, a) {
		if (!(r is Num) || r < 0 || r > 255 ||
			!(g is Num) || g < 0 || g > 255 ||
			!(b is Num) || b < 0 || b > 255 ||
			!(a is Num) || a < 0 || a > 255) {
			Fiber.abort("background() expects r,g,b,a to be numbers from the range 0..255")
		}
		return background_(r/255, g/255, b/255, a/255)
	}
	background(r, g, b) { background(r, g, b, 255) }
	background(lightness, alpha) { background(lightness, lightness, lightness, alpha) }
	background(lightness) { background(lightness, lightness, lightness, 255) }
	
	foreign begin_(mode)
	begin(mode) {
		if (!(mode is Num) || !mode.isInteger || mode < GL.POINTS || mode > GL.POLYGON) {
			Fiber.abort("begin() expects a valid mode")
		}
		return begin_(mode)
	}
	
	foreign end()
	
	foreign height
	
	foreign size_(width, height)
	size(width, height) {
		if (!(width is Num) || !width.isInteger) {
			Fiber.abort("size() expects width to be an integer")
		} else if (!(height is Num) || !height.isInteger) {
			Fiber.abort("size() expects height to be an integer")
		}
		return size_(width, height)
	}
	
	foreign sleep_(seconds)
	sleep(seconds) {
		if (!(seconds is Num) || seconds < 0) {
			Fiber.abort("sleep() expects a non-negative number")
		}
		return sleep_(seconds)
	}
	
	foreign title_(title)
	title(title) {
		if (!(title is String)) {
			Fiber.abort("title() expects a string")
		}
		return title_(title)
	}
	
	foreign vertex_(x, y, z)
	vertex(x, y, z) {
		if (!(x is Num) || !(y is Num) || !(z is Num)) {
			Fiber.abort("vertex() expects numbers")
		}
		// x = (x - width/2)/(width/2) = 2*(x - width/2)/width = (2*x - width)/width
		x = (2*x - width)/width
		y = -(2*y - height)/height
		return vertex_(x, y, z)
	}
	vertex(x, y) { vertex(x, y, 0) }
	
	foreign width
}
