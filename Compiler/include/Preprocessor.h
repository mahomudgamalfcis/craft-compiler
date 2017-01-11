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
 * File:   Preprocessor.h
 * Author: Daniel McCarthy
 *
 * Created on 08 January 2017, 01:58
 */

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <memory>
#include <map>
#include <string>
#include "CompilerEntity.h"

class Tree;
class Branch;
class MacroIfDefBranch;
class MacroDefineBranch;

class EXPORT Preprocessor : public CompilerEntity
{
public:
    Preprocessor(Compiler* compiler);
    virtual ~Preprocessor();
    void setTree(std::shared_ptr<Tree> tree);
    void process();
    bool is_macro(std::string macro_name);
    bool is_definition_registered(std::string definition_name);
    void define_definition(std::string definition_name, std::string value);
    std::string get_definition_value(std::string definition_name);
private:
    void process_macro(std::shared_ptr<Branch> macro);
    void process_macro_ifdef(std::shared_ptr<MacroIfDefBranch> macro_ifdef_branch);
    void process_macro_define(std::shared_ptr<MacroDefineBranch> macro_define_branch);
    std::string evaluate_expression(std::shared_ptr<Branch> value_branch);
    std::string evaluate_expression_part(std::shared_ptr<Branch> value_branch);
    bool is_string_numeric_only(std::string str);
    std::shared_ptr<Tree> tree;
    std::map<std::string, std::string> definitions;
};

#endif /* PREPROCESSOR_H */
