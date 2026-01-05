#!/usr/bin/env python3
"""Window with Grid containing MouseDrawing, Clear and Predict buttons and a
text label. Mirrors the C++ `Window` behaviour.
"""
import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk  # noqa: E402  (require_version must run before importing Gtk)
import numpy as np  # noqa: E402
from mouse_drawing import MouseDrawing  # noqa: E402


class Window(Gtk.Window):
    def __init__(self, nn_model):
        super().__init__(title="MNIST example")
        # Allow creating the window without a model (user commented model out)
        self.nn = nn_model
        self.set_border_width(10)

        self.my_grid = Gtk.Grid()
        self.my_grid.set_column_homogeneous(True)
        self.add(self.my_grid)

        self.mouse_drawing = MouseDrawing()
        self.my_grid.attach(self.mouse_drawing, 0, 0, 2, 1)

        self.clear_button = Gtk.Button(label="Clear")
        self.clear_button.connect("clicked", self.on_clear_clicked)
        self.my_grid.attach_next_to(self.clear_button, self.mouse_drawing, Gtk.PositionType.BOTTOM, 1, 1)

        self.predict_button = Gtk.Button(label="Predict")
        self.predict_button.connect("clicked", self.on_predict_clicked)
        self.my_grid.attach_next_to(self.predict_button, self.clear_button, Gtk.PositionType.RIGHT, 1, 1)

        self.text_view = Gtk.Label(label="You drew: ")
        self.my_grid.attach_next_to(self.text_view, self.clear_button, Gtk.PositionType.BOTTOM, 2, 1)

        self.show_all()

    def on_clear_clicked(self, button):
        self.mouse_drawing.clear_screen()
        print("Clear clicked!")

    def on_predict_clicked(self, button):
        print("Predict clicked!")

        # Query model input details and prepare the input with correct shape/dtype
        input_details = self.nn.interpreter.get_input_details()[0]
        input_shape = tuple(input_details['shape'])

        # Get screen
        model_input = self.mouse_drawing.export_to_vector(
            self.nn.get_dtype(),
            w=28,
            h=28,
            scale=255.0,
            target_shape=input_shape,
        )

        out = self.nn.infer(model_input)
        if len(out) == 0:
            display = "No output"
        else:
            idx = int(np.argmax(np.array(out)))
            display = f"You drew a: {idx}"

        print(display)
        self.text_view.set_text(display)
