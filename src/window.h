//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//
// Definition of a window with a grid containing 2 buttons and a drawing area.
#pragma once

#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "mouse_drawing.h"

// Window used to keep all the widgets
// it contains a grid widget that houses
// 2 buttons
// 1 drawing area
// 1 text area
class Window : public Gtk::Window {
 public:
  Window();
  virtual ~Window();

 protected:
  // Signal handlers:
  void on_clear_clicked();
  void on_predict_clicked();

  // Child widgets:
  Gtk::Grid my_grid;
  MouseDrawing mouse_drawing;
  Gtk::Button clear_button, predict_button;
  Gtk::Label text_view;
};
