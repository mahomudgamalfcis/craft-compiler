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
 * File:   StandardScopeBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 15 December 2016, 18:19
 * 
 * Description: The root class for all standard scope branches.
 */

#include "StandardScopeBranch.h"
#include "branches.h"

StandardScopeBranch::StandardScopeBranch(Compiler* compiler, std::string name, std::string value) : ScopeBranch(compiler, name, value)
{
}

StandardScopeBranch::~StandardScopeBranch()
{
}

int StandardScopeBranch::getScopeSize(GET_SCOPE_SIZE_OPTIONS options, std::function<bool(std::shared_ptr<Branch> child_branch) > elem_proc_start, std::function<bool(std::shared_ptr<Branch> child_branch) > elem_proc_end, bool *should_stop)
{
    // Should we stop here at this current scope?
    if (!invoke_scope_size_proc_if_possible(elem_proc_start, this->getptr(), should_stop))
    {
        return 0;
    }

    bool stop = false;
    int size = 0;

    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        if (!invoke_scope_size_proc_if_possible(elem_proc_start, child, should_stop))
        {
            stop = true;
            break;
        }

        if (child->getBranchType() == BRANCH_TYPE_VDEF)
        {
            std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(child);
            size += vdef_branch->getSize();
        }
        else
        {
            if (options & GET_SCOPE_SIZE_INCLUDE_SUBSCOPES)
            {
                std::string child_type = child->getType();
                if (child_type == "FOR")
                {
                    std::shared_ptr<FORBranch> for_branch = std::dynamic_pointer_cast<FORBranch>(child);
                    size += for_branch->getScopeSize(options, elem_proc_start, elem_proc_end, should_stop);
                    if (*should_stop)
                    {
                        break;
                    }
                }
            }
        }

        if (!invoke_scope_size_proc_if_possible(elem_proc_end, child, should_stop))
        {
            stop = true;
            break;
        }
    }


    
    // Lets invoke the end proc for this scope
    if(!invoke_scope_size_proc_if_possible(elem_proc_end, this->getptr(), should_stop))
    {
        return size;
    }

    if (!stop)
    {
        // Shall we include the parent scopes size?
        if (options & GET_SCOPE_SIZE_INCLUDE_PARENT_SCOPES)
        {
            if (hasLocalScope())
            {
                size += getLocalScope()->getScopeSize(options, elem_proc_start, elem_proc_end, should_stop);
            }
        }
    }

    return size;
}

std::shared_ptr<VDEFBranch> StandardScopeBranch::getVariableDefinitionBranch(std::shared_ptr<VarIdentifierBranch> var_iden, bool lookup_scope, bool no_follow)
{
    // Unsure about this
    std::shared_ptr<Branch> branch_to_stop_in_local_scope = var_iden->getParent();
    
    std::shared_ptr<VDEFBranch> found_branch = NULL;
    std::shared_ptr<Branch> var_iden_name_branch = var_iden->getVariableNameBranch();
    // Check local scope
    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        if (child->getBranchType() == BRANCH_TYPE_VDEF)
        {
            std::shared_ptr<VDEFBranch> c_vdef = std::dynamic_pointer_cast<VDEFBranch>(child);
            std::shared_ptr<VarIdentifierBranch> c_vdef_var_iden = c_vdef->getVariableIdentifierBranch();
            std::shared_ptr<Branch> c_var_name_branch = c_vdef_var_iden->getVariableNameBranch();
            if (c_var_name_branch->getValue() == var_iden_name_branch->getValue())
            {
                // Match on local scope!
                found_branch = std::dynamic_pointer_cast<VDEFBranch>(child);
                break;
            }
        }
        else if(branch_to_stop_in_local_scope == child)
        {  
            // Definitions wont be below its access so break
            break;
        }
    }

    if (found_branch == NULL)
    {
        // The root scope will never have a parent and therefore will never have scopes.
        if (lookup_scope && hasParent())
        {
            /* Ok we need to lookup the scope as we did not find the result in our own scope
             * This will act as a recursive action until either the variable is found or it is not */
            found_branch = getLocalScope()->getVariableDefinitionBranch(var_iden, true, no_follow);
        }
    }

    // Are we allowed to follow a structure access?
    if (!no_follow)
    {
        if (var_iden->hasStructureAccessBranch())
        {
            if (found_branch != NULL
                    && found_branch->getType() == "STRUCT_DEF")
            {
                // We have a structure access branch so we need to keep going

                std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(found_branch);
                std::shared_ptr<BODYBranch> struct_body = struct_def_branch->getStructBody();
                std::shared_ptr<VarIdentifierBranch> next_var_iden_branch =
                        std::dynamic_pointer_cast<VarIdentifierBranch>(var_iden->getStructureAccessBranch()->getFirstChild());
                found_branch = struct_body->getVariableDefinitionBranch(next_var_iden_branch, false);
            }
        }
    }


    return found_branch;
}

std::shared_ptr<VDEFBranch> StandardScopeBranch::getVariableDefinitionBranch(std::string var_name, bool lookup_scope)
{

}
