from pybFoam import volScalarField, fvMesh, vector
from pydantic import BaseModel
import pybFoam
import numpy as np
from typing import Protocol, List, Any, Callable
from pathlib import Path
import os


class TimeSeriesWriter(Protocol):
    def __init__(
        self,
        name: str,
        header: List[str],
        region: str = "",
        dir_name: str = "postProcessing",
    ) -> None:
        pass

    def create_file(self):
        pass

    def append_data(self, data: List[Any]):
        pass


class csvTimeSeriesWriter(TimeSeriesWriter):
    def __init__(
        self,
        name: str,
        header: List[str],
        region: str = "",
        dir_name: str = "postProcessing",
    ) -> None:
        self.name = name
        self.header = ["t"] + header
        self.dir_name = dir_name
        self.file_name = Path(self.dir_name, self.name, f"{self.name}.csv")

    def create_file(self):
        self.file_name.parent.mkdir(parents=True, exist_ok=True)
        with open(self.file_name, "w", encoding="utf8") as f:
            f.write(",".join(self.header))
            f.write(os.linesep)

    def write_data(self, t: float, data: List[Any]):
        with open(self.file_name, "a", encoding="utf8") as f:
            f.write(f"{t},")
            f.write(",".join(data))
            f.write(os.linesep)


def field_aggregation(fields: List[Any], agg_func: List[Callable]) -> List[str]:
    pass


class Force:
    def __init__(self, mesh: fvMesh, bc_names: List[str], p_name: str = "p") -> None:
        self.mesh = mesh
        self.p_name = p_name
        self.bc_names = bc_names

    def header(self) -> List[str]:
        return ["force_x", "force_y", "force_z"]

    def calcForces(self) -> vector:
        p = volScalarField.from_registry(self.mesh,self.p_name)
        force = vector(0, 0, 0)
        for bc in self.bc_names:
            force += pybFoam.sum(self.mesh.Sf()[bc] * p[bc])

        return force

    def compute(self) -> List[str]:
        force = self.calcForces()
        return [str(force[0]), str(force[1]), str(force[2])]


class Sum(BaseModel):
    header: List[str]
    fields: List[str]


    def compute(self):
        return self.header