#!/usr/bin/env python3
import tensorflow as tf
import numpy as np

# 1. Load and Prepare Data
mnist = tf.keras.datasets.mnist
(x_train, y_train), (x_test, y_test) = mnist.load_data()

# Normalize and RESHAPE to (28, 28, 1)
x_test = x_test.astype(np.float32) / 255.0
x_test = np.expand_dims(x_test, axis=-1)

# 2. Load Model
model = tf.keras.models.load_model('cnn_tf.keras')

# 3. Setup Converter
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]

# 4. Representative Dataset Generator
def representative_data_gen():
    # Use 100 samples for calibration
    for input_value in tf.data.Dataset.from_tensor_slices(x_test).batch(1).take(100):
        yield [input_value]

converter.representative_dataset = representative_data_gen

# 5. Enforce Full Integer Quantization
# This ensures that all operations are quantized to int8
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8

# 6. Convert and Save
tflite_model = converter.convert()
with open('cnn_tf_quant.tflite', 'wb') as f:
    f.write(tflite_model)

print("Quantized model saved successfully!")
