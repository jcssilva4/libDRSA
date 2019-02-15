from distutils.core import setup, Extension

# the c++ extension module: https://docs.python.org/3/distutils/apiref.html#distutils.core.Extension
extension_mod = Extension("DRSA",
                            sources = ['DRSAmodule.cpp'],
                            include_dirs=['~/Desktop/DRSA/include'],
                            library_dirs = ['../lib'],
                            language = "c++",
                            extra_objects = ['../lib/libdrsa.a'],
                            #extra_compile_args = ['-fPIC', '-static', '-lcalculator'],
                            #extra_link_args = ['-static']
                            )

setup(name = "Dominance-Based Rough Sets Approach",
      version = '1.0',
      description = 'Decision rule induction from data',
      author = 'Julio C. S. Silva',
      author_email = 'jcss4@cin.ufpe.br',
      url = 'www.greefo.org.br',
      ext_modules = [extension_mod])