#ifndef CODEGEN_CONTEXT_H
#define CODEGEN_CONTEXT_H
#include "utils/ast.hpp"

#include <string>
#include <fstream>
#include <list>
#include <map>
#include <iomanip>

static llvm::LLVMContext llvm_context;

namespace spc
{
    class ArrayTypeNode;
    class RecordTypeNode;  

    class CodegenException : public std::exception {
    public:
        explicit CodegenException(const std::string &description) : description(description) {};
        const char *what() const noexcept {
            return description.c_str();
        }
    private:
        // unsigned id;
        std::string description;
    };

    class CodegenContext
    {
    private:
        std::unique_ptr<llvm::Module> _module;
        llvm::IRBuilder<> builder;
        std::map<std::string, llvm::Type *> aliases;
        std::map<std::string, std::shared_ptr<ArrayTypeNode>> arrAliases;
        std::map<std::string, std::shared_ptr<std::pair<int, int>>> arrTable;
        std::map<std::string, std::shared_ptr<RecordTypeNode>> recAliases;
        std::map<std::string, llvm::Value*> locals;
        std::map<std::string, llvm::Value*> consts;
        std::map<std::string, llvm::Constant*> constVals;
        std::ofstream of;

        void createTempStr()
        {
            auto *ty = llvm::Type::getInt8Ty(llvm_context);
            llvm::Constant *z = llvm::ConstantInt::get(ty, 0);
            llvm::ArrayType* arr = llvm::ArrayType::get(ty, 256);
            std::vector<llvm::Constant *> initVector;
            for (int i = 0; i < 256; i++)
                initVector.push_back(z);
            auto *variable = llvm::ConstantArray::get(arr, initVector);

            // std::cout << "Created array" << std::endl;

            new llvm::GlobalVariable(*_module, variable->getType(), false, llvm::GlobalVariable::ExternalLinkage, variable, "__tmp_str");
        }

    public:
        bool is_subroutine;
        std::list<std::string> traces;
        llvm::Function *printfFunc, *sprintfFunc, *scanfFunc, *absFunc, *fabsFunc, *sqrtFunc, *strcpyFunc, *strcatFunc, *getcharFunc, *strlenFunc, *atoiFunc;

        std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
        std::unique_ptr<llvm::legacy::PassManager> mpm;

        std::ofstream &log() { return of; }

        static std::string getLLVMTypeName(const llvm::Type *ty)
        {
            // static std::map<int, std::string> typeIDMap = {
            //     {11, "INTEGER/CHAR/BOOL"}, {15, "STRING"}, {3, "DOUBLE"}, {14, "ARRAY/STRING"}, {13, "RECORD"}, {11, "FUNCTION"}
            // };
            int id = ty->getTypeID();
            llvm::Type *arrTy;
            switch (id)
            {
            case 11:
                if (ty->isIntegerTy(1)) return "Boolean";
                if (ty->isIntegerTy(8)) return "Char";
                if (ty->isIntegerTy(32)) return "Integer/Long";
                return "Unknown";
            case 15: 
                if (ty->getPointerElementType()->isIntegerTy(8)) return "String";
                return "Unknown";
            case 3:
                return "Real";
            case 14:
                arrTy = ty->getArrayElementType();
                if (arrTy->isIntegerTy(8)) return "String";
                return "Array of " + getLLVMTypeName(arrTy);
            case 13:
                return "Record";
            case 12:
                return "Function";
            case 0:
                return "Void";
            default:
                return "Unknown";
            }
        }

        void printGlobals()
        {
            std::cout << "Globals Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(39) << '+' << '+' << std::endl;
            std::cout << '|' << std::setfill(' ') << std::setw(19) << "Name" << '|' << std::setw(38) << "Type" << '|' << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(39) << '+' << '+' << std::endl;
            auto &gbList = _module->getGlobalList();
            for (auto itr = gbList.begin(); itr != gbList.end(); itr++)
            {
                // llvm::Global *gv = itr;
                std::string name = itr->getName();
                std::string type = getLLVMTypeName(itr->getType()->getPointerElementType());

                std::cout << '|' << std::setfill(' ') << std::setw(19) << name << '|' << std::setw(38) << type << '|' << std::endl;
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(39) << '+' << '+' << std::endl;
            }
        }

        void printFuncs()
        {
            std::cout << "Funcion Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            std::cout << '|' << std::setw(19) << std::setfill(' ')  << "Name" << '|' << std::setw(19) << "Returns" << '|' << std::setw(38) << "Paremeters" << '|' << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            auto &funcList = _module->getFunctionList();
            for (auto itr = funcList.begin(); itr != funcList.end(); itr++)
            {
                // llvm::Global *gv = itr;
                std::string name = itr->getName();
                auto *funcTy = itr->getFunctionType();
                std::string type = getLLVMTypeName(funcTy->getReturnType());
                std::string paras = "";
                for (auto param = funcTy->param_begin(); param != funcTy->param_end(); param++)
                {
                    if (param == funcTy->param_begin()) paras = paras + getLLVMTypeName(*param);
                    else paras = paras + ", " + getLLVMTypeName(*param);
                }
                std::cout << '|' << std::setw(19) << std::setfill(' ')  << name << '|' << std::setw(19) << type << '|' << std::setw(38) << paras << '|' << std::endl;
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            }
        }

        void printAliases()
        {
            std::cout << "Aliases Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            std::cout << '|' << std::setw(19) << std::setfill(' ')  << "Function" << '|' << std::setw(19) << "Name" << '|' << std::setw(38) << "Type" << '|' << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            for (auto it = aliases.cbegin(); it != aliases.cend(); it++)
            {
                std::string c = it->first;
                int pos = c.find('.');
                std::string c1;
                if (pos == -1) c1 = "main";
                else c1 = c.substr(0, pos);     
                std::string c2 = c.substr(pos + 1, c.length());
                llvm::Type *val = it->second;
                std::string c3 = getLLVMTypeName(val);
                std::cout << '|' << std::setw(19) << std::setfill(' ')  << c1 << '|' << std::setw(19) << c2 << '|' << std::setw(38) << c3 << '|' << std::endl;
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            }
        }

        void printArrAliases()
        {
            std::cout << "Array Aliases Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            std::cout << '|' << std::setw(19) << std::setfill(' ')  << "Function" << '|' << std::setw(19) << "Name" << '|' << std::setw(38) << "Type" << '|' << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            for (auto it = arrAliases.cbegin(); it != arrAliases.cend(); it++)
            {
                std::string c = it->first;
                int pos = c.find('.');
                std::string c1;
                if (pos == -1) c1 = "main";
                else c1 = c.substr(0, pos);    
                std::string c2 = c.substr(pos + 1, c.length());
                auto val = it->second;

                auto start = val->range_start->codegen(*this);
                llvm::ConstantInt *startI = llvm::dyn_cast<llvm::ConstantInt>(start);
                assert(startI != nullptr);
                int startInt = startI->getSExtValue();

                auto end = val->range_end->codegen(*this);
                llvm::ConstantInt *endI = llvm::dyn_cast<llvm::ConstantInt>(end);
                assert(endI != nullptr);
                int endInt = endI->getSExtValue();

                std::string type = getLLVMTypeName(val->itemType->getLLVMType(*this));
                std::string info = "[" + std::to_string(startInt) + ", " + std::to_string(endInt) + "] of " + type;

                std::cout << '|' << std::setw(19) << std::setfill(' ')  << c1 << '|' << std::setw(19) << c2 << '|' << std::setw(38) << info << '|' << std::endl;
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            }
        }

        void printArrTable()
        {
            std::cout << "Array Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            std::cout << '|' << std::setw(19) << std::setfill(' ')  << "Function" << '|' << std::setw(19) << "Name" << '|' << std::setw(38) << "Type" << '|' << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            for (auto it = arrTable.cbegin(); it != arrTable.cend(); it++)
            {
                std::string c = it->first;
                int pos = c.find('.');
                std::string c1;
                if (pos == -1) c1 = "main";
                else c1 = c.substr(0, pos);     
                std::string c2 = c.substr(pos + 1, c.length());
                auto val = it->second;
                std::string c3 = "[" + std::to_string((*val).first) + ", " + std::to_string((*val).second) + "]";
                std::cout << '|' << std::setw(19) << std::setfill(' ')  << c1 << '|' << std::setw(19) << c2 << '|' << std::setw(38) << c3 << '|' << std::endl;
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            }
        }

        void printRecAliases()
        {
            std::cout << "Record Aliases Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(69) << '+' << '+' << std::endl;
            std::cout << '|' << std::setw(19) << std::setfill(' ')  << "Function" << '|' << std::setw(19) << "Name" << '|' << std::setw(68) << "Members" << '|' << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(69) << '+' << '+' << std::endl;
            for (auto it = recAliases.cbegin(); it != recAliases.cend(); it++)
            {
                std::string c = it->first;
                int pos = c.find('.');
                std::string c1;
                if (pos == -1) c1 = "main";
                else c1 = c.substr(0, pos);      
                std::string c2 = c.substr(pos + 1, c.length());
                auto val = it->second;
                std::string type = getLLVMTypeName(val->getLLVMType(*this));
                std::string info = type + "{";
                for(auto iy = val->field.begin(); iy != val->field.end(); iy++ )
                {   
                    std::string n = (*iy)->name->name;
                    std::string t = getLLVMTypeName((*iy)->type->getLLVMType(*this));
                    if(iy == val->field.begin()) info = info + n + ": " + t;
                    else info = info + ", " + n + ": " + t;
                }
                info = info + "}";
                

                std::cout << '|' << std::setw(19) << std::setfill(' ')  << c1 << '|' << std::setw(19) << c2 << '|' << std::setw(68) << info << '|' << std::endl;
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(69) << '+' << '+' << std::endl;
            }
        }

        void printLocals()
        {
            std::cout << "Locals Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            std::cout << '|' << std::setw(19) << std::setfill(' ')  << "Function" << '|' << std::setw(19) << "Name" << '|' << std::setw(38) << "Type" << '|' << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            for (auto it = locals.cbegin(); it != locals.cend(); it++)
            {
                std::string c = it->first;
                int pos = c.find('.');
                std::string c1 = c.substr(0, pos);      
                std::string c2 = c.substr(pos + 1, c.length());
                llvm::Value *val = it->second;
                std::string c3 = getLLVMTypeName(val->getType()->getPointerElementType());

                std::cout << '|' << std::setw(19) << std::setfill(' ')  << c1 << '|' << std::setw(19) << c2 << '|' << std::setw(38) << c3 << '|' << std::endl;
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            }
        }

        void printConsts()
        {
            std::cout << "Consts Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            std::cout << '|' << std::setw(19) << std::setfill(' ')  << "Function" << '|' << std::setw(19) << "Name" << '|' << std::setw(38) << "Type" << '|' << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            for (auto it = consts.cbegin(); it != consts.cend(); it++)
            {
                std::string c = it->first;
                int pos = c.find('.');
                std::string c1;
                if (pos == -1) c1 = "main";
                else c1 = c.substr(0, pos);
                std::string c2 = c.substr(pos + 1, c.length());
                llvm::Value *val = it->second;
                std::string c3 = getLLVMTypeName(val->getType());

                std::cout << '|' << std::setw(19) << std::setfill(' ')  << c1 << '|' << std::setw(19) << c2 << '|' << std::setw(38) << c3 << '|' << std::endl;
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(39) << '+' << '+' << std::endl;
            }
        }
        void printConstant(llvm::Constant *c)
        {
            auto ty = c->getType();
            if (ty->isIntegerTy()) // int
            {
                llvm::ConstantInt *ci = llvm::dyn_cast<llvm::ConstantInt>(c);
                assert(ci != nullptr);
                if (ty->isIntegerTy(32))
                {
                    int intVal = ci->getSExtValue();
                    std::cout << std::setw(19) << "Integer/Long" << '|' << std::setw(19) << intVal << '|' << std::endl;
                }
                else if (ty->isIntegerTy(8))
                {
                    char charVal = ci->getSExtValue();
                    std::cout << std::setw(19) << "Char" << '|' << std::setw(19) << charVal << '|' << std::endl;
                }
                else if (ty->isIntegerTy(1))
                {
                    bool boolVal = ci->getZExtValue();
                    std::cout << std::setw(19) << "Boolean" << '|' << std::setw(19) << std::boolalpha << boolVal << '|' << std::endl;
                }
            }
            else if (ty->isDoubleTy())
            {
                llvm::ConstantFP *cf = llvm::dyn_cast<llvm::ConstantFP>(c);
                assert(cf != nullptr);
                double dVal = cf->getValueAPF().convertToDouble();
                std::cout << std::setw(19) << "Real" << '|' << std::setw(19) <<  dVal << '|' << std::endl;
            }
        }

        void printConstVals()
        {
            std::cout << "ConstVals Table:" << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(20) << '+' << std::setw(20) << '+' << '+' << std::endl;
            std::cout << '|' << std::setw(19) << std::setfill(' ')  << "Function" << '|' << std::setw(19) << "Name" << '|' << std::setw(19) << "Type" 
                        << '|' << std::setw(19) << "Value" << '|'  << std::endl;
            std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(20) << '+' << std::setw(20) << '+' << '+' << std::endl;
            for (auto it = constVals.cbegin(); it != constVals.cend(); it++)
            {
                std::string c = it->first;
                int pos = c.find('.');
                std::string c1;
                if (pos == -1) c1 = "main";
                else c1 = c.substr(0, pos);
                std::string c2 = c.substr(pos + 1, c.length());
                llvm::Constant *val = it->second;

                std::cout << '|' << std::setw(19) << std::setfill(' ')  << c1 << '|' << std::setw(19) << c2 << '|';
                printConstant(val);
                std::cout << std::left << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << '+' << std::setw(20) << '+' << std::setw(20) << '+' << '+' << std::endl;
            }
        }

        CodegenContext(const std::string &module_id, bool opt = false)
            : builder(llvm::IRBuilder<>(llvm_context)), _module(std::make_unique<llvm::Module>(module_id, llvm_context)), is_subroutine(false), of("compile.log")
        {
            if (of.fail())
                throw CodegenException("Fails to open compile log");

            createTempStr();

            auto printfTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context)}, true);
            printfFunc = llvm::Function::Create(printfTy, llvm::Function::ExternalLinkage, "printf", *_module);

            auto sprintfTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context), llvm::Type::getInt8PtrTy(llvm_context)}, true);
            sprintfFunc = llvm::Function::Create(sprintfTy, llvm::Function::ExternalLinkage, "sprintf", *_module);

            auto scanfTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context)}, true);
            scanfFunc = llvm::Function::Create(scanfTy, llvm::Function::ExternalLinkage, "scanf", *_module);

            auto absTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt32Ty(llvm_context)}, false);
            absFunc = llvm::Function::Create(absTy, llvm::Function::ExternalLinkage, "abs", *_module);

            auto fabsTy = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm_context), {llvm::Type::getDoubleTy(llvm_context)}, false);
            fabsFunc = llvm::Function::Create(fabsTy, llvm::Function::ExternalLinkage, "fabs", *_module);

            auto sqrtTy = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm_context), {llvm::Type::getDoubleTy(llvm_context)}, false);
            sqrtFunc = llvm::Function::Create(sqrtTy, llvm::Function::ExternalLinkage, "sqrt", *_module);

            auto strcpyTy = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context), llvm::Type::getInt8PtrTy(llvm_context)}, false);
            strcpyFunc = llvm::Function::Create(strcpyTy, llvm::Function::ExternalLinkage, "strcpy", *_module);

            auto strcatTy = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context), llvm::Type::getInt8PtrTy(llvm_context)}, false);
            strcatFunc = llvm::Function::Create(strcatTy, llvm::Function::ExternalLinkage, "strcat", *_module);

            auto strlenTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context)}, false);
            strlenFunc = llvm::Function::Create(strlenTy, llvm::Function::ExternalLinkage, "strlen", *_module);

            auto atoiTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context)}, false);
            atoiFunc = llvm::Function::Create(atoiTy, llvm::Function::ExternalLinkage, "atoi", *_module);

            auto getcharTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), false);
            getcharFunc = llvm::Function::Create(getcharTy, llvm::Function::ExternalLinkage, "getchar", *_module);

            printfFunc->setCallingConv(llvm::CallingConv::C);
            sprintfFunc->setCallingConv(llvm::CallingConv::C);
            scanfFunc->setCallingConv(llvm::CallingConv::C);
            absFunc->setCallingConv(llvm::CallingConv::C);
            fabsFunc->setCallingConv(llvm::CallingConv::C);
            sqrtFunc->setCallingConv(llvm::CallingConv::C);
            strcpyFunc->setCallingConv(llvm::CallingConv::C);
            strcatFunc->setCallingConv(llvm::CallingConv::C);
            strlenFunc->setCallingConv(llvm::CallingConv::C);
            atoiFunc->setCallingConv(llvm::CallingConv::C);
            getcharFunc->setCallingConv(llvm::CallingConv::C);

            if (opt)
            {
                fpm = std::make_unique<llvm::legacy::FunctionPassManager>(_module.get());
                fpm->add(llvm::createPromoteMemoryToRegisterPass());
                fpm->add(llvm::createInstructionCombiningPass());
                fpm->add(llvm::createReassociatePass());
                fpm->add(llvm::createGVNPass());
                fpm->add(llvm::createCFGSimplificationPass());
                fpm->doInitialization();
                mpm = std::make_unique<llvm::legacy::PassManager>();
                mpm->add(llvm::createConstantMergePass());
                mpm->add(llvm::createFunctionInliningPass());
            }

            // std::cout << builder.getInt32Ty()->getTypeID() << std::endl;
            // std::cout << builder.getInt8Ty()->getTypeID() << std::endl;
            // std::cout << builder.getInt8PtrTy()->getTypeID() << std::endl;
            // std::cout << builder.getDoubleTy()->getTypeID() << std::endl;
            // std::cout << llvm::ArrayType::get(builder.getInt8Ty(), 256)->getTypeID() << std::endl;
            // std::cout << llvm::StructType::get(llvm_context, {builder.getInt8Ty(), builder.getInt8Ty()})->getTypeID() << std::endl;
            // std::cout << getcharTy->getTypeID() << std::endl;

        }
        ~CodegenContext()
        {
            if (of.is_open()) of.close();
            printGlobals();
            std::cout << std::endl;
            printFuncs();
            std::cout << std::endl;
            printAliases();
            std::cout << std::endl;
            printArrAliases();
            std::cout << std::endl;
            printArrTable();
            std::cout << std::endl;
            printRecAliases();
            std::cout << std::endl;
            printConsts();
            std::cout << std::endl;
            printLocals();
            std::cout << std::endl;
            printConstVals();
            std::cout << std::endl;
        }

        llvm::Value *getTempStrPtr()
        {
            auto *value = _module->getGlobalVariable("__tmp_str");
            if (value == nullptr)
                throw CodegenException("Global temp string not found");
            llvm::Value *zero = llvm::ConstantInt::getSigned(builder.getInt32Ty(), 0);
            return builder.CreateInBoundsGEP(value, {zero, zero});
        }

        std::string getTrace() 
        {
            if (traces.empty()) return "main";
            return traces.back(); 
        }

        llvm::Value *getLocal(const std::string &key) 
        {
            auto V = locals.find(key);
            if (V == locals.end())
                return nullptr;
            return V->second;
        };
        bool setLocal(const std::string &key, llvm::Value *value) 
        {
            if (getLocal(key))
                return false;
            locals[key] = value;
            return true;
        }
        llvm::Value *getConst(const std::string &key) 
        {
            auto V = consts.find(key);
            if (V == consts.end())
                return nullptr;
            return V->second;
        };
        bool setConst(const std::string &key, llvm::Value *value) 
        {
            if (getConst(key))
                return false;
            consts[key] = value;
            return true;
        }
        llvm::Constant* getConstVal(const std::string &key) 
        {
            auto V = constVals.find(key);
            if (V == constVals.end())
                return nullptr;
            return V->second;
        };
        llvm::ConstantInt* getConstInt(const std::string &key) 
        {
            auto V = constVals.find(key);
            if (V == constVals.end())
                return nullptr;
            llvm::Constant *val = V->second;
            if (!val->getType()->isIntegerTy())
                throw CodegenException("Case branch must be integer type!");
            return llvm::cast<llvm::ConstantInt>(val);
        };
        bool setConstVal(const std::string &key, llvm::Constant* value) 
        {
            if (getConstVal(key))
                return false;
            constVals[key] = value;
            return true;
        }
        std::shared_ptr<std::pair<int, int>> getArrayEntry(const std::string &key) 
        {
            auto V = arrTable.find(key);
            if (V == arrTable.end())
                return nullptr;
            return V->second;
        }
        bool setArrayEntry(const std::string &key, const std::shared_ptr<std::pair<int, int>> &value) 
        {
            if (getArrayEntry(key))
                return false;
            assert(value != nullptr);
            arrTable[key] = value;
            return true;
        }
        bool setArrayEntry(const std::string &key, const int start, const int end) 
        {
            if (getArrayEntry(key))
                return false;
            assert(start <= end);
            auto value = std::make_shared<std::pair<int, int>>(start, end);
            arrTable[key] = value;
            return true;
        }
        std::shared_ptr<ArrayTypeNode> getArrayAlias(const std::string &key) 
        {
            auto V = arrAliases.find(key);
            if (V == arrAliases.end())
                return nullptr;
            return V->second;
        }
        bool setArrayAlias(const std::string &key, const std::shared_ptr<ArrayTypeNode> &value) 
        {
            if (getArrayAlias(key))
                return false;
            assert(value != nullptr);
            arrAliases[key] = value;
            return true;
        }
        std::shared_ptr<RecordTypeNode> getRecordAlias(const std::string &key) 
        {
            auto V = recAliases.find(key);
            if (V == recAliases.end())
                return nullptr;
            return V->second;
        }
        bool setRecordAlias(const std::string &key, const std::shared_ptr<RecordTypeNode> &value) 
        {
            if (getRecordAlias(key))
                return false;
            assert(value != nullptr);
            recAliases[key] = value;
            return true;
        }
        llvm::Type *getAlias(const std::string &key) 
        {
            auto V = aliases.find(key);
            if (V == aliases.end())
                return nullptr;
            return V->second;
        }
        bool setAlias(const std::string &key, llvm::Type *value) 
        {
            if (getAlias(key))
                return false;
            aliases[key] = value;
            return true;
        }
        llvm::IRBuilder<> &getBuilder()
        {
            return builder;
        }
        std::unique_ptr<llvm::Module> &getModule() {
            return _module;
        }
        void dump() 
        {
            _module->print(llvm::errs(), nullptr);
        }

    };
    
    
} // namespace spc


#endif
