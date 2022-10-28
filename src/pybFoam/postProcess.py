from pybFoam import volScalarField, fvMesh
import numpy as np
from typing import Protocol, List, Any, Callable
from pathlib import Path
import os


def compute_average(field_name: str, mesh: fvMesh):
    vf = volScalarField(field_name, mesh)
    return np.average(vf["internalField"].to_numpy())


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


class csvTimeSeries(TimeSeriesWriter):
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
        with open(self.file_name, "w", encoding="utf8") as f:
            f.write(",".join(self.header))
            f.write(os.linesep)

    def append_data(self, t: float, data: List[Any]):
        with open(self.file_name, "w", encoding="utf8") as f:
            f.write(",".join(data))
            f.write(os.linesep)


def field_aggregation(fields: List[Any], agg_func: List[Callable]) -> List[str]:
    pass

class Output:
    pass

class Surface:
    pass

class TimeSeries:
    pass

class postProcessBuilder:

    def __init__(self):
        self.time_series : List[Output] = None
        self.surface : List[Output] = None
        self.field : List[Output] = None

    def add_timeseries(self) -> "postProcessBuilder":
        return self

    def add_surface(self) -> "postProcessBuilder":
        return self

    def create(self):
        pass
