/*
    Craft Compiler v0.1.0 - The standard compiler for the Craft programming language.
    Copyright (C) 2016  Daniel McCarthy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   TreeImprover.h
 * Author: Daniel McCarthy
 *
 * Created on 10 December 2016, 12:52
 */

#ifndef TREEIMPROVER_H
#define TREEIMPROVER_H

#include <memory>
#include <vector>
#include "CompilerEntity.h"
class Tree;
class Branch;
class FuncBranch;
class FuncCallBranch;
class EBranch;
class VarIdentifierBranch;
class BODYBranch;
class IFBranch;
class WhileBranch;
class FORBranch;
class PTRBranch;
class STRUCTDEFBranch;

struct improvement
{
    bool is_in_struct;
    std::vector<std::shared_ptr<STRUCTDEFBranch>> nested_struct_branches_im_in;
    
    improvement();
    virtual ~improvement();
    void push_struct_def_branch(std::shared_ptr<STRUCTDEFBranch> struct_def_branch);
    std::shared_ptr<STRUCTDEFBranch> pop_struct_def_branch();
    bool isInStruct(std::string struct_name);
    std::shared_ptr<STRUCTDEFBranch> getStructDefFromStack(std::string struct_def_name);
};

class EXPORT TreeImprover : public CompilerEntity
{
public:
    TreeImprover(Compiler* compiler);
    virtual ~TreeImprover();

    void setTree(std::shared_ptr<Tree> tree);
    void improve();
    void improve_expression(std::shared_ptr<EBranch> expression_branch, struct improvement* improvement, bool is_root = true);
private:
    void improve_top(struct improvement* improvement);
    void improve_branch(std::shared_ptr<Branch> branch, struct improvement* improvement);
    void improve_func(std::shared_ptr<FuncBranch> func_branch, struct improvement* improvement);
    void improve_func_arguments(std::shared_ptr<Branch> func_args_branch, struct improvement* improvement);
    void improve_func_call(std::shared_ptr<FuncCallBranch> func_call_branch, struct improvement* improvement);
    void improve_body(std::shared_ptr<BODYBranch> body_branch,struct improvement* improvement, bool* has_return_branch = NULL);
    void improve_var_iden(std::shared_ptr<VarIdentifierBranch> var_iden_branch, struct improvement* improvement);
    void improve_if(std::shared_ptr<IFBranch> if_branch, struct improvement* improvement);
    void improve_while(std::shared_ptr<WhileBranch> while_branch, struct improvement* improvement);
    void improve_for(std::shared_ptr<FORBranch> for_branch, struct improvement* improvement);
    void improve_ptr(std::shared_ptr<PTRBranch> ptr_branch, struct improvement* improvement);

    std::shared_ptr<Tree> tree;
    VARIABLE_TYPE current_var_type;


};

#endif /* TREEIMPROVER_H */

