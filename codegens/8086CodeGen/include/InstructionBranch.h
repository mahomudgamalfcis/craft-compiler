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
 * File:   InstructionBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 22:31
 */

#ifndef INSTRUCTIONBRANCH_H
#define INSTRUCTIONBRANCH_H

#include "CustomBranch.h"

class InstructionBranch : public CustomBranch
{
public:
    InstructionBranch(Compiler* compiler);
    virtual ~InstructionBranch();

    void setInstructionNameBranch(std::shared_ptr<Branch> ins_name_branch);
    std::shared_ptr<Branch> getInstructionNameBranch();

    void setLeftBranch(std::shared_ptr<Branch> left_branch);
    void setRightBranch(std::shared_ptr<Branch> right_branch);

    std::shared_ptr<Branch> getLeftBranch();
    std::shared_ptr<Branch> getRightBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:

};

#endif /* INSTRUCTIONBRANCH_H */

