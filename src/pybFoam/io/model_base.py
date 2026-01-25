from typing import TypeVar, get_origin, get_args, Optional, Union, Annotated, Literal, Any, Type
import os
import json
import yaml

import pybFoam
from pybFoam.pybFoam_core import dictionary
from pydantic import BaseModel, Field

type_dispatch: dict[Type[Any], Any] = {
    str: lambda s: s,  # Keep as string
    int: lambda i: int(i),
    float: lambda f: float(f),
    bool: lambda b: bool(b),
    pybFoam.pybFoam_core.Word: lambda w: pybFoam.pybFoam_core.Word(w),
    pybFoam.pybFoam_core.vector: lambda v: pybFoam.pybFoam_core.vector(*v),
    pybFoam.pybFoam_core.tensor: lambda t: pybFoam.pybFoam_core.tensor(*t),
    pybFoam.pybFoam_core.wordList: lambda w: pybFoam.pybFoam_core.wordList(w),
    pybFoam.pybFoam_core.scalarField: lambda s: pybFoam.pybFoam_core.scalarField(s),
    pybFoam.pybFoam_core.vectorField: lambda v: pybFoam.pybFoam_core.vectorField(v),
    pybFoam.pybFoam_core.tensorField: lambda t: pybFoam.pybFoam_core.tensorField(t),
}

T = TypeVar('T')

def _unwrap_type(tp: Type[T] | Any) -> Type[Any]:
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
    def from_file(cls, path: str) -> Any:
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
    def from_yaml(cls: Type[Any], y: Any) -> Any:
        return cls._from_mapping(y, source="yaml")

    @classmethod
    def from_json(cls: Type[Any], j: Any) -> Any:
        return cls._from_mapping(j, source="json")

    @classmethod
    def from_ofdict(cls: Type[Any], d: Any) -> Any:
        # Convert OpenFOAM dictionary to a mapping for _from_mapping
        mapping: dict[str, Any] = {}
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
    def _from_mapping(cls: Type[Any], data: Any, source: str) -> Any:

        mapping: dict[str, Any] = {}

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
