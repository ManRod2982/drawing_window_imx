#!/usr/bin/env python3
"""Simple wrapper to LiteRT model, tflite can be used as well by just changing the import.

Methods:
- get_dtype(): returns input dtype ("float32"/"int8"/"uint8")
- infer(input_list): runs inference and returns output
"""
from ai_edge_litert.interpreter import Interpreter
import numpy as np


class NnModel:
    def __init__(self, model_path: str, delegate_path: str = None, verbose: bool = False):
        if Interpreter is None:
            raise ImportError("No Interpreter available.")

        self.verbose = verbose
        self.model_path = model_path

        # Create interpreter
        if delegate_path is not None:
            # attempt to load external delegate if provided (platform specific)
            try:
                from ai_edge_litert.interpreter import load_delegate

                delegate = load_delegate(delegate_path)
                self.interpreter = Interpreter(model_path=model_path, experimental_delegates=[delegate])
            except Exception as e:
                raise RuntimeError(f"Failed to load delegate: {e}")
        else:
            self.interpreter = Interpreter(model_path=model_path)

        self.interpreter.allocate_tensors()

        if self.verbose:
            print(f"Loaded model: {model_path})")
            print("Input details:", self.interpreter.get_input_details())
            print("Output details:", self.interpreter.get_output_details())

    def get_dtype(self) -> str:
        details = self.interpreter.get_input_details()[0]
        # details['dtype'] is a numpy dtype
        return str(np.dtype(details.get('dtype')).name)

    def infer(self, input_data: np.ndarray, input_dtype: str = None) -> np.ndarray:
        """Run inference.

        This minimal example requires the caller to provide a NumPy ndarray as
        input.

        Returns a NumPy ndarray with the model output.
        """
        # Set tensor and invoke
        input_details = self.interpreter.get_input_details()[0]
        self.interpreter.set_tensor(input_details['index'], input_data)
        self.interpreter.invoke()

        out_details = self.interpreter.get_output_details()[0]
        output_data = self.interpreter.get_tensor(out_details['index'])

        if self.verbose:
            print(f"Output: {output_data}")

        # Return flattened NumPy array (fast for callers that use numpy)
        return output_data
