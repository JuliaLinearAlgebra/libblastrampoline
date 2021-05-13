function get_translation_unit_spelling(tu)
    cxstr = clang_getTranslationUnitSpelling(tu)
    ptr = clang_getCString(cxstr)
    s = unsafe_string(ptr)
    clang_disposeString(cxstr)
    return s
end

function cu_children_visitor(cursor::CXCursor, parent::CXCursor, list)::Cuint
    push!(list, cursor)
    return CXChildVisit_Continue
end

function children(cursor::CXCursor)
    list = CXCursor[]
    cu_visitor_cb = @cfunction(
        cu_children_visitor, Cuint, (CXCursor, CXCursor, Ref{Vector{CXCursor}})
    )
    GC.@preserve list ccall(
        (:clang_visitChildren, LibClang.libclang),
        UInt32,
        (CXCursor, CXCursorVisitor, Any),
        cursor,
        cu_visitor_cb,
        list,
    )
    return list
end

function get_filename(x::CXFile)
    cxstr = clang_getFileName(x)
    ptr = clang_getCString(cxstr)
    ptr == C_NULL && return ""
    s = unsafe_string(ptr)
    clang_disposeString(cxstr)
    return s
end

function get_cursor_filename(c::CXCursor)
    file = Ref{CXFile}(C_NULL)
    location = clang_getCursorLocation(c)
    clang_getExpansionLocation(location, file, Ref{Cuint}(0), Ref{Cuint}(0), Ref{Cuint}(0))
    if file[] != C_NULL
        str = GC.@preserve file get_filename(file[])
        return str
    else
        return ""
    end
end

function get_cursor_spelling(c::CXCursor)
    cxstr = clang_getCursorSpelling(c)
    ptr = clang_getCString(cxstr)
    s = unsafe_string(ptr)
    clang_disposeString(cxstr)
    return s
end
