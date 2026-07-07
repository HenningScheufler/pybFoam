// Smoke test: verify pybFoamEmbed headers and exported target link cleanly.
// Not meant to run — compiling and linking is the signal.

#include "pyInterp.hpp"

extern "C" void pybFoamEmbed_smoke(Foam::Time& time)
{
    Foam::pyInterp::New(time);
}
