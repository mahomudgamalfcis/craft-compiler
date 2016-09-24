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
 * File:   ParserRule.h
 * Author: Daniel McCarthy
 *
 * Created on 30 May 2016, 02:28
 */

#ifndef PARSERRULE_H
#define PARSERRULE_H

#include <string>
#include <vector>
#include <memory>
#include "Stack.h"
#include "Branch.h"
#include "ParserRuleRequirement.h"

class ParserRule {
public:
    ParserRule(std::string name);
    virtual ~ParserRule();  
    void canCreateBranch(bool branchable);
    bool isBranchable();
    std::string getName();
    void addRequirement(std::shared_ptr<ParserRuleRequirement> requirement);
    Stack<std::shared_ptr<ParserRuleRequirement>> getRequirements();
    size_t getTotalRequirements();
    std::shared_ptr<ParserRuleRequirement> getRequirementByIndex(int index);
private:
    std::string name;
    Stack<std::shared_ptr<ParserRuleRequirement>> requirements;
    bool branchable;

};

#endif /* PARSERRULE_H */
