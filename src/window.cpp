//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//
// Window definition, this is where the whole layout is defined
#include "window.h"

#include <iostream>

// Window implementation with a:
// clear_button: used to clear the screen
// predict_button: used to save the screen to an image and get the NN info
// Drawing area
// Everything is enclosed in a Gtk::Grid widget
Window::Window()
    : clear_button("Clear"), predict_button("Predict") {
  set_title("NMIST example");
  set_border_width(10);

  // Adds grid to the window and sets the same width to all the columns
  add(my_grid);
  my_grid.set_column_homogeneous(true);

  // Add MouseDrawing to the grid at column 0, row 0, width of 2 and height of 1
  my_grid.attach(mouse_drawing, 0, 0, 2, 1);

  // Connect the clear_button method
  clear_button.signal_clicked().connect(
      sigc::mem_fun(*this, &Window::on_clear_clicked));
  // Add clear_button below to the mouse_drawing with a width of 1 and height of
  // 1
  my_grid.attach_next_to(clear_button, mouse_drawing, Gtk::POS_BOTTOM, 1, 1);

  // Connect the predict button method
  predict_button.signal_clicked().connect(
      sigc::mem_fun(*this, &Window::on_predict_clicked));
  // Add the predict button next to the clear button with a width of 1 and a
  // height of 1
  my_grid.attach_next_to(predict_button, clear_button, Gtk::POS_RIGHT, 1, 1);

  my_grid.attach_next_to(text_view, clear_button, Gtk::POS_BOTTOM, 2, 1);
  text_view.set_text("You drew: ");

  // Show everything on the window
  show_all_children();
}

Window::~Window() {}

// Calls the clear_screen method on the MouseDrawing area
void Window::on_clear_clicked() {
  // Clear screen
  mouse_drawing.clear_screen();
  std::cout << "Clear clicked!" << std::endl;
}

// Helper function used to get the index of the
// maximum value in a Vector
int get_max_index(std::vector<double> vec) {
  auto max_iter = std::max_element(vec.begin(), vec.end());
  return std::distance(vec.begin(), max_iter);
}

// Calls the save_screen method on the MouseDrawing area
void Window::on_predict_clicked() {
  // Save screen to file
  std::cout << "Predict clicked!" << std::endl;
  auto drawing = mouse_drawing.export_to_vector(28, 28, 255.0);
  //int number = get_max_index(result);
  //std::string display = "You drew a: " + std::to_string(number);
  //std::cout << display << std::endl;
  //text_view.set_text(display);
}
