"""Test checkMesh functionality"""

import pytest
import pybFoam.pybFoam_core as pyb
import pybFoam.bind_checkmesh as checkmesh
from pathlib import Path
import subprocess
import re
import json
from typing import Optional, List, Tuple


def extract_int(pattern: str, text: str) -> Optional[int]:
    """Extract an integer value from text using a regex pattern"""
    if m := re.search(pattern, text):
        return int(m.group(1))
    return None


def extract_float(pattern: str, text: str) -> Optional[float]:
    """Extract a float value from text using a regex pattern"""
    if m := re.search(pattern, text):
        return float(m.group(1))
    return None


def extract_float_pair(pattern: str, text: str) -> Optional[Tuple[float, float]]:
    """Extract a pair of float values from text using a regex pattern"""
    if m := re.search(pattern, text):
        return (float(m.group(1)), float(m.group(2)))
    return None


def extract_float_triple(pattern: str, text: str) -> Optional[Tuple[float, float, float]]:
    """Extract three float values from text using a regex pattern"""
    if m := re.search(pattern, text):
        return (float(m.group(1)), float(m.group(2)), float(m.group(3)))
    return None


def extract_vector(pattern: str, text: str) -> Optional[List[float]]:
    """Extract a vector (3 floats) from text and return as list"""
    if m := re.search(pattern, text):
        return [float(m.group(1)), float(m.group(2)), float(m.group(3))]
    return None


def extract_bounding_box(text: str) -> Optional[Tuple[List[float], List[float]]]:
    """Extract bounding box min and max coordinates"""
    pattern = r'Overall domain bounding box \(([-\d.e+\- ]+)\) \(([-\d.e+\- ]+)\)'
    if m := re.search(pattern, text):
        bb_min = [float(x) for x in m.group(1).split()]
        bb_max = [float(x) for x in m.group(2).split()]
        return (bb_min, bb_max)
    return None


def parse_checkmesh_output(cmd_output):
    """Parse checkMesh output and return a hierarchical dictionary matching the JSON structure"""
    parsed = {
        "mesh_stats": {},
        "cell_types": {},
        "topology": {},
        "geometry": {},
        "quality": {}
    }
    
    # Parse mesh stats using helper functions
    if (val := extract_int(r'points:\s+(\d+)', cmd_output)) is not None:
        parsed['mesh_stats']['points'] = val
    if (val := extract_int(r'faces:\s+(\d+)', cmd_output)) is not None:
        parsed['mesh_stats']['faces'] = val
    if (val := extract_int(r'internal faces:\s+(\d+)', cmd_output)) is not None:
        parsed['mesh_stats']['internal_faces'] = val
    if (val := extract_int(r'cells:\s+(\d+)', cmd_output)) is not None:
        parsed['mesh_stats']['cells'] = val
    if (val := extract_float(r'faces per cell:\s+([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['mesh_stats']['faces_per_cell'] = val
    if (val := extract_int(r'boundary patches:\s+(\d+)', cmd_output)) is not None:
        parsed['mesh_stats']['boundary_patches'] = val
    if (val := extract_int(r'point zones:\s+(\d+)', cmd_output)) is not None:
        parsed['mesh_stats']['point_zones'] = val
    if (val := extract_int(r'face zones:\s+(\d+)', cmd_output)) is not None:
        parsed['mesh_stats']['face_zones'] = val
    if (val := extract_int(r'cell zones:\s+(\d+)', cmd_output)) is not None:
        parsed['mesh_stats']['cell_zones'] = val
    
    # Parse cell type counts
    if (val := extract_int(r'hexahedra:\s+(\d+)', cmd_output)) is not None:
        parsed['cell_types']['hexahedra'] = val
    if (val := extract_int(r'prisms:\s+(\d+)', cmd_output)) is not None:
        parsed['cell_types']['prisms'] = val
    if (val := extract_int(r'wedges:\s+(\d+)', cmd_output)) is not None:
        parsed['cell_types']['wedges'] = val
    if (val := extract_int(r'pyramids:\s+(\d+)', cmd_output)) is not None:
        parsed['cell_types']['pyramids'] = val
    if (val := extract_int(r'tet wedges:\s+(\d+)', cmd_output)) is not None:
        parsed['cell_types']['tet_wedges'] = val
    if (val := extract_int(r'tetrahedra:\s+(\d+)', cmd_output)) is not None:
        parsed['cell_types']['tetrahedra'] = val
    if (val := extract_int(r'polyhedra:\s+(\d+)', cmd_output)) is not None:
        parsed['cell_types']['polyhedra'] = val
    
    # Parse geometry metrics
    if (bb := extract_bounding_box(cmd_output)) is not None:
        parsed['geometry']['bounding_box_min'] = bb[0]
        parsed['geometry']['bounding_box_max'] = bb[1]
    
    if (val := extract_int(r'Mesh has (\d+) geometric', cmd_output)) is not None:
        parsed['geometry']['geometric_directions'] = val
    if (val := extract_int(r'Mesh has (\d+) solution', cmd_output)) is not None:
        parsed['geometry']['solution_directions'] = val
    
    if (pair := extract_float_pair(r'Minimum face area = ([-\d.e+\-]+)\.\s+Maximum face area = ([-\d.e+\-]+)\.', cmd_output)) is not None:
        parsed['geometry']['min_face_area'] = pair[0]
        parsed['geometry']['max_face_area'] = pair[1]
    
    if (triple := extract_float_triple(r'Min volume = ([-\d.e+\-]+)\.\s+Max volume = ([-\d.e+\-]+)\.\s+Total volume = ([-\d.e+\-]+)\.', cmd_output)) is not None:
        parsed['geometry']['min_volume'] = triple[0]
        parsed['geometry']['max_volume'] = triple[1]
        parsed['geometry']['total_volume'] = triple[2]
    
    if (pair := extract_float_pair(r'Mesh non-orthogonality Max:\s+([-\d.e+\-]+)\s+average:\s+([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['max_non_orthogonality'] = pair[0]
        parsed['geometry']['avg_non_orthogonality'] = pair[1]
    
    if (val := extract_float(r'Max skewness = ([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['max_skewness'] = val
    
    if (pair := extract_float_pair(r'Min/max edge length = ([-\d.e+\-]+) ([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['min_edge_length'] = pair[0]
        parsed['geometry']['max_edge_length'] = pair[1]
    
    if (vec := extract_vector(r'Boundary openness \(([-\d.e+\-]+) ([-\d.e+\-]+) ([-\d.e+\-]+)\)', cmd_output)) is not None:
        parsed['geometry']['boundary_openness'] = vec
    
    if (val := extract_float(r'Max cell openness = ([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['max_cell_openness'] = val
    
    if (val := extract_float(r'Max aspect ratio = ([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['max_aspect_ratio'] = val
    
    if (pair := extract_float_pair(r'Face flatness.*?min = ([-\d.e+\-]+)\s+average = ([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['min_face_flatness'] = pair[0]
        parsed['geometry']['avg_face_flatness'] = pair[1]
    
    if (pair := extract_float_pair(r'Cell determinant.*?minimum:\s+([-\d.e+\-]+)\s+average:\s+([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['min_cell_determinant'] = pair[0]
        parsed['geometry']['avg_cell_determinant'] = pair[1]
    
    if (pair := extract_float_pair(r'Face interpolation weight.*?minimum:\s+([-\d.e+\-]+)\s+average:\s+([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['min_face_weight'] = pair[0]
        parsed['geometry']['avg_face_weight'] = pair[1]
    
    if (pair := extract_float_pair(r'Face volume ratio.*?minimum:\s+([-\d.e+\-]+)\s+average:\s+([-\d.e+\-]+)', cmd_output)) is not None:
        parsed['geometry']['min_face_volume_ratio'] = pair[0]
        parsed['geometry']['avg_face_volume_ratio'] = pair[1]
    
    # Check if mesh passed
    parsed['passed'] = "Mesh OK" in cmd_output
    parsed['failed'] = "Failed" in cmd_output and "mesh checks" in cmd_output
    
    # Set error counts (0 if mesh passed)
    parsed['topology']['errors'] = 0 if parsed['passed'] else -1
    parsed['topology']['passed'] = parsed['passed']
    parsed['geometry']['errors'] = 0 if parsed['passed'] else -1
    parsed['geometry']['passed'] = parsed['passed']
    parsed['quality']['errors'] = 0
    parsed['quality']['passed'] = True
    parsed['total_errors'] = 0 if parsed['passed'] else -1
    
    return parsed


@pytest.fixture
def cube_case():
    """Get path to the existing cube test case."""
    test_dir = Path(__file__).parent
    case_path = test_dir / "cube"
    assert case_path.exists(), f"Cube case not found at {case_path}"
    return case_path


def test_checkmesh_all_options(cube_case):
    """Test checkMesh with -allGeometry and -allTopology options, validating dictionary matches parsed OpenFOAM output"""
    
    # Run actual checkMesh command
    cmd = ["checkMesh", "-case", str(cube_case), "-allGeometry", "-allTopology"]
    result_cmd = subprocess.run(cmd, capture_output=True, text=True)
    cmd_output = result_cmd.stdout
    
    # Parse checkMesh output using extraction function
    parsed = parse_checkmesh_output(cmd_output)
    
    # Write parsed checkMesh output to JSON
    parsed_output_file = Path(__file__).parent / "checkmesh_parsed.json"
    with open(parsed_output_file, "w") as f:
        json.dump(parsed, f, indent=2)
    print(f"\n✓ Parsed checkMesh output written to {parsed_output_file}")
    
    # Verify command succeeded
    assert result_cmd.returncode == 0, f"checkMesh command failed with return code {result_cmd.returncode}"
    assert parsed['passed'], "Command line checkMesh should pass"
    
    # Run Python binding with same options
    argv = [str(cube_case), "-case", str(cube_case)]
    arglist = pyb.argList(argv)
    time = pyb.Time(arglist)
    mesh = pyb.fvMesh(time)
    
    result = checkmesh.checkMesh(mesh, check_topology=True, all_topology=True, all_geometry=True, check_quality=False)
    
    # Write Python binding result to JSON file
    output_file = Path(__file__).parent / "checkmesh_output.json"
    with open(output_file, "w") as f:
        json.dump(result, f, indent=2)
    print(f"✓ Python binding result written to {output_file}")
    
    # Verify basic structure
    assert "mesh_stats" in result
    assert "cell_types" in result
    assert "topology" in result
    assert "geometry" in result
    
    # Verify mesh stats match
    assert result["mesh_stats"]["points"] == parsed["mesh_stats"]["points"]
    assert result["mesh_stats"]["cells"] == parsed["mesh_stats"]["cells"]
    assert result["mesh_stats"]["faces"] == parsed["mesh_stats"]["faces"]
    assert result["mesh_stats"]["internal_faces"] == parsed["mesh_stats"]["internal_faces"]
    assert result["mesh_stats"]["boundary_patches"] == parsed["mesh_stats"]["boundary_patches"]
    assert result["mesh_stats"]["faces_per_cell"] == parsed["mesh_stats"]["faces_per_cell"]
    
    # Verify zones (only if present in both)
    if "point_zones" in parsed["mesh_stats"]:
        assert result["mesh_stats"]["point_zones"] == parsed["mesh_stats"]["point_zones"]
    if "face_zones" in parsed["mesh_stats"]:
        assert result["mesh_stats"]["face_zones"] == parsed["mesh_stats"]["face_zones"]
    if "cell_zones" in parsed["mesh_stats"]:
        assert result["mesh_stats"]["cell_zones"] == parsed["mesh_stats"]["cell_zones"]
    
    # Verify cell type counts
    assert result["cell_types"]["hexahedra"] == parsed["cell_types"]["hexahedra"]
    assert result["cell_types"]["prisms"] == parsed["cell_types"]["prisms"]
    assert result["cell_types"]["wedges"] == parsed["cell_types"]["wedges"]
    assert result["cell_types"]["pyramids"] == parsed["cell_types"]["pyramids"]
    assert result["cell_types"]["tet_wedges"] == parsed["cell_types"]["tet_wedges"]
    assert result["cell_types"]["tetrahedra"] == parsed["cell_types"]["tetrahedra"]
    assert result["cell_types"]["polyhedra"] == parsed["cell_types"]["polyhedra"]
    
    # Verify geometry metrics (with tolerance for floating point)
    def approx_equal(a, b, rel_tol=1e-6):
        """Check if two values are approximately equal"""
        if isinstance(a, list) and isinstance(b, list):
            return all(abs(x - y) < abs(x) * rel_tol + 1e-12 for x, y in zip(a, b))
        return abs(a - b) < abs(a) * rel_tol + 1e-12
    
    assert approx_equal(result["geometry"]["min_volume"], parsed["geometry"]["min_volume"])
    assert approx_equal(result["geometry"]["max_volume"], parsed["geometry"]["max_volume"])
    assert approx_equal(result["geometry"]["total_volume"], parsed["geometry"]["total_volume"])
    assert approx_equal(result["geometry"]["min_face_area"], parsed["geometry"]["min_face_area"])
    assert approx_equal(result["geometry"]["max_face_area"], parsed["geometry"]["max_face_area"])
    assert approx_equal(result["geometry"]["min_edge_length"], parsed["geometry"]["min_edge_length"])
    assert approx_equal(result["geometry"]["max_edge_length"], parsed["geometry"]["max_edge_length"])
    
    # Verify error counts and pass status
    assert result["passed"] == parsed["passed"]
    assert result["topology"]["errors"] == 0
    assert result["geometry"]["errors"] == 0
    assert result["total_errors"] == 0
    assert result["passed"] is True
    
    print(f"\n✓ All values from Python dictionary match parsed OpenFOAM checkMesh output")


if __name__ == "__main__":
    import sys
    sys.path.insert(0, str(Path(__file__).parent.parent.parent))
    cube = Path(__file__).parent / "cube"
    
    test_checkmesh_all_options(cube)
    print("\nAll checkMesh tests passed!")

