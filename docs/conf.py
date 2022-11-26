import subprocess, os

def configureDoxyfile(input_dir, output_dir):
    with open('Doxyfile.in', 'r') as file :
        filedata = file.read()

    filedata = filedata.replace('@DOXYGEN_INPUT_DIR_JOINED@', input_dir)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)

    with open('Doxyfile', 'w') as file:
        file.write(filedata)

# Check if we're running on Read the Docs' servers
read_the_docs_build = (os.environ.get('READTHEDOCS', None) == 'True')

breathe_projects = {}

if read_the_docs_build:
    input_dir = "../src ../include ../include/QtNodes/internal"
    output_dir = '_build'
    configureDoxyfile(input_dir, output_dir)
    subprocess.call('doxygen', shell=True)
    breathe_projects['QtNodes'] = output_dir + '/xml/'


# -- Project information -----------------------------------------------------

project = 'QtNodes'
copyright = '2022, Dmitry Pinaev'
author = 'Dmitry Pinaev'

# The full version, including alpha/beta/rc tags
release = '3.0'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [ "breathe", "sphinx_rtd_theme" ]


#Breathe configuration
breathe_default_project = "QtNodes"
breathe_default_members = ('members', 'undoc-members')

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static', 'css']
