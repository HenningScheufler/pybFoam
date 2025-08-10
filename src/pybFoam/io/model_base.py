
import os
import json
import yaml
import pybFoam
from pybFoam import dictionary
from pydantic import BaseModel

type_dispatch = {
    pybFoam.Word: lambda v: pybFoam.Word(v),
    pybFoam.vector: lambda v: pybFoam.vector(*v),
    pybFoam.tensor: lambda v: pybFoam.tensor(*v),
    pybFoam.wordList: lambda v: pybFoam.wordList(v),
    pybFoam.scalarField: lambda v: pybFoam.scalarField(v),
    pybFoam.vectorField: lambda v: pybFoam.vectorField(v),
    pybFoam.tensorField: lambda v: pybFoam.tensorField(v),
}

class IOModelBase(BaseModel):

    model_config = {"arbitrary_types_allowed": True}


    
    @classmethod
    def from_file(cls, path):
        ext = os.path.splitext(path)[1].lower()
        if ext in {".yaml", ".yml"}:
            with open(path) as f:
                data = yaml.safe_load(f)
            return cls.from_yaml(data)
        elif ext == ".json":
            with open(path) as f:
                data = json.load(f)
            return cls.from_json(data)
        else:
            # Assume OpenFOAM dictionary
            d = dictionary.read(path)
            return cls.from_ofdict(d)

    @classmethod
    def from_yaml(cls, y):
        return cls._from_mapping(y, source="yaml")

    @classmethod
    def from_json(cls, j):
        return cls._from_mapping(j, source="json")

    @classmethod
    def from_ofdict(cls, d):
        # Convert OpenFOAM dictionary to a mapping for _from_mapping
        mapping = {}
        for field, typ in cls.__annotations__.items():
            if hasattr(d, 'isDict') and d.isDict(field):
                # Always populate nested sub-dictionaries
                if issubclass(typ, IOModelBase):
                    mapping[field] = typ.from_ofdict(d.subDict(field))
            else:
                try:
                    mapping[field] = d.get[typ](field)
                except Exception:
                    # If the field is missing, set to None (let Pydantic handle required/optional)
                    mapping[field] = None
        return cls(**mapping)

    @classmethod
    def _from_mapping(cls, data, source):
        
        mapping = {}

        for field, typ in cls.__annotations__.items():
            if field not in data:
                continue
            val = data[field]
            if isinstance(typ, type) and issubclass(typ, IOModelBase):
                mapping[field] = typ._from_mapping(val, source)
            elif typ in type_dispatch:
                mapping[field] = type_dispatch[typ](val)
            else:
                mapping[field] = val
        return cls(**mapping)
