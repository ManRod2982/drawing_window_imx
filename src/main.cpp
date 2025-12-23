//
// Copyright (c) Manuel Rodriguez.
// Licensed under the MIT license. See LICENSE file in the project root for
// details.
//

#include <window.h>

// Creates application and runs it
int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

  Window window;

  return app->run(window);
}
