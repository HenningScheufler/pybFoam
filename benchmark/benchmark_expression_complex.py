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
        f"--xla_cpu_multi_thread_eigen={multi_thread} intra_op_parallelism_threads={n}"
    )
    os.environ["NPROC"] = f"{n}"
    os.environ["OMP_NUM_THREADS"] = str(n)
    os.environ["MKL_NUM_THREADS"] = str(n)
    os.environ["OPENBLAS_NUM_THREADS"] = str(n)


configure_threads(1)

import timeit

import jax
import jax.numpy as jnp
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

from pybFoam import scalarField


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
    n_repeat = 100
    duration = timeit.timeit(bench, number=n_repeat)
    # t0 = time.perf_counter() # lower overhead than timeit
    # bench()
    # t1 = time.perf_counter()
    # duration = t1 - t0
    add_data(n_elements, duration / n_repeat, "pybFoam")

    bench = numpy_expression(n_elements)
    duration = timeit.timeit(bench, number=n_repeat)
    # t0 = time.perf_counter() # lower overhead than timeit
    # bench()
    # t1 = time.perf_counter()
    # duration = t1 - t0
    add_data(n_elements, duration / n_repeat, "NumPy")

    bench = jax_expression(n_elements)
    # First call to compile
    bench()
    duration = timeit.timeit(bench, number=n_repeat)
    # t0 = time.perf_counter() # lower overhead than timeit
    # bench()
    # t1 = time.perf_counter()
    # duration = t1 - t0
    add_data(n_elements, duration / n_repeat, "JAX")


df_vector_add = pd.DataFrame(vector_add_data)
df_vector_add["time_per_element [ns]"] = (
    df_vector_add["duration"] / df_vector_add["n_elements"] * 1e9
)  # time per element in nanoseconds
print("Vector Addition Benchmark:", df_vector_add)

# Save results to CSV
df_vector_add.to_csv("results/benchmark_complex.csv", index=False)
print("\nResults saved to: results/benchmark_complex.csv")

# Create pivot table for markdown
pivot_df = df_vector_add.pivot(index="n_elements", columns="method", values="time_per_element [ns]")
pivot_df = pivot_df.round(2)

# Save markdown table
with open("results/benchmark_complex.md", "w") as f:
    f.write("# Complex Expression Benchmark\n\n")
    f.write("Expression: `(x * y + b) / (a + 1.0)`\n\n")
    f.write("where:\n")
    f.write("- `x = a * b + c`\n")
    f.write("- `y = d - a * c`\n\n")
    f.write("## Results (time per element in nanoseconds)\n\n")
    f.write(pivot_df.to_markdown())
    f.write("\n\n## Summary\n\n")
    for n_elem in [1_000_000, 10_000_000]:
        if n_elem in pivot_df.index:
            f.write(f"\n### {n_elem:,} elements:\n\n")
            for method in pivot_df.columns:
                f.write(f"- **{method}**: {pivot_df.loc[n_elem, method]:.2f} ns per element\n")

print("Results table saved to: results/benchmark_complex.md")

sns.lineplot(
    data=df_vector_add, x="n_elements", y="time_per_element [ns]", hue="method", marker="o"
)
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Number of Elements")
plt.ylabel("Time per Element (nanoseconds)")
plt.title("Expression: (x * y + b) / (a + 1.0)")
plt.savefig("results/bench_complex.png")
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
