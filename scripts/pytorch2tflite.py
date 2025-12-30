#!/usr/bin/env python3

import torch
import numpy
import ai_edge_torch
from pytorch_model import NeuralNetwork

# Load pretrained model
model = NeuralNetwork()
model.load_state_dict(torch.load("pytorch_model.pth", weights_only=True))
print(model)
# Set model to evaluation mode
model.eval()

# Export the model to tflite
sample = (torch.rand(1, 1, 28, 28),)
normal_output = model(*sample)
edge_model = ai_edge_torch.convert(model.eval(), sample)
edge_output = edge_model(*sample)

# validate model
if (numpy.allclose(
    normal_output.detach().numpy(),
    edge_output,
    atol=1e-5,
    rtol=1e-5,
)):
    print("Inference result with Pytorch and TfLite was within tolerance")
else:
    print("Something wrong with Pytorch --> TfLite")

edge_model.export('pytorch_cnn.tflite')
