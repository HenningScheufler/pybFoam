Installation
============

Prerequisites
-------------

* **OpenFOAM** v2312 or higher, with the environment sourced:

  .. code-block:: bash

     source /path/to/OpenFOAM/etc/bashrc

* **Python** 3.9 or higher
* **CMake** 3.18 or higher
* A C++17-capable compiler

Install from PyPI
-----------------

.. code-block:: bash

   pip install pybFoam

Install from source
-------------------

.. code-block:: bash

   git clone https://github.com/HenningScheufler/pybFoam
   cd pybFoam
   pip install .

Development install
-------------------

.. code-block:: bash

   pip install -e .[all]

To regenerate type stubs after a development install:

.. code-block:: bash

   ./scripts/generate_stubs.sh
