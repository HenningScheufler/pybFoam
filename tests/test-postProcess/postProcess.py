from typing import Callable
import pybFoam
from pybFoam import time_series
from typing import Callable, List





class postProcess():

    def __init__(self,mesh: pybFoam.fvMesh):
        self.mesh = mesh
        self.csv1 = time_series.csvTimeSeriesWriter(name="pyforce",header=["fx","fy","fz"])
        self.csv1.create_file()
        self.f = time_series.Force(mesh,["lowerWall"])

    def execute(self):
        self.csv1.write_data(self.mesh.time().value(),self.f.compute())

    def write(self):
        pass

    def end(self):
        pass

# ppf = postProcessFunctions(mesh)
# ppb = pybFoam.postProcess.postProcessBuilder()

# @ppf.time_series()
# def add_force():
#     return [time_series.Force()]


    # csv1 = time_series.csvTimeSeries(
    #     mesh, writeFrequency=time_series.time_step(1), functions=[f]
    # )
    # csv1.execute()

    # sets
