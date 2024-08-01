extends Node2D

var accumulator = 0.0
var removed = false

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	accumulator+= delta
	if(accumulator > 7.0 && !removed) :
		removed = true
		remove_child($Node2D)
