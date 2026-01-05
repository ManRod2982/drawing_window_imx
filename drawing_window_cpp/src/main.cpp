//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//

#include <cstring>  // Required for strcmp
#include <iostream>

#include "nn_model.h"
#include "window.h"

// Creates application and runs it
int main(int argc, char* argv[]) {

  bool verbose = false;
  const char* delegate_path = nullptr;
  const char* model_path = nullptr;

  // Require model
  if (argc == 1) {
    std::cerr << "Error: Requires -m model_path!" << std::endl;
    return 1;
  }

  // Parse command line arguments
  for (int i = 1; i < argc; ++i) {
    // Checking for verbosity flag
    if (std::strcmp(argv[i], "-v") == 0 ||
        std::strcmp(argv[i], "--verbose") == 0) {
      std::cout << "Verbose mode enabled." << std::endl;
      verbose = true;
    }
    // Example of reading a value for an option
    else if (std::strcmp(argv[i], "-m") == 0 ||
             std::strcmp(argv[i], "--model") == 0) {
      if (i + 1 < argc) {  // Check if the next argument exists
        model_path = argv[i + 1];
        std::cout << "Model file: " << model_path << std::endl;
        i++;  // Skip the next argument as it's the value
      } else {
        std::cerr << "Error: --model requires a filename." << std::endl;
        return 1;  // Exit with an error code
      }
    }
    // Example of reading a value for an option
    else if (std::strcmp(argv[i], "-d") == 0 ||
             std::strcmp(argv[i], "--delegate") == 0) {
      if (i + 1 < argc) {  // Check if the next argument exists
        delegate_path = argv[i + 1];
        std::cout << "Delegate path: " << delegate_path << std::endl;
        i++;  // Skip the next argument as it's the value
      } else {
        std::cerr << "Error: --delegate requires a filename." << std::endl;
        return 1;  // Exit with an error code
      }
    }
    // Handle other arguments or positional arguments
    else {
      std::cout << "Unknown option: " << argv[i] << "!" << std::endl;
      std::cout << "Requires: -m model_path " << std::endl;
      std::cout << "Optional:\n -d delegate_path -d\n verbose mode -v"
                << std::endl;
    }
  }

  // Create model with parsed parameters
  NnModel nn(model_path, delegate_path, verbose);

  // Create execution window
  char* program_name_only[] = {argv[0], nullptr};
  int modified_argc = 1;
  auto app = Gtk::Application::create("org.gtkmm.examples.base");
  Window window(nn);
  return app->run(window, modified_argc, program_name_only);
}
