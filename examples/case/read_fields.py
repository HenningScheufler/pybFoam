import pybFoam as pf

# Create OpenFOAM time and mesh
time = pf.Time(".", ".")
mesh = pf.fvMesh(time)

# Access fields
p_rgh = pf.volScalarField.read_field(mesh, "p_rgh")
U = pf.volVectorField.read_field(mesh, "U")

# Compute gradients using finite volume calculus
grad_p = pf.fvc.grad(p_rgh)
div_U = pf.fvc.div(U)

# Convert to NumPy arrays for analysis
import numpy as np

p_array = np.asarray(p_rgh["internalField"])
print(f"Pressure range: {p_array.min():.3f} to {p_array.max():.3f}")
