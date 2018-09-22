# viz.ml
HackCMU 2018: a custom gpu-based interactive dimensionality reduction visualization engine in c++ for introspection of high dimensional big data sets from machine learning
## Inspiration

Unfortunately, most machine learning algorithms manipulate training data very opaquely - it's hard to tell how, if at all, the algorithm finds features in the data. Because of this, we thought it would be valuable to provide an interactive tool for visualizing spacial patterns in data sets that are represented as high-dimensional vectors. Algorithms for this purpose are typically expensive, requiring the user to wait minutes to view a static image - here, we provide it interactively.

[Download](https://github.com/TheNumbat/viz.ml/releases/download/0.1/viz.ml.zip)

## What it does

Our tool takes MNIST-style data sets (MNIST itself, as well as FashionMNIST) and visualizes the data points projected into three dimensions via a choice of algorithms, the simplest being a choice of three axes of the high-dimensional data to map to x/y/z. 

More interestingly, we also implemented [t-SNE](http://jmlr.csail.mit.edu/papers/volume9/vandermaaten08a/vandermaaten08a.pdf), an algorithm that attempts to preserve neighborhood topology for each data point. Visualizing the results allows the user to get a sense of how the algorithm groups different types of data points, for example, that wide zeros are mapped farther from the origin than thin ones.

![MNIST Axis](https://i.imgur.com/qUwfvnH.png)
![FahsionMNSIT t-SNE](https://i.imgur.com/HJ9zsTx.png)

## How we built it

The tool is built in C++ on a custom framework using OpenGL, SDL2, and Dear ImGui. Graphics features include an instanced scene, interactive interface, antialiasing, and object picking. Data is loaded from the [MNIST](http://yann.lecun.com/exdb/mnist/) and [FashionMNIST](https://github.com/zalandoresearch/fashion-mnist) sets.
