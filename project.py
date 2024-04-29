import lldb

# def __lldb_init_module(debugger, dict):
#     debugger.HandleCommand('type summary add -x "Thing<.+>" -F project.xyz')
    # debugger.HandleCommand('type summary add -x "Thing" -F project.xyz')

def make_string_from_pointer_with_offset(pointer, len):
    try:
        data = pointer.GetPointeeData(0, len)
        bytes_data = bytearray(data.uint8)
        char_list = [chr(byte % 256) for byte in bytes_data]
        return " [size = " + str(len) + "] " + '"' + ''.join(char_list) + '"'
    except Exception as e:
        pass
        return "__error__"

def FixedCharSpan_SummaryProvider(value, dict):
    ptr = value.GetChildMemberWithName('ptr')
    size = value.GetChildMemberWithName('size').GetValueAsSigned()
    return make_string_from_pointer_with_offset(ptr, size)

def StringKey_SummaryProvider(value, dict):
    ptr = value.GetChildMemberWithName('cbuffer')
    size = value.GetChildMemberWithName('length').GetValueAsSigned()
    return make_string_from_pointer_with_offset(ptr, size)

def IntKey_SummaryProvider(value, dict):
    key = value.GetChildMemberWithName('key').GetValueAsSigned()
    return str(key)

class Array_SyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.update()

    def num_children(self):
        return self.child_count + 1

    def get_child_index(self, name):
        try:
            return int(name.lstrip("[").rstrip("]"))
        except:
            return -1

    def update(self):
        self.data_type = self.valobj.GetChildMemberWithName("array").GetType().GetPointeeType()
        self.data_size = self.data_type.GetByteSize()
        self.child_count = self.valobj.GetChildMemberWithName('size').GetValueAsSigned()

    def has_children(self):
        return True

    def get_child_at_index(self, index):
        if(index == 0):
            return self.valobj.CreateChildAtOffset("size", 8, self.valobj.GetChildMemberWithName('size').GetType())
        index -= 1
        if index < 0:
            return None

        if index >= self.num_children():
            return None

        offset = index * self.data_size

        return self.valobj.GetChildMemberWithName('array').CreateChildAtOffset("[" + str(index) + "]", offset, self.data_type)

class List_SyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.update()

    def num_children(self):
        return self.item_count + 2

    def get_child_index(self, name):
        if(name == "size"):
            return 0
        if(name == "capacity"):
            return 1

        return int(name.lstrip("[").rstrip("]"))

    def update(self):
        self.data_type = self.valobj.GetChildMemberWithName("array").GetType().GetPointeeType()
        self.data_size = self.data_type.GetByteSize()
        self.item_count = self.valobj.GetChildMemberWithName('size').GetValueAsSigned()
        self.item_capacity = self.valobj.GetChildMemberWithName('capacity').GetValueAsSigned()

    def has_children(self):
        return True

    def get_child_at_index(self, index):
        if(index == 0):
            return self.valobj.CreateChildAtOffset("size", 8, self.valobj.GetChildMemberWithName('size').GetType())

        if(index == 1):
            return self.valobj.CreateChildAtOffset("capacity", 12, self.valobj.GetChildMemberWithName('capacity').GetType())

        index -= 2

        offset = index * self.data_size

        return self.valobj.GetChildMemberWithName('array').CreateChildAtOffset("[" + str(index) + "]", offset, self.data_type)


def List_Summary(valobj, dict):
    val = valobj.GetChildMemberWithName("size")
    if(val.IsValid()):
        return "size = " + str(valobj.GetChildMemberWithName("size").GetValueAsSigned())
    return ""

def is_map_bit_set(ptr, idx):
    map_idx = idx >> 6  # divide by 64
    shift = idx - (map_idx << 6)  # multiply by 64
    return (ptr[map_idx] & (1 << shift)) != 0

class Map_SyntheticProvider:

    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def update(self):
        index = 0
        size = self.valobj.GetChildMemberWithName("size").GetValueAsSigned()
        self.capacity = 1 << self.valobj.GetChildMemberWithName("exponent").GetValueAsSigned()
        map = self.valobj.GetChildMemberWithName("pMap").GetPointeeData(0, self.capacity).uint64
        keys = self.valobj.GetChildMemberWithName("pKeys")
        values = self.valobj.GetChildMemberWithName("pValues")
        key_type = keys.GetType().GetPointeeType()
        key_size = key_type.GetByteSize()
        self.value_type = values.GetType().GetPointeeType()
        value_size = self.value_type.GetByteSize()

        self.children = []

        found = 0
        while(index < self.capacity and found != size):
            if(is_map_bit_set(map, index)):
                key_offset = index * key_size
                value_offset = index * value_size
                index_str = "[" + str(found) + "]"
                self.children.append(keys.CreateChildAtOffset("key   " + index_str, key_offset, key_type))
                self.children.append(values.CreateChildAtOffset("value" + index_str, value_offset, self.value_type))
                found = found + 1
            index += 1

    def num_children(self):
        return 3 + len(self.children)

    def get_child_index(self, name):
        return name

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.CreateChildAtOffset("size", 8, self.valobj.GetChildMemberWithName('size').GetType())

        if index == 1:
            return self.valobj.CreateValueFromExpression("capacity", f"(int){self.capacity}")

        if index == 2:
            return self.valobj.CreateChildAtOffset("threshold", 4, self.valobj.GetChildMemberWithName('threshold').GetType())

        return self.children[index - 3]
