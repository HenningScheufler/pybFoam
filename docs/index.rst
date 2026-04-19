pybFoam Documentation
=====================

pybFoam provides Python bindings for OpenFOAM via nanobind. It enables direct
manipulation of OpenFOAM cases, fields, meshes, and solvers from Python, with
zero-copy NumPy access to field data.

**Prerequisite:** Source OpenFOAM v2312+ (``source /path/to/OpenFOAM/etc/bashrc``)
before building or running anything shown here.

.. admonition:: Which doc should I read?
   :class: tip

   * **New here?** Start with a tutorial — they walk through an end-to-end
     workflow.
   * **Have a specific task?** A how-to guide gives step-by-step recipes.
   * **Looking up an API or flag?** See the reference.
   * **Want to understand the design?** Read the explanation section.

.. toctree::
   :maxdepth: 2
   :caption: Tutorials

   auto_tutorials/index

.. toctree::
   :maxdepth: 2
   :caption: How-to guides

   auto_how_to/index
   how-to/use_turbulence_thermo
   how-to/parallel_runs

.. toctree::
   :maxdepth: 1
   :caption: Reference

   reference/api
   reference/installation
   reference/build_system
   reference/cli_and_poe
   reference/supported_versions

.. toctree::
   :maxdepth: 1
   :caption: Explanation

   explanation/architecture
   explanation/zero_copy_numpy
   explanation/sampling_configs
   explanation/openfoam_interop

.. toctree::
   :maxdepth: 1
   :caption: Project

   contributing
   changelog

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
