import lldb
import re


def VectorSummary(vector, dict, options):
    arrayValue = vector.GetChildMemberWithName("mValue")

    summary = "["
    for i in range(arrayValue.GetNumChildren()):
        summary += arrayValue.GetChildAtIndex(i).GetValue()
        if i != arrayValue.GetNumChildren() - 1:
            summary += ", "
    summary += "]"


    return summary


def MatrixSummary(value, dict, options):
    summary = "["

    arrayValue = value.GetChildMemberWithName("mValue")
    for i in range(arrayValue.GetNumChildren()):
        for j in range(arrayValue.GetChildAtIndex(i).GetNumChildren()):
            summary += arrayValue.GetChildAtIndex(i).GetChildAtIndex(j).GetValue()
            if j < arrayValue.GetChildAtIndex(i).GetNumChildren() - 1:
                summary += ", "
        summary += "] "
        if i != arrayValue.GetNumChildren() - 1:
            summary += "["

    return summary


def OptionalSummary(value, dict, options):
    has_value = value.GetChildMemberWithName("mHasValue").GetValue() == "true"

    if has_value:
        return "value.GetChildAtIndex(1).GetChildAtIndex(0).GetValue()
    else:
        return "None"




def __lldb_init_module(debugger, _dict):
    debugger.HandleCommand('type summary add -C true -w Strawberry -F TypeRenderers.OptionalSummary -x Strawberry::Core::Optional<.+>')
    debugger.HandleCommand('type summary add -C true -w Strawberry -F TypeRenderers.VectorSummary -x Strawberry::Core::Math::Vector<.+>')
    debugger.HandleCommand('type summary add -C true -w Strawberry -F TypeRenderers.MatrixSummary -x Strawberry::Core::Math::Matrix<.+>')
    debugger.HandleCommand('type category enable Strawberry')


