
class Sketch {
	//colorMode { _colorMode }
	//colorMode=(cm) { _colorMode = cm }
	
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
}
