# import os

# N = 8

# os.environ["XLA_FLAGS"] = (
#     f"--xla_cpu_multi_thread_eigen=true "
#     f"intra_op_parallelism_threads={N}"
# )
import os

def configure_threads(n=1):
    multi_thread = True if n > 1 else False
    os.environ["XLA_FLAGS"] = (
        f"--xla_cpu_multi_thread_eigen={multi_thread} "
        f"intra_op_parallelism_threads={n}"
    )
    os.environ["NPROC"] = f"{n}"
    os.environ["OMP_NUM_THREADS"] = str(n)
    os.environ["MKL_NUM_THREADS"] = str(n)
    os.environ["OPENBLAS_NUM_THREADS"] = str(n)

configure_threads(1)

from pybFoam import scalarField
import numpy as np
import timeit
import time
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import jax
import jax.numpy as jnp


def pybfoam_expression(n_elements):
    a = scalarField([1.1] * n_elements)
    b = scalarField([2.2] * n_elements)
    c = scalarField([3.3] * n_elements)
    d = scalarField([4.4] * n_elements)

    def run():
        x = a * b + c
        y = d - a * c
        return (x * y + b) / (a + 1.0)
    return run

def numpy_expression(n_elements):
    a = np.full(n_elements, 1.1)
    b = np.full(n_elements, 2.2)
    c = np.full(n_elements, 3.3)
    d = np.full(n_elements, 4.4)

    def run():
        x = a * b + c
        y = d - a * c
        return (x * y + b) / (a + 1.0)

    return run

def jax_expression(n_elements):
    a = jnp.full(n_elements, 1.1)
    b = jnp.full(n_elements, 2.2)
    c = jnp.full(n_elements, 3.3)
    d = jnp.full(n_elements, 4.4)

    @jax.jit
    def run():
        x = a * b + c
        y = d - a * c
        return (x * y + b) / (a + 1.0)

    return run

vector_add_data = {
    "n_elements": [],
    "duration": [],
    "method": [],
    # "jax": []
}

def add_data(n_elements, duration, method):
    vector_add_data["n_elements"].append(n_elements)
    vector_add_data["duration"].append(duration)
    vector_add_data["method"].append(method)

for n_elements in [10, 100, 1000, 10_000, 100_000, 1_000_000, 10_000_000]:

    bench = pybfoam_expression(n_elements)
    # duration = timeit.timeit(bench, number=1)
    t0 = time.perf_counter() # lower overhead than timeit
    bench()
    t1 = time.perf_counter()
    duration = t1 - t0
    add_data(n_elements, duration, "pybFoam")

    bench = numpy_expression(n_elements)
    # duration = timeit.timeit(bench, number=1)
    t0 = time.perf_counter() # lower overhead than timeit
    bench()
    t1 = time.perf_counter()
    duration = t1 - t0
    add_data(n_elements, duration, "NumPy")

    bench = jax_expression(n_elements)
    # First call to compile
    bench()
    # duration = timeit.timeit(bench, number=1)
    t0 = time.perf_counter() # lower overhead than timeit
    bench()
    t1 = time.perf_counter()
    duration = t1 - t0
    add_data(n_elements, duration, "JAX")


df_vector_add = pd.DataFrame(vector_add_data)
df_vector_add["time_per_element [ns]"] = df_vector_add["duration"] / df_vector_add["n_elements"] * 1e9  # time per element in nanoseconds
print("Vector Addition Benchmark:",df_vector_add)

sns.lineplot(data=df_vector_add, x="n_elements", y="time_per_element [ns]", hue="method", marker="o")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Number of Elements")
plt.ylabel("Time per Element (nanoseconds)")
plt.title("Expression: (x * y + b) / (a + 1.0)")
plt.savefig("bench_complex.png")
n_elements = 10_000_000
timings = df_vector_add[df_vector_add["n_elements"] == n_elements]
print(f"Timings for {n_elements} elements:")
for _, row in timings.iterrows():
    print(f"{row['method']}: {row['time_per_element [ns]']:.2f} ns per element")
n_elements = 1_000_000
print("")
timings = df_vector_add[df_vector_add["n_elements"] == n_elements]
print(f"Timings for {n_elements} elements:")
for _, row in timings.iterrows():
    print(f"{row['method']}: {row['time_per_element [ns]']:.2f} ns per element")

plt.show()



# %%
