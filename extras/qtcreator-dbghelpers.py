from dumper import Children

################################################################################

def qdump__sq__maths__Vector(d, value):
    size = value.type[0]
    keys = ["x", "y", "z", "w"][:size]
    d.putValue("[ " + ", ".join([str(value[key].value()) for key in keys]) + " ]")
    d.putNumChild(size)
    if d.isExpanded():
        with Children(d, size):
            for key in keys:
                d.putSubItem(key, value[key])

def qdump__sq__maths__Matrix(d, value):
    w = value.type[0]
    h = value.type[1]
    data = value["data"]
    d.putValue("Matrix%dx%d" % (w, h))
    d.putNumChild(w * h)
    if d.isExpanded():
        with Children(d):
            for x in range(w):
                for y in range(h):
                    d.putSubItem("[%d][%d]" % (x, y), data[x*w+y])

def qdump__sq__maths__Quaternion(d, value):
    keys = ["x", "y", "z", "w"]
    d.putValue("[ " + ", ".join([str(value[key].value()) for key in keys]) + " ]")
    d.putNumChild(4)
    if d.isExpanded():
        with Children(d):
            for key in keys:
                d.putSubItem(key, value[key])

def qdump__sq__maths__RandomRange(d, value):
    size = value.type[0]
    keys = ["min", "max"]
    d.putValue("[ " + ", ".join([str(value[key].value()) for key in keys]) + " ]")
    d.putNumChild(2)
    if d.isExpanded():
        with Children(d):
            for key in keys:
                d.putSubItem(key, value[key])

################################################################################

def qdump__sq__StackString(d, value):
    d.putSimpleCharArray(value["mData"].address())

def qdump__sq__StackVector(d, value):
    data = value["mData"].address()
    size = value["mSize"].integer()
    d.putItemCount(size)
    d.putPlotData(data, size, value.type[0])

################################################################################

def qdump__nlohmann__json_abi_v3_11_2__basic_json(d, value):
    mtype = value["m_type"].integer()
    mvalue = value["m_value"]
    if mtype == 0:
        d.putValue("null")
    elif mtype == 1:
        d.putItem(mvalue["object"])
        d.putValue("object")
    elif mtype == 2:
        d.putItem(mvalue["array"])
        d.putValue("array")
    elif mtype == 3:
        d.putItem(mvalue["string"])
    elif mtype == 4:
        d.putItem(mvalue["boolean"])
    elif mtype == 5:
        d.putItem(mvalue["number_integer"])
    elif mtype == 6:
        d.putItem(mvalue["number_unsigned"])
    elif mtype == 7:
        d.putItem(mvalue["number_float"])
    elif mtype == 8:
        d.putItem(mvalue["binary"])
    elif mtype == 9:
        d.putItem(mvalue["discarded"])
    else:
        d.putValue("invalid type")
    d.putBetterType("sq::JsonValue")
