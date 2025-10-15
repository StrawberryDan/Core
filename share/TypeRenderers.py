import lldb
import re


def VectorSummary(vector, dict, options):
    typename = vector.GetType().GetCanonicalType().GetName()
    template_args = re.search('Vector<(.+),(.+)>', typename)
    valueType = template_args.group(1)
    vectorSize = int(template_args.group(2))

    arrayValue = vector.GetChildMemberWithName("mValue")

    summary = "["
    for i in range(arrayValue.GetNumChildren()):
        summary += arrayValue.GetChildAtIndex(i).GetValue()
        if i != vectorSize - 1:
            summary += ", "
    summary += "]"


    return summary


def MatrixSummary(value, dict, options):
    summary = ""

    typename = value.GetType().GetCanonicalType().GetName()
    template_args = re.search('Matrix<(.+),(.+),(.+)>', typename)
    valueType = template_args.group(1)
    width = int(template_args.group(2))
    height = int(template_args.group(3))

    arrayValue = value.GetChildMemberWithName("mValue")
    for i in range(arrayValue.GetNumChildren()):
        for j in range(arrayValue.GetChildAtIndex(i).GetNumChildren()):
            summary += arrayValue.GetChildAtIndex(i).GetChildAtIndex(j).GetValue()
            if j < arrayValue.GetChildAtIndex(i).GetNumChildren() - 1:
                summary += ", "
        summary += "    "

    return summary




def __lldb_init_module(debugger, _dict):
    debugger.HandleCommand('type summary add -C true -w Strawberry -F TypeRenderers.VectorSummary -x Strawberry::Core::Math::Vector<.+>')
    debugger.HandleCommand('type summary add -C true -w Strawberry -F TypeRenderers.MatrixSummary -x Strawberry::Core::Math::Matrix<.+>')
    debugger.HandleCommand('type category enable Strawberry')


