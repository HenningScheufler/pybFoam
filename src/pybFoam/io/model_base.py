from typing import get_origin, get_args, Optional, Union, Annotated, Literal
import os
import json
import yaml
import pybFoam
from pybFoam import dictionary
from pydantic import BaseModel, Field

type_dispatch = {
    str: lambda v: v,  # Keep as string
    int: lambda v: int(v),
    float: lambda v: float(v),
    bool: lambda v: bool(v),
    pybFoam.Word: lambda v: pybFoam.Word(v),
    pybFoam.vector: lambda v: pybFoam.vector(*v),
    pybFoam.tensor: lambda v: pybFoam.tensor(*v),
    pybFoam.wordList: lambda v: pybFoam.wordList(v),
    pybFoam.scalarField: lambda v: pybFoam.scalarField(v),
    pybFoam.vectorField: lambda v: pybFoam.vectorField(v),
    pybFoam.tensorField: lambda v: pybFoam.tensorField(v),
}


def _unwrap_type(tp):
    # peel Annotated[T, ...] and Union[T, None]
    if get_origin(tp) is Annotated:
        tp = get_args(tp)[0]
    if get_origin(tp) in (Optional, Union):
        args = [a for a in get_args(tp) if a is not type(None)]
        if args:
            tp = args[0]
    if get_origin(tp) is Literal:
        return str
    return tp


class IOModelMixin:
    """
    Mixin class to provide common functionality for IO models.
    """

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
        for name, f in cls.model_fields.items():  # <-- v2 API
            # Pick the key to read from the OpenFOAM dict
            key = f.validation_alias or f.alias or name
            typ = _unwrap_type(f.annotation)

            if hasattr(d, "isDict") and d.isDict(key):
                # Always populate nested sub-dictionaries
                if issubclass(typ, IOModelBase):
                    mapping[key] = typ.from_ofdict(d.subDict(key))
            else:
                try:
                    mapping[key] = d.get[typ](key)
                except Exception as e:
                    continue
        return cls(**mapping)

    @classmethod
    def _from_mapping(cls, data, source):

        mapping = {}

        for field_name, field_info in cls.model_fields.items():
            # Use the field alias if available, otherwise use the field name
            key = field_info.validation_alias or field_info.alias or field_name

            if key not in data:
                continue
            val = data[key]
            typ = _unwrap_type(field_info.annotation)

            if isinstance(typ, type) and issubclass(typ, IOModelBase):
                mapping[field_name] = typ._from_mapping(val, source)
            else:
                dispatch_type = typ
                mapping[field_name] = type_dispatch[dispatch_type](val)
        return cls(**mapping)


class IOModelBase(IOModelMixin, BaseModel):

    model_config = {"arbitrary_types_allowed": True}
