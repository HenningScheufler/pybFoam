"""``icoFoam``-equivalent transient incompressible solver, in Python.

Direct port of OpenFOAM's ``icoFoam`` C++ solver. The PISO body inside
:meth:`IcoFoam.step` mirrors the upstream solver line-for-line so that
readers can compare them side-by-side.

Run from the command line in this directory::

    blockMesh
    python icoFoam.py

Or import :class:`IcoFoam` from a script and drive the time loop
yourself — that is what tutorial T3 does to sample fields between
steps.
"""

from __future__ import annotations

from pathlib import Path
from typing import Tuple

from pybFoam import (
    Info,
    Time,
    Word,
    adjustPhi,
    argList,
    constrainHbyA,
    constrainPressure,
    createPhi,
    dictionary,
    fvc,
    fvm,
    fvMesh,
    fvScalarMatrix,
    fvVectorMatrix,
    pisoControl,
    setRefCell,
    solve,
    surfaceScalarField,
    volScalarField,
    volVectorField,
)
from pybFoam.meshing import generate_blockmesh

__all__ = ["IcoFoam"]


class IcoFoam:
    """Transient incompressible PISO solver.

    Parameters
    ----------
    case
        Path to an OpenFOAM case directory. Must contain
        ``system/controlDict``, ``system/fvSolution``,
        ``constant/transportProperties``, and ``0/{U,p,nu}``. If
        ``constant/polyMesh/`` is missing, the mesh is generated from
        ``system/blockMeshDict`` automatically.

    Notes
    -----
    A single momentum predictor followed by ``nCorrectors`` PISO
    pressure corrections, with optional non-orthogonal sub-corrections.

    Drive one step at a time with :meth:`step` (so callers can sample,
    plot, or modify fields between steps), or :meth:`run` to advance
    to ``endTime`` non-stop.
    """

    def __init__(self, case: Path) -> None:
        self._case = Path(case).resolve()
        self._args = argList([str(self._case), "-case", str(self._case)])
        self._time = Time(self._args)

        if not (self._case / "constant" / "polyMesh").is_dir():
            block_dict_path = self._case / "system" / "blockMeshDict"
            if not block_dict_path.is_file():
                raise FileNotFoundError(
                    f"Neither constant/polyMesh nor system/blockMeshDict found under {self._case}."
                )
            generate_blockmesh(self._time, dictionary.read(str(block_dict_path)))

        self._mesh = fvMesh(self._time)
        self._p, self._U, self._phi, self._nu = self._read_fields()

        fv_solution = dictionary.read(str(self._case / "system" / "fvSolution"))
        self._p_ref_cell, self._p_ref_value = setRefCell(self._p, fv_solution.subDict("PISO"))
        self._mesh.setFluxRequired(Word("p"))

        self._piso = pisoControl(self._mesh)

    def _read_fields(
        self,
    ) -> Tuple[volScalarField, volVectorField, surfaceScalarField, volScalarField]:
        p = volScalarField.read_field(self._mesh, "p")
        U = volVectorField.read_field(self._mesh, "U")
        phi = createPhi(U)
        nu = volScalarField.read_field(self._mesh, "nu")
        return p, U, phi, nu

    @property
    def case(self) -> Path:
        return self._case

    @property
    def time(self) -> Time:
        return self._time

    @property
    def mesh(self) -> fvMesh:
        return self._mesh

    @property
    def U(self) -> volVectorField:
        return self._U

    @property
    def p(self) -> volScalarField:
        return self._p

    @property
    def phi(self) -> surfaceScalarField:
        return self._phi

    def step(self) -> bool:
        """Advance the solver by one time step.

        Returns ``True`` if a step was performed, ``False`` once
        ``endTime`` has been reached. Idiom::

            while solver.step():
                ...
        """
        if not self._time.loop():
            return False

        Info(f"Time = {self._time.timeName()}")

        U, p, phi, nu = self._U, self._p, self._phi, self._nu
        piso = self._piso

        UEqn = fvVectorMatrix(fvm.ddt(U) + fvm.div(phi, U) - fvm.laplacian(nu, U))

        if piso.momentumPredictor():
            solve(UEqn + fvc.grad(p))

        while piso.correct():
            rAU = volScalarField(Word("rAU"), 1.0 / UEqn.A())
            HbyA = volVectorField(constrainHbyA(rAU * UEqn.H(), U, p))

            phiHbyA = surfaceScalarField(
                Word("phiHbyA"),
                fvc.flux(HbyA) + fvc.interpolate(rAU) * fvc.ddtCorr(U, phi),
            )

            adjustPhi(phiHbyA, U, p)
            constrainPressure(p, U, phiHbyA, rAU)

            while piso.correctNonOrthogonal():
                pEqn = fvScalarMatrix(fvm.laplacian(rAU, p) - fvc.div(phiHbyA))
                pEqn.setReference(self._p_ref_cell, self._p_ref_value, False)
                pEqn.solve(p.select(piso.finalInnerIter()))

                if piso.finalNonOrthogonalIter():
                    phi.assign(phiHbyA - pEqn.flux())

            U.assign(HbyA - rAU * fvc.grad(p))
            U.correctBoundaryConditions()

        self._time.write(True)
        self._time.printExecutionTime()
        return True

    def run(self) -> None:
        """Run from the current time to ``endTime``."""
        while self.step():
            pass


if __name__ == "__main__":
    Info("Running icoFoam (Python)")
    IcoFoam(Path.cwd()).run()
    Info("End")
