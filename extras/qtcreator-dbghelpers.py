from dumper import Children
import stdtypes

################################################################################
#   some fixes for aliases that qtcreator seems to have trouble with           #
################################################################################

# std::tuple currently lacks a qtcreator printer
#def qdump__sq__Tuple(d, value):
#    return stdtypes.qdump__std__tuple(d, value)

#def qdump__sq__Pair(d, value):
#    return stdtypes.qdump__std__pair(d, value)

#def qform__sq__Array():
#    return stdtypes.qform__std__array()
#def qdump__sq__Array(d, value):
#    return stdtypes.qdump__std__array(d, value)

#def qform__sq__Vector():
#    return stdtypes.qform__std__vector()
#def qdump__sq__Vector(d, value):
#    return stdtypes.qdump__std__vector(d, value)

#def qdump__sq__Optional(d, value):
#    return stdtypes.qdump__std__optional(d, value)

# std::unique_ptr has a qtcreator printer, but it isn't used
#def qdump__sq__UniquePtr(d, value):
#    return stdtypes.qdump__std__unique_ptr(d, value)

#def qdump__sq__InitList(d, value):
#    return stdtypes.qdump__std__initializer_list(d, value)

def qform__sq__String():
    return stdtypes.qform__std__string()
def qdump__sq__String(d, value):
    return stdtypes.qdump__std__string(d, value)

# std::string_view currently lacks a qtcreator printer
#def qform__sq__StringView():
#    return stdtypes.qform__std__string_view()
#def qdump__sq__StringView(d, value):
#    return stdtypes.qdump__std__string_view(d, value)

################################################################################
#   special printers for sqee maths types, could be improved                   #
################################################################################

LETTERS = [ 'x', 'y', 'z', 'w' ]

def qdump__sq__maths__Vector(d, value):
    d.putValue(value.type.name)
    data, size = value["data"], value.type[0]
    d.putNumChild(size)
    if d.isExpanded():
        with Children(d):
            for i in range(size):
                d.putSubItem(LETTERS[i], data[i])

def qdump__sq__maths__Quaternion(d, value):
    d.putValue(value.type.name)
    data = value["data"]
    d.putNumChild(4)
    if d.isExpanded():
        with Children(d):
            for i in range(4):
                d.putSubItem(LETTERS[i], data[i])
