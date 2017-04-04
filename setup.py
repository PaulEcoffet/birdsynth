from distutils.core import setup, Extension
import numpy as np
from Cython.Build import cythonize


ext_modules = [Extension('birdsynth', ['birdsynth/birdsynth.pyx',
                                       'birdsynth/finch.c',
                                       'birdsynth/rk4.c'])]

setup(
    name='birdsynth',
    version='1.1',
    include_dirs=[np.get_include()],  # Add Include path of numpy
    ext_modules=cythonize(ext_modules),
)
