## Drawing Window -- Python

A small interactive application to draw digits and run inference using a
TensorFlow Lite model. This folder contains a compact Python GUI (GTK3)
that lets you freehand-draw a digit, clear the canvas, and run a TFLite
model to predict the drawn digit (designed for MNIST-style models).

Key features
------------
- Lightweight GTK3-based drawing surface to capture strokes.
- Export canvas to a 28x28 grayscale image suitable for MNIST models.
- Thin wrapper around a TFLite interpreter to prepare input and run
  inference.

Environment setup
---------------

```bash
# create + activate a virtual environment (recommended)
python3 -m venv venv
source venv/bin/activate

# install Python dependencies
pip install -r requirements.txt
```

If you cannot use the `requirements.txt`, at minimum you will need:

```bash
pip install pygobject pycairo numpy ai_edge_litert
```

Quick start — run the GUI
------------------------
Simply copy the python files on your target, the entry point for the project is [run.py](run.py)
execute the script and provide it with a model.

Usage:
- `-m/--model`  : path to a .tflite model file (required)
- `-d/--delegate`: path to a delegate .so (optional, platform-specific)
- `-v/--verbose`: enable verbose logging

```bash
./run.py -m pytorch_cnn.tflite
```

> **_NOTE_** Unlike the C++ version the python version uses the latest LiteRT release, therefore the delegate used is different.
to use the tflite delegate change the import under [nn_model.py](nn_model.py).

```bash
./run.py -m cnn_tf_quant_vela.tflite -d /usr/lib/liblitert_ethosu_delegate.so
```

How to use the app
-----------------------------------
- Use the left mouse button (or touchscreen) to draw a digit on the canvas.
- Press the Clear button to erase the canvas and start over.
- Press the Predict button to run the model on the current drawing.
- The app will display the predicted digit and if verbose was selected the confidence
  or output scores.

Where to find models
--------------------
This repository contains example [models](../models) to generate different models using both Pytorch and Tensorflow.

Project structure
-----------------------
- [run.py](run.py) -- CLI entrypoint. Parses args and launches the GTK application.
- [window.py](window.py) -- GTK window and UI wiring. Connects buttons (Clear, Predict)
  to the drawing surface and model wrapper.
- [mouse_drawing.py](mouse_drawing.py) -- Drawing widget (a Gtk.DrawingArea) and helpers to
  export the canvas to a 28x28 NumPy image suitable for MNIST models.
- [nn_model.py](nn_model.py) — Thin wrapper around the TFLite interpreter used by the
  app: prepares input tensors, invokes the interpreter, and returns outputs.

Troubleshooting
---------------
- GTK import errors: install system GTK3 development/runtime packages for
  your distribution (Debian/Ubuntu: `sudo apt install libgtk-3-dev`).
- If imports fail in the virtualenv, ensure it is activated and `pip install -r requirements.txt` completed successfully.
- Interpreter missing: the code references a project-specific interpreter
  implementation. If you see errors, install the interpreter package used
  by this repo (look in `requirements.txt` for the exact package name).

License
-------
This software is available under the MIT license.
