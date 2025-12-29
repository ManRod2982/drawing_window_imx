//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//
// Window definition, this is where the whole layout is defined
#include "window.h"
#include "tensorflow/lite/delegates/external/external_delegate.h"
#include "tensorflow/lite/interpreter_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/optional_debug_tools.h"

#include <iostream>

// Window implementation with a:
// clear_button: used to clear the screen
// predict_button: used to save the screen to an image and get the NN info
// Drawing area
// Everything is enclosed in a Gtk::Grid widget
Window::Window() : clear_button("Clear"), predict_button("Predict") {
  set_title("NMIST example");
  set_border_width(10);

  // Create TFLite interpreter
  // Load model
  const char *model_path = "cnn_quant_vela.tflite";
  std::unique_ptr<tflite::FlatBufferModel> model =
      tflite::FlatBufferModel::BuildFromFile(model_path);
  if (!model) {
    std::cerr << "Failed to load model: " << model_path << std::endl;
    throw std::invalid_argument("Invalid model");
  }

  // Build the interpreter
  // Resolver makes sure the operations in the model are available to the
  // interpreter
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder(*model, resolver)(&interpreter_);
  if (interpreter_ == nullptr) {
    std::cerr << "Failed to build interpreter" << std::endl;
    throw std::invalid_argument("Invalid interpreter");
  }

  // Setup delegate
  // Create external delegate option and pass the QNN Delegate library
  // location so it can be dynamically loaded. Here we assume the delegate is
  // located at the path /data/local/tmp/qnn_delegate/.
  TfLiteExternalDelegateOptions external_delegate_options =
      TfLiteExternalDelegateOptionsDefault("/usr/lib/libethosu_delegate.so");
  // Create the External Delegate. This will load the QNN Delegate.
  TfLiteDelegate *external_delegate =
      TfLiteExternalDelegateCreate(&external_delegate_options);

  // Add External Delegate into TFLite Interpreter to automatically delegate
  // nodes.
  interpreter_->ModifyGraphWithDelegate(external_delegate);

  // Allocate tensors for the model
  if (interpreter_->AllocateTensors() != kTfLiteOk) {
    std::cerr << "Failed to allocate tensors" << std::endl;
    throw std::runtime_error("Unable to allocate tensors");
  }

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
int get_max_index(std::vector<int8_t> vec) {
  auto max_iter = std::max_element(vec.begin(), vec.end());
  return std::distance(vec.begin(), max_iter);
}

std::vector<int8_t> quantize(std::vector<float> vec) {
  std::vector<int8_t> res;
  for (const auto &num : vec) {
    res.push_back((num / 0.00392157) + 128);
  }

  return res;
}

// Calls the save_screen method on the MouseDrawing area
void Window::on_predict_clicked() {
  // Save screen to file
  std::cout << "Predict clicked!" << std::endl;
  auto drawing = mouse_drawing.export_to_vector(28, 28, 255.0);

  // Print debug info
  tflite::PrintInterpreterState(interpreter_.get());
  std::cout << "Tensors size: " << interpreter_->tensors_size() << std::endl;
  std::cout << "Nodes size: " << interpreter_->nodes_size() << std::endl;
  std::cout << "Inputs: " << interpreter_->inputs().size() << std::endl;
  std::cout << "Input 0 name: " << interpreter_->GetInputName(0) << std::endl;
  int t_size = interpreter_->tensors_size();
  for (int i = 0; i < t_size; i++) {
    if (interpreter_->tensor(i)->name)
      std::cout << i << ": " << interpreter_->tensor(i)->name << ", "
                << interpreter_->tensor(i)->bytes << ", "
                << interpreter_->tensor(i)->type << ", "
                << interpreter_->tensor(i)->params.scale << ", "
                << interpreter_->tensor(i)->params.zero_point << std::endl;
  }

  int input_test = interpreter_->inputs()[0];
  std::cout << "input: " << input_test << std::endl;
  const std::vector<int> inputs = interpreter_->inputs();
  const std::vector<int> outputs = interpreter_->outputs();
  std::cout << "number of inputs: " << inputs.size();
  std::cout << "number of outputs: " << outputs.size();

  // Fill input buffer
  std::cout << "Filling input buffer" << std::endl;
  auto quant = quantize(drawing);
  int8_t *input = interpreter_->typed_input_tensor<int8_t>(0);
  std::cout << quant.size() * sizeof(int8_t) << std::endl;
  std::memcpy(input, quant.data(), quant.size() * sizeof(int8_t));

  // Invoke TFLite model
  std::cout << "Invoking model" << std::endl;
  if (interpreter_->Invoke() != kTfLiteOk) {
    std::cerr << "Failed to invoke Interpreter!" << std::endl;
  }

  // Get results
  std::cout << "Get results!" << std::endl;
  int8_t *output = interpreter_->typed_output_tensor<int8_t>(0);
  std::vector<int8_t> output_vec(10);
  std::memcpy(output_vec.data(), output, sizeof(int8_t) * output_vec.size());

  for (size_t i = 0; i < output_vec.size(); i++) {
    std::cout << i << ": " << std::to_string(output_vec[i]) << std::endl;
  }

  int number = get_max_index(output_vec);
  std::string display = "You drew a: " + std::to_string(number);
  std::cout << display << std::endl;
  text_view.set_text(display);
}
