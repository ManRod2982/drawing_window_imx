#!/usr/bin/env python3
"""Mouse drawing widget: provides a drawing area that records mouse strokes,
can clear, save to PNG and export a scaled grayscale vector suitable for
MNIST-like models.
"""
import gi
import cairo
import numpy as np

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gdk  # noqa: E402  (require_version must run before importing Gtk)


class MouseDrawing(Gtk.DrawingArea):
    def __init__(self, width=250, height=250, brush_size=10.0):
        super().__init__()
        self.drawing_area_w = width
        self.drawing_area_h = height
        self.brush_size = brush_size
        self.set_size_request(self.drawing_area_w, self.drawing_area_h)

        # Create surface that stores the drawing history
        self.surface = cairo.ImageSurface(cairo.FORMAT_ARGB32,
                                          self.drawing_area_w,
                                          self.drawing_area_h)

        # State
        self.state = "clear"  # "clear" or "draw"
        self.left_clicked = False
        self.points = []  # list of (x, y)

        # Enable events
        self.add_events(Gdk.EventMask.BUTTON_PRESS_MASK |
                        Gdk.EventMask.BUTTON_RELEASE_MASK |
                        Gdk.EventMask.POINTER_MOTION_MASK)

        # Connect signals
        self.connect("draw", self.on_draw)
        self.connect("button-press-event", self.on_button_press_event)
        self.connect("button-release-event", self.on_button_release_event)
        self.connect("motion-notify-event", self.on_motion_notify_event)

    # Public API
    def clear_screen(self):
        self.state = "clear"
        self.points.clear()
        self.queue_draw()

    def save_screen(self, filename="image.png"):
        self.surface.write_to_png(filename)

    def export_to_vector(self, dtype, w=28, h=28, scale=255.0, target_shape=None):
        """Return a flat NumPy array of grayscale pixels scaled to (w,h).

        dtype: "float32" -> np.float32 values in ~[0.0, 1.0]
               "int8"  -> np.int8 values in [-128..127]
               "uint8" -> np.uint8 values in [0..255]
        """
        # Create scaled surface
        scaled_surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, w, h)
        ctx = cairo.Context(scaled_surface)

        scale_x = float(w) / self.drawing_area_w
        scale_y = float(h) / self.drawing_area_h
        ctx.scale(scale_x, scale_y)

        # Paint original surface into scaled context
        pattern = cairo.SurfacePattern(self.surface)
        ctx.set_source(pattern)
        ctx.paint()

        data = scaled_surface.get_data()
        stride = scaled_surface.get_stride()

        if dtype == "float32":
            out = np.empty(w * h, dtype=np.float32)
        elif dtype == "int8":
            out = np.empty(w * h, dtype=np.int8)
        elif dtype == "uint8":
            out = np.empty(w * h, dtype=np.uint8)
        else:
            # default to uint8
            print(f"Unkwon dtype {dtype}, selecting numpy.uint8")
            out = np.empty(w * h, dtype=np.uint8)

        idx = 0
        for y in range(h):
            row_off = y * stride
            for x in range(w):
                offset = row_off + x * 4
                b = data[offset]
                g = data[offset + 1]
                r = data[offset + 2]
                # Convert to grayscale using luminance
                gray = int(0.2126 * r + 0.7152 * g + 0.0722 * b)
                if dtype == "float":
                    out[idx] = float(gray / scale)
                elif dtype == "int8":
                    out[idx] = np.int8(gray - 128)
                else:  # uint8 or default
                    out[idx] = np.uint8(gray)
                idx += 1

        # If caller requests a specific target_shape (e.g. model input shape)
        # validate product and reshape accordingly. target_shape should be a
        # tuple compatible with NumPy reshape, for example (1,28,28,1).
        if target_shape is not None:
            # ensure it's a tuple
            if not isinstance(target_shape, (tuple, list)):
                raise TypeError("target_shape must be a tuple or list")
            if int(np.prod(target_shape)) != out.size:
                raise ValueError(
                    f"target_shape product {int(np.prod(target_shape))} does not match image size {out.size}"
                )
            return out.reshape(tuple(target_shape))

        return out

    # Event handlers
    def on_draw(self, widget, cr):
        # Paint stored surface
        cr.set_source_surface(self.surface, 0, 0)
        cr.paint()

        if self.state == "clear":
            cr.set_source_rgb(0, 0, 0)
            cr.paint()
            self.state = "draw"

        if self.state == "draw":
            cr.save()
            cr.set_line_width(0)
            cr.set_source_rgb(1, 1, 1)
            for (x, y) in self.points:
                cr.arc(x, y, self.brush_size, 0, 2 * 3.14159)
                cr.fill()
            cr.restore()

        # Save current cr into the surface so next draw will restore it
        # We must copy the widget's target into our stored surface using
        # the widget allocation offset (negative) — this mirrors the C++ code
        # which avoids shrinking/offset issues when GTK provides fresh contexts.
        allocation = self.get_allocation()
        t_ctx = cairo.Context(self.surface)
        # cr.get_target() is the widget surface; offset it so the stored
        # surface receives the correct region
        t_ctx.set_source_surface(cr.get_target(), -allocation.x, -allocation.y)
        t_ctx.paint()

        return False

    def on_button_press_event(self, widget, event):
        if event.button == 1:
            self.left_clicked = True
            self.state = "draw"
            self.points.append((event.x, event.y))
            return True
        return False

    def on_button_release_event(self, widget, event):
        if event.button == 1:
            self.left_clicked = False
            return True
        return False

    def on_motion_notify_event(self, widget, event):
        if self.left_clicked:
            self.points.append((event.x, event.y))
            self.queue_draw()
            return True
        return False
