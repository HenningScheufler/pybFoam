from pybFoam import Pstream


def test_master_serial() -> None:
    """In serial, master() should return True."""
    assert Pstream.master() is True


def test_par_run_serial() -> None:
    """In serial, parRun() should return False."""
    assert Pstream.parRun() is False


def test_my_proc_no_serial() -> None:
    """In serial, myProcNo() should return 0."""
    assert Pstream.myProcNo() == 0


def test_nprocs_serial() -> None:
    """In serial, nProcs() should return 1."""
    assert Pstream.nProcs() == 1
