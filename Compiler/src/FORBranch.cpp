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
 * File:   FORBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 20 October 2016, 04:19
 * 
 * Description: 
 */

#include "FORBranch.h"
#include "BODYBranch.h"
#include "VDEFBranch.h"

FORBranch::FORBranch(Compiler* compiler) : ScopeBranch(compiler, "FOR", "")
{
}

FORBranch::~FORBranch()
{
}

void FORBranch::setInitBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("init_branch", branch);
}

void FORBranch::setCondBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("cond_branch", branch);
}

void FORBranch::setLoopBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("loop_branch", branch);
}

void FORBranch::setBodyBranch(std::shared_ptr<BODYBranch> branch)
{
    CustomBranch::registerBranch("body_branch", branch);
}

std::shared_ptr<Branch> FORBranch::getInitBranch()
{
    return CustomBranch::getRegisteredBranchByName("init_branch");
}

std::shared_ptr<Branch> FORBranch::getCondBranch()
{
    return CustomBranch::getRegisteredBranchByName("cond_branch");
}

std::shared_ptr<Branch> FORBranch::getLoopBranch()
{
    return CustomBranch::getRegisteredBranchByName("loop_branch");
}

std::shared_ptr<BODYBranch> FORBranch::getBodyBranch()
{
    return std::dynamic_pointer_cast<BODYBranch>(CustomBranch::getRegisteredBranchByName("body_branch"));
}

int FORBranch::getScopeSize(GET_SCOPE_SIZE_OPTIONS options, std::function<bool(std::shared_ptr<Branch> child_branch) > elem_proc_start, std::function<bool(std::shared_ptr<Branch> child_branch) > elem_proc_end, bool *should_stop)
{
    int size = 0;
    std::shared_ptr<Branch> init_branch = getInitBranch();
    if (!invoke_scope_size_proc_if_possible(elem_proc_start, init_branch, should_stop))
    {
        return 0;
    }


    if (init_branch->getType() == "V_DEF")
    {
        std::shared_ptr<VDEFBranch> vdef_init_branch = std::dynamic_pointer_cast<VDEFBranch>(init_branch);
        // The INIT branch of our "for" loop is a V_DEF so its a declaration, which means it has a size
        size = getCompiler()->getSizeOfVarDef(vdef_init_branch);
    }


    // Invoke the end for the init branch
    if (!invoke_scope_size_proc_if_possible(elem_proc_end, init_branch, should_stop))
    {
        return size;
    }

    // Do we include subscopes?
    if (options & GET_SCOPE_SIZE_INCLUDE_SUBSCOPES)
    {

        std::shared_ptr<BODYBranch> body_branch = getBodyBranch();
        size += body_branch->getScopeSize(options, elem_proc_start, elem_proc_end, should_stop);
    }

    // Shall we include the parent scopes size?
    if (options & GET_SCOPE_SIZE_INCLUDE_PARENT_SCOPES)
    {
        if (hasLocalScope())
        {
            // Lets check that we are aloud to invoke the parent scope
            if (invoke_scope_size_proc_if_possible(elem_proc_start, getLocalScope(), should_stop))
            {
                size += getLocalScope()->getScopeSize(options, elem_proc_start, elem_proc_end, should_stop);
                // Lets invoke the end proc
                invoke_scope_size_proc_if_possible(elem_proc_end, getLocalScope(), should_stop);
            }

        }
    }

    return size;
}

std::shared_ptr<VDEFBranch> FORBranch::getVariableDefinitionBranch(std::shared_ptr<VarIdentifierBranch> var_iden, bool lookup_scope, bool no_follow)
{
    // Check the body scope
    std::shared_ptr<BODYBranch> body_branch = getBodyBranch();
    std::shared_ptr<VDEFBranch> vdef_branch = body_branch->getVariableDefinitionBranch(var_iden, false, no_follow);
    if (vdef_branch != NULL)
    {
        // Variable definition found in body scope
        return vdef_branch;
    }

    // Check the init scope
    std::shared_ptr<Branch> init_branch = getInitBranch();
    if (init_branch->getBranchType() == BRANCH_TYPE_VDEF)
    {
        std::shared_ptr<VDEFBranch> vdef_init_branch = std::dynamic_pointer_cast<VDEFBranch>(init_branch);
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = vdef_init_branch->getVariableIdentifierBranch();
        if (var_iden_branch->getVariableNameBranch()->getValue() == var_iden->getVariableNameBranch()->getValue())
        {
            // Found !
            return vdef_init_branch;
        }
    }


    // Nothing was found look up our local scope
    if (lookup_scope && hasParent())
    {
        return getLocalScope()->getVariableDefinitionBranch(var_iden, lookup_scope, no_follow);
    }
}

std::shared_ptr<VDEFBranch> FORBranch::getVariableDefinitionBranch(std::string var_name, bool lookup_scope)
{

}

void FORBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<FORBranch> for_branch_cloned = std::dynamic_pointer_cast<FORBranch>(cloned_branch);
    for_branch_cloned->setInitBranch(getInitBranch()->clone());
    for_branch_cloned->setCondBranch(getCondBranch()->clone());
    for_branch_cloned->setLoopBranch(getLoopBranch()->clone());
    for_branch_cloned->setBodyBranch(std::dynamic_pointer_cast<BODYBranch>(getBodyBranch()->clone()));
}

std::shared_ptr<Branch> FORBranch::create_clone()
{
    return std::shared_ptr<Branch>(new FORBranch(getCompiler()));
}