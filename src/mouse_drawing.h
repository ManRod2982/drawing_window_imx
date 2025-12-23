//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//
// Definition of MouseDrawing, a modified Gtk::DrawingArea used to handle mouse
// events and draw when the left mouse button is clicked.
#pragma once

#include <gtkmm/drawingarea.h>

#include <vector>

// MouseDrawing Definition
// It handles mouse events and defines a default drawing area
// it also keeps state betwen different on_draw calls
class MouseDrawing : public Gtk::DrawingArea {
public:
  MouseDrawing();
  // Used to clear the screen
  void clear_screen(void);
  // Used to generate an image file from the current screen
  void save_screen(void);
  // Exports screen to a grayscale vector
  // takes the width and height to be scaled to
  std::vector<double> export_to_vector(int w, int h, double scale);

  virtual ~MouseDrawing();
  // State definitions to decide how to handle on_draw calls
  enum class DrawState {
    clear, // Clears the screen
    draw   // Draws the points currently stored on the screen
  };

protected:
  // Draw handler
  virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr);
  // Handles mouse click events
  bool on_button_press_event(GdkEventButton *event);
  // Handles when the click is released
  bool on_button_release_event(GdkEventButton *event);
  // Handles whenever the mouse moves over the area
  bool on_motion_notify_event(GdkEventMotion *event);

private:
  // Default width of the drawing area
  int drawing_area_w = 250;
  // Default height of the drawing area
  int drawing_area_h = 250;
  // Default brush size used to draw the circles with the mouse
  double brush_size = 10.0;
  // On the first call we clear the screen
  DrawState state = DrawState::clear;
  // Used to signal when the left mouse button is pressed
  bool left_clicked = false;
  // Used to store the cairo context between on_draw calls
  Cairo::RefPtr<Cairo::Surface> surface;
  // Used to store mouse activity data
  struct Point {
    double x, y;
  };
  // Stores all the mouse coordinates while the left mouse is clicked
  std::vector<Point> points;
};
