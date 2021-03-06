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
 * File:   VDEFBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 02:31
 */

#ifndef VDEFBRANCH_H
#define VDEFBRANCH_H

#include "CustomBranch.h"
#include "VarIdentifierBranch.h"

class DataTypeBranch;
class EXPORT VDEFBranch : public CustomBranch
{
public:
    VDEFBranch(Compiler* compiler, std::string branch_name = "V_DEF", std::string branch_value = "");
    virtual ~VDEFBranch();

    void setDataTypeBranch(std::shared_ptr<DataTypeBranch> branch);
    void setVariableIdentifierBranch(std::shared_ptr<Branch> branch);
    void setValueExpBranch(std::shared_ptr<Branch> branch);
    void setPointer(bool is_pointer, int depth = 0);
    void setVariableType(VARIABLE_TYPE var_type);

    std::shared_ptr<DataTypeBranch> getDataTypeBranch();
    std::shared_ptr<VarIdentifierBranch> getVariableIdentifierBranch();
    std::shared_ptr<Branch> getValueExpBranch();
    std::shared_ptr<Branch> getNameBranch();

    VARIABLE_TYPE getVariableType();

    bool hasValueExpBranch();
    int getPositionRelScope(POSITION_OPTIONS options = 0);
    int getPositionRelZero(POSITION_OPTIONS options = 0);

    bool isPointer();
    int getPointerDepth();
    bool isSigned();
    bool isPrimitive();
    bool hasCustomDataTypeSize();
    int getSize();
    
    virtual int getBranchType();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    int custom_data_type_size;
    VARIABLE_TYPE var_type;
    bool is_pointer;
    int ptr_depth;
};

#endif /* VDEFBRANCH_H */

