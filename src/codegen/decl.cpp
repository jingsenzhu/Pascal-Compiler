#include "utils/ast.hpp"
#include "codegen_context.hpp"

namespace spc
{

    llvm::Value *VarDeclNode::createGlobalArray(CodegenContext &context, const std::shared_ptr<ArrayTypeNode> &arrTy)
    {
        // std::shared_ptr<ArrayTypeNode> arrTy = cast_node<ArrayTypeNode>(this->type);
        context.log() << "\tCreating array " << this->name->name << std::endl;
        auto *ty = arrTy->itemType->getLLVMType(context);
        llvm::Constant *z; // zero
        if (ty->isIntegerTy()) 
            z = llvm::ConstantInt::get(ty, 0);
        else if (ty->isDoubleTy())
            z = llvm::ConstantFP::get(ty, 0.0);
        else if (ty->isStructTy())
        {
            std::vector<llvm::Constant*> zeroes;
            auto *recTy = llvm::cast<llvm::StructType>(ty);
            for (auto itr = recTy->element_begin(); itr != recTy->element_end(); itr++)
                zeroes.push_back(llvm::Constant::getNullValue(*itr));
            z = llvm::ConstantStruct::get(recTy, zeroes);
            std::shared_ptr<RecordTypeNode> rec;
            if (is_ptr_of<RecordTypeNode>(arrTy->itemType))
                rec = cast_node<RecordTypeNode>(arrTy->itemType);
            else if (is_ptr_of<AliasTypeNode>(arrTy->itemType))
            {
                std::string aliasName = cast_node<AliasTypeNode>(arrTy->itemType)->name->name;
                for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                    if ((rec = context.getRecordAlias(*rit + "." + aliasName)) != nullptr)
                        break;
                if (rec == nullptr) rec = context.getRecordAlias(aliasName);
                if (rec == nullptr) assert(0 && "Fatal Error: Unexpected behavior!");
            }
            else assert(0 && "Fatal Error: Unexpected behavior!");
            context.setRecordAlias(this->name->name + "[]", rec);
            rec->insertNestedRecord(this->name->name + "[]", context);
        }
        else if (ty->isArrayTy()) // String
        {
            z = llvm::Constant::getNullValue(ty);
            std::shared_ptr<ArrayTypeNode> arr;
            if (is_ptr_of<ArrayTypeNode>(arrTy->itemType))
                arr = cast_node<ArrayTypeNode>(arrTy->itemType);
            else if (is_ptr_of<StringTypeNode>(arrTy->itemType))
                arr = nullptr;
            else if (is_ptr_of<AliasTypeNode>(arrTy->itemType))
            {
                std::string aliasName = cast_node<AliasTypeNode>(arrTy->itemType)->name->name;
                for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                    if ((arr = context.getArrayAlias(*rit + "." + aliasName)) != nullptr)
                        break;
                if (arr == nullptr) arr = context.getArrayAlias(aliasName);
                if (arr == nullptr) assert(0 && "Fatal Error: Unexpected behavior!");
            }
            else assert(0 && "Fatal Error: Unexpected behavior!");
            if (arr == nullptr) // String
                context.setArrayEntry(this->name->name + "[]", 0, 255);
            else
            {
                context.setArrayEntry(this->name->name + "[]", arr);
                arr->insertNestedArray(this->name->name + "[]", context);
            }
        }
        else 
            throw CodegenException("Unsupported type of array");

        llvm::ConstantInt *startIdx = llvm::dyn_cast<llvm::ConstantInt>(arrTy->range_start->codegen(context));
        if (!startIdx)
            throw CodegenException("Start index invalid");
        // else if (startIdx->getSExtValue() < 0)
        //     throw CodegenException("Start index must be greater than zero!");
        int start = startIdx->getSExtValue();
        
        int len = 0;
        llvm::ConstantInt *endIdx = llvm::dyn_cast<llvm::ConstantInt>(arrTy->range_end->codegen(context));
        int end = endIdx->getSExtValue();
        if (!endIdx)
            throw CodegenException("End index invalid");
        else if (start > end)
            throw CodegenException("End index must be greater than start index!");
        else if (endIdx->getBitWidth() <= 32)
            len = end - start + 1;
        else
            throw CodegenException("End index overflow");

        context.log() << "\tArray info: start: " << start << " end: " << end << " len: " << len << std::endl;
        llvm::ArrayType* arr = llvm::ArrayType::get(ty, len);
        std::vector<llvm::Constant *> initVector;
        for (int i = 0; i < len; i++)
            initVector.push_back(z);
        auto *variable = llvm::ConstantArray::get(arr, initVector);

        llvm::Value *gv = new llvm::GlobalVariable(*context.getModule(), variable->getType(), false, llvm::GlobalVariable::ExternalLinkage, variable, this->name->name);
        context.log() << "\tCreated array " << this->name->name << std::endl;

        context.setArrayEntry(this->name->name, start, end);
        context.log() << "\tInserted to array table" << std::endl;

        return gv;
    }

    llvm::Value *VarDeclNode::createArray(CodegenContext &context, const std::shared_ptr<ArrayTypeNode> &arrTy)
    {
        // std::shared_ptr<ArrayTypeNode> arrTy = cast_node<ArrayTypeNode>(this->type);
        context.log() << "\tCreating array " << this->name->name << std::endl;
        auto *ty = arrTy->itemType->getLLVMType(context);
        llvm::Constant *constant;
        if (ty->isIntegerTy()) 
            constant = llvm::ConstantInt::get(ty, 0);
        else if (ty->isDoubleTy())
            constant = llvm::ConstantFP::get(ty, 0.0);
        else if (ty->isStructTy())
        {
            constant = nullptr;
            std::shared_ptr<RecordTypeNode> rec;
            if (is_ptr_of<RecordTypeNode>(arrTy->itemType))
                rec = cast_node<RecordTypeNode>(arrTy->itemType);
            else if (is_ptr_of<AliasTypeNode>(arrTy->itemType))
            {
                std::string aliasName = cast_node<AliasTypeNode>(arrTy->itemType)->name->name;
                for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                    if ((rec = context.getRecordAlias(*rit + "." + aliasName)) != nullptr)
                        break;
                if (rec == nullptr) rec = context.getRecordAlias(aliasName);
                if (rec == nullptr) assert(0 && "Fatal Error: Unexpected behavior!");
            }
            else assert(0 && "Fatal Error: Unexpected behavior!");
            context.setRecordAlias(context.getTrace() + "." + this->name->name + "[]", rec);
            rec->insertNestedRecord(context.getTrace() + "." + this->name->name + "[]", context);
        }
        else if (ty->isArrayTy())
        {
            constant = nullptr;
            std::shared_ptr<ArrayTypeNode> arr;
            if (is_ptr_of<ArrayTypeNode>(arrTy->itemType))
                arr = cast_node<ArrayTypeNode>(arrTy->itemType);
            else if (is_ptr_of<StringTypeNode>(arrTy->itemType))
                arr = nullptr;
            else if (is_ptr_of<AliasTypeNode>(arrTy->itemType))
            {
                std::string aliasName = cast_node<AliasTypeNode>(arrTy->itemType)->name->name;
                for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                    if ((arr = context.getArrayAlias(*rit + "." + aliasName)) != nullptr)
                        break;
                if (arr == nullptr) arr = context.getArrayAlias(aliasName);
                if (arr == nullptr) assert(0 && "Fatal Error: Unexpected behavior!");
            }
            else assert(0 && "Fatal Error: Unexpected behavior!");
            if (arr == nullptr) // String
                context.setArrayEntry(this->name->name + "[]", 0, 255);
            else
            {
                context.setArrayEntry(this->name->name + "[]", arr);
                arr->insertNestedArray(this->name->name + "[]", context);
            }
        }
        else
            throw CodegenException("Unsupported type of array");

        llvm::ConstantInt *startIdx = llvm::dyn_cast<llvm::ConstantInt>(arrTy->range_start->codegen(context));
        if (!startIdx)
            throw CodegenException("Start index invalid");
        // else if (startIdx->getSExtValue() < 0)
        //     throw CodegenException("Start index must be greater than zero!");
        int start = startIdx->getSExtValue();
        
        unsigned len = 0;
        llvm::ConstantInt *endIdx = llvm::dyn_cast<llvm::ConstantInt>(arrTy->range_end->codegen(context));
        int end = endIdx->getSExtValue();
        if (!endIdx)
            throw CodegenException("End index invalid");
        else if (start > end)
            throw CodegenException("End index must be greater than start index!");
        else if (endIdx->getBitWidth() <= 32)
            len = end - start + 1;
        else
            throw CodegenException("End index overflow");
        
        context.log() << "\tArray info: start: " << start << " end: " << end << " len: " << len << std::endl;
        // llvm::ConstantInt *space = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), len);
        llvm::ArrayType *arrayTy = llvm::ArrayType::get(ty, len);
        // auto *local = context.getBuilder().CreateAlloca(ty, space);
        auto *local = context.getBuilder().CreateAlloca(arrayTy);
        auto success = context.setLocal(context.getTrace() + "." + this->name->name, local);
        if (!success) throw CodegenException("Duplicate identifier in var section of function " + context.getTrace() + ": " + this->name->name);
        context.log() << "\tCreated array " << this->name->name << std::endl;

        context.setArrayEntry(context.getTrace() + "." + this->name->name, start, end);
        context.log() << "\tInserted to array table" << std::endl;

        return local;
    }
    
    llvm::Value *VarDeclNode::codegen(CodegenContext &context)
    {
        if (context.is_subroutine)
        {
            if (type->type == Type::Alias)
            {
                std::string aliasName = cast_node<AliasTypeNode>(type)->name->name;
                context.log() << "\tSearching alias " << aliasName << std::endl;
                std::shared_ptr<ArrayTypeNode> arrTy = nullptr;
                for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                {
                    if ((arrTy = context.getArrayAlias(*rit + "." + aliasName)) != nullptr)
                        break;
                }
                if (arrTy == nullptr)
                    arrTy = context.getArrayAlias(aliasName);
                if (arrTy != nullptr)
                {
                    context.log() << "\tAlias is array" << std::endl;
                    return createArray(context, arrTy);
                }
                std::shared_ptr<RecordTypeNode> recTy = nullptr;
                for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                {
                    if ((recTy = context.getRecordAlias(*rit + "." + aliasName)) != nullptr)
                        break;
                }
                if (recTy == nullptr)
                    recTy = context.getRecordAlias(aliasName);
                if (recTy != nullptr)
                {
                    context.log() << "\tAlias is record" << std::endl;
                    context.setRecordAlias(context.getTrace() + "." + name->name, recTy);
                    recTy->insertNestedRecord(context.getTrace() + "." + name->name, context);
                }
            }
            if (type->type == Type::Array)
                return createArray(context, cast_node<ArrayTypeNode>(this->type));
            else if (type->type == Type::String)
            {
                auto arrTy = make_node<ArrayTypeNode>(0, 255, Type::Char);
                return createArray(context, arrTy);
            }
            else
            {
                if (type->type == Type::Record)
                {
                    context.setRecordAlias(context.getTrace() + "." + name->name, cast_node<RecordTypeNode>(type));
                    cast_node<RecordTypeNode>(type)->insertNestedRecord(context.getTrace() + "." + name->name, context);
                }
                auto *local = context.getBuilder().CreateAlloca(type->getLLVMType(context));
                auto success = context.setLocal(context.getTrace() + "." + name->name, local);
                if (!success) throw CodegenException("Duplicate identifier in var section of function " + context.getTrace() + ": " + name->name);
                return local;
            }
        }
        else
        {
            if (context.getModule()->getGlobalVariable(name->name) != nullptr)
                throw CodegenException("Duplicate global variable: " + name->name);
            if (type->type == Type::Alias)
            {
                std::string aliasName = cast_node<AliasTypeNode>(type)->name->name;
                context.log() << "\tSearching alias " << aliasName << std::endl;
                std::shared_ptr<ArrayTypeNode> arrTy = context.getArrayAlias(aliasName);
                if (arrTy != nullptr)
                {
                    context.log() << "\tAlias is array" << std::endl;
                    return createGlobalArray(context, arrTy);
                }
                std::shared_ptr<RecordTypeNode> recTy = nullptr;
                for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                {
                    if ((recTy = context.getRecordAlias(*rit + "." + aliasName)) != nullptr)
                        break;
                }
                if (recTy == nullptr)
                    recTy = context.getRecordAlias(aliasName);
                if (recTy != nullptr)
                {
                    context.log() << "\tAlias is record" << std::endl;
                    context.setRecordAlias(name->name, recTy);
                    recTy->insertNestedRecord(context.getTrace() + "." + name->name, context);
                }
            }
            if (type->type == Type::Array)
                return createGlobalArray(context, cast_node<ArrayTypeNode>(this->type));
            else if (type->type == Type::String)
            {
                auto arrTy = make_node<ArrayTypeNode>(0, 255, Type::Char);
                return createGlobalArray(context, arrTy);
            }
            else
            {
                if (type->type == Type::Record)
                {
                    context.setRecordAlias(name->name, cast_node<RecordTypeNode>(type));
                    cast_node<RecordTypeNode>(type)->insertNestedRecord(context.getTrace() + "." + name->name, context);
                }
                auto *ty = type->getLLVMType(context);
                llvm::Constant *constant;
                if (ty->isIntegerTy()) 
                    constant = llvm::ConstantInt::get(ty, 0);
                else if (ty->isDoubleTy())
                    constant = llvm::ConstantFP::get(ty, 0.0);
                else if (ty->isStructTy())
       	        {
                    std::vector<llvm::Constant*> zeroes;
                    auto *recTy = llvm::cast<llvm::StructType>(ty);
                    for (auto itr = recTy->element_begin(); itr != recTy->element_end(); itr++)
                        zeroes.push_back(llvm::Constant::getNullValue(*itr));
                    constant = llvm::ConstantStruct::get(recTy, zeroes);
                    // int n = recTy->getNumElements();
                    // for (int i = 0; i < n; i++)
                    // {
                    //     zeroes.push_back(llvm::Constant::getNullValue(recTy->getTypeAtIndex(i)));
                    // }
                }
                else
                    throw CodegenException("Unknown type");
                return new llvm::GlobalVariable(*context.getModule(), ty, false, llvm::GlobalVariable::ExternalLinkage, constant, name->name);
            }
        }
    }

    llvm::Value *ConstDeclNode::codegen(CodegenContext &context)
    {
        if (context.is_subroutine)
        {
            if (val->type == Type::String)
            {
                context.log() << "\tConst string declare" << std::endl;
                auto strVal = cast_node<StringNode>(val);
                auto *constant = llvm::ConstantDataArray::getString(llvm_context, strVal->val, true);
                bool success = context.setConst(context.getTrace() + "." + name->name, constant);
                if (!success) throw CodegenException("Duplicate identifier in const section of function " + context.getTrace() + ": " + name->name);
                context.log() << "\tAdded to symbol table" << std::endl;
                auto *gv = new llvm::GlobalVariable(*context.getModule(), constant->getType(), true, llvm::GlobalVariable::ExternalLinkage, constant, context.getTrace() + "." + name->name);
                context.log() << "\tCreated global variable" << std::endl;
                return gv;
            }
            else
            {
                context.log() << "\tConst declare" << std::endl;
                auto *constant = llvm::cast<llvm::Constant>(val->codegen(context));
                assert(constant != nullptr);
                bool success = context.setConst(context.getTrace() + "." + name->name, constant);
                success &= context.setConstVal(context.getTrace() + "." + name->name, constant);
                if (!success) throw CodegenException("Duplicate identifier in const section of function " + context.getTrace() + ": " + name->name);
                context.log() << "\tAdded to symbol table" << std::endl;
                return nullptr;
            } 
        }
        else
        {
            if (val->type == Type::String)
            {
                context.log() << "\tConst string declare" << std::endl;
                auto strVal = cast_node<StringNode>(val);
                auto *constant = llvm::ConstantDataArray::getString(llvm_context, strVal->val, true);
                context.setConst(name->name, constant);
                context.log() << "\tAdded to symbol table" << std::endl;
                auto *gv = new llvm::GlobalVariable(*context.getModule(), constant->getType(), true, llvm::GlobalVariable::ExternalLinkage, constant, name->name);
                context.log() << "\tCreated global variable" << std::endl;
                return gv;
            }
            else
            {
                context.log() << "\tConst declare" << std::endl;
                auto *constant = llvm::cast<llvm::Constant>(val->codegen(context));
                bool success = context.setConst(name->name, constant);
                success &= context.setConstVal(name->name, constant);
                if (!success) throw CodegenException("Duplicate identifier in const section of main program: " + name->name);
                context.log() << "\tAdded to symbol table" << std::endl;
                return nullptr;
            } 
        }
    }

    llvm::Value *TypeDeclNode::codegen(CodegenContext &context)
    {
        if (context.is_subroutine)
        {
            if (type->type == Type::Array)
            {
                bool success = context.setArrayAlias(context.getTrace() + "." + name->name, cast_node<ArrayTypeNode>(type));
                if (!success) throw CodegenException("Duplicate type alias in function " + context.getTrace() + ": " + name->name);
                context.log() << "\tArray alias in function " << context.getTrace() << ": " << name->name << std::endl;
            }
            else if (type->type == Type::Record)
            {
                bool success = context.setAlias(context.getTrace() + "." + name->name, type->getLLVMType(context));
                success &= context.setRecordAlias(context.getTrace() + "." + name->name, cast_node<RecordTypeNode>(type));
                if (!success) throw CodegenException("Duplicate type alias in function " + context.getTrace() + ": " + name->name);
                context.log() << "\tRecord alias in function " << context.getTrace() << ": " << name->name << std::endl;
            }
            else
            {
                bool success = context.setAlias(context.getTrace() + "." + name->name, type->getLLVMType(context));
                if (!success) throw CodegenException("Duplicate type alias in function " + context.getTrace() + ": " + name->name);
            }
        }
        else
        {
            if (type->type == Type::Array)
            {
                bool success = context.setArrayAlias(name->name, cast_node<ArrayTypeNode>(type));
                if (!success) throw CodegenException("Duplicate type alias in main program: " + name->name);
                context.log() << "\tGlobal array alias: " << name->name << std::endl;
            }
            else if (type->type == Type::Record)
            {
                bool success = context.setAlias(name->name, type->getLLVMType(context));
                success &= context.setRecordAlias(name->name, cast_node<RecordTypeNode>(type));
                if (!success) throw CodegenException("Duplicate type alias in main program: " + name->name);
                context.log() << "\tGlobal record alias: " << name->name << std::endl;
            }
            else
            {
                bool success = context.setAlias(name->name, type->getLLVMType(context));
                if (!success) throw CodegenException("Duplicate type alias in main program: " + name->name);
            }        
        }
        return nullptr;
    }


} // namespace spc
