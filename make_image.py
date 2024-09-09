import numpy as np
import matplotlib.pyplot as plt

image_before = np.loadtxt("input_example_image.txt")
plt.imshow(-np.resize(image_before, (720,1280)), cmap = 'Greys')
plt.show()


image_after = np.loadtxt("scalar_output_image.txt")
plt.imshow(-np.resize(image_after, (720,1280)), cmap = 'Greys')
plt.show()