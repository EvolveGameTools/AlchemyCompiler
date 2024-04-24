#pragma once

#include "./TypeInfo.h"
#include "./ExpressionBuilder.h"

namespace Alchemy::Compilation {

    namespace TypeUtil {

        bool CanRead(Expression* expression) {
            if (expression == nullptr) {
                return false;
            }
            switch (expression->expressionType) {
                case ExpressionType::FieldAccess: {
                    FieldAccessExpression* f = (FieldAccessExpression*) expression;
                    return CanRead(f);
                }
                default:
                    return true;
            }
        }

        int32 GetByteSize(BuiltInTypeName name) {
            switch (name) {

                case BuiltInTypeName::Void:
                case BuiltInTypeName::Invalid:
                    return 0;

                case BuiltInTypeName::Bool:
                case BuiltInTypeName::Char:
                case BuiltInTypeName::Byte:
                case BuiltInTypeName::Sbyte:
                    return 1;

                case BuiltInTypeName::Short:
                case BuiltInTypeName::Ushort:
                    return 2;

                case BuiltInTypeName::Int:
                case BuiltInTypeName::Uint:
                case BuiltInTypeName::Float:
                case BuiltInTypeName::Color32:
                    return 4;

                case BuiltInTypeName::Int2:
                case BuiltInTypeName::Uint2:
                case BuiltInTypeName::Float2:
                case BuiltInTypeName::Color64:
                case BuiltInTypeName::Long:
                case BuiltInTypeName::Ulong:
                case BuiltInTypeName::Double:
                case BuiltInTypeName::Dynamic:
                case BuiltInTypeName::String:
                case BuiltInTypeName::Object:
                    return 8;

                case BuiltInTypeName::Int3:
                case BuiltInTypeName::Uint3:
                case BuiltInTypeName::Float3:
                    return 12;

                case BuiltInTypeName::Uint4:
                case BuiltInTypeName::Int4:
                case BuiltInTypeName::Float4:
                case BuiltInTypeName::Color:
                    return 16;

            }
        }

        bool IsImplicitNumericConversion(ResolvedType& source, ResolvedType& destination) {
            BuiltInTypeName tcSource = source.builtInTypeName;
            BuiltInTypeName tcDest = destination.builtInTypeName;

            switch (tcSource) {

                case BuiltInTypeName::Sbyte:
                    switch (tcDest) {
                        case BuiltInTypeName::Int16:
                        case BuiltInTypeName::Int32:
                        case BuiltInTypeName::Int64:
                        case BuiltInTypeName::Single:
                        case BuiltInTypeName::Double:
                            return true;
                        default:
                            return false;
                    }

                case BuiltInTypeName::Byte:
                    switch (tcDest) {
                        case BuiltInTypeName::Int16:
                        case BuiltInTypeName::UInt16:
                        case BuiltInTypeName::Int32:
                        case BuiltInTypeName::UInt32:
                        case BuiltInTypeName::Int64:
                        case BuiltInTypeName::UInt64:
                        case BuiltInTypeName::Single:
                        case BuiltInTypeName::Double:
                            return true;
                        default:
                            return false;
                    }

                case BuiltInTypeName::Int16:
                    switch (tcDest) {
                        case BuiltInTypeName::Int32:
                        case BuiltInTypeName::Int64:
                        case BuiltInTypeName::Single:
                        case BuiltInTypeName::Double:
                            return true;
                        default:
                            return false;
                    }

                case BuiltInTypeName::UInt16:
                    switch (tcDest) {
                        case BuiltInTypeName::Int32:
                        case BuiltInTypeName::UInt32:
                        case BuiltInTypeName::Int64:
                        case BuiltInTypeName::UInt64:
                        case BuiltInTypeName::Single:
                        case BuiltInTypeName::Double:
                            return true;
                        default:
                            return false;
                    }

                case BuiltInTypeName::Int32:
                    switch (tcDest) {
                        case BuiltInTypeName::Int64:
                        case BuiltInTypeName::Single:
                        case BuiltInTypeName::Double:
                            return true;
                        default:
                            return false;
                    }

                case BuiltInTypeName::UInt32:
                    switch (tcDest) {
                        case BuiltInTypeName::UInt32:
                        case BuiltInTypeName::UInt64:
                        case BuiltInTypeName::Single:
                        case BuiltInTypeName::Double:
                            return true;
                        default:
                            return false;
                    }

                case BuiltInTypeName::Int64:
                case BuiltInTypeName::UInt64:
                    switch (tcDest) {
                        case BuiltInTypeName::Single:
                        case BuiltInTypeName::Double:
                            return true;
                        default:
                            return false;
                    }

                case BuiltInTypeName::Char:
                    switch (tcDest) {
                        case BuiltInTypeName::UInt16:
                        case BuiltInTypeName::Int32:
                        case BuiltInTypeName::UInt32:
                        case BuiltInTypeName::Int64:
                        case BuiltInTypeName::UInt64:
                        case BuiltInTypeName::Single:
                        case BuiltInTypeName::Double:
                            return true;
                        default:
                            return false;
                    }

                case BuiltInTypeName::Single:
                    return tcDest == BuiltInTypeName::Double;

                default:
                    return false;

            }
        }

        struct ParameterConversion {

            Expression* expression {};
            MethodInfo* userConversion {};
            ResolvedType convertTo;
            bool requiresConversion {};

            ParameterConversion() = default;

            ParameterConversion(Expression* expression, bool requiresConversion, ResolvedType convertTo, MethodInfo* userConversion = nullptr)
                : expression(expression)
                , requiresConversion(requiresConversion)
                , convertTo(convertTo)
                , userConversion(userConversion) {}

            Expression* Convert(ExpressionTree* et) {
                if (!requiresConversion) {
                    return expression;
                }

                if (userConversion != nullptr) {
                    ArgumentExpression* argument = et->allocator->New<ArgumentExpression>(ArgumentPassByModifier::None, expression, et->fileInfo->fileId, expression->tokenRange);
                    return et->allocator->New<StaticMethodCall>(userConversion, CheckedArray<ArgumentExpression*>(&argument, 1), et->fileInfo->fileId, expression->tokenRange);
                }

                return et->allocator->New<DirectCastExpression>(expression, convertTo, et->fileInfo->fileId, expression->tokenRange);
            }

        };

        struct Candidate {

            MethodInfo* methodInfo;
            CheckedArray<ParameterInfo> parameters;

            Candidate(CheckedArray<ParameterInfo> dependencies)
                : methodInfo(nullptr)
                , parameters(dependencies) {}

            Candidate(MethodInfo* methodInfo, CheckedArray<ParameterInfo> dependencies)
                : methodInfo(methodInfo)
                , parameters(dependencies) {}

        };

        bool AreEquivalent(ResolvedType& a, ResolvedType& b) {
            return a.Equals(&b);
        }

        bool IsConvertible(ResolvedType& type) {

            if (type.IsEnum()) {
                return true;
            }

            switch (type.builtInTypeName) {
                case BuiltInTypeName::Bool:
                case BuiltInTypeName::Char:
                case BuiltInTypeName::Sbyte:
                case BuiltInTypeName::Byte:
                case BuiltInTypeName::Int16:
                case BuiltInTypeName::UInt16:
                case BuiltInTypeName::Int32:
                case BuiltInTypeName::UInt32:
                case BuiltInTypeName::Int64:
                case BuiltInTypeName::UInt64:
                case BuiltInTypeName::Single:
                case BuiltInTypeName::Double:
                    return true;

                default:
                    return false;
            }
        }


        bool HasIdentityPrimitiveOrNullableConversion(ResolvedType source, ResolvedType dest) {

            if (source.Equals(&dest)) {
                return true;
            }

            ResolvedType nonNullableSrc = source.ToNonNullable();

            if (source.IsNullable() && dest.Equals(&nonNullableSrc)) {
                return true;
            }

            ResolvedType nonNullableDst = dest.ToNonNullable();

            if (dest.IsNullable() && source.Equals(&nonNullableDst)) {
                return true;
            }

            return IsConvertible(source) && IsConvertible(dest) && !nonNullableDst.EqualsRef(ResolvedType(BuiltInTypeName::Bool));

        }

        bool AreReferenceAssignable(ResolvedType& dest, ResolvedType& src) {
            return AreEquivalent(dest, src) || !dest.IsValueType() && !src.IsValueType() && dest.IsAssignableFrom(src);
        }

        bool HasReferenceEquality(ResolvedType& left, ResolvedType& right) {
            if (left.IsValueType() || right.IsValueType()) {
                return false;
            }

            return left.IsInterface() || right.IsInterface() || AreReferenceAssignable(left, right) || AreReferenceAssignable(right, left);
        }

        bool IsLegalExplicitVariantDelegateConversion(ResolvedType source, ResolvedType dest) {
            return false;
        }

        struct NumericConversion {

            MethodInfo* methodInfo;
            ParameterInfo* parameterInfo;

        };

        bool HasReferenceConversion(ResolvedType& source, ResolvedType& dest) {

            if (source.IsVoid() || dest.IsVoid()) {
                return false;
            }

            // todo -- we may need to figure out dynamic here

            ResolvedType nonNullableType1 = source.ToNonNullable();
            ResolvedType nonNullableType2 = dest.ToNonNullable();
            return nonNullableType1.IsAssignableFrom(nonNullableType2) || nonNullableType2.IsAssignableFrom(nonNullableType1) || (source.IsInterface() || dest.IsInterface()) ||
                   (IsLegalExplicitVariantDelegateConversion(source, dest) || source.builtInTypeName == BuiltInTypeName::Object || dest.builtInTypeName == BuiltInTypeName::Object);
        }

        bool TryGetUserDefinedCoercionMethod(ResolvedType type, ResolvedType targetType, MethodInfo** conversionMethod) {
            return false;
        }

        bool TryConvertExpressionScored(Expression* expression, ResolvedType targetType, int32* points, ParameterConversion* conversion) {
            ResolvedType argType = expression->GetType();
            MethodInfo* userMethod;

            // how do we handle nullable conversions?
            if (targetType.Equals(&argType)) {
                *points = 100;
                *conversion = ParameterConversion(expression, false, targetType);
                return true;
            }

            if (targetType.IsNullable() && !argType.IsNullable()) {

                // we want to make a nullable, can direct cast just handle that? Probably easiest that way
                if (targetType.ToNonNullable().EqualsRef(argType)) {
                    *points = 50;
                    *conversion = ParameterConversion(expression, true, targetType);
                    return true;
                }
            }

            // if target type is a generic we require an exact match, no conversions (ie T must match T)
            if ((targetType.typeInfo->flags & TypeInfoFlags::IsGeneric) != 0) {
                *points = 0;
                return false;
            }

            if (HasIdentityPrimitiveOrNullableConversion(argType, targetType)) {
                *points = 50;
                *conversion = ParameterConversion(expression, true, targetType);
                return true;
            }

            if (HasReferenceConversion(argType, targetType)) {
                *points = 20;
                *conversion = ParameterConversion(expression, true, targetType);
                return true;
            }

            if (TryGetUserDefinedCoercionMethod(argType, targetType, &userMethod)) {
                *points = 10;
                *conversion = ParameterConversion(expression, true, targetType, userMethod);
                return true;
            }

            if (argType.IsArithmetic() && targetType.IsArithmetic() && IsImplicitNumericConversion(argType, targetType)) {
                *points = 5;
                *conversion = ParameterConversion(expression, true, targetType);
                return true;
            }

            return false;
        }

        bool TryConvertExpression(Expression* expression, ResolvedType targetType, ExpressionTree* et, Expression** converted) {

            int32 points = 0;
            ParameterConversion conversion;

            if (TryConvertExpressionScored(expression, targetType, &points, &conversion)) {
                *converted = conversion.Convert(et);
                return true;
            }

            return false;

        }

        void GatherTypeArguments(ResolvedType resolvedType, FixedPodList<ResolvedType>* resolvedTypes) {
            resolvedTypes->Add(resolvedType);

            for (int32 i = 0; i < resolvedType.typeInfo->genericArgumentCount; i++) {
                GatherTypeArguments(resolvedType.typeInfo->genericArguments[i], resolvedTypes);
            }

        }

        int32 CountTypeArguments(TypeInfo* typeInfo) {
            int32 cnt = 1;
            for (int32 i = 0; i < typeInfo->genericArgumentCount; i++) {
                cnt += CountTypeArguments(typeInfo->genericArguments[i].typeInfo);
            }
            return cnt;
        }

//        MethodInfo * ResolveGenericMethod() {
//            CheckedArray<TypeInfo> generics = candidate.methodInfo->GetGenericArguments();
//
//            struct ResolvedGeneric {
//                bool isResolved {};
//                ResolvedType resolvedType;
//            };
//
//            CheckedArray<ResolvedGeneric> resolved(et->tempAllocator->Allocate<ResolvedGeneric>(generics.size), generics.size);
//
//            for (int32 i = 0; i < candidate.parameters.size; i++) {
//
//                if (!candidate.parameters[i].type.IsOrContainsGeneric()) {
//                    continue;
//                }
//
//                // first goal is to resolve the T types
//                // if we can't resolve or don't match, candidate fails
//                TempAllocator::ScopedMarker scopedMarker(et->tempAllocator);
//
//                ParameterInfo* parameterInfo = &candidate.parameters[i];
//                TypeInfo* ptr = parameterInfo->type.typeInfo;
//
//                int32 cnt = CountTypeArguments(ptr);
//                int32 argCnt = CountTypeArguments(arguments[i]->GetType().typeInfo);
//
//                if (cnt != argCnt) {
//                    return false;
//                }
//
//                FixedPodList<ResolvedType> closedConstructedParameterTypes(et->tempAllocator->AllocateUncleared<ResolvedType>(cnt), cnt);
//                FixedPodList<ResolvedType> openConstructedParameterTypes(et->tempAllocator->AllocateUncleared<ResolvedType>(cnt), cnt);
//
//                GatherTypeArguments(ResolvedType(ptr), &openConstructedParameterTypes);
//                GatherTypeArguments(arguments[i]->GetType(), &closedConstructedParameterTypes);
//
//                for (int32 x = 0; x < openConstructedParameterTypes.size; x++) {
//                    ResolvedType resolvedType = openConstructedParameterTypes[x];
//                    if (!resolvedType.typeInfo->IsGeneric()) {
//                        continue;
//                    }
//
//                    FixedCharSpan name = resolvedType.typeInfo->GetTypeName();
//                    int32 idx = -1;
//                    for (int32 k = 0; k < generics.size; k++) {
//                        if (generics[k].GetTypeName() == name) {
//                            idx = k;
//                            break;
//                        }
//                    }
//
//                    assert(idx != -1);
//
//                    // if we've already solved it's type, make sure we have an exact match (todo -- maybe except for the root type since that's interface convertable)
//                    if (!resolved[idx].isResolved) {
//                        resolved[idx].isResolved = true;
//                        resolved[idx].resolvedType = resolvedType;
//                    }
//                    else if (!resolved[idx].resolvedType.EqualsRef(closedConstructedParameterTypes[x])) {
//                        return false; // maybe we need to log this an error for reporting? improvement for later
//                    }
//                }
//
//            }
//
////                candidate.methodInfo = GenericTypeCache_MakeGenericMethod(et->fileInfo->genericTypeCache, candidate.methodInfo, resolved);
////            candidate.parameters = candidate.methodInfo->GetParameters();
//        }

    }

}