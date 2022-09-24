import game, assets, const, renderer, textbox, keyboard, operator

print("Hello from python!\n")

game.create_window()

# load resources.
texture0 = assets.load_texture("/images/test.png")
font0 = assets.load_font("/fonts/Roboto-Regular.ttf", 20)
text0 = assets.create_textbox(10, 10, 680, 40)

# setup variables.
renderer.set_background('#5eba7d') # set the clear color (94, 186, 125).
textbox.set_text(text0, font0, "Hello World") # set the text on text0.
x = 0
x_speed = 2
fullscreen = False

# handle the update event.
def update_callback(dt):
	global x_speed, x
	# print("Delta: ", dt)
	if (x_speed > 0 and x + 2 > 390): x_speed = -2
	if (x_speed < 0 and x - 2 < 10):	x_speed = 2
	x += x_speed
	if (keyboard.key_clicked(const.KB_ESCAPE)): game.exit()
	if (keyboard.key_clicked(const.KB_F)):
		global fullscreen
		fullscreen = not fullscreen
		renderer.set_window_mode(fullscreen if 1 else 0)
		print(fullscreen if "window_mode: fullscreen" else "window_mode: window")

#handle the draw event
def draw_callback(dt):
	global x
	renderer.begin()
	renderer.clear()
	renderer.draw_texture(texture0, x, 10, 100, 100)
	# renderer.draw_sub_texture(texture0, x, 10, 100, 100, 0, 0, 20, 20)
	renderer.draw_textbox(text0)
	renderer.present()

# attach events & run game.
game.on("update", "update_callback")
game.on("draw", "draw_callback")
game.run()