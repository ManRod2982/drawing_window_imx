//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//
// NnModel definition
#include "nn_model.h"

#include <iostream>

#include "tensorflow/lite/delegates/external/external_delegate.h"
#include "tensorflow/lite/interpreter_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/optional_debug_tools.h"

NnModel::NnModel(const char *model_path, const char *delegate_path,
                 bool verbose)
    : verbose_(verbose) {
  // Create TFLite interpreter
  // Load model
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

  // Setup delegate if required
  if (delegate_path != nullptr) {
    // Create external delegate option and pass the delegate library
    TfLiteExternalDelegateOptions external_delegate_options =
        TfLiteExternalDelegateOptionsDefault(delegate_path);
    // Create the External Delegate. This will load the delegate.
    TfLiteDelegate *external_delegate =
        TfLiteExternalDelegateCreate(&external_delegate_options);

    // Add External Delegate into TFLite Interpreter to automatically delegate
    // nodes.
    if (interpreter_->ModifyGraphWithDelegate(external_delegate) != kTfLiteOk) {
      std::cerr << "Failed to add delegate" << std::endl;
      throw std::runtime_error("Unable to add delegate");
    }
  }

  // Allocate tensors for the model
  if (interpreter_->AllocateTensors() != kTfLiteOk) {
    std::cerr << "Failed to allocate tensors" << std::endl;
    throw std::runtime_error("Unable to allocate tensors");
  }

  if(verbose_)
  {
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

  }
}
