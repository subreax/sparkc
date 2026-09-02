"""GDB pretty-printers for Spark compiler SKR.  Load: source gdb/skr_pretty.py"""
import gdb


def _type_name(v):
    t = v.type.strip_typedefs()
    if t.code == gdb.TYPE_CODE_REF:
        t = t.target().strip_typedefs()
    return str(t).replace("const ", "").strip()


def _obj(v):
    t = v.type.strip_typedefs()
    if t.code == gdb.TYPE_CODE_PTR:
        return None if int(v) == 0 else v.dereference()
    return v.referenced_value() if t.code == gdb.TYPE_CODE_REF else v


def _cast(v, name):
    v = _obj(v)
    return None if v is None else v.address.cast(gdb.lookup_type(name).pointer()).dereference()


def _str(v):
    v = _obj(v)
    if v is None or int(v["str"]) == 0:
        return "<null>"
    return v["str"].string(length=int(v["length"]))


def _constant(v):
    v = _obj(v)
    if v is None:
        return "<null>"
    kind = int(v["type"].dereference()["kind"])
    if kind == 0:
        return str(int(_cast(v, "IntConstant")["val"]))
    if kind == 1:
        return str(float(_cast(v, "FloatConstant")["val"]))
    return "<unknown Constant kind {}>".format(kind)


def _value(v):
    v = _obj(v)
    if v is None:
        return "<null>"
    kind = int(v["kind"])
    if kind == 0:
        return _constant(_cast(v, "SkrConst")["c"])
    if kind == 1:
        return "'{}'".format(_str(_cast(v, "SkrVar")["id"]))
    return "<unknown SkrValue kind {}>".format(kind)


def _instruction(v):
    v = _obj(v)
    if v is None:
        return "<null>"
    kind = int(v["kind"])
    if kind == 0:
        x = _cast(v, "SkrBinary"); ops = ["+", "-", "*", "/", "%", "==", "!=", "<", "<=", ">", ">="]
        op = int(x["op"]); op = ops[op] if 0 <= op < len(ops) else "<op:{}>".format(op)
        return "{} = {} {} {}".format(_value(x["dst"]), _value(x["left"]), op, _value(x["right"]))
    if kind == 1:
        x = _cast(v, "SkrCopy"); return "{} = {}".format(_value(x["to"]), _value(x["from"]))
    if kind == 2:
        return "jump {}".format(_str(_cast(v, "SkrJump")["label"]))
    if kind == 3:
        return "{}:".format(_str(_cast(v, "SkrLabel")["label"]))
    if kind == 4:
        x = _cast(v, "SkrBranch"); ops = ["==", "!=", "<", "<=", ">", ">="]
        op = int(x["op"]); op = ops[op] if 0 <= op < len(ops) else "<op:{}>".format(op)
        return "branch {} {} {}, {}".format(_value(x["left"]), op, _value(x["right"]), _str(x["label"]))
    if kind == 5:
        x = _cast(v, "SkrFunCall"); args = x["args"]; mem = args["mem"]
        rendered = [_value((mem + i).dereference()) for i in range(int(args["itemsCount"]))]
        call = "call {}({})".format(_str(x["name"]), ", ".join(rendered))
        return "{} = {}".format(_value(x["retVar"]), call) if int(x["retVar"]) else call
    if kind == 6:
        x = _cast(v, "SkrInt2Float"); return "{} = int2float {}".format(_value(x["dst"]), _value(x["src"]))
    if kind == 7:
        x = _cast(v, "SkrFloat2Int"); return "{} = float2int {}".format(_value(x["dst"]), _value(x["src"]))
    if kind == 8:
        x = _cast(v, "SkrLoad"); return "{} = load {}[{}]".format(_value(x["to"]), _value(x["from"]), int(x["fromOffset"]))
    if kind == 9:
        x = _cast(v, "SkrStore"); return "store {}[{}] = {}".format(_value(x["to"]), int(x["toOffset"]), _value(x["from"]))
    if kind == 10:
        x = _cast(v, "SkrGetAddr"); return "{} = addr {}".format(_value(x["to"]), _value(x["var"]))
    if kind == 11:
        x = _cast(v, "SkrCopyToOffset"); return "{}[{}] = {}".format(_value(x["to"]), int(x["toOffset"]), _value(x["from"]))
    if kind == 12:
        x = _cast(v, "SkrCopyFromOffset"); return "{} = {}[{}]".format(_value(x["to"]), _value(x["from"]), int(x["fromOffset"]))
    return "<unknown SkrInstruction kind {}>".format(kind)


class _Printer:
    def __init__(self, value, render, children=None):
        self.value, self.render, self._children = value, render, children
    def to_string(self):
        try: return self.render(self.value)
        except gdb.error as error: return "<SKR printer error: {}>".format(error)
    def children(self):
        if self._children is not None:
            yield from self._children(self.value)


_INSTRUCTION_TYPES = [
    "SkrBinary", "SkrCopy", "SkrJump", "SkrLabel", "SkrBranch", "SkrFunCall",
    "SkrInt2Float", "SkrFloat2Int", "SkrLoad", "SkrStore", "SkrGetAddr",
    "SkrCopyToOffset", "SkrCopyFromOffset",
]


def _instruction_children(value):
    """Expose the fields of the type selected by SkrInstruction::kind."""
    obj = _obj(value)
    if obj is None:
        return
    kind = int(obj["kind"])
    yield ("kind", obj["kind"])
    if not 0 <= kind < len(_INSTRUCTION_TYPES):
        return
    derived = _cast(obj, _INSTRUCTION_TYPES[kind])
    # The first field is the SkrInstruction base subobject, already shown as kind.
    for field in derived.type.fields():
        if field.is_base_class:
            continue
        yield (field.name, derived[field.name])


def skr_printer_factory(value):
    # Exact comparison avoids claiming std::vector<SkrValue*> as a SkrValue.
    name = _type_name(value); bare = name[:-1].strip() if name.endswith("*") else name
    if bare == "SkrValue": return _Printer(value, _value)
    if bare == "SkrInstruction": return _Printer(value, _instruction, _instruction_children)
    if bare == "Constant": return _Printer(value, _constant)
    if bare == "StringRef": return _Printer(value, _str)
    return None


def register_skr_printers(objfile=None):
    printers = gdb.pretty_printers if objfile is None else objfile.pretty_printers
    if skr_printer_factory not in printers: printers.append(skr_printer_factory)


register_skr_printers()
