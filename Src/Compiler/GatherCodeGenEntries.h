#pragma once

#include "../JobSystem/Job.h"
#include "./VisitEntry.h"
#include "../Util/Writer.h"

namespace Alchemy::Compilation {

    struct ScheduleInfo {
        void* cookie;
        void (*scheduleTypeInfoFn)(void*, TypeInfo*);
        void (*scheduleMethodInfoFn)(void*, MethodInfo*);
    };

    struct GenerateImpl {

        int32 indent;
        ScheduleInfo scheduleInfo;
        PodList<char>* outputBuffer;
        LinearAllocator* allocator;
        TempAllocator* tempAllocator;
        MSISet<ResolvedType>* visitedTypes;
        CheckedArray<GenericReplacement> genericReplacements;

        GenerateImpl(TempAllocator* tempAllocator, LinearAllocator* allocator, PodList<char>* outputBuffer, MSISet<ResolvedType> * visitedTypes, ScheduleInfo scheduleInfo, CheckedArray<GenericReplacement> genericReplacements)
            : allocator(allocator)
            , tempAllocator(tempAllocator)
            , outputBuffer(outputBuffer)
            , indent(0)
            , scheduleInfo(scheduleInfo)
            , visitedTypes(visitedTypes)
            , genericReplacements(genericReplacements)
            {}

        void AddMethod(MethodInfo* methodInfo) {

            bool expected = false;
            bool desired = true;

            if (methodInfo->hasCodeGen.compare_exchange_strong(expected, desired)) {
                scheduleInfo.scheduleMethodInfoFn(scheduleInfo.cookie, methodInfo);
                // we need to add these to a list right?
                // or maybe we don't care? we have the codegen outputs already
            }

        }

        void AddType(ResolvedType resolvedType) {

            if (resolvedType.IsVoid()) {
                return;
            }

            TypeInfo* typeInfo = resolvedType.typeInfo;

            if (typeInfo->IsGenericTypeDefinition()) {
                // resolve it, we should know what the input generics are
                // GenericTypeCache_MakeGenericType();
            }

            visitedTypes->TryAdd(resolvedType);

        }

        void WriteMethodName(MethodInfo* methodInfo) {
            // typename_methodName_gencnt_argcnt_base32encode
            Write(methodInfo->declaringType->GetTypeName());
            Write("_");
            Write(methodInfo->GetName());
            Write("_");
            WriteInt(methodInfo->genericArgumentCount);
            Write("_");
            WriteInt(methodInfo->parameterCount);
            Write("_");
            // todo -- if we need a reproducible output across compile passes we can probably
            // map this in a stored way that we can retrieve. I don't know how yet.
            EncodePointerBase32(methodInfo, outputBuffer->Reserve(kBase32PointerLength));
        }

        void WriteChar(char c) const {
            outputBuffer->Add(c);
        }

        void Write(FixedCharSpan span) const {
            outputBuffer->AddRange(span.ptr, span.size);
        }

        void Write(const char* str) const {
            outputBuffer->AddRange(str, (int32) strlen(str));
        }

        void WriteInt(int32 value) {
            int32 reserve = IntToAsciiCount(value);
            char* p = outputBuffer->Reserve(reserve);
            IntToAscii(value, p);
        }

        void WriteType(ResolvedType resolvedType, bool emitStructKeyword = true) {

            // add to a set maybe. we'll need to output our structs eventually
            if (resolvedType.IsVoid()) {
                Write("void");
                return;
            }

            if (emitStructKeyword && resolvedType.IsNullable() || resolvedType.IsArray() || !IsPrimitiveTypeName(resolvedType.builtInTypeName)) {
                Write("struct ");
            }

            // we need to see how we handle nullable pointers, can probably decay to just using a void *
            if (resolvedType.IsNullable()) {
                Write("nullable_"); // could shorten this to "n_" if we need to save characters
            }

            switch (resolvedType.builtInTypeName) {
                case BuiltInTypeName::Invalid:
                    assert(resolvedType.typeInfo != nullptr);
                    outputBuffer->AddRange(resolvedType.typeInfo->typeName, resolvedType.typeInfo->typeNameLength);
                    outputBuffer->Add('_');
                    WriteInt(resolvedType.typeInfo->genericArgumentCount);
                    outputBuffer->Add('_');
                    EncodePointerBase32(resolvedType.typeInfo, outputBuffer->Reserve(kBase32PointerLength));
                    break;
                case BuiltInTypeName::Int:
                    Write("int32");
                    break;
                case BuiltInTypeName::Int2:
                    Write("int2");
                    break;
                case BuiltInTypeName::Int3:
                    Write("int3");
                    break;
                case BuiltInTypeName::Int4:
                    Write("int4");
                    break;
                case BuiltInTypeName::Uint:
                    Write("uint32");
                    break;
                case BuiltInTypeName::Uint2:
                    Write("uint2");
                    break;
                case BuiltInTypeName::Uint3:
                    Write("uint3");
                    break;
                case BuiltInTypeName::Uint4:
                    Write("uint4");
                    break;
                case BuiltInTypeName::Float:
                    Write("float");
                    break;
                case BuiltInTypeName::Float2:
                    Write("float2");
                    break;
                case BuiltInTypeName::Float3:
                    Write("float3");
                    break;
                case BuiltInTypeName::Float4:
                    Write("float4");
                    break;
                case BuiltInTypeName::Bool:
                    Write("bool");
                    break;
                case BuiltInTypeName::Char:
                    Write("unsigned char");
                    break;
                case BuiltInTypeName::Color:
                    Write("color");
                    break;
                case BuiltInTypeName::Color32:
                    Write("color32");
                    break;
                case BuiltInTypeName::Color64:
                    Write("color64");
                    break;
                case BuiltInTypeName::Byte:
                    Write("uint8");
                    break;
                case BuiltInTypeName::Sbyte:
                    Write("int8");
                    break;
                case BuiltInTypeName::Short:
                    Write("int16");
                    break;
                case BuiltInTypeName::Ushort:
                    Write("uint16");
                    break;
                case BuiltInTypeName::Long:
                    Write("int64");
                    break;
                case BuiltInTypeName::Ulong:
                    Write("uint64");
                    break;
                case BuiltInTypeName::Dynamic:
                    Write("dynamic");
                    break;
                case BuiltInTypeName::String:
                    Write("string");
                    break;
                case BuiltInTypeName::Object:
                    Write("object");
                    break;
                case BuiltInTypeName::Double:
                    Write("double");
                    break;
                case BuiltInTypeName::Void:
                    Write("void");
                    break;
            }

            if (resolvedType.IsArray()) {
                Write("_array"); // could shorten to _a if we need to save characters
            }

        }

        void WriteIndent() const {
            static const char* s_Indent = "    ";
            for (int32 i = 0; i < indent; i++) {
                outputBuffer->AddRange(s_Indent, 4);
            }
        }

        void TraceTypeInfo(TypeInfo* typeInfo) {
            // we need a struct definition
            // this might be a 2 parter, visit then register
            // because we'll need to emit in the right order to handle
            // struct dependencies. either we make sure to visit in the right order
            // or we top sort after visiting

            // we will need to crawl the types and figure out
            // which non pointer types need forward declaration
            // I believe if we handle making sure structs are not recursive we can
            // forward declare all the class types and then top sort the struct types
            // then emit class struct bodies

            // if output type annotations -> spit out fully qualified name comment
            Write("struct ");
            WriteType(ResolvedType(typeInfo), false);
            WriteChar(' ');
            WriteChar('{');
            WriteChar('\n');
            indent++;
            for (int32 i = 0; i < typeInfo->fieldCount; i++) {
                FieldInfo* fieldInfo = &typeInfo->fields[i];
                AddType(fieldInfo->type);
                WriteIndent();
                WriteType(fieldInfo->type);
                WriteChar(' ');
                Write(fieldInfo->GetName());
                WriteChar(';');
                WriteChar('\n');
            }

            for (int32 i = 0; i < typeInfo->propertyCount; i++) {
                PropertyInfo* propertyInfo = &typeInfo->properties[i];
                AddType(propertyInfo->type);
                if (!propertyInfo->IsBackedProperty()) {
                    continue;
                }
                WriteIndent();
                WriteType(propertyInfo->type);
                WriteChar(' ');
                Write(propertyInfo->GetName());
                WriteChar(';');
                WriteChar('\n');
            }

            indent--;
            WriteChar('}');
            WriteChar(';');
            WriteChar('\n');
            WriteChar('\n');
        }

        void VisitInstanceMethodCall(InstanceMethodCall* instanceMethodCall) {

            AddType(instanceMethodCall->instance->GetType());
            AddMethod(instanceMethodCall->methodInfo);

            WriteMethodName(instanceMethodCall->methodInfo);

            WriteChar('(');

            Visit(instanceMethodCall->instance);
            if (instanceMethodCall->arguments.size != 0) {
                WriteChar(',');
                WriteChar(' ');
            }

            for (int32 i = 0; i < instanceMethodCall->arguments.size; i++) {
                ArgumentExpression* arg = instanceMethodCall->arguments[i];
                if (arg->passByModifier != ArgumentPassByModifier::None) {
                    WriteChar('&');
                }
                Visit(instanceMethodCall->arguments[i]->expression);
                if (i != instanceMethodCall->arguments.size) {
                    WriteChar(',');
                    WriteChar(' ');
                }
            }
            WriteChar(')');

        }

        void VisitStaticMethodCall(StaticMethodCall* staticMethodCall) {

            AddMethod(staticMethodCall->methodInfo);

            WriteMethodName(staticMethodCall->methodInfo);

            WriteChar('(');

            if (staticMethodCall->arguments.size != 0) {
                WriteChar(',');
                WriteChar(' ');
            }

            for (int32 i = 0; i < staticMethodCall->arguments.size; i++) {
                ArgumentExpression* arg = staticMethodCall->arguments[i];
                if (arg->passByModifier != ArgumentPassByModifier::None) {
                    WriteChar('&');
                }
                Visit(staticMethodCall->arguments[i]->expression);
                if (i != staticMethodCall->arguments.size) {
                    WriteChar(',');
                    WriteChar(' ');
                }
            }

            WriteChar(')');
        }

        FixedCharSpan TraceMethodInfo(MethodInfo* methodInfo) {
            return FinalizeOutput();
        }

        void TraceConstructor(ConstructorInfo* methodInfo) {}

        void TraceProperty(PropertyInfo* propertyInfo) {}

        void TraceIndexer(MethodInfo* methodInfo) {}

        void Visit(Expression* expression) {
            if (expression == nullptr) {
                return;
            }

//            if(expression->IsReducable()) {
//                expression = expression->Reduce();
//                assert(expression != nullptr);
//            }

#define VISIT_CASE(x) case ExpressionType::x: {Visit##x((x*)expression); return; }

            switch (expression->expressionType) {

//                VISIT_CASE(LocalVariableDeclaration)
                VISIT_CASE(InstanceMethodCall)
                VISIT_CASE(StaticMethodCall)

                default: {
                    break;
                }
            }
#undef VISIT_CASE


        }

        FixedCharSpan FinalizeOutput() const {
            int32 size = outputBuffer->size;
            outputBuffer->size = 0;
            char* p = allocator->AllocateUncleared<char>(size + 1);
            memcpy(p, outputBuffer->array, size);
            p[size] = 0;
            return FixedCharSpan(p, size);
        }

        void MakeMethodSignature(MethodInfo* pInfo) {

            WriteType(pInfo->returnType);
            WriteChar(' ');
            WriteMethodName(pInfo);
            WriteChar('(');

            if (!pInfo->IsStatic()) {

                WriteType(ResolvedType(pInfo->declaringType));
                Write("* pThis");
                if (pInfo->parameterCount != 0) {
                    WriteChar(',');
                    WriteChar(' ');
                }
            }

            for (int32 i = 0; i < pInfo->parameterCount; i++) {

                ParameterInfo* parameterInfo = &pInfo->parameters[i];

                WriteType(parameterInfo->type);

                if (parameterInfo->passByModifier != ArgumentPassByModifier::None) {
                    WriteChar('*');
                }

                WriteChar(' ');

                Write(parameterInfo->GetName());

                if (i != pInfo->parameterCount - 1) {
                    WriteChar(',');
                    WriteChar(' ');
                }
            }

            WriteChar(')');

        }

        FixedCharSpan MakeMethodForwardDeclaration(MethodInfo* pInfo) {

            AddType(pInfo->returnType);

            AddType(ResolvedType(pInfo->declaringType));

            for (int32 i = 0; i < pInfo->parameterCount; i++) {
                AddType(pInfo->parameters[i].type);
            }

            MakeMethodSignature(pInfo);
            WriteChar(';');
            return FinalizeOutput();
        }

    };

    struct GatherCodeGenEntries : Alchemy::Jobs::IJob {

        VisitEntry visitEntry;
        SharedWorkerData* sharedData;

        GatherCodeGenEntries(VisitEntry visitEntry, SharedWorkerData* sharedWorkerData)
            : visitEntry(visitEntry)
            , sharedData(sharedWorkerData) {}

        static void ScheduleTypeInfo(void* cookie, TypeInfo* typeInfo) {
            GatherCodeGenEntries* pThis = (GatherCodeGenEntries*) cookie;
            pThis->Schedule(GatherCodeGenEntries(VisitEntry(VisitEntryType::Type, typeInfo), pThis->sharedData));
        }

        static void ScheduleMethodInfo(void* cookie, MethodInfo* methodInfo) {
            GatherCodeGenEntries* pThis = (GatherCodeGenEntries*) cookie;

            {
                std::unique_lock lock(*pThis->sharedData->mtx);
            }

            pThis->Schedule(GatherCodeGenEntries(VisitEntry(VisitEntryType::Method, methodInfo), pThis->sharedData));
        }

        void Execute() {

            GenerateImpl impl(
                GetAllocator(),
                sharedData->perWorkerAllocators[GetWorkerId()],
                sharedData->perWorkerOutputBuffers[GetWorkerId()],
                sharedData->perWorkerTypeSets[GetWorkerId()],
                ScheduleInfo {this, &ScheduleTypeInfo, &ScheduleMethodInfo},
                CheckedArray<GenericReplacement>(nullptr, 0)
            );

            impl.outputBuffer->size = 0;

            PodList<CodeGenOutput>* codeGen = sharedData->perWorkerCodeGenOutputs[GetWorkerId()];

            // each thread has a hashset of resolved types that it has touched
            // after tracing methods, we add all the types we touched to a global set

            // when a new method (or property, or constructor, or indexer) is scheduled for visiting

            switch (visitEntry.entryType) {

                case VisitEntryType::Type: {
                    impl.TraceTypeInfo((TypeInfo*) visitEntry.entry); // maybe not a job, pretty straight forward
                    break;
                }
                case VisitEntryType::Method: {
                    codeGen->Add(CodeGenOutput(CodeGenOutputType::MethodSignature, impl.MakeMethodForwardDeclaration((MethodInfo*) visitEntry.entry)));
                    codeGen->Add(CodeGenOutput(CodeGenOutputType::MethodImplementation, impl.TraceMethodInfo((MethodInfo*) visitEntry.entry)));
                    break;
                }
                case VisitEntryType::Property: {

                    break;
                }
                case VisitEntryType::Indexer: {

                    break;
                }
                case VisitEntryType::Constructor: {

                    break;
                }

            }

        }

    };

}