# from setuptools import setup
# from Cython.Build import cythonize
# from setuptools.extension import Extension

# ext_modules = [
#     Extension(
#         "swap_heartbeat",
#         sources=["swap_heartbeat.pyx", "packet_sender.c"],
#         libraries=[],
#         include_dirs=["."],
#         language="c",
#     )
# ]

# setup(
#     name="swap_heartbeat",
#     ext_modules=cythonize(ext_modules),
#     version="0.0.1"
# )

from setuptools import setup, Extension
from Cython.Build import cythonize

# Define the Cython extension
extensions = [
    Extension(
        name="swap_heartbeat",
        sources=["swap_heartbeat/swap_heartbeat.pyx", "swap_heartbeat/packet_sender.c"],
        include_dirs=["swap_heartbeat/"],
        extra_compile_args=["-Wall", "-O2"],  # Optimization flags
    )
]

setup(
    name="swap-heartbeat",
    version="0.1.0",
    author="Chris Kitras",
    author_email="chkitras@byu.edu",
    description="This is a simple heartbeat library that sends out a packet at a specified interval with high precision.",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    packages=["swap_heartbeat"],
    ext_modules=cythonize(extensions),
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: C",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.6",
    install_requires=[
        "Cython",
    ],
    zip_safe=False,
)
