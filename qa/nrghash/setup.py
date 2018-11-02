
from setuptools import setup, Extension
import os

this_dir = os.path.realpath(os.path.join(__file__, '..'))
src_dir = os.path.realpath(os.path.join(this_dir,'..','..','src'))

nrghash = Extension(
    'nrghash',
    define_macros = [
        ('__STDC_WANT_LIB_EXT1__',1),
        ('USE_SECURE_MEMZERO',1),
    ],
    include_dirs = [
        this_dir,
        src_dir,
    ],
    sources = [
        'nrghashmodule.cpp',
        os.path.join(src_dir, 'crypto', 'egihash.cpp'),
        os.path.join(src_dir, 'crypto', 'keccak-tiny.c'),
    ],
    extra_compile_args=['-std=c++11'])

setup(
    name = 'nrghash',
    version = '1.0',
    description = 'Energi Hash function',
    ext_modules = [nrghash])
