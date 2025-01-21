from setuptools import setup, Extension
from Cython.Build import cythonize

# Define the Cython extension
extensions = [
    Extension(
        name="swap_heartbeat.swap_heartbeat",
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
    package_dir={"": "."},
    ext_modules=cythonize(extensions),
    python_requires=">=3.6",
    install_requires=[
        "Cython",
    ],
    zip_safe=False,
    include_package_data=True
)
