//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//

#include <window.h>

#include <simple_nn.hh>

// Creates application and runs it
int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

  // Create Neural Network from pre-trained file
  SimpleNN nn("training_30_3-0_10_deep");
  Window window(nn);

  return app->run(window);
}
