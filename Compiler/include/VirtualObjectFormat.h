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
 * File:   VirtualObjectFormat.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 15:41
 */

#ifndef VIRTUALOBJECTFORMAT_H
#define VIRTUALOBJECTFORMAT_H

#include <vector>
#include <memory>

#include "VirtualSegment.h"
#include "Stream.h"
#include "CompilerEntity.h"


class EXPORT VirtualObjectFormat : public CompilerEntity
{
public:
    VirtualObjectFormat(Compiler* compiler);
    virtual ~VirtualObjectFormat();

    std::shared_ptr<VirtualSegment> createSegment(std::string segment_name);
    std::shared_ptr<VirtualSegment> getSegment(std::string segment_name);
    std::vector<std::shared_ptr<VirtualSegment>> getSegments();

    bool hasSegment(std::string segment_name);
    
    void registerGlobalReference(std::shared_ptr<VirtualSegment> segment, std::string ref_name, int offset);
    std::vector<std::shared_ptr<GLOBAL_REF>> getGlobalReferences();
    std::vector<std::shared_ptr<GLOBAL_REF>> getGlobalReferencesForSegment(std::string segment_name);

    bool hasGlobalReference(std::string ref_name);
    std::shared_ptr<GLOBAL_REF> getGlobalReferenceByName(std::string ref_name);
    
    void registerExternalReference(std::string ref_name);
    std::vector<std::string> getExternalReferences();
    bool hasExternalReference(std::string ref_name);
    bool hasExternalReferences();

    Stream* getObjectStream();
    
    void append(std::shared_ptr<VirtualObjectFormat> obj_format);
    virtual void read(std::shared_ptr<Stream> input_stream) = 0;
    virtual void finalize() = 0;
protected:
    virtual std::shared_ptr<VirtualSegment> new_segment(std::string segment_name, uint32_t origin) = 0;
private:
    Stream object_stream;
    std::vector<std::shared_ptr<VirtualSegment>> segments;
    std::vector<std::string> external_references;
};

#endif /* VIRTUALOBJECTFORMAT_H */

