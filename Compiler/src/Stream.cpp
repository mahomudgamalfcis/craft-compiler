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
 * File:   Stream.cpp
 * Author: Daniel McCarthy
 *
 * Created on 20 June 2016, 01:18
 * 
 * Description: A basic stream class
 * 
 * Note: This stream class may not support big endian machines at its current state.
 * 
 * All write operations push to the back of the stack and all read operations read from the front of the stack.
 */

#include "Stream.h"
#include <string.h>
#include <iostream>
#include <map>

Stream::Stream()
{
    setPosition(0);
    this->joint_pos = 0;
    setOverwriteMode(false);
}

Stream::~Stream()
{
}

void Stream::loadFromFile(std::string filename)
{
    std::ifstream ifs;
    ifs.open(filename, std::ios::binary);
    if (!ifs.is_open())
    {
        throw Exception("Failed to open: " + filename);
    }

    this->loadFrom_ifstream(&ifs);
}

void Stream::loadFrom_ifstream(std::ifstream* stream)
{
    while (stream->good())
    {
        this->write8(stream->get());
    }
}

void Stream::setPosition(int position)
{
    this->pos = position;
}

void Stream::setOverwriteMode(bool overwrite_mode)
{
    this->overwrite_mode = overwrite_mode;
}

void Stream::write8(uint8_t c, int pos, bool ignore_joined_parents)
{
    bool is_custom_pos = true;
    // No position provided? lets use the current position
    if (pos == -1)
    {
        pos = this->pos;
        is_custom_pos = false;
    }

    if (this->vector.size() < 0)
    {
        throw Exception("out of bounds", "void Stream::write8(uint8_t c, int pos, bool ignore_joined_streams)");
    }

    if (isOverwriteModeEnabled())
    {
        if (this->vector.size() <= pos)
        {
            throw Exception("Attempting to overwrite position: " + std::to_string(pos) +
                            " but this position is out of bounds. Vector size: " + std::to_string(this->vector.size()),
                            "void Stream::write8(uint8_t c, int pos, bool ignore_joined_streams)");
        }
        vector.at(pos) = c;
    }
    else
    {
        vector.insert(vector.begin() + pos, c);
    }

    std::shared_ptr<Stream> joint_stream = getJoinedStreamForPosition(pos);
    if (joint_stream != NULL)
    {
        // A stream has been joint to the current position so lets write to the joint stream
        bool is_joint_stream_overwrite_enabled = joint_stream->isOverwriteModeEnabled();
        // Change the overwrite mode to our own overwrite mode
        joint_stream->setOverwriteMode(isOverwriteModeEnabled());
        std::map<std::shared_ptr<Stream>, int>::iterator it = this->joined_streams.find(joint_stream);
        int joint_stream_start_pos = it->second;
        int joint_write_rel_pos = pos - joint_stream_start_pos;
        joint_stream->write8(c, joint_write_rel_pos, true);
        // Restore the previous overwrite mode for this stream
        joint_stream->setOverwriteMode(is_joint_stream_overwrite_enabled);
    }

    if (!ignore_joined_parents)
    {
        updateDataForJoinedParents(c, pos);
    }

    if (!is_custom_pos)
        this->pos++;
}

void Stream::write16(uint16_t s)
{
    uint8_t c1 = s & 0xff;
    uint8_t c2 = s >> 8;
    write8(c1);
    write8(c2);
}

void Stream::write32(uint32_t i)
{
    uint16_t s1 = i & 0xffff;
    uint16_t s2 = i >> 16;
    write16(s1);
    write16(s2);
}

void Stream::writeStr(std::string str, bool write_null_terminator, size_t fill_to)
{
    writeStr(str.c_str(), write_null_terminator, fill_to);
}

/* Should probably try to write this better when I am a bit
 more awake. A bit sleepy at the moment*/
void Stream::writeStr(const char* str, bool write_null_terminator, size_t fill_to)
{
    int i = 0;
    while (str[i] != 0)
    {
        write8(str[i]);
        i++;
    }

    if (write_null_terminator)
    {
        write8(0);
        i++;
    }

    if (fill_to != -1 && fill_to - i > 0)
    {
        // We need to fill it to a maximum point
        for (; i < fill_to; i++)
        {
            write8(0);
        }
    }
}

void Stream::writeStream(Stream* stream, int offset, int total)
{
    if (offset >= 0)
    {
        if (total <= -1)
        {
            throw Exception("Offset is defined but total is not. While using an offset a total must be present.", "void Stream::writeStream(Stream* stream, int offset = -1, int total = -1)");
        }
    }
    else
    {
        offset = 0;
        total = stream->getSize();
    }

    int old_pos = stream->getPosition();
    stream->setPosition(offset);
    int total_wrote = 0;
    while (stream->hasInput()
                && total_wrote < total)
    {
        write8(stream->read8());
        total_wrote++;
    }
    stream->setPosition(old_pos);
}

void Stream::writeStream(std::shared_ptr<Stream> stream, int offset, int total)
{
    writeStream(stream.get(), offset, total);
}

void Stream::joinStream(std::shared_ptr<Stream> stream)
{
    if (isJointWith(stream))
    {
        throw Exception("The stream provided is already joined with this stream", "void Stream::joinStream(std::shared_ptr<Stream> stream)");
    }

    int stream_begins = getSize();
    writeStream(stream);
    this->joined_streams[stream] = stream_begins;
    stream->newJointParent(shared_from_this());
}

void Stream::newJointParent(std::shared_ptr<Stream> stream)
{
    if (!stream->hasJoinedChild(shared_from_this()))
    {
        throw Exception("The stream does not have you as a child already.", "Stream::newJointParent(std::shared_ptr<Stream> stream)");
    }

    if (hasJoinedParent(stream))
    {
        throw Exception("The stream provided is already a joined parent", "Stream::newJointParent(std::shared_ptr<Stream> stream)");
    }

    this->parent_joined_streams.push_back(stream);
}

void Stream::overwrite8(int pos, uint8_t c)
{
    // Work on this what if overwrite mode is already set? You have an issue then
    setOverwriteMode(true);
    int old_pos = getPosition();
    setPosition(pos);
    write8(c);
    setPosition(old_pos);
    setOverwriteMode(false);
}

void Stream::overwrite16(int pos, uint16_t s)
{
    // Work on this what if overwrite mode is already set? You have an issue then
    setOverwriteMode(true);
    int old_pos = getPosition();
    setPosition(pos);
    write16(s);
    setPosition(old_pos);
    setOverwriteMode(false);
}

void Stream::overwrite32(int pos, uint32_t i)
{
    // Work on this what if overwrite mode is already set? You have an issue then
    setOverwriteMode(true);
    int old_pos = getPosition();
    setPosition(pos);
    write32(i);
    setPosition(old_pos);
    setOverwriteMode(false);
}

uint8_t Stream::peek8(int pos)
{
    uint8_t c;
    if (this->vector.size() <= pos)
    {
        throw Exception("uint8_t Stream::peek8(int pos) stream out of bounds");
    }
    c = this->vector.at(pos);

    return c;
}

uint16_t Stream::peek16(int pos)
{
    uint8_t c1 = peek8(pos);
    uint8_t c2 = peek8(pos + 1);

    uint16_t result = (c2 << 8 | c1);
    return result;
}

uint32_t Stream::peek32(int pos)
{
    uint16_t s1 = peek16(pos);
    uint16_t s2 = peek16(pos + 2);

    uint32_t result = (s2 << 16 | s1);
    return result;
}

uint8_t Stream::read8()
{
    if (this->vector.size() <= pos)
    {
        throw Exception("uint8_t Stream::read8(): stream out of bounds");
    }
    uint8_t c = peek8(pos);
    pos++;
    return c;
}

uint16_t Stream::read16()
{
    uint16_t result = peek16(pos);
    pos += 2;
    return result;
}

uint32_t Stream::read32()
{
    uint32_t result = peek32(pos);
    pos += 4;
    return result;
}

std::string Stream::readStr()
{
    std::string str = "";
    uint8_t c;
    while ((c = read8()) != 0)
    {
        str += c;
    }

    return str;
}

// Splits the stream into a vector of chunks
std::vector<std::shared_ptr<Stream>> Stream::chunkSplit(int chunk_size)
{
    // Ok we first need to know how many chunks we have
    int total_chunks;
    int cur_stream_size = getSize();
    total_chunks = cur_stream_size / chunk_size;
    if ((cur_stream_size % chunk_size) != 0)
    {
        // Ok our stream doesn't divide with no remainder so there must be one more chunk
        total_chunks++;
    }

    // Now that we know how many chunks we have lets create a new stream for every chunk
    std::vector<std::shared_ptr < Stream>> chunk_streams;
    for (int i = 0; i < total_chunks; i++)
    {
        int offset = i * chunk_size;
        std::shared_ptr<Stream> stream = std::shared_ptr<Stream>(new Stream());
        stream->writeStream(shared_from_this(), offset, chunk_size);
        chunk_streams.push_back(stream);
    }

    return chunk_streams;
}

size_t Stream::getSize()
{
    return vector.size();
}

std::shared_ptr<Stream> Stream::getJoinedStreamForPosition(int pos)
{
    std::map<std::shared_ptr<Stream>, int>::iterator it;
    for (it = this->joined_streams.begin(); it != this->joined_streams.end(); it++)
    {
        std::shared_ptr<Stream> stream = it->first;
        int stream_start_pos = it->second;
        if (pos >= stream_start_pos
                && pos < stream_start_pos + stream->getSize())
        {
            // Our position is in range, lets return this stream
            return stream;
        }
    }

    return NULL;
}

int Stream::getJoinedStreamPosition(std::shared_ptr<Stream> stream)
{
    if (!hasJoinedChild(stream))
    {
        throw Exception("The stream provided is not a joined child of this stream", "int Stream::getJoinedStreamPosition(std::shared_ptr<Stream> stream)");
    }
    return this->joined_streams.at(stream);
}

bool Stream::isEmpty()
{
    return vector.empty();
}

bool Stream::hasInput()
{
    return pos < vector.size();
}

bool Stream::hasJoinedChild(std::shared_ptr<Stream> stream)
{
    return this->joined_streams.count(stream);
}

bool Stream::hasJoinedParent(std::shared_ptr<Stream> stream)
{
    return std::find(this->parent_joined_streams.begin(), this->parent_joined_streams.end(), stream) != this->parent_joined_streams.end();
}

bool Stream::isJointWith(std::shared_ptr<Stream> stream)
{
    return hasJoinedChild(stream) || hasJoinedParent(stream);
}

bool Stream::isOverwriteModeEnabled()
{
    return this->overwrite_mode;
}

void Stream::empty()
{
    vector.erase(this->vector.begin(), this->vector.end());
}

int Stream::getPosition()
{
    return pos;
}

char* Stream::getBuf()
{
    return (char*) vector.data();
}

char* Stream::toNewBuf()
{
    int size = getSize();
    char* data = new char[size];
    memcpy(data, getBuf(), size);
    return data;
}

void Stream::updateDataForJoinedParents(uint8_t c, int pos_rel_to_us)
{
    for (std::shared_ptr<Stream> stream : this->parent_joined_streams)
    {
        int our_pos_on_stream = stream->getJoinedStreamPosition(shared_from_this());
        int abs_pos = our_pos_on_stream + pos_rel_to_us;
        bool was_overwrite_enabled = stream->isOverwriteModeEnabled();
        stream->setOverwriteMode(isOverwriteModeEnabled());
        stream->write8(c, abs_pos, true);
        stream->setOverwriteMode(was_overwrite_enabled);
    }
}