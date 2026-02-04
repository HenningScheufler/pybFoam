# %%
from pybFoam import dictionary

d = dictionary.read("/home/henning/libsAndApps/pybFoam/examples/case/system/controlDict")

toc = d.toc()
print("len: len(toc) =", len(toc))
for entry in toc:
    print(f"{entry}")
# %%
d.get[float]("startTime")


# %%
d.get[str]("application")
# %%
