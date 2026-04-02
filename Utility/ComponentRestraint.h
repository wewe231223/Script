#pragma once
#include <concepts>
#include <tuple>
#include <utility>
#include <type_traits>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

namespace Game {
    struct ComponentInspectionField;
}

template <typename T>
concept TrivialComponent =
std::is_trivially_copyable_v<T> &&
std::is_trivially_destructible_v<T> &&
std::is_standard_layout_v<T>;

template <typename TComponent, typename... TMemberPointers>
struct LuaComponentDefinition final {
    using ComponentType = TComponent;
    using FieldTuple = std::tuple<std::pair<const char*, TMemberPointers>...>;

    const char* mTypeName{};
    FieldTuple mFieldBindings{};
};

template <typename TComponent, typename... TMemberPointers>
constexpr LuaComponentDefinition<TComponent, TMemberPointers...> MakeLuaComponentDefinition(const char* TypeName, std::pair<const char*, TMemberPointers>... FieldBindings) {
    LuaComponentDefinition<TComponent, TMemberPointers...> Definition{};
    Definition.mTypeName = TypeName;
    Definition.mFieldBindings = typename LuaComponentDefinition<TComponent, TMemberPointers...>::FieldTuple{ FieldBindings... };
    return Definition;
}

template <typename TComponent>
struct LuaComponentDefinitionTraits;

template <typename TComponent>
concept HasLuaComponentDefinition = requires {
    { LuaComponentDefinitionTraits<TComponent>::Create() };
};

template <typename TType, typename... TMemberPointers>
struct LuaTypeDefinition final {
    using ValueType = TType;
    using BindingTuple = std::tuple<std::pair<const char*, TMemberPointers>...>;

    const char* mTypeName{};
    BindingTuple mBindings{};
};

template <typename TType, typename... TMemberPointers>
constexpr LuaTypeDefinition<TType, TMemberPointers...> MakeLuaTypeDefinition(const char* TypeName, std::pair<const char*, TMemberPointers>... Bindings) {
    LuaTypeDefinition<TType, TMemberPointers...> Definition{};
    Definition.mTypeName = TypeName;
    Definition.mBindings = typename LuaTypeDefinition<TType, TMemberPointers...>::BindingTuple{ Bindings... };
    return Definition;
}

template <typename TType>
struct LuaTypeDefinitionTraits;

template <typename TType>
concept HasLuaTypeDefinition = requires {
    { LuaTypeDefinitionTraits<TType>::Create() };
};

#define COMPONENT_DECL_DECLARE_FIELD_HELPER(R, Data, Element) BOOST_PP_TUPLE_ELEM(2, 0, Element) BOOST_PP_TUPLE_ELEM(2, 1, Element){};
#define COMPONENT_DECL_DECLARE_METHOD_HELPER(R, Data, Element) BOOST_PP_TUPLE_ELEM(2, 0, Element);
#define COMPONENT_DECL_APPEND_FIELD_BINDING_HELPER(R, TypeName, Element) , std::pair<const char*, decltype(&TypeName::BOOST_PP_TUPLE_ELEM(2, 1, Element))>{ BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 1, Element)), &TypeName::BOOST_PP_TUPLE_ELEM(2, 1, Element) }
#define COMPONENT_DECL_APPEND_METHOD_BINDING_HELPER(R, TypeName, Element) , std::pair<const char*, decltype(&TypeName::BOOST_PP_TUPLE_ELEM(2, 1, Element))>{ BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 1, Element)), &TypeName::BOOST_PP_TUPLE_ELEM(2, 1, Element) }
#define ComponentField(Type, Name) ((Type, Name))
#define ComponentMethod(Signature, Name) ((Signature, Name))
#define ComponentFields(...) __VA_ARGS__
#define ComponentMethods(...) __VA_ARGS__
#define LUA_TYPE_APPEND_FIELD_BINDING_HELPER(R, TypeName, Element) , std::pair<const char*, decltype(&TypeName::BOOST_PP_TUPLE_ELEM(2, 1, Element))>{ BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 1, Element)), &TypeName::BOOST_PP_TUPLE_ELEM(2, 1, Element) }
#define LUA_TYPE_APPEND_METHOD_BINDING_HELPER(R, TypeName, Element) , std::pair<const char*, decltype(&TypeName::BOOST_PP_TUPLE_ELEM(2, 1, Element))>{ BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 1, Element)), &TypeName::BOOST_PP_TUPLE_ELEM(2, 1, Element) }

/*
ComponentDecl 사용 규칙:
1) FieldsSeq에는 ComponentFields(...) 안에 ComponentField(Type, Name)을 작성한다.
2) MethodsSeq에는 ComponentMethods(...) 안에 ComponentMethod(Signature, Name)을 작성한다.
3) 메서드가 없으면 BOOST_PP_SEQ_NIL을 전달한다.

예시:
ComponentDecl(Vec3,
    ComponentFields(
        ComponentField(float, mX)
        ComponentField(float, mY)
        ComponentField(float, mZ)
    ),
    ComponentMethods(
        ComponentMethod(float GetLengthSquared() const, GetLengthSquared)
    ));
*/

#define ComponentDecl(TypeName, FieldsSeq, MethodsSeq) \
struct TypeName { \
    BOOST_PP_SEQ_FOR_EACH(COMPONENT_DECL_DECLARE_FIELD_HELPER, _, FieldsSeq) \
    BOOST_PP_SEQ_FOR_EACH(COMPONENT_DECL_DECLARE_METHOD_HELPER, _, MethodsSeq) \
}; \
static_assert(TrivialComponent<TypeName>, \
"\n\n[FATAL ERROR] Invalid Component Layout: " #TypeName \
"\n- Reason: All components must be Trivially Copyable and have Standard Layout.\n"); \
template <> \
struct LuaComponentDefinitionTraits<TypeName> final { \
    static constexpr auto Create() { \
        return MakeLuaComponentDefinition<TypeName>(BOOST_PP_STRINGIZE(TypeName) BOOST_PP_SEQ_FOR_EACH(COMPONENT_DECL_APPEND_FIELD_BINDING_HELPER, TypeName, FieldsSeq) BOOST_PP_SEQ_FOR_EACH(COMPONENT_DECL_APPEND_METHOD_BINDING_HELPER, TypeName, MethodsSeq)); \
    } \
}

#define LuaTypeDefinitionDeclWithName(TypeName, LuaTypeName, FieldsSeq, MethodsSeq) \
template <> \
struct LuaTypeDefinitionTraits<TypeName> final { \
    static constexpr auto Create() { \
        return MakeLuaTypeDefinition<TypeName>(LuaTypeName BOOST_PP_SEQ_FOR_EACH(LUA_TYPE_APPEND_FIELD_BINDING_HELPER, TypeName, FieldsSeq) BOOST_PP_SEQ_FOR_EACH(LUA_TYPE_APPEND_METHOD_BINDING_HELPER, TypeName, MethodsSeq)); \
    } \
}

#define LuaTypeDefinitionDecl(TypeName, FieldsSeq, MethodsSeq) LuaTypeDefinitionDeclWithName(TypeName, BOOST_PP_STRINGIZE(TypeName), FieldsSeq, MethodsSeq)
