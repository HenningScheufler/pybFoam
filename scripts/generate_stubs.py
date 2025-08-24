#!/usr/bin/env python3
"""
Script to generate Python stub files for pybFoam pybind11 modules.
Run this after building the package to get type hints for IDEs.
"""

import subprocess
import sys
import os
from pathlib import Path

def run_stubgen(module_name, output_dir):
    """Run pybind11-stubgen for a specific module"""
    cmd = [
        "pybind11-stubgen", 
        f"pybFoam.{module_name}",
        "-o", str(output_dir)
    ]
    
    print(f"Generating stubs for pybFoam.{module_name}...")
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        print(f"✓ Generated stubs for {module_name}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ Failed to generate stubs for {module_name}: {e}")
        print(f"  stdout: {e.stdout}")
        print(f"  stderr: {e.stderr}")
        return False
    except FileNotFoundError:
        print("✗ pybind11-stubgen not found. Install with: pip install pybind11-stubgen")
        return False

def main():
    # Get the source directory
    script_dir = Path(__file__).parent
    src_dir = script_dir.parent / "src" / "pybFoam"
    
    if not src_dir.exists():
        print(f"Error: Source directory {src_dir} not found")
        sys.exit(1)
    
    # List of pybind11 modules to generate stubs for
    modules = [
        "pybFoam_core",
        "fvc", 
        "thermo",
        "turbulence",
        "runTimeTables"
    ]
    
    print("Generating Python stub files for pybFoam modules...")
    print(f"Output directory: {src_dir}")
    
    success_count = 0
    for module in modules:
        if run_stubgen(module, src_dir):
            success_count += 1
    
    print(f"\nCompleted: {success_count}/{len(modules)} modules")
    
    if success_count == len(modules):
        print("✓ All stub files generated successfully!")
        
        # Create a py.typed marker file
        py_typed = src_dir / "py.typed"
        py_typed.touch()
        print(f"✓ Created {py_typed}")
        
    else:
        print("⚠ Some stub generation failed. Make sure:")
        print("  1. pybFoam is installed: pip install -e .")
        print("  2. pybind11-stubgen is installed: pip install pybind11-stubgen")
        print("  3. OpenFOAM environment is sourced (for OpenFOAM modules)")

if __name__ == "__main__":
    main()
