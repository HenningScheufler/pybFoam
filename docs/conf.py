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

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "pybFoam"
copyright = "2025, Henning Scheufler"
author = "Henning Scheufler"
release = "0.4.3"

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
}

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "furo"
html_theme_options = {
    "canonical_url": "",
    "analytics_id": "",  #  Provided by Google in your dashboard
    "display_version": True,
    "prev_next_buttons_location": "bottom",
    "style_external_links": False,
    "logo_only": False,
    # Toc options
    "collapse_navigation": True,
    "sticky_navigation": True,
    "navigation_depth": 4,
    "includehidden": True,
    "titles_only": False,
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


def setup(app):
    import inspect

    from sphinx.ext import autodoc
    from sphinx.util import inspect as sphinx_inspect

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
