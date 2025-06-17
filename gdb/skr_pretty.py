class SkrValuePrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        kind = int(self.val['kind'])
        if kind == 0:
            real_obj = cast(self.val, "SkrConst")
            return real_obj['c'].dereference()

        elif kind == 1:
            real_obj = cast(self.val, "SkrVar")
            return f"'{real_obj['id'].string()}'"

        else:
            return f"<unknown_value_{kind}>"



class ConstantPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        type = int(self.val['type'].dereference()['kind'])
        if type == 0:
            intC = cast(self.val, "IntConstant")
            return str(int(intC['val']))

        elif type == 1:
            floatC = cast(self.val, "FloatConstant")
            return str(float(floatC['val']))
    
        else:
            return f"<unknown_constant_{type}>"



class SkrInstructionPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        kind = int(self.val['kind'])

        if kind == 1:
            skrCopy = cast(self.val, "SkrCopy")
            return f"{SkrValuePrinter(skrCopy['to'].dereference()).to_string()} = {SkrValuePrinter(skrCopy['from'].dereference()).to_string()}"

        return f"<unknown_skr_instr_{kind}>"


def cast(val, targetType):
    return val.address.cast(gdb.lookup_type(targetType).pointer()).dereference()


def skr_value_printer_factory(val):
    if "SkrValue" in str(val.type):
        if "*" in str(val.type):
            return SkrValuePrinter(val.dereference())
        return SkrValuePrinter(val)
    else:
        return None
    
def constant_printer_factory(val):
    if "Constant" in str(val.type):
        return ConstantPrinter(val)
    else:
        return None
    
def skr_instr_printer_factory(val):
    if "SkrInstruction" in str(val.type):
        if "*" in str(val.type):
            return SkrInstructionPrinter(val.dereference())
        return SkrInstructionPrinter(val)
    return None

gdb.pretty_printers.append(skr_value_printer_factory)
gdb.pretty_printers.append(constant_printer_factory)
gdb.pretty_printers.append(skr_instr_printer_factory)
