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
 * File:   Branch.h
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:41
 */

#ifndef BRANCH_H
#define BRANCH_H

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Exception.h"
#include "def.h"

class ScopeBranch;
class RootBranch;

class EXPORT Branch : public std::enable_shared_from_this<Branch>
{
public:
    Branch(std::string type, std::string value);
    virtual ~Branch();

    void addChild(std::shared_ptr<Branch> branch, std::shared_ptr<Branch> child_to_place_ahead_of = NULL, bool force_add = false);
    virtual void replaceChild(std::shared_ptr<Branch> child, std::shared_ptr<Branch> new_branch);
    void replaceSelf(std::shared_ptr<Branch> replacee_branch);
    virtual void removeChild(std::shared_ptr<Branch> child);
    void removeSelf();
    void replaceWithChildren();
    void setRemoved(bool is_removed);
    void setReplaced(std::shared_ptr<Branch> replacee_branch);
    void iterate_children(std::function<void(std::shared_ptr<Branch> child_branch) > iterate_func);
    int count_children(std::string type = "any", std::function<bool(std::shared_ptr<Branch> branch) > counter_function=NULL);
    void setParent(std::shared_ptr<Branch> branch);
    void setValue(std::string value);
    void setRoot(std::shared_ptr<RootBranch> root_branch);
    void setRootScope(std::shared_ptr<ScopeBranch> root_scope, bool set_to_all_children = false);
    void setLocalScope(std::shared_ptr<ScopeBranch> local_scope, bool set_to_all_children = false);

    std::shared_ptr<Branch> getFirstChild();
    std::shared_ptr<Branch> getSecondChild();
    std::shared_ptr<Branch> getThirdChild();
    std::shared_ptr<Branch> getFourthChild();
    std::vector<std::shared_ptr<Branch>> getChildren();
    bool hasChild(std::shared_ptr<Branch> branch);
    bool hasChildren();
    std::shared_ptr<Branch> getParent();
    bool hasParent();
    bool isChildAheadOfChild(std::shared_ptr<Branch> child1, std::shared_ptr<Branch> child2);
    std::shared_ptr<Branch> getFirstChildOfType(std::string type);
    bool hasChildOfType(std::string type);
    std::shared_ptr<Branch> lookUpTreeUntilParentTypeFound(std::string parent_type_to_find);
    std::shared_ptr<Branch> lookDownTreeUntilFirstChildOfType(std::string type);
    std::shared_ptr<Branch> lookDownTreeUntilLastChildOfType(std::string type);
    std::string getType();
    std::string getValue();

    std::shared_ptr<RootBranch> getRoot();
    std::shared_ptr<ScopeBranch> getRootScope();
    std::shared_ptr<ScopeBranch> getLocalScope();
    bool hasLocalScope();
    bool hasRootScope();

    std::shared_ptr<Branch> getptr();

    bool wasReplaced();
    bool isRemoved();
    std::shared_ptr<Branch> getReplaceeBranch();

    virtual int getBranchType();
    void validate_identity_on_tree();
    virtual void validity_check();
    virtual void rebuild();
    virtual std::shared_ptr<Branch> clone();
private:
    int getChildPosition(std::shared_ptr<Branch> child);
    std::string type;
    std::string value;
    std::vector<std::shared_ptr<Branch>> children;
    std::shared_ptr<Branch> parent;
    std::shared_ptr<Branch> replacee_branch;
    bool is_removed;
    // Points to the highest point of the tree the root.
    std::shared_ptr<RootBranch> root_branch;

    std::shared_ptr<ScopeBranch> root_scope;
    std::shared_ptr<ScopeBranch> local_scope;

};

#endif /* BRANCH_H */

