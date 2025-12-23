//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//
// Implementation of MouseDrawing class
#include "mouse_drawing.h"

#include <gdk/gdkkeysyms.h>

#include <iostream>

// MouseDrawing ctor sets the drawing area default width and height
// a surface to store the drawings and enables the handling of mouse
// events.
MouseDrawing::MouseDrawing() {
  set_size_request(drawing_area_w, drawing_area_h);
  this->surface = Cairo::ImageSurface::create(Cairo::Format::FORMAT_ARGB32,
                                              drawing_area_w, drawing_area_h);

  // Enable the events you wish to receive
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
             Gdk::POINTER_MOTION_MASK);
}

MouseDrawing::~MouseDrawing() {}

// Method used to clear the screen, it empties the vector
// used to store the mouse coordinates after a click and
// triggers a redraw while chaning the state to "clear"
void MouseDrawing::clear_screen() {
  this->state = DrawState::clear;
  points.clear();
  queue_draw();
}

// Saves the current surface to a png file called "image"
void MouseDrawing::save_screen() { this->surface->write_to_png("image.png"); }

std::vector<double> MouseDrawing::export_to_vector(int w, int h, double scale) {
  // Create new Cairo surface for the scaled surface
  Cairo::RefPtr<Cairo::ImageSurface> scaled_surface =
      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, w, h);
  Cairo::RefPtr<Cairo::Context> scaled_context =
      Cairo::Context::create(scaled_surface);

  double scale_x = (double)w / drawing_area_w;
  double scale_y = (double)h / drawing_area_h;
  scaled_context->scale(scale_x, scale_y);

  // 3. Draw the original surface onto the scaled context of the new surface
  Cairo::RefPtr<Cairo::SurfacePattern> pattern =
      Cairo::SurfacePattern::create(this->surface);
  scaled_context->set_source(pattern);
  scaled_context->paint();

  // 4. Extract pixel data and convert to grayscale vector
  uint8_t *data = scaled_surface->get_data();
  int stride = scaled_surface->get_stride();
  std::vector<double> grayscale_pixels(w * h);

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      // Cairo typically uses CAIRO_FORMAT_ARGB32, which is BGRX in memory on
      // little-endian systems
      unsigned char *pixel = data + y * stride + x * 4;
      unsigned char blue = pixel[0];
      unsigned char green = pixel[1];
      unsigned char red = pixel[2];

      // Grayscale conversion using luminance formula (approx)
      unsigned char gray = static_cast<unsigned char>(
          0.2126 * red + 0.7152 * green + 0.0722 * blue);
      grayscale_pixels[y * w + x] = gray / scale;
    }
  }

  return grayscale_pixels;
}

// Main function where the drawing is handled
// a state variable is used to decide when to draw and
// when to clear the screen.
// GTK provides and empty cairo context on each on_draw call
// therefore we store the context on a surface and restore the
// context every time we are called.
bool MouseDrawing::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  auto allocation = this->get_allocation();
  // Restore cairo context
  // each time on_draw is called a fresh context is created
  // this way our surface keeps the history
  cr->save();
  cr->set_source(this->surface, 0.0, 0.0);
  cr->restore();
  cr->paint();

  // Check the state to determine our action
  switch (this->state) {
  case DrawState::clear:
    // Set black background
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->paint();
    this->state = DrawState::draw;
    break;

  case DrawState::draw:
    cr->save();
    cr->set_line_width(0);
    cr->set_source_rgb(1.0, 1.0, 1.0);
    // Draw circles where the mouse has moved
    for (const auto &point : points) {
      cr->arc(point.x, point.y, brush_size, 0, 2 * M_PI);
      cr->fill();
    }
    cr->restore();
    break;
  default:
    break;
  }

  // Save surface so that we can restore the context on the next call
  Cairo::RefPtr<Cairo::Context> t_context =
      Cairo::Context::create(this->surface);
  t_context->set_source(cr->get_target(), -allocation.get_x(),
                        -allocation.get_y());
  t_context->paint();

  return true;
}

// Checks for left mouse clicks and starts logging the mouse
// coordinates on our points vector to be drawn later
bool MouseDrawing::on_button_press_event(GdkEventButton *event) {
  if (event->button == 1) { // Left mouse button
    this->left_clicked = true;
    this->state = DrawState::draw;
    points.push_back({event->x, event->y});
    std::cout << event->x << " " << event->y << std::endl;
    return true; // Event handled
  }
  return false;
}

// Checks when the click is released to stop drawing.
bool MouseDrawing::on_button_release_event(GdkEventButton *event) {
  if (event->button == 1) {
    this->left_clicked = false;
    return true; // Event handled
  }
  return false;
}

// Logs new coordinates as we move with the button pressed down.
// if movement is detected but the left button was released do nothing.
bool MouseDrawing::on_motion_notify_event(GdkEventMotion *event) {
  if (this->left_clicked) {
    points.push_back({event->x, event->y});
    queue_draw(); // Request a redraw
    std::cout << event->x << " " << event->y << std::endl;
    return true; // Event handled
  }
  return false;
}
