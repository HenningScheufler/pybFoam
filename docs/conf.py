# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# Force C numeric locale. Sphinx/Babel activate the user locale for i18n, which
# on German systems sets LC_NUMERIC=de_DE.UTF-8 (decimal separator ","). When
# autodoc imports pybFoam, OpenFOAM's dictionary parser then fails to read
# "2.0" in $WM_PROJECT_DIR/etc/controlDict with FOAM FATAL IO ERROR.
import locale
import os

locale.setlocale(locale.LC_NUMERIC, "C")

# OpenFOAM installs a SIGFPE trap that fires on NaN / divide-by-zero /
# overflow at the CPU level. During gallery execution, some Python
# string-formatting paths (PyOS_double_to_string) hit that trap and
# abort the build. Disable the trap before any OpenFOAM symbol is
# loaded — this only affects the doc build, not users' own scripts.
os.environ.setdefault("FOAM_SIGFPE", "false")

# Headless pyvista. Tutorials and how-tos use pyvista.Plotter, which
# needs an off-screen GL context inside the doc build. start_xvfb()
# spawns a virtual framebuffer on Linux; OFF_SCREEN=True suppresses
# any window creation so the build works in headless CI.
try:
    import pyvista as _pv

    _pv.OFF_SCREEN = True
    _pv.BUILDING_GALLERY = True
    _pv.set_plot_theme("document")
    if hasattr(_pv, "start_xvfb"):
        try:
            _pv.start_xvfb()
        except OSError:
            pass
except ImportError:
    pass

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "pybFoam"
copyright = "2025, Henning Scheufler"
author = "Henning Scheufler"
release = "0.5.1"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinx.ext.autodoc",
    "sphinxcontrib.mermaid",
    "sphinx.ext.intersphinx",
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.todo",
    "sphinx.ext.coverage",
    "sphinx.ext.mathjax",
    "sphinx.ext.ifconfig",
    "sphinx.ext.viewcode",
    "sphinx_sitemap",
    "sphinx.ext.inheritance_diagram",
    "sphinx_gallery.gen_gallery",
]

# Many gallery / how-to pages share section titles ("See also",
# "Prerequisites", "Set up case + mesh"). autosectionlabel raises a
# duplicate-label warning per collision otherwise. Prefixing each
# label with the document path makes them globally unique.
autosectionlabel_prefix_document = True
# Only label top-level section headings. autodoc embeds NumPy-style
# "Parameters" / "Returns" / "Raises" subsection headings inside API
# reference pages once per documented function, so without a depth
# cap they all collide on the *same* page.
autosectionlabel_maxdepth = 1

# Resolve type hints in our own docstrings against external doc sets.
# This silences "py:class reference target not found: pathlib.Path"
# style warnings when sphinx renders the docstrings of clone_case
# and friends.
intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "numpy": ("https://numpy.org/doc/stable", None),
    "pyvista": ("https://docs.pyvista.org", None),
}

# Targets that have no public doc page we can link to. Each entry is
# (kind, qualified_name) — sphinx will skip the cross-reference and
# render just the name in code style. Keep this tight: prefer fixing
# the docstring over expanding this list.
nitpick_ignore = [
    # nanobind-bound classes we expose internally but do not document
    # at the top level (yet). Added to docstrings as type hints.
    ("py:class", "pybFoam.pybFoam_core.faceList"),
    ("py:class", "pybFoam.pybFoam_core.fvBoundaryMesh"),
    # pyvista forward reference — string-typed hint in viz.py.
    ("py:class", "pv.POpenFOAMReader"),
    # Pydantic generates these internally on validators; not part of
    # any public API surface.
    ("py:class", "annotated_types.Gt"),
    # Bare 'Path' name when used as a string forward reference. The
    # fully qualified pathlib.Path resolves via intersphinx; the
    # unqualified one does not.
    ("py:class", "Path"),
]

sphinx_gallery_conf = {
    "examples_dirs": ["../examples/tutorials", "../examples/how-to"],
    "gallery_dirs": ["auto_tutorials", "auto_how_to"],
    "filename_pattern": r"/example_",
    # Ignore any .py whose basename doesn't start with ``example_`` — raw
    # scripts, Allrun helpers, solver demos etc. aren't gallery pages but
    # still live under examples/. (sphinx-gallery matches the basename,
    # not the full path.)
    "ignore_pattern": r"^(?!example_).*\.py$",
    "remove_config_comments": True,
    "download_all_examples": False,
    "plot_gallery": "True",
    # Scrape both matplotlib figures and pyvista plotter screenshots
    # from gallery scripts.
    "image_scrapers": ("matplotlib", "pyvista"),
}

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "furo"
html_theme_options = {
    # Furo accepts a small number of options; keep only those it
    # honours. The previous configuration mixed in sphinx_rtd_theme
    # keys (canonical_url, collapse_navigation, …) that furo rejects
    # with deprecation warnings.
    "sidebar_hide_name": False,
    "navigation_with_keys": True,
}

html_static_path = ["_static"]

html_baseurl = "https://henning.github.io/pybFoam/"


# -- autodoc for nanobind bindings -------------------------------------------
# Nanobind-bound callables have type ``nanobind.nb_func`` (and methods are
# ``nb_method``). These fail ``inspect.isfunction`` / ``isbuiltin`` / ``isroutine``,
# so Sphinx's default FunctionDocumenter rejects them and ``automodule ... :members:``
# silently skips every bound function. Teach autodoc to accept them by replacing
# the relevant predicates with wider ones that also return True for nb_func /
# nb_method.


# Sphinx-gallery auto-generates `auto_tutorials/index.rst` and
# `auto_how_to/index.rst`. We list each tutorial / how-to individually
# in `index.rst` instead of going through these gallery indices, but
# the indices still exist on disk. Marking them `:orphan:` keeps them
# reachable by URL while excluding them from the sidebar — otherwise
# furo renders both the gallery index *and* the per-page entries,
# which produces duplicate sidebar entries.
_GALLERY_INDEX_DOCS = {"auto_tutorials/index", "auto_how_to/index"}


def _orphan_gallery_indices(app, docname, source):
    if docname in _GALLERY_INDEX_DOCS and not source[0].lstrip().startswith(":orphan:"):
        source[0] = ":orphan:\n\n" + source[0]


def setup(app):
    import inspect

    from sphinx.ext import autodoc
    from sphinx.util import inspect as sphinx_inspect

    app.connect("source-read", _orphan_gallery_indices)

    _NB_TYPE_NAMES = {"nb_func", "nb_method"}

    def _is_nanobind_callable(obj) -> bool:
        return type(obj).__name__ in _NB_TYPE_NAMES

    _orig_isfunction = sphinx_inspect.isfunction
    _orig_isbuiltin = sphinx_inspect.isbuiltin

    def _patched_isfunction(obj):
        return _orig_isfunction(obj) or _is_nanobind_callable(obj)

    def _patched_isbuiltin(obj):
        return _orig_isbuiltin(obj) or _is_nanobind_callable(obj)

    sphinx_inspect.isfunction = _patched_isfunction
    sphinx_inspect.isbuiltin = _patched_isbuiltin

    _orig_can_document = autodoc.FunctionDocumenter.can_document_member

    @classmethod
    def _can_document_member(cls, member, membername, isattr, parent):
        if _is_nanobind_callable(member):
            return True
        return _orig_can_document(member, membername, isattr, parent)

    autodoc.FunctionDocumenter.can_document_member = _can_document_member

    return {"parallel_read_safe": True, "parallel_write_safe": True}
