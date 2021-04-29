import "quillz" for Sketch
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
	}
	
	draw() {
		sleep(2)
		title("Quillz sketch, counter is %(_cnt)")
		var r = _rng.float(0, 256)
		var g = _rng.float(0, 256)
		var b = _rng.float(0, 256)
		System.print("New background: %(r),%(g),%(b),255")
		background(r, g, b, 255)
		_cnt = _cnt + 1
	}
}
