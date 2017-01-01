/*
    Goblin compiler v1.0 - The standard compiler for the Goblin language.
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
 * File:   FuncDefBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 18 December 2016, 23:33
 * 
 * Description: 
 */

#include "FuncDefBranch.h"
#include "FuncArgumentsBranch.h"

FuncDefBranch::FuncDefBranch(Compiler* compiler) : CustomBranch(compiler, "FUNC_DEF", "")
{
}

FuncDefBranch::FuncDefBranch(Compiler* compiler, std::string type, std::string value) : CustomBranch(compiler, type, value)
{

}

FuncDefBranch::~FuncDefBranch()
{
}

void FuncDefBranch::setReturnTypeBranch(std::shared_ptr<Branch> returnTypeBranch)
{
    this->registerBranch("func_return_type_branch", returnTypeBranch);
}

void FuncDefBranch::setNameBranch(std::shared_ptr<Branch> nameBranch)
{
    this->registerBranch("func_name_branch", nameBranch);
}

void FuncDefBranch::setArgumentsBranch(std::shared_ptr<FuncArgumentsBranch> argumentsBranch)
{
    this->registerBranch("func_arguments_branch", argumentsBranch);
}

std::shared_ptr<Branch> FuncDefBranch::getReturnTypeBranch()
{
    return this->getRegisteredBranchByName("func_return_type_branch");
}

std::shared_ptr<Branch> FuncDefBranch::getNameBranch()
{
    return this->getRegisteredBranchByName("func_name_branch");
}

std::shared_ptr<FuncArgumentsBranch> FuncDefBranch::getArgumentsBranch()
{
    return std::dynamic_pointer_cast<FuncArgumentsBranch>(this->getRegisteredBranchByName("func_arguments_branch"));
}

void FuncDefBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<FuncDefBranch> func_branch_cloned = std::dynamic_pointer_cast<FuncDefBranch>(cloned_branch);
    func_branch_cloned->setReturnTypeBranch(getReturnTypeBranch()->clone());
    func_branch_cloned->setNameBranch(getNameBranch()->clone());
    func_branch_cloned->setArgumentsBranch(std::dynamic_pointer_cast<FuncArgumentsBranch>(getArgumentsBranch()->clone()));
}

std::shared_ptr<Branch> FuncDefBranch::create_clone()
{
    return std::shared_ptr<Branch>(new FuncDefBranch(getCompiler(), getType(), getValue()));
}