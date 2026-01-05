# Drawing window

This is a simple example application that allows to write a digit in black and white and use a pre-trained Tensorflow lite model to
identify the digit.

![alt text](imgs/window_example.png "Mnist window")


Two implementations are provided:
- C++ implementation under [drawing_window_cpp](drawing_window_cpp/)
- Python implementation under [drawing_window_python](drawing_window_python/)

Instructions on using each implementation are provided under each folder.

Model examples and examples on converting them to Tensorflow lite format are provided in the [models](models/) subfolder.

> **_NOTE_** No pre-processing is performed on the image before being passed into the model, so if the drawing is not center or small
the model will likely fail, also note the MNIST training data is slightly different from what we provide the model. However it
does a pretty good job generalizing.

# License

This software is available under the MIT license.
