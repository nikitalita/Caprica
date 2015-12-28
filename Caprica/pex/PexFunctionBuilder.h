#pragma once

#include <cstdint>
#include <vector>

#include <papyrus/parser/PapyrusFileLocation.h>

#include <pex/PexInstruction.h>
#include <pex/PexLocalVariable.h>
#include <pex/PexString.h>

namespace caprica { namespace pex {

// We use this rather than repeating this information in multiple places.
#define OPCODES(ARG1, ARG2, ARG3, ARG4, ARG5) \
  ARG3(iadd, IAdd, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(fadd, FAdd, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(isub, ISub, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(fsub, FSub, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(imul, IMul, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(fmul, FMul, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(idiv, IDiv, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(fdiv, FDiv, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(imod, IMod, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG2(not, Not, PexValue::Identifier, dest, PexValue, src) \
  ARG2(ineg, INeg, PexValue::Identifier, dest, PexValue, src) \
  ARG2(fneg, FNeg, PexValue::Identifier, dest, PexValue, src) \
  ARG2(assign, Assign, PexValue::Identifier, dest, PexValue, src) \
  ARG2(cast, Cast, PexValue::Identifier, dest, PexValue, src) \
  ARG3(cmpeq, CmpEq, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(cmplt, CmpLt, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(cmplte, CmpLte, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(cmpgt, CmpGt, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(cmpgte, CmpGte, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG1(ret, Return, PexValue, val) \
  ARG3(strcat, StrCat, PexValue::Identifier, dest, PexValue, arg1, PexValue, arg2) \
  ARG3(propget, PropGet, PexString, propName, PexValue::Identifier, baseObj, PexValue::Identifier, dest) \
  ARG3(propset, PropSet, PexString, propName, PexValue::Identifier, baseObj, PexValue, val) \
  ARG2(arraycreate, ArrayCreate, PexValue::Identifier, dest, PexValue, length) \
  ARG2(arraylength, ArrayLength, PexValue::Identifier, dest, PexValue::Identifier, arr) \
  ARG3(arraygetelement, ArrayGetElement, PexValue::Identifier, dest, PexValue, index, PexValue::Identifier, arr) \
  ARG3(arraysetelement, ArraySetElement, PexValue::Identifier, arr, PexValue, index, PexValue, val) \
  ARG4(arrayfindelement, ArrayFindElement, PexValue::Identifier, dest, PexValue::Identifier, arr, PexValue, element, PexValue, startIndex) \
  ARG4(arrayrfindelement, ArrayRFindElement, PexValue::Identifier, dest, PexValue::Identifier, arr, PexValue, element, PexValue, startIndex) \
  ARG3(is, Is, PexValue::Identifier, dest, PexValue::Identifier, src, PexValue::Identifier, type) \
  ARG1(structcreate, StructCreate, PexValue::Identifier, dest) \
  ARG3(structget, StructGet, PexValue::Identifier, dest, PexValue::Identifier, baseObj, PexString, memberName) \
  ARG3(structset, StructSet, PexValue::Identifier, baseObj, PexString, memberName, PexValue, val) \
  ARG5(arrayfindstruct, ArrayFindStruct, PexValue::Identifier, baseObj, PexValue::Identifier, dest, PexString, memberName, PexValue, valueToSearchFor, PexValue, startIndex) \
  ARG5(arrayrfindstruct, ArrayRFindStruct, PexValue::Identifier, baseObj, PexValue::Identifier, dest, PexString, memberName, PexValue, valueToSearchFor, PexValue, startIndex) \
  ARG3(arrayadd, ArrayAdd, PexValue::Identifier, baseObj, PexValue, element, PexValue, count) \
  ARG3(arrayinsert, ArrayInsert, PexValue::Identifier, baseObj, PexValue, element, PexValue, index) \
  ARG1(arrayremovelast, ArrayRemoveLast, PexValue::Identifier, dest) \
  ARG3(arrayremove, ArrayRemove, PexValue::Identifier, baseObj, PexValue, index, PexValue, count) \
  ARG1(arrayclear, ArrayClear, PexValue::Identifier, dest) \

/*Jmp,
//JmpT,
//JmpF,
//CallMethod,
//CallParent,
//CallStatic,*/


namespace op {

struct nop { };

#define OP_ARG1(name, opcode, argType1, argName1) \
struct name { \
  PexValue a1; \
  name(argType1 argName1) : a1(argName1) { } \
};
#define OP_ARG2(name, opcode, argType1, argName1, argType2, argName2) \
struct name { \
  PexValue a1, a2; \
  name(argType1 argName1, argType2 argName2) : a1(argName1), a2(argName2) { } \
};
#define OP_ARG3(name, opcode, argType1, argName1, argType2, argName2, argType3, argName3) \
struct name { \
  PexValue a1, a2, a3; \
  name(argType1 argName1, argType2 argName2, argType3 argName3) : a1(argName1), a2(argName2), a3(argName3) { } \
};
#define OP_ARG4(name, opcode, argType1, argName1, argType2, argName2, argType3, argName3, argType4, argName4) \
struct name { \
  PexValue a1, a2, a3, a4; \
  name(argType1 argName1, argType2 argName2, argType3 argName3, argType4 argName4) : a1(argName1), a2(argName2), a3(argName3), a4(argName4) { } \
};
#define OP_ARG5(name, opcode, argType1, argName1, argType2, argName2, argType3, argName3, argType4, argName4, argType5, argName5) \
struct name { \
  PexValue a1, a2, a3, a4, a5; \
  name(argType1 argName1, argType2 argName2, argType3 argName3, argType4 argName4, argType5 argName5) : a1(argName1), a2(argName2), a3(argName3), a4(argName4), a5(argName5) { } \
};
OPCODES(OP_ARG1, OP_ARG2, OP_ARG3, OP_ARG4, OP_ARG5)
#undef OP_ARG1
#undef OP_ARG2
#undef OP_ARG3
#undef OP_ARG4
#undef OP_ARG5
}

struct PexFunctionBuilder final
{
  PexFunctionBuilder& operator <<(op::nop& instr) { return push(PexOpCode::Nop); }

#define OP_ARG1(name, opcode, at1, an1) \
  PexFunctionBuilder& operator <<(op::name& instr) { return push(PexOpCode::opcode, instr.a1); }
#define OP_ARG2(name, opcode, at1, an1, at2, an2) \
  PexFunctionBuilder& operator <<(op::name& instr) { return push(PexOpCode::opcode, instr.a1, instr.a2); }
#define OP_ARG3(name, opcode, at1, an1, at2, an2, at3, an3) \
  PexFunctionBuilder& operator <<(op::name& instr) { return push(PexOpCode::opcode, instr.a1, instr.a2, instr.a3); }
#define OP_ARG4(name, opcode, at1, an1, at2, an2, at3, an3, at4, an4) \
  PexFunctionBuilder& operator <<(op::name& instr) { return push(PexOpCode::opcode, instr.a1, instr.a2, instr.a3, instr.a4); }
#define OP_ARG5(name, opcode, at1, an1, at2, an2, at3, an3, at4, an4, at5, an5) \
  PexFunctionBuilder& operator <<(op::name& instr) { return push(PexOpCode::opcode, instr.a1, instr.a2, instr.a3, instr.a4, instr.a5); }
OPCODES(OP_ARG1, OP_ARG2, OP_ARG3, OP_ARG4, OP_ARG5)
#undef OP_ARG1
#undef OP_ARG2
#undef OP_ARG3
#undef OP_ARG4
#undef OP_ARG5

  PexFunctionBuilder& operator <<(const papyrus::parser::PapyrusFileLocation& loc) {
    currentLocation = loc;
    return *this;
  }

  void populateFunction(PexFunction* func, PexDebugFunctionInfo* debInfo) {
    func->instructions = instructions;
    func->locals = locals;
    debInfo->instructionLineMap.reserve(instructionLocations.size());
    for (auto& l : instructionLocations)
      debInfo->instructionLineMap.push_back(l.buildPex());
  }

  PexLocalVariable* allocTemp(PexFile* file, papyrus::PapyrusType tp) {
    auto loc = new PexLocalVariable();
    std::stringstream ss;
    ss << "::temp" << currentTempI++;
    loc->name = file->getString(ss.str());
    loc->type = tp.buildPex(file);
    locals.push_back(loc);
    return loc;
  }

  void freeIfTemp(PexValue val) {
    // TODO: alloc & free temps.
  }

private:
  papyrus::parser::PapyrusFileLocation currentLocation{ };
  std::vector<papyrus::parser::PapyrusFileLocation> instructionLocations{ };
  std::vector<PexInstruction*> instructions{ };
  std::vector<PexLocalVariable*> locals{ };
  size_t currentTempI = 0;

  template<typename... Args>
  PexFunctionBuilder& push(PexOpCode op, Args&&... args) {
    instructionLocations.push_back(currentLocation);
    instructions.push_back(new PexInstruction(op, std::vector<PexValue>{ args... }));
    return *this;
  }
};

}}