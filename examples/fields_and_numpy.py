#%%
from pybFoam import scalarField
import numpy as np

a = scalarField([0.0]*10 )
b = scalarField([1.0]*10 )
c = scalarField([2.0]*10 )


# %%
np_a = np.asarray(a)
np_b = np.asarray(b)
np_c = np.asarray(c)
print("Numpy arrays:")
print("a:", np_a)
print("b:", np_b)
print("c:", np_c)
c += a + b + 5.0
# %%
print("Numpy arrays:")
print("a:", np_a)
print("b:", np_b)
print("c:", np_c)
# %%
np_c[0:2] = 42.0
print("mod c:", np_c)
# %%
