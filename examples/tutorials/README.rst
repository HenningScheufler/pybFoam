Tutorials
=========

End-to-end walkthroughs that build a single skill at a time. Each
tutorial runs at documentation build time against the cases shipped
under ``examples/`` — the printed numbers and figures on the page
are exactly what the script produced.

Read them in order:

1. **Read, modify, and write OpenFOAM dictionaries.** The first core
   primitive: open ``system/controlDict``, change a value, write it
   back. Every later tutorial leans on this pattern.
2. **Build and operate on a scalarField.** The second core primitive:
   construct a ``scalarField`` from a NumPy array and apply OpenFOAM's
   elementwise math (``sin``, ``+``, ``*``, in-place ``+=``).
3. **Modify initial conditions of the cavity case.** Combine the two
   primitives on a real ``volVectorField`` — replace the cavity's
   uniform ``U`` with an analytic vortex through a NumPy view, write
   it back, and visualise the result with pyvista.
4. **Run the cavity case and sample a line over time.** Drive the
   icoFoam PISO loop one step at a time, sample velocity on a
   horizontal mid-line each step, and plot the profile evolving in
   time alongside a final pyvista snapshot.
