The following folder contains several example scripts to train models on pytorch
and tensorflow frameworks and conver them to tensorflow lite format for inference
on an embedded target.

# Enviroment setup

Python 3 must be available on the device and to setup the enviroment one can simply do:

```
python3 -m venv myenv
source myenv/bin/activate
pip install -r requirements.txt
```

This will install all the packages required to run all the scripts in this sub folder.

# Scripts overview

| Script | Overview |
| ------ | -------- |
| train_pytorch.py   | Script used to train the pytorch model and save a .pth file with the weights and biases of the model. |
| pytorch_model.py   | Pytorch model description, since pytorch models do not save the graph structure the model is saved in a separate file. That allows us to load the model in different scripts without repeating it. |
| pytorch2onnx.py    | Script used to convert the pre-trained Pytorch model to onnx format. |
| pytorch2tflite.py  | Script used to convert the pre-trained Pytorch model to Tensorflow lite format. |
| train_tf.py        | Script used to define and train a model using Tensorflow/keras, after training it saves the model in keras format. |
| tf2quant_tflite.py | Script used to load a pre-trained keras model, quantize it and convert it to tensorflow lite format. |
