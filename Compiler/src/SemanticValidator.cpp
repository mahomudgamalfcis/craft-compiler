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
 * File:   SemanticValidator.cpp
 * Author: Daniel McCarthy
 *
 * Created on 10 December 2016, 00:07
 * 
 * Description: Semantically validates the abstract syntax tree.
 */

#include "SemanticValidator.h"
#include "Tree.h"
#include "branches.h"

SemanticValidator::SemanticValidator(Compiler* compiler) : CompilerEntity(compiler)
{
    this->logger = std::shared_ptr<Logger>(new Logger());
}

SemanticValidator::~SemanticValidator()
{
}

void SemanticValidator::setTree(std::shared_ptr<Tree> tree)
{
    this->tree = tree;
}

void SemanticValidator::validate()
{
    // Validate the top of the tree
    validate_top(this->tree->root);
}

void SemanticValidator::validate_top(std::shared_ptr<RootBranch> root_branch)
{
    for (std::shared_ptr<Branch> child : root_branch->getChildren())
    {
        validate_part(child);
    }
}

void SemanticValidator::validate_part(std::shared_ptr<Branch> branch)
{
    std::string type = branch->getType();
    if (type == "FUNC")
    {
        validate_function(std::dynamic_pointer_cast<FuncBranch>(branch));
    }
    else if (type == "BODY")
    {
        validate_body(std::dynamic_pointer_cast<BODYBranch>(branch));
    }
    else if (type == "V_DEF")
    {
        validate_vdef(std::dynamic_pointer_cast<VDEFBranch>(branch));
    }
    else if (type == "STRUCT_DEF")
    {
        validate_structure_definition(std::dynamic_pointer_cast<STRUCTDEFBranch>(branch));
    }
    else if (type == "STRUCT")
    {
        validate_structure(std::dynamic_pointer_cast<STRUCTBranch>(branch));
    }
    else if (type == "VAR_IDENTIFIER")
    {
        validate_var_access(std::dynamic_pointer_cast<VarIdentifierBranch>(branch));
    }
    else if (type == "ASSIGN")
    {
        validate_assignment(std::dynamic_pointer_cast<AssignBranch>(branch));
    }
}

void SemanticValidator::validate_function(std::shared_ptr<FuncBranch> func_branch)
{
    // Register the function error logged if function is already registered
    register_function(func_branch);

    // Validate the function arguments
    for (std::shared_ptr<Branch> func_arg_child : func_branch->getArgumentsBranch()->getChildren())
    {
        validate_part(func_arg_child);
    }


    // Validate the function body
    validate_part(func_branch->getBodyBranch());
}

void SemanticValidator::validate_body(std::shared_ptr<BODYBranch> body_branch)
{
    for (std::shared_ptr<Branch> child : body_branch->getChildren())
    {
        validate_part(child);
    }
}

void SemanticValidator::validate_vdef(std::shared_ptr<VDEFBranch> vdef_branch)
{
    std::shared_ptr<VarIdentifierBranch> vdef_var_iden_branch = vdef_branch->getVariableIdentifierBranch();
    std::string vdef_var_iden_name = vdef_var_iden_branch->getVariableNameBranch()->getValue();
    // Check to see if the variable definition is already registered in this scope
    for (std::shared_ptr<Branch> child : vdef_branch->getLocalScope()->getChildren())
    {
        // We check for a cast rather than type as other branches can extend V_DEF branch
        std::shared_ptr<VDEFBranch> vdef_child = std::dynamic_pointer_cast<VDEFBranch>(child);
        if (vdef_child != NULL)
        {
            std::shared_ptr<VarIdentifierBranch> vdef_child_var_iden_branch = vdef_child->getVariableIdentifierBranch();
            std::string vdef_child_var_iden_name = vdef_child_var_iden_branch->getVariableNameBranch()->getValue();
            // Is this the current branch we are validating?
            if (vdef_child == vdef_branch)
                continue;

            if (vdef_var_iden_name == vdef_child_var_iden_name)
            {
                // We already have a variable of this name on this scope
                logger->error("The variable \"" + vdef_var_iden_name + "\" has been redeclared", vdef_branch);
            }
        }
    }
}

/*
 * Validates that variable access is valid
 * upon validating a variable structure access many of the framework functions cannot be used to their fullest
 * this is because the tree improver has not yet improved the branches so that the framework can be used in full.
 */
void SemanticValidator::validate_var_access(std::shared_ptr<VarIdentifierBranch> var_iden_branch)
{
    // We need to check that the var identifier links to a valid variable.
    std::shared_ptr<VDEFBranch> root_vdef_branch = var_iden_branch->getVariableDefinitionBranch(true);

    if (root_vdef_branch == NULL)
    {
        // No V_DEF branch exists for this var identifier branch so it is illegal
        logger->error("The variable \"" + var_iden_branch->getVariableNameBranch()->getValue() + "\" could not be found", var_iden_branch);
    }
    else if (root_vdef_branch->getType() == "STRUCT_DEF"
            && var_iden_branch->hasStructureAccessBranch())
    {
        std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(root_vdef_branch);
        std::string struct_name = struct_def_branch->getDataTypeBranch()->getValue();
        // This is a structure definition so far the root of the structure is valid but its access may not be 

        std::shared_ptr<STRUCTBranch> struct_branch = this->tree->getGlobalStructureByName(struct_name);
        std::shared_ptr<VarIdentifierBranch> current = var_iden_branch->getStructureAccessBranch()->getVarIdentifierBranch();
        while (true)
        {
            std::shared_ptr<Branch> child = NULL;
            for (std::shared_ptr<Branch> c : struct_branch->getStructBodyBranch()->getChildren())
            {
                if (c->getBranchType() == BRANCH_TYPE_VDEF)
                {
                    std::shared_ptr<VDEFBranch> c_vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(c);
                    if (c_vdef_branch->getNameBranch()->getValue() == current->getVariableNameBranch()->getValue())
                        child = c_vdef_branch;
                }
            }
            if (child == NULL)
            {
                // The variable on the structure does not exist.
                this->logger->error("The variable \"" + current->getVariableNameBranch()->getValue()
                                    + "\" does not exist in structure \"" + struct_branch->getStructNameBranch()->getValue() + "\"", current);
                break;
            }

            if (current->hasStructureAccessBranch())
            {
                struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(child);
                std::string struct_name = struct_def_branch->getDataTypeBranch()->getValue();
                // Check that the structure is declared
                if (!this->tree->isGlobalStructureDeclared(struct_name))
                {
                    this->logger->error("The structure \"" + struct_name + "\" does not exist", current);
                    break;
                }
                else
                {
                    // Its declared so assign it for later
                    struct_branch = this->tree->getGlobalStructureByName(struct_name);
                }

                // Change the current variable in question
                current = current->getStructureAccessBranch()->getVarIdentifierBranch();
            }
            else
            {
                break;
            }
        }
    }
}

void SemanticValidator::validate_assignment(std::shared_ptr<AssignBranch> assign_branch)
{
    // Validate the variable access
    validate_var_access(assign_branch->getVariableToAssignBranch());

    // Validate that the value is legal

}

void SemanticValidator::validate_structure_definition(std::shared_ptr<STRUCTDEFBranch> struct_def_branch)
{
    // Validate the structure type
    std::string struct_data_type = struct_def_branch->getDataTypeBranch()->getValue();
    if (!this->tree->isGlobalStructureDeclared(struct_data_type))
    {
        this->logger->error("The structure variable has an illegal type of \"" + struct_data_type + "\"", struct_def_branch);
    }
    // Validate the V_DEF as all STRUCT_DEF's are V_DEF's
    validate_vdef(struct_def_branch);

}

void SemanticValidator::validate_structure(std::shared_ptr<STRUCTBranch> structure_branch)
{
    std::string struct_name = structure_branch->getStructNameBranch()->getValue();
    if (this->tree->isGlobalStructureDeclared(struct_name))
    {
        this->logger->error("The structure \"" + struct_name + "\" has been redeclared", structure_branch);
    }

    // Validate the structure body
    validate_body(structure_branch->getStructBodyBranch());
}

void SemanticValidator::validate_expression(std::shared_ptr<EBranch> e_branch)
{
    std::shared_ptr<Branch> left = e_branch->getFirstChild();
    std::shared_ptr<Branch> right = e_branch->getSecondChild();
}

    void SemanticValidator::validate_value(std::shared_ptr<Branch> branch, std::string requirement_type)
    {
        
    }
void SemanticValidator::register_function(std::shared_ptr<FuncDefBranch> func_def_branch)
{
    // Validate the function
    std::string func_name = func_def_branch->getNameBranch()->getValue();
    if (hasFunction(func_name))
    {
        critical_error("The function: \"" + func_name + "\" has already been declared but you are attempting to redeclare it", func_def_branch);
    }

    // Register it
    this->functions[func_name] = func_def_branch;
}

bool SemanticValidator::hasFunction(std::string function_name)
{
    return this->functions.find(function_name) != this->functions.end();
}

std::shared_ptr<FuncDefBranch> SemanticValidator::getFunction(std::string function_name)
{
    if (!hasFunction(function_name))
    {
        throw Exception("Function: " + function_name + " has not been found", "std::shared_ptr<FuncDefBranch> getFunction(std::string function_name)");
    }

    return this->functions[function_name];
}

void SemanticValidator::critical_error(std::string message, std::shared_ptr<Branch> branch)
{
    this->logger->error(message, std::dynamic_pointer_cast<CustomBranch>(branch));
    throw Exception("Critical Error!");
}

std::shared_ptr<Logger> SemanticValidator::getLogger()
{
    return this->logger;
}
