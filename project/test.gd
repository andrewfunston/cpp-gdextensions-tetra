extends Node2D

var accumulator = 0.0
var removed = false

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	
func _input(event):
	if event.is_action_pressed("ui_cancel"):
		get_tree().quit()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	#accumulator+= delta
	#if(accumulator > 5.0 && !removed) :
		#removed = true
		#remove_child($TetraDisplayBlock)
