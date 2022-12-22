from pybFoam import volScalarField, mag, fvMesh


def area(geometry: fvMesh):
    return mag(geometry.Sf())

def positions(geometry: fvMesh):
    return geometry.Cf()

 