#!/usr/bin/env python3

import torch
from torch import nn

# Define model
class NeuralNetwork(nn.Module):
    def __init__(self):
        super().__init__()
        self.cnn = nn.Sequential(
            # Input 28x28x1, after padding 32x32x1, output 28x28x16
            nn.Conv2d(in_channels=1, out_channels=16, kernel_size=5, padding=2),
            nn.ReLU(),
            # Input 28x28x16, output 26x26x32
            nn.Conv2d(in_channels=16, out_channels=32, kernel_size=3),
            nn.ReLU(),
            nn.Dropout(p=0.2),
            # Input 26x26x32, output 13x13x32
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Flatten(),
            nn.Linear(13*13*32, 100),
            nn.ReLU(),
            nn.Dropout(p=0.2),
            nn.Linear(100, 10)
        )

    def forward(self, x):
        logits = self.cnn(x)
        return logits
