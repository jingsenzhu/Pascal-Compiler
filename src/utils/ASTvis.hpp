#ifndef __ASTVIS__H__
#define __ASTVIS__H__

#include "utils/ast.hpp"

#include <fstream>

namespace spc {
    class ASTvis {
    public:
        ASTvis(const std::string &output = "spc.output.tex") : of(output, of.trunc | of.out)
        {
            if (!of.is_open())
            {
                std::cout << "failed to open file\n";
            }
        }
        ~ASTvis() = default;
        void travAST(const std::shared_ptr<ProgramNode>& prog);

    private:
        int travProgram(const std::shared_ptr<ProgramNode>& prog);
        int travRoutineBody(const std::shared_ptr<BaseRoutineNode>& prog);

        int travCONST(const std::shared_ptr<ConstDeclList>& const_declListAST);
        int travTYPE(const std::shared_ptr<TypeDeclList>& type_declListAST);
        int travVAR(const std::shared_ptr<VarDeclList>& var_declListAST);
        int travSubprocList(const std::shared_ptr<RoutineList>& subProc_declListAST);
        int travSubproc(const std::shared_ptr<RoutineNode>& subProc_AST);
        int travCompound(const std::shared_ptr<CompoundStmtNode>& compound_declListAST);

        int travStmt(const std::shared_ptr<StmtNode>&p_stmp);
        int travStmt(const std::shared_ptr<IfStmtNode>&p_stmp);
        int travStmt(const std::shared_ptr<WhileStmtNode>&p_stmp);
        int travStmt(const std::shared_ptr<ForStmtNode>&p_stmp);
        int travStmt(const std::shared_ptr<RepeatStmtNode>&p_stmp);
        int travStmt(const std::shared_ptr<ProcStmtNode>&p_stmp);
        int travStmt(const std::shared_ptr<AssignStmtNode>&p_stmp);
        int travStmt(const std::shared_ptr<CaseStmtNode>&p_stmp);

        int travExpr(const std::shared_ptr<ExprNode>& expr);
        int travExpr(const std::shared_ptr<BinaryExprNode>& expr);
        // int travExpr(const std::shared_ptr<UnaryExprNode>& expr);
        int travExpr(const std::shared_ptr<ArrayRefNode>& expr);
        int travExpr(const std::shared_ptr<RecordRefNode>& expr);
        int travExpr(const std::shared_ptr<ProcNode>& expr);
        int travExpr(const std::shared_ptr<CustomProcNode>& expr);
        int travExpr(const std::shared_ptr<SysProcNode>& expr);

        int node_cnt    = 0;
        int subproc_cnt = 0;
        int stmt_cnt    = 0;
        int leaf_cnt    = 0;
        std::fstream of;
        std::string texHeader = "\\documentclass{minimal} \n\
\\usepackage{tikz} \n\n\
\%\%\%< \n\
\\usepackage{verbatim} \n\
\\usepackage[active,tightpage]{preview} \n\
\\PreviewEnvironment{tikzpicture} \n\
\\setlength\\PreviewBorder{5pt}% \n\%\%\%> \n\n\
\\begin{comment} \n\
:Title: AST                 \n\
:Tags: Trees; Syntax        \n\
:Author: Eric Morris        \n\
:Slug: abstract-syntax-tree \n\
\\end{comment} \n\n\n\n\
\\usetikzlibrary{trees}  \n\n\n\
\\begin{document}\n\
\\tikzstyle{every node}=[draw=black,thick,anchor=west]  \n\
\\tikzstyle{selected}=[draw=red,fill=red!30]            \n\
\\tikzstyle{optional}=[dashed,fill=gray!50]             \n\
\\begin{tikzpicture}[\n\
    grow via three points={one child at (0.5,-0.7) and   \n\
    two children at (0.5,-0.7) and (0.5,-1.4)},          \n\
    edge from parent path={(\\tikzparentnode.south) |- (\\tikzchildnode.west)}  \n\
]\n";
        std::string texTail = ";\n\n\\end{tikzpicture}\n\\end{document}\n";
        std::string texNone = "child [missing] {}\n";
    };
}

#endif