# Always prefer setuptools over distutils
from setuptools import setup, find_packages
# To use a consistent encoding
from codecs import open
from os import path

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, '../README.md'), encoding='utf-8') as f:
    long_description = f.read()

with open(path.join(here, './requirements.txt')) as f:
    requirements = f.read().splitlines()

setup(
    name='ehome-server',
    version='0.0.1',
    description='Python bindings of EHome server for HikVision IP camera.',
    long_description=long_description,
    url='https://github.com/corenel/ip-camera-ehome-server',
    author='Yusu Pan',
    author_email='xxdsox@gmail.com',
    classifiers=[
        'Development Status :: 3 - Beta',
        'Intended Audience :: Information Technology',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
    ],
    keywords='utility camera server',
    py_modules=["ehome_server"],
    package_data={'ehome_server': '_ehome_server.so'},

)
