import "quillz" for GL, Sketch
import "random" for Random

#!sketch
class Main is Sketch {
	construct preinit() {
		System.print("Initializing counter to 1")
		_cnt = 1
		_rng = Random.new()
	}
	
	setup() {
		System.print("setup() called")
		size(640, 480)
	}
	
	draw() {
		sleep(2)
		title("Frame #%(_cnt) | %(year())-%(month())-%(day()) %(hour()):%(minute()):%(second()), weekday=%(weekDay())")
		var r = _rng.float(0, 255)
		var g = _rng.float(0, 255)
		var b = _rng.float(0, 255)
		System.print("New background: %(r),%(g),%(b),255")
		background(r, g, b, 255)
		begin(GL.POLYGON)
		vertex(width/4, height/4)
		vertex(3*width/4, height/4)
		vertex(3*width/4, 3*height/4)
		vertex(width/4, 3*height/4)
		end()
		_cnt = _cnt + 1
	}
}
