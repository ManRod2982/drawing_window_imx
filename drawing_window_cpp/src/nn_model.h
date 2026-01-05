//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//
// Definition of an NnModel, a wrapper for TFlite that initializes the model and
// performs inference
#pragma once

#include <iostream>

#include "tensorflow/lite/interpreter.h"

// NnModel used to initialize the TFlite model and delegate
// it also performs the inference
class NnModel {
 public:
  /// NnModel Ctor
  /// Creates an instance of a Neural Network model based on the tflite
  /// model provided by model_path and uses the delegate if any is passed
  /// The verbose flag can be useful in debugging since it prints more details
  NnModel(const char *model_path, const char *delegate_path, bool verbose);

  // Method used to get the data type required by the model
  TfLiteType get_dtype() { return interpreter_->input_tensor(0)->type; };

  /// Method used to call the inference on the model
  /// It can be used for float, int8_t or uint8_t type depending on the model
  /// it fills the input tensors, calls inference and returns the output vector
  template <typename T>
  std::vector<T> infer(const std::vector<T> &input);

 private:
  // TFlite interpreter
  std::unique_ptr<tflite::Interpreter> interpreter_;
  // Verbosity flag
  bool verbose_;
};

template <typename T>
std::vector<T> NnModel::infer(const std::vector<T> &input) {
  // Fill input buffer
  if (verbose_) std::cout << "Filling input buffer" << std::endl;
  T *input_tensor = interpreter_->typed_input_tensor<T>(0);
  std::memcpy(input_tensor, input.data(), input.size() * sizeof(T));

  // Invoke TFLite model
  if (verbose_) std::cout << "Invoking model" << std::endl;
  if (interpreter_->Invoke() != kTfLiteOk) {
    std::cerr << "Failed to invoke Interpreter!" << std::endl;
    return {};
  }

  // Get results
  if (verbose_) std::cout << "Get results!" << std::endl;
  auto tensor_size_bytes = interpreter_->output_tensor(0)->bytes;
  T *output_tensor = interpreter_->typed_output_tensor<T>(0);
  size_t element_count = tensor_size_bytes / sizeof(T);
  std::vector<T> output_vec(output_tensor, output_tensor + element_count);

  if (verbose_) {
    for (size_t i = 0; i < output_vec.size(); i++) {
      std::cout << std::to_string(i) << ": " << std::to_string(output_vec[i])
                << std::endl;
    }
  }

  return output_vec;
}
