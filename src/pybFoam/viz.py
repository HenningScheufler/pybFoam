"""Visualization helpers built on pyvista.

The package itself does not depend on pyvista — these helpers import it
lazily so a pybFoam install without the ``[docs]`` extra still works.

The chosen integration point is pyvista's :class:`pyvista.POpenFOAMReader`
(VTK's ``vtkOpenFOAMReader``). This reads any OpenFOAM case directory
directly from disk, which means **fields must be written before they
can be plotted** (e.g. ``U.write()`` or ``runTime.write(True)``). In
return we get correct handling of boundary patches, multiple time
directories, and every primitive field type for free, without
recompiling C++ bindings.

Typical use::

    case = clone_case("cavity")
    # ... modify fields, run solver, etc. ...
    U.write()                              # to disk
    reader = open_case(case)
    grid = reader.read()["internalMesh"]   # pyvista MultiBlock
    grid.plot(scalars="U")
"""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING, Iterable, Optional, Union

if TYPE_CHECKING:
    import pyvista as pv

__all__ = ["open_case"]


def _ensure_foam_marker(case: Path) -> Path:
    """Create or return ``<case>/<case_name>.foam``.

    ``vtkOpenFOAMReader`` keys on a ``.foam`` placeholder file; the file
    is empty and only its extension matters.
    """
    marker = case / f"{case.name}.foam"
    if not marker.exists():
        marker.touch()
    return marker


def open_case(
    case: Union[str, Path],
    *,
    time: Optional[float] = None,
    cell_arrays: Optional[Iterable[str]] = None,
) -> "pv.POpenFOAMReader":
    """Open an OpenFOAM case as a pyvista reader.

    Parameters
    ----------
    case
        Path to the case directory (one containing ``system/``,
        ``constant/``, time directories).
    time
        If given, set the reader's active time value to this. Pass the
        actual physical time (e.g. ``0.05``), not the time index.
        Default: leave the reader on its first available time.
    cell_arrays
        Names of cell arrays (fields) to enable. ``None`` enables all
        cell arrays found by the reader (cheap; the data is loaded
        lazily on ``read()``).

    Returns
    -------
    pyvista.POpenFOAMReader
        A configured reader. Call ``reader.read()`` to obtain the
        :class:`pyvista.MultiBlock` of meshes (key ``"internalMesh"``
        holds the volume mesh).

    Raises
    ------
    ImportError
        If pyvista is not installed.
    FileNotFoundError
        If ``case`` does not exist.
    """
    try:
        import pyvista as pv
    except ImportError as exc:  # pragma: no cover - exercised in docs build
        raise ImportError(
            "pyvista is required for pybFoam.viz. "
            "Install with `pip install pybFoam[docs]`."
        ) from exc

    case_path = Path(case).resolve()
    if not case_path.is_dir():
        raise FileNotFoundError(f"Case directory does not exist: {case_path}")

    marker = _ensure_foam_marker(case_path)
    reader = pv.POpenFOAMReader(str(marker))

    if cell_arrays is None:
        reader.enable_all_cell_arrays()
    else:
        reader.disable_all_cell_arrays()
        for name in cell_arrays:
            reader.enable_cell_array(name)

    if time is not None:
        reader.set_active_time_value(time)  # type: ignore[no-untyped-call]

    return reader
