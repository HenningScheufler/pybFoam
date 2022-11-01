from typing import Literal, Dict
from pybFoam import dictionary
from pydantic import BaseModel, Field
from pybFoam import (
    dictionary,
    Word,
    vector,
    vectorField,
    dictionaryEntry,
    keyType,
    entry,
    wordList,
)

py_to_Foam = {str: Word}

def convert_to_OF(T):
    if type(T) in py_to_Foam:
        return py_to_Foam[type(T)](T)
    return T


def create_ofDict(ofdict: dictionary, d: Dict):
    for k, v in d.items():
        if type(v) == dict:
            ofsubdict = dictionary()
            create_ofDict(ofsubdict, v)
            dEntry = dictionaryEntry(keyType(k), ofdict, ofsubdict)
            ofdict.add(dEntry, False)
        else:
            ofdict.add(k, convert_to_OF(v))
    return ofdict

class FoamFile(BaseModel):
    version: str = "2.0"
    format: str = "ascii"
    class_name: str
    location: str
    object: str

    def dict(self, **kwargs):
        d = super().dict(**kwargs)
        d["class"] = d.pop("class_name")
        return d


class ControlDict(BaseModel):
    FoamFile: FoamFile = FoamFile(
        class_name="dictionary", location='"system"', object='"controlDict"'
    )
    application: str
    startFrom: str = "latestTime"
    startTime: float
    stopAt: str = "endTime"
    endTime: float
    deltaT: float
    writeControl: str = "adjustable"
    writeInterval: float
    purgeWrite: float = 0
    writeFormat: str = "ascii"
    writePrecision: float = 6
    writeCompression: str = "on"
    timeFormat: str = "general"
    timePrecision: float = 6
    runTimeModifiable: str = "yes"
    adjustTimeStep: str = "yes"


