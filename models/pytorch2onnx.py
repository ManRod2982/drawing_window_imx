#!/usr/bin/env python3

import torch
from pytorch_model import NeuralNetwork

# Load pretrained model
model = NeuralNetwork()
model.load_state_dict(torch.load("pytorch_model.pth", weights_only=True))
print(model)
# Set model to Evaluation mode
model.eval()

# Export the model to ONNX
sample = (torch.rand(1, 1, 28, 28),)
onnx_model = torch.onnx.export(model, sample, dynamo=True)
onnx_model.save("pytorch_cnn.onnx")
