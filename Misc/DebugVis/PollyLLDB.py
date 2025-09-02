import sys
from itertools import count
import lldb


def summary_image(obj, dict):
    impl = obj.GetChildMemberWithName('_impl').Dereference()
    width = impl.GetChildMemberWithName('width')
    height = impl.GetChildMemberWithName('height')
    name = impl.GetChildMemberWithName('_asset_name')
    return f'{name} ({width}x{height})'


def summary_string(obj, dict):
    original_value = obj.GetNonSyntheticValue()
    size = original_value.GetChildMemberWithName('_size').GetValueAsUnsigned()

    if size == 0:
        return '""'
    else:
        # Fetch string contents
        data = original_value.GetChildMemberWithName('_data')
        string = data.GetPointeeData(0, size)

        # Append null character
        string.Append(lldb.SBData.CreateDataFromInt(0))

        error = lldb.SBError()
        string = string.GetString(error, 0)

        if error.Fail():
            return '<error: ' + error.GetCString() + '>'
        else:
            return '"' + string + '"'


def get_value_of_any(tag, buffer):
    if tag == 0:
        return None

    target = lldb.debugger.GetSelectedTarget()

    if tag == 1:
        targetType = target.GetBasicType(lldb.eBasicTypeChar)
    elif tag == 2:
        targetType = target.GetBasicType(lldb.eBasicTypeUChar)
    elif tag == 3:
        targetType = target.GetBasicType(lldb.eBasicTypeShort)
    elif tag == 4:
        targetType = target.GetBasicType(lldb.eBasicTypeUShort)
    elif tag == 5:
        targetType = target.GetBasicType(lldb.eBasicTypeInt)
    elif tag == 6:
        targetType = target.GetBasicType(lldb.eBasicTypeUInt)
    elif tag == 7:
        targetType = target.GetBasicType(lldb.eBasicTypeFloat)
    elif tag == 8:
        targetType = target.GetBasicType(lldb.eBasicTypeDouble)
    elif tag == 9:
        targetType = target.GetBasicType(lldb.eBasicTypeBool)
    elif tag == 10:
        targetType = target.FindFirstType('Vec2')
    elif tag == 11:
        targetType = target.FindFirstType('Vec3')
    elif tag == 12:
        targetType = target.FindFirstType('Vec4')
    elif tag == 13:
        targetType = target.FindFirstType('Color')
    elif tag == 14:
        targetType = target.FindFirstType('Matrix')
    elif tag == 15:
        targetType = target.FindFirstType('String')
    elif tag == 16:
        targetType = target.FindFirstType('StringView')
    elif tag == 17:
        targetType = target.GetBasicType(lldb.eBasicTypeVoidPointer)
    else:
        return None

    return buffer.Cast(targetType).Clone('value')


def summary_any(obj, dict):
    originalValue = obj.GetNonSyntheticValue()
    tag = originalValue.GetChildMemberWithName('_type').GetValueAsUnsigned()
    buffer = originalValue.GetChildMemberWithName('_buffer')

    value = get_value_of_any(tag, buffer)

    if value is None:
        return 'empty'
    else:
        return value


def summary_vec2(obj, dict):
    x = obj.GetChildMemberWithName('x').GetValue()
    y = obj.GetChildMemberWithName('y').GetValue()

    return f'[{x}; {y}]'


def summary_vec3(obj, dict):
    x = obj.GetChildMemberWithName('x').GetValue()
    y = obj.GetChildMemberWithName('y').GetValue()
    z = obj.GetChildMemberWithName('z').GetValue()

    return f'[{x}; {y}; {z}]'


def summary_vec4(obj, dict):
    x = obj.GetChildMemberWithName('x').GetValue()
    y = obj.GetChildMemberWithName('y').GetValue()
    z = obj.GetChildMemberWithName('z').GetValue()
    w = obj.GetChildMemberWithName('w').GetValue()

    return f'[{x}; {y}; {z}; {w}]'


def summary_color(obj, dict):
    r = obj.GetChildMemberWithName('r').GetValue()
    g = obj.GetChildMemberWithName('g').GetValue()
    b = obj.GetChildMemberWithName('b').GetValue()
    a = obj.GetChildMemberWithName('a').GetValue()

    return f'[r={r}; g={g}; b={b}; a={a}]'


def summary_rectf(obj, dict):
    x = obj.GetChildMemberWithName('x').GetValue()
    y = obj.GetChildMemberWithName('y').GetValue()
    width = obj.GetChildMemberWithName('width').GetValue()
    height = obj.GetChildMemberWithName('height').GetValue()

    return f'[x={x}; y={y}; width={width}; height={height}]'


def summary_uniqueptr(obj, dict):
    ptr = obj.GetChildMemberWithName('_ptr')

    if ptr.GetValueAsUnsigned() == 0:
        return 'empty'
    else:
        return GetSummaryOrValue(ptr)


def summary_degrees(obj, dict):
    value = obj.GetChildMemberWithName('value').GetValue()

    return f'{value} deg'


def summary_radians(obj, dict):
    value = obj.GetChildMemberWithName('value').GetValue()

    return f'{value} rad'


def summary_list(obj, dict):
    originalValue = obj.GetNonSyntheticValue()
    _data = originalValue.GetChildMemberWithName('_data')
    _size = _data.GetChildMemberWithName('_size')
    size = _size.GetValueAsUnsigned()

    if size == 0:
        result = 'empty'
    else:
        result = f'[size={size}]'

        capacity = _data.GetChildMemberWithName('_capacity').GetValueAsUnsigned()
        inlineCapacity = _data.GetChildMemberWithName('_inline_capacity').GetValueAsUnsigned()

        if capacity <= inlineCapacity:
            result += ' (inlined)'

    return result


def summary_maybe(obj, dict):
    originalValue = obj.GetNonSyntheticValue()
    _is_active = originalValue.GetChildMemberWithName('_is_active')

    has_value = False

    if _is_active.GetByteSize() == 0 or _is_active is None:
        _value = originalValue.GetChildMemberWithName('_value')
        has_value = _value.GetValueAsUnsigned() != 0
    else:
        has_value = _is_active.GetValueAsUnsigned() == 1

    if has_value:
        value = originalValue.GetChildMemberWithName('_value')
        return GetSummaryOrValue(value)
    else:
        return 'none'


def summary_sprite(obj, dict):
    return 'Some Sprite!'


def GetSummaryOrValue(obj):
    result = obj.GetSummary()
    if result is not None:
        return result

    result = obj.GetValue()
    if result is not None:
        return result

    return obj


def summary_pair(obj, dict):
    first = GetSummaryOrValue(obj.GetChildMemberWithName('first'))
    second = GetSummaryOrValue(obj.GetChildMemberWithName('second'))

    str = f'[first={first}; second={second}]'

    if len(str) < 50:
        return str
    else:
        return ''


def summary_arraylike(obj, dict):
    size = obj.GetNonSyntheticValue().GetChildMemberWithName('_size').GetValueAsUnsigned()
    if size == 0:
        return 'empty'
    else:
        return f'[size={size}]'


def summary_sortedset_and_sortedmap(obj, dict):
    originalValue = obj.GetNonSyntheticValue()
    tree = originalValue.GetChildMemberWithName('_tree')
    size = tree.GetChildMemberWithName('_size').GetValueAsUnsigned()
    if size == 0:
        return 'empty'
    else:
        return f'[size={size}]'


class ListChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self._data = None
        self._size = None
        self._capacity = None
        self.v_size = 0
        self.data_type = None
        self.v_data_size = None
        self.update()

    def num_children(self):
        # Capacity is always available.
        return 1 + self.v_size

    def get_child_index(self, name):
        return 0

    def get_child_at_index(self, index):
        if index == 0:
            return self._capacity.Clone('capacity')
        else:
            index -= 1
            offset = index * self.v_data_size
            return self._data.CreateChildAtOffset('[' + str(index) + ']', offset, self.data_type)

    def update(self):
        data = self.valueObject.GetChildMemberWithName('_data')
        self._data = data.GetChildMemberWithName('_data_ptr')
        self._size = data.GetChildMemberWithName('_size')
        self._capacity = data.GetChildMemberWithName('_capacity')
        self.v_size = self._size.GetValueAsUnsigned()
        self.data_type = self.valueObject.GetType().GetTemplateArgumentType(0)
        self.v_data_size = self.data_type.GetByteSize()

    def has_children(self):
        return True


class LinkedListChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self.count = 0
        self.update()

    def num_children(self):
        return self.count

    def get_child_index(self, name):
        return 0

    def get_child_at_index(self, index):
        node = self.valueObject.GetChildMemberWithName('_front')
        i = index
        while i > 0:
            node = node.GetChildMemberWithName('next')
            i = i - 1

        return node.GetChildMemberWithName('value').Clone(f'[{index}]')

    def update(self):
        self.count = self.valueObject.GetChildMemberWithName('_size').GetValueAsUnsigned()

    def has_children(self):
        return self.count > 0


class ArrayChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self.items = None
        self.count = 0
        self.update()

    def num_children(self):
        return self.count

    def get_child_index(self, name):
        return 0

    def get_child_at_index(self, index):
        return self.items.GetChildAtIndex(index)

    def update(self):
        self.items = self.valueObject.GetChildMemberWithName('items');
        self.count = len(self.items)

    def has_children(self):
        return True


def GetLeft(node):
    return node.GetChildMemberWithName('left')


def GetRight(node):
    return node.GetChildMemberWithName('right')


def GetParent(node):
    return node.GetChildMemberWithName('parent')


def IsNull(obj):
    return obj.GetValueAsUnsigned() == 0


def EqualPtr(lhs, rhs):
    return lhs.GetValueAsUnsigned() == rhs.GetValueAsUnsigned()


class SortedSetChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self.childCount = 0
        self.items = []
        self.update()

    def num_children(self):
        return self.childCount

    def get_child_index(self, name):
        return 0

    def get_child_at_index(self, index):
        return self.items[index].GetChildMemberWithName('value').Clone(f'[{index}]')

    def update(self):
        tree = self.valueObject.GetChildMemberWithName('_tree')
        size = tree.GetChildMemberWithName('_size').GetValueAsUnsigned()
        self.childCount = size

        self.items.clear()
        node = tree.GetChildMemberWithName('_root_node')

        while not IsNull(node) and not IsNull(GetLeft(node)):
            node = GetLeft(node)

        self.items.append(node)
        nextIsLeft = False

        while not IsNull(node):
            if nextIsLeft and not IsNull(GetLeft(node)):
                node = GetLeft(node)
            elif not IsNull(GetRight(node)):
                node = GetRight(node)

                while not IsNull(node) and not IsNull(GetLeft(node)):
                    node = GetLeft(node)

                nextIsLeft = True
            elif not IsNull(GetParent(node)):
                while not IsNull(node) and not IsNull(GetParent(node)) and EqualPtr(node, GetRight(
                        GetParent(node))):
                    node = GetParent(node)

                if not IsNull(node) and not IsNull(GetParent(node)):
                    node = GetParent(node)
                else:
                    break

                nextIsLeft = False
            else:
                break

            self.items.append(node)

    def has_children(self):
        return self.childCount > 0


class SortedMapChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self.childCount = 0
        self.items = []
        self.keyType = valueObject.GetType().GetTemplateArgumentType(0)
        self.update()

    def num_children(self):
        return self.childCount

    def get_child_index(self, name):
        return 0

    def get_child_at_index(self, index):
        return self.items[index].GetChildMemberWithName('value').Clone(f'[{index}]')

    def update(self):
        tree = self.valueObject.GetChildMemberWithName('_tree')
        size = tree.GetChildMemberWithName('_size').GetValueAsUnsigned()
        self.childCount = size

        self.items.clear()
        node = tree.GetChildMemberWithName('_root_node')

        while not IsNull(node) and not IsNull(GetLeft(node)):
            node = GetLeft(node)

        self.items.append(node)
        nextIsLeft = False

        while not IsNull(node):
            if nextIsLeft and not IsNull(GetLeft(node)):
                node = GetLeft(node)
            elif not IsNull(GetRight(node)):
                node = GetRight(node)

                while not IsNull(node) and not IsNull(GetLeft(node)):
                    node = GetLeft(node)

                nextIsLeft = True
            elif not IsNull(GetParent(node)):
                while not IsNull(node) and not IsNull(GetParent(node)) and EqualPtr(node, GetRight(
                        GetParent(node))):
                    node = GetParent(node)

                if not IsNull(node) and not IsNull(GetParent(node)):
                    node = GetParent(node)
                else:
                    break

                nextIsLeft = False
            else:
                break

            self.items.append(node)

    def has_children(self):
        return self.childCount > 0


class MaybeChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self.isActive = False
        self.isRef = False
        self.m_value = None
        self.update()

    def num_children(self):
        if self.isActive:
            return 1
        else:
            return 0

    def get_child_at_index(self, index):
        if self.isActive and index == 0:
            if self.isRef:
                return self.m_value.Dereference().Clone('ref')
            else:
                return self.m_value.Clone('value')
        else:
            return None

    def update(self):
        _is_active = self.valueObject.GetChildMemberWithName('_is_active')
        m_value = self.valueObject.GetChildMemberWithName('_value')

        if _is_active is None or _is_active.GetByteSize() == 0:
            # It's a Maybe<T&>
            self.isActive = True
            self.isRef = True
            self.m_value = m_value
        else:
            self.isActive = _is_active.GetValueAsUnsigned() == 1
            self.isRef = False
            if self.isActive:
                self.m_value = m_value
            else:
                self.m_value = None

        # if self.isActive:
        # templateType = self.valueObject.GetType().GetTemplateArgumentType(0)
        # self.m_value = self.valueObject.GetChildAtIndex(0).Cast(templateType)
        #     self.m_value = m_value
        # else:
        #     self.m_value = None

    def has_children(self):
        return self.isActive


class AnyChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self.tag = None
        self.value = None
        self.update()

    def num_children(self):
        if self.value is None:
            return 0
        else:
            return 2

    def get_child_at_index(self, index):
        if index == 0:
            return self.tag
        elif index == 1:
            return self.value
        else:
            return None

    def update(self):
        self.tag = self.valueObject.GetChildMemberWithName('_type').Clone('type')
        buffer = self.valueObject.GetChildMemberWithName('_buffer')
        self.value = get_value_of_any(self.tag.GetValueAsUnsigned(), buffer)

    def has_children(self):
        return self.value is not None


class StringChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self.child_count = 0
        self.m_data = None
        self.m_size = None
        self.v_is_small = None
        self.m_capacity = None
        self.is_view = valueObject.GetTypeName() == 'StringView'

        self.extra_children = 2  # Data + Size by default

        # For String, we also show the capacity and small-string status
        if not self.is_view:
            self.extra_children += 2

        self.update()

    def num_children(self):
        if self.m_size.GetValueAsUnsigned() == 0:
            return 0
        return self.child_count

    def get_child_at_index(self, index):
        if index == 0:
            return self.m_data
        elif index == 1:
            return self.m_size
        elif index == 2 and not self.is_view:
            return self.m_capacity
        elif index == 3 and not self.is_view:
            return self.v_is_small
        else:
            char_index = index - self.extra_children
            ch = self.m_data.GetPointeeData(char_index)
            target = lldb.debugger.GetSelectedTarget()
            charType = target.GetBasicType(lldb.eBasicTypeChar)
            val = target.CreateValueFromData(f'[{char_index}]', ch, charType)
            return val

    def update(self):
        self.m_data = self.valueObject.GetChildMemberWithName('_data').Clone('data')
        self.m_size = self.valueObject.GetChildMemberWithName('_size').Clone('size')

        string_size = self.m_size.GetValueAsUnsigned()

        if not self.is_view:
            target = lldb.debugger.GetSelectedTarget()
            boolType = target.GetBasicType(lldb.eBasicTypeBool)
            d_is_small = lldb.SBData.CreateDataFromInt(string_size <= 31)

            self.m_capacity = self.valueObject.GetChildMemberWithName('_capacity').Clone('capacity')
            self.v_is_small = target.CreateValueFromData(f'is small', d_is_small, boolType)

        self.child_count = self.extra_children + string_size

    def has_children(self):
        return True


class SpanChildrenProvider:
    def __init__(self, valueObject, internalDict):
        self.valueObject = valueObject
        self.m_data = None
        self.m_size = None
        self.v_size = 0
        self.data_type = None
        self.v_data_size = None
        self.update()

    def num_children(self):
        return self.v_size

    def get_child_index(self, name):
        return 0

    def get_child_at_index(self, index):
        offset = index * self.v_data_size
        return self.m_data.CreateChildAtOffset('[' + str(index) + ']', offset, self.data_type)

    def update(self):
        self.m_data = self.valueObject.GetChildMemberWithName('_data')
        self.m_size = self.valueObject.GetChildMemberWithName('_size')
        self.v_size = self.m_size.GetValueAsUnsigned()
        self.data_type = self.valueObject.GetType().GetTemplateArgumentType(0)
        self.v_data_size = self.data_type.GetByteSize()

    def has_children(self):
        return self.v_size > 0


def __lldb_init_module(debugger, dict):
    print('Polly LLDB debugging modules loaded. https://polly2d.org.')

    debugger.HandleCommand('type summary add pl::Vec2 -F polly.summary_vec2')
    debugger.HandleCommand('type summary add pl::Vec2i -F polly.summary_vec2')
    debugger.HandleCommand('type summary add pl::Vec2ui -F polly.summary_vec2')

    debugger.HandleCommand('type summary add pl::Vec3 -F polly.summary_vec3')
    debugger.HandleCommand('type summary add pl::Vec3i -F polly.summary_vec3')

    debugger.HandleCommand('type summary add pl::Vec4 -F polly.summary_vec4')
    debugger.HandleCommand('type summary add pl::Vec4i -F polly.summary_vec4')

    debugger.HandleCommand('type summary add pl::Color -F polly.summary_color')
    debugger.HandleCommand('type summary add pl::Rectf -F polly.summary_rectf')
    debugger.HandleCommand('type summary add pl::Degrees -F polly.summary_degrees')
    debugger.HandleCommand('type summary add pl::Radians -F polly.summary_radians')
    debugger.HandleCommand('type summary add pl::sprite -F polly.summary_sprite')
    debugger.HandleCommand('type summary add pl::StringView -F polly.summary_string')
    debugger.HandleCommand('type summary add pl::String -F polly.summary_string')
    debugger.HandleCommand('type summary add pl::Any -F polly.summary_any')
    debugger.HandleCommand('type summary add pl::ByteBlob -F polly.summary_arraylike')
    debugger.HandleCommand('type summary add -x "^pl::Pair<.+>$" -F polly.summary_pair')
    debugger.HandleCommand('type summary add -x "^pl::UniquePtr<.+>$" -F polly.summary_uniqueptr')
    debugger.HandleCommand('type summary add -x "^pl::Maybe<.+>$" -F polly.summary_maybe')
    debugger.HandleCommand('type summary add -x "^pl::List<.+>$" -F polly.summary_list')
    debugger.HandleCommand('type summary add -x "^pl::LinkedList<.+>$" -F polly.summary_arraylike')
    debugger.HandleCommand('type summary add -x "^pl::SortedSet<.+>$" -F polly.summary_sortedset_and_sortedmap')
    debugger.HandleCommand('type summary add -x "^pl::SortedMap<.+>$" -F polly.summary_sortedset_and_sortedmap')
    debugger.HandleCommand('type summary add -x "^pl::Span<.+>$" -F polly.summary_arraylike')
    debugger.HandleCommand('type summary add -x "^pl::MutableSpan<.+>$" -F polly.summary_arraylike')

    debugger.HandleCommand(
        'type synthetic add -x "^pl::Maybe<.+>$" --python-class polly.MaybeChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add pl::Any -python-class polly.AnyChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add -x "^pl::List<.+>$" --python-class polly.ListChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add -x "^LinkedList<.+>$" --python-class polly.LinkedListChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add -x "^pl::Span<.+>$" --python-class polly.SpanChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add -x "^pl::MutableSpan<.+>$" --python-class polly.SpanChildrenProvider')

    # debugger.HandleCommand(
    #     'type synthetic add -x "^pl::Array<.+>$" --python-class polly.ArrayChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add -x "^pl::SortedSet<.+>$" --python-class polly.SortedSetChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add -x "pl::SortedMap" --python-class polly.SortedMapChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add pl::String --python-class polly.StringChildrenProvider')

    debugger.HandleCommand(
        'type synthetic add pl::StringView --python-class polly.StringChildrenProvider')

    debugger.HandleCommand('type summary add pl::ShaderParameter --summary-string "${var.name}"')
