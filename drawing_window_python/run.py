#!/usr/bin/env python3
"""Runner script for the Python MNIST drawing application.

Usage: python run.py -m model.tflite [-d delegate.so] [-v]
"""
import argparse
import sys
import gi
from nn_model import NnModel

gi.require_version("Gtk", "3.0")

from gi.repository import Gtk  # noqa: E402  (require_version must run before importing Gtk)
from window import Window  # noqa: E402  (Window imports Gtk; keep ordering)


def main(argv=None):
    parser = argparse.ArgumentParser(description="Python MNIST drawing app")
    parser.add_argument("-m", "--model", required=True, help="Path to .tflite model file")
    parser.add_argument("-d", "--delegate", default=None, help="Optional delegate library path")
    parser.add_argument("-v", "--verbose", default=False, action="store_true", help="Verbose output")
    args = parser.parse_args(argv)

    nn = NnModel(args.model, delegate_path=args.delegate, verbose=args.verbose)

    app = Gtk.Application.new(None, 0)
    win = Window(nn)
    win.set_default_size(300, 350)
    app.connect("activate", lambda a: a.add_window(win))

    # Run the GTK app
    return app.run(None)


if __name__ == "__main__":
    sys.exit(main())
