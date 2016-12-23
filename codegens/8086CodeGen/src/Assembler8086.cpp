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
 * File:   Assembler8086.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 16:44
 * 
 * Description: The parser and assembler for the 8086 code generator,
 * note that no checking of input is done here as the assembly is generated from the code generator
 * so it is expected to be valid. It can also be generated by the programmer using inline assembly so checking of input
 * may be done in the future, but for now its expected that they get it right.
 * 
 * The assembler also only supports one label per operand, this assembler is only intended for the code generator
 * and those using inline assembly. It is not designed for complicated assembling.
 * 
 * Please seek here: https://courses.engr.illinois.edu/ece390/resources/opcodes.html#Technical
 * for opcode information.
 */

#include "Assembler8086.h"
#include "common.h"
#include "def.h"
#include "LabelBranch.h"
#include "InstructionBranch.h"
#include "SegmentBranch.h"
#include "OperandBranch.h"
#include "EBranch.h"


const char ins_sizes[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 3, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 3, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0
};


const bool has_oo_mmm[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 3, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0
};

Assembler8086::Assembler8086(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format) : Assembler(compiler, object_format)
{
    Assembler::addKeyword("segment");
    Assembler::addKeyword("extern");
    Assembler::addKeyword("global");
    Assembler::addKeyword("byte");
    Assembler::addKeyword("word");

    Assembler::addRegister("ax");
    Assembler::addRegister("ah");
    Assembler::addRegister("al");
    Assembler::addRegister("cx");
    Assembler::addRegister("ch");
    Assembler::addRegister("cl");
    Assembler::addRegister("dx");
    Assembler::addRegister("dh");
    Assembler::addRegister("dl");
    Assembler::addRegister("bx");
    Assembler::addRegister("bh");
    Assembler::addRegister("bl");

    Assembler::addRegister("di");
    Assembler::addRegister("si");
    Assembler::addRegister("bp");
    Assembler::addRegister("sp");

    // Not all the instructions that are implemented, but enough for now
    Assembler::addInstruction("mov");
    Assembler::addInstruction("push");
    Assembler::addInstruction("pop");
    Assembler::addInstruction("add");
    Assembler::addInstruction("sub");
    Assembler::addInstruction("mul");
    Assembler::addInstruction("div");
    Assembler::addInstruction("xor");
    Assembler::addInstruction("and");
    Assembler::addInstruction("or");
    Assembler::addInstruction("int");
    Assembler::addInstruction("lea");
    Assembler::addInstruction("call");
    Assembler::addInstruction("ret");

    this->left = NULL;
    this->right = NULL;
    this->segment = NULL;
    this->sstream = NULL;
    this->cur_offset = 0;
}

Assembler8086::~Assembler8086()
{
}

std::shared_ptr<Branch> Assembler8086::parse()
{
#ifdef DEBUG_MODE
    debug_output_tokens(Assembler::getTokens());
#endif

    root = std::shared_ptr<Branch>(new Branch("root", ""));
    while (hasTokens())
    {
        parse_part();
    }

    // Lets get all the branches
    while (hasBranches())
    {
        pop_front_branch();
        root->addChild(getPoppedBranch());
    }

#ifdef DEBUG_MODE
    debug_output_branch(root);
#endif

    return root;

}

void Assembler8086::exp_handler()
{
    shift();
}

void Assembler8086::left_exp_handler()
{
    exp_handler();
}

void Assembler8086::right_exp_handler()
{
    exp_handler();
}

std::shared_ptr<InstructionBranch> Assembler8086::new_ins_branch()
{
    return std::shared_ptr<InstructionBranch>(new InstructionBranch(getCompiler()));
}

void Assembler8086::parse_part()
{
    if (is_next_segment())
    {
        parse_segment();
    }
    else if (is_next_label())
    {
        parse_label();
    }
    else if (is_next_instruction())
    {
        parse_ins();
    }
    else
    {
        peek();
        throw AssemblerException("void Assembler8086::parse_part():  unexpected token \"" + getPeakTokenValue() + "\" this instruction or syntax may not be implemented.");
    }

}

std::shared_ptr<Branch> Assembler8086::get_identifier_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found)
{
    std::shared_ptr<Branch> result_branch = NULL;
    if (branch->getType() != "E")
    {
        if (branch->getType() == "identifier")
        {
            result_branch = branch;
        }
    }
    else
    {
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(branch);
        e_branch->iterate_expressions([&](std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) -> void
        {
            if (left_branch->getType() == "identifier")
            {
                result_branch = left_branch;
            }
            else if (right_branch->getType() == "identifier")
            {
                result_branch = right_branch;
            }
        });
    }

    if (remove_once_found)
    {
        if (result_branch != NULL)
        {
            result_branch->removeSelf();
        }
    }

    return result_branch;
}

std::shared_ptr<Branch> Assembler8086::get_register_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found)
{
    std::shared_ptr<Branch> result_branch = NULL;
    if (branch->getType() != "E")
    {
        if (branch->getType() == "register")
        {
            result_branch = branch;
        }
    }
    else
    {
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(branch);
        e_branch->iterate_expressions([&](std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) -> void
        {
            if (left_branch->getType() == "register")
            {
                result_branch = left_branch;
            }
            else if (right_branch->getType() == "register")
            {
                result_branch = right_branch;
            }
        });
    }

    if (remove_once_found)
    {
        if (result_branch != NULL)
        {
            result_branch->removeSelf();
        }
    }

    return result_branch;
}

std::shared_ptr<Branch> Assembler8086::get_number_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found)
{
    std::shared_ptr<Branch> result_branch = NULL;
    if (branch->getType() != "E")
    {
        if (branch->getType() == "number")
        {
            result_branch = branch;
        }
    }
    else
    {
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(branch);
        e_branch->iterate_expressions([&](std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) -> void
        {
            if (left_branch->getType() == "number")
            {
                result_branch = left_branch;
            }
            else if (right_branch->getType() == "number")
            {
                result_branch = right_branch;
            }
        });
    }

    if (remove_once_found)
    {
        if (result_branch != NULL)
        {
            result_branch->removeSelf();
        }
    }

    return result_branch;
}

void Assembler8086::handle_operand_exp(std::shared_ptr<OperandBranch> operand_branch)
{
    // Parse the expression
    parse_expression();
    pop_branch();
    // Now lets cut down the expression a bit and sum up the numbers
    std::shared_ptr<Branch> r_exp = sum_expression(getPoppedBranch());

    operand_branch->setNumberBranch(get_number_branch_from_exp(r_exp));
    operand_branch->setRegisterBranch(get_register_branch_from_exp(r_exp));
    operand_branch->setIdentifierBranch(get_identifier_branch_from_exp(r_exp));
}

void Assembler8086::parse_operand()
{
    std::shared_ptr<OperandBranch> operand_branch = std::shared_ptr<OperandBranch>(new OperandBranch(getCompiler()));
    peek();
    if (is_peek_keyword("byte"))
    {
        // Ok we have byte access here
        shift_pop();
        operand_branch->setDataSize(OPERAND_DATA_SIZE_BYTE);
    }
    else if (is_peek_keyword("word"))
    {
        // Ok a word access
        shift_pop();
        operand_branch->setDataSize(OPERAND_DATA_SIZE_WORD);
    }

    peek();
    if (is_peek_symbol("["))
    {
        // Ok this will be memory access
        shift_pop();
        operand_branch->setMemoryAccess(true);
        handle_operand_exp(operand_branch);
        // Ok ending memory access shift off the "]" token.
        shift_pop();
    }
    else
    {
        handle_operand_exp(operand_branch);
    }



    push_branch(operand_branch);
}

void Assembler8086::parse_segment()
{
    // Shift and pop the segment name we don't need it anymore
    shift_pop();

    // Next up is the segment name, we need this
    shift_pop();
    std::shared_ptr<Branch> segment_name_branch = Assembler::getPoppedBranch();

    std::shared_ptr<SegmentBranch> segment_root = std::shared_ptr<SegmentBranch>(new SegmentBranch(getCompiler()));
    segment_root->setSegmentNameBranch(segment_name_branch);

    // Create the contents branch and add it to the segment branch
    std::shared_ptr<Branch> contents_branch = std::shared_ptr<Branch>(new Branch("CONTENTS", ""));
    segment_root->setContentsBranch(contents_branch);

    while (hasTokens())
    {
        if (is_next_segment())
        {
            /* We should stop now as we are at another segment, segments are independent from other segments*/
            break;
        }

        parse_part();
        pop_branch();
        // Add the branch to the segment contents branch
        contents_branch->addChild(getPoppedBranch());
    }

    push_branch(segment_root);

}

void Assembler8086::parse_label()
{
    // Shift and pop the label name
    shift_pop();
    std::shared_ptr<Branch> label_name_branch = Assembler::getPoppedBranch();
    // Shift and pop the symbol ":" we do not need it anymore
    shift_pop();

    // Create the label branch
    std::shared_ptr<LabelBranch> label_branch = std::shared_ptr<LabelBranch>(new LabelBranch(getCompiler()));
    label_branch->setLabelNameBranch(label_name_branch);

    // Create the label contents branch and add it to the label branch
    std::shared_ptr<Branch> label_contents_branch = std::shared_ptr<Branch>(new Branch("CONTENTS", ""));
    label_branch->setContentsBranch(label_contents_branch);

    while (hasTokens())
    {
        if (is_next_label() ||
                is_next_segment())
        {
            /* We should stop now as we are at another label or segment
             all labels and segments should be independent from other labels and segments.*/
            break;
        }

        parse_part();
        pop_branch();
        // Add the branch to the label contents branch
        label_contents_branch->addChild(getPoppedBranch());
    }

    push_branch(label_branch);
}

void Assembler8086::parse_ins()
{
    std::shared_ptr<Branch> name_branch = NULL;
    std::shared_ptr<OperandBranch> dest_op = NULL;
    std::shared_ptr<OperandBranch> source_op = NULL;

    // Shift and pop the instruction
    shift_pop();
    name_branch = getPoppedBranch();

    // Do we have an expression
    peek();
    if (is_next_valid_operand())
    {
        // Next will be the left operand
        parse_operand();
        // Pop it off
        pop_branch();
        dest_op = std::dynamic_pointer_cast<OperandBranch>(getPoppedBranch());

        // Do we have a second operand?
        peek();
        if (is_peek_symbol(","))
        {
            // Now we need to shift and pop off the comma ","
            shift_pop();

            // Finally a final expression which will be the second operand
            parse_operand();

            // Pop it off
            pop_branch();
            source_op = std::dynamic_pointer_cast<OperandBranch>(getPoppedBranch());
        }
    }

    // Put it all together
    std::shared_ptr<InstructionBranch> ins_branch = std::shared_ptr<InstructionBranch>(new InstructionBranch(getCompiler()));
    ins_branch->setInstructionNameBranch(name_branch);
    ins_branch->setLeftBranch(dest_op);
    ins_branch->setRightBranch(source_op);

    debug_output_branch(ins_branch);

    // Push the finished branch to the stack
    push_branch(ins_branch);
}

bool Assembler8086::is_next_valid_operand()
{
    return (is_peek_type("identifier")
            || is_peek_type("number")
            || is_peek_type("register")
            || is_peek_symbol("[")
            || is_peek_type("keyword"));
}

bool Assembler8086::is_next_segment()
{
    peek();
    return (is_peek_type("keyword")
            && is_peek_value("segment"));
}

bool Assembler8086::is_next_label()
{
    peek();
    if (is_peek_type("identifier"))
    {
        peek(1);
        if (is_peek_symbol(":"))
        {
            // This is a label
            return true;
        }
    }

    return false;
}

bool Assembler8086::is_next_instruction()
{
    peek();
    return is_peek_type("instruction");
}

void Assembler8086::generate()
{
    assembler_pass_1();
    assembler_pass_2();

    std::cout << this->cur_offset << std::endl;
}

void Assembler8086::assembler_pass_1()
{
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        if (branch->getType() == "SEGMENT")
        {
            // Reset the current offset ready for the next segment
            this->cur_offset = 0;
            pass_1_segment(std::dynamic_pointer_cast<SegmentBranch>(branch));
        }
        else
        {
            throw new AssemblerException("void Assembler8086::generate(): branch requires a segment.");
        }
    }
}

void Assembler8086::pass_1_segment(std::shared_ptr<SegmentBranch> segment_branch)
{
    // Register the segment
    register_segment(segment_branch);

    // Now we need to pass through the children
    for (std::shared_ptr<Branch> child : segment_branch->getContentsBranch()->getChildren())
    {
        pass_1_part(child);
    }
}

void Assembler8086::pass_1_part(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "LABEL")
    {
        // This is a label, therefore we need to give it, its position.
        std::shared_ptr<LabelBranch> label_branch = std::dynamic_pointer_cast<LabelBranch>(branch);
        label_branch->setOffset(this->cur_offset);
    }
    else if (branch->getType() == "INSTRUCTION")
    {
        this->cur_offset += get_instruction_size(std::dynamic_pointer_cast<InstructionBranch>(branch));
    }
    else if (branch->getType() == "keyword")
    {

    }
    else
    {
        throw AssemblerException("void Assembler8086::pass_1_part(std::shared_ptr<Branch> branch): "
                                 "expecting label, instruction or keyword but a \"" + branch->getType() + "\" was provided");
    }
}

void Assembler8086::get_modrm_from_instruction(std::shared_ptr<InstructionBranch> ins_branch, char* oo, char* rrr, char* mmm)
{
    left = ins_branch->getLeftBranch();
    right = ins_branch->getRightBranch();

    if (left->getType() == "register")
    {
        if (right->getType() == "register")
        {
            *oo = USE_REG_NO_ADDRESSING_MODE;
            *rrr = get_reg(right->getRegisterBranch()->getValue());
            *mmm = get_reg(left->getRegisterBranch()->getValue());
        }
    }
    else if (left->isAccessingMemory())
    {
        if (right->isOnlyImmediate())
        {
            *oo = DISPLACEMENT_IF_MMM_110;
            *mmm = 0b110;
        }
    }

}

int Assembler8086::get_offset_from_oomod(char oo, char mmm)
{
    int offset = 0;
    if (oo == 0b00)
    {
        if (mmm == 0b110)
        {
            offset += 2;
        }
    }
    else if (oo == 0b01)
    {
        offset += 1;
    }
    else if (oo == 0b10)
    {
        offset += 2;
    }

    return offset;
}

int Assembler8086::get_instruction_size(std::shared_ptr<InstructionBranch> ins_branch)
{
    int opcode = get_instruction_type(ins_branch);
    int size = ins_sizes[opcode];
    if (has_oo_mmm[opcode])
    {
        get_modrm_from_instruction(ins_branch, &oo, &rrr, &mmm);
        size += get_offset_from_oomod(oo, mmm);
    }

    return size;
}

void Assembler8086::register_segment(std::shared_ptr<SegmentBranch> segment_branch)
{
    std::shared_ptr<VirtualObjectFormat> obj_format = Assembler::getObjectFormat();
    std::shared_ptr<VirtualSegment> segment = obj_format->createSegment(segment_branch->getSegmentNameBranch()->getValue());
    this->segments.push_back(segment);
}

void Assembler8086::switch_to_segment(std::string segment_name)
{
    for (std::shared_ptr<VirtualSegment> segment : this->segments)
    {
        if (segment->getName() == segment_name)
        {
            this->segment = segment;
            this->sstream = this->segment->getStream();
            return;
        }
    }

    throw AssemblerException("void Assembler8086::switch_to_segment(std::string segment_name): \"" + segment_name + "\" does not exist");
}

void Assembler8086::assembler_pass_2()
{
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        if (branch->getType() == "SEGMENT")
        {
            generate_segment(std::dynamic_pointer_cast<SegmentBranch>(branch));
        }
        else
        {
            throw new AssemblerException("void Assembler8086::generate(): branch requires a segment.");
        }
    }
}

void Assembler8086::generate_part(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "INSTRUCTION")
    {
        generate_instruction(std::dynamic_pointer_cast<InstructionBranch>(branch));
    }

}

void Assembler8086::generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch)
{

    INSTRUCTION_TYPE ins_type = get_instruction_type(instruction_branch);

    switch (ins_type)
    {
    case MOV_REG_TO_REG_W0:
    case MOV_REG_TO_REG_W1:
        generate_mov_reg_to_reg(ins_type, instruction_branch);
        break;
    case MOV_IMM_TO_REG_W0:
    case MOV_IMM_TO_REG_W1:
        generate_mov_imm_to_reg(ins_type, instruction_branch);
        break;
    case MOV_IMM_TO_MEM_W0:
    case MOV_IMM_TO_MEM_W1:
        generate_mov_imm_to_mem(ins_type, instruction_branch);
        break;
    case MOV_ACC_TO_MEMOFFS_W0:
    case MOV_ACC_TO_MEMOFFS_W1:
        generate_mov_acc_to_mem_offs(ins_type, instruction_branch);
        break;
    case MOV_MEMOFFS_TO_ACC_W0:
    case MOV_MEMOFFS_TO_ACC_W1:
        generate_mov_mem_offs_to_acc(ins_type, instruction_branch);
        break;
    }

}

void Assembler8086::generate_mov_reg_to_reg(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch)
{
    left = instruction_branch->getLeftBranch();
    right = instruction_branch->getRightBranch();

    // Get the modrm for this instruction
    get_modrm_from_instruction(instruction_branch, &oo, &rrr, &mmm);

    sstream->write8(ins_type);
    sstream->write8(bind_modrm(oo, rrr, mmm));

}

void Assembler8086::generate_mov_imm_to_reg(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch)
{
    left = instruction_branch->getLeftBranch();
    right = instruction_branch->getRightBranch();

    rrr = get_reg(left->getRegisterBranch()->getValue());
    op = ins_type << 3 | rrr;
    sstream->write8(op);

    if (ins_type == MOV_IMM_TO_REG_W0)
    {
        write_abs_static8(right);
    }
    else
    {
        write_abs_static16(right);
    }
}

void Assembler8086::generate_mov_imm_to_mem(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch)
{
    left = instruction_branch->getLeftBranch();
    right = instruction_branch->getRightBranch();

    // Get the modrm for this instruction
    get_modrm_from_instruction(instruction_branch, &oo, &rrr, &mmm);

    // Write the opcode
    sstream->write8(ins_type);

    // Write the MOD/RM
    sstream->write8(bind_modrm(oo, 0, mmm));

    // I know IF statement is not needed here but this is for future proof, incase I implement more of this instruction
    if (oo == DISPLACEMENT_IF_MMM_110
            && mmm == 0b110)
    {
        // Ok displacement is required
        // Lets write the offset
        write_abs_static16(left);
    }

    // Write the value
    if (ins_type == MOV_IMM_TO_MEM_W0)
    {
        write_abs_static8(right);
    }
    else
    {
        write_abs_static16(right);
    }
}

void Assembler8086::generate_mov_acc_to_mem_offs(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch)
{
    left = instruction_branch->getLeftBranch();
    right = instruction_branch->getRightBranch();

    // Write the opcode
    sstream->write8(ins_type);

    // Next comes the offset
    write_abs_static16(left);
}

void Assembler8086::generate_mov_mem_offs_to_acc(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch)
{
    left = instruction_branch->getLeftBranch();
    right = instruction_branch->getRightBranch();

    // Write the opcode
    sstream->write8(ins_type);

    // Next comes the offset
    write_abs_static16(left);
}

char Assembler8086::bind_modrm(char oo, char rrr, char mmm)
{
    return (oo << 6 | rrr << 3 | mmm);
}

int Assembler8086::get_static_from_branch(std::shared_ptr<OperandBranch> branch)
{
    int value = 0;
    if (branch->hasNumberBranch())
    {
        value += std::stoi(branch->getNumberBranch()->getValue());
    }
    
    if (branch->hasIdentifierBranch())
    {
        value += get_label_offset(branch->getIdentifierBranch()->getValue());
    }

    return value;
}

void Assembler8086::write_abs_static8(std::shared_ptr<OperandBranch> branch)
{
    sstream->write8(get_static_from_branch(branch));
}

void Assembler8086::write_abs_static16(std::shared_ptr<OperandBranch> branch)
{
    sstream->write16(get_static_from_branch(branch));
}

std::shared_ptr<LabelBranch> Assembler8086::get_label_branch(std::string label_name)
{
    for (std::shared_ptr<Branch> child : segment_branch->getContentsBranch()->getChildren())
    {
        if (child->getType() == "LABEL")
        {
            std::shared_ptr<LabelBranch> lbl_branch = std::dynamic_pointer_cast<LabelBranch>(child);
            if (lbl_branch->getLabelNameBranch()->getValue() == label_name)
                return lbl_branch;
        }
    }

    return NULL;
}

int Assembler8086::get_label_offset(std::string label_name)
{
    return get_label_branch(label_name)->getOffset();
}

void Assembler8086::generate_segment(std::shared_ptr<SegmentBranch> branch)
{
    this->segment_branch = branch;
    switch_to_segment(branch->getSegmentNameBranch()->getValue());
    for (std::shared_ptr<Branch> child : branch->getContentsBranch()->getChildren())
    {
        generate_part(child);
    }
}

INSTRUCTION_TYPE Assembler8086::get_instruction_type(std::shared_ptr<InstructionBranch> instruction_branch)
{

    std::shared_ptr<Branch> instruction_name_branch = instruction_branch->getInstructionNameBranch();
    std::string instruction_name = instruction_name_branch->getValue();
    if (instruction_name == "mov")
    {
        // This is a move instruction
        return get_mov_ins_type(instruction_branch);
    }

    throw AssemblerException("INSTRUCTION_TYPE Assembler8086::get_instruction_type(std::shared_ptr<InstructionBranch> instruction_branch): Invalid assembler instruction");
}

INSTRUCTION_TYPE Assembler8086::get_mov_ins_type(std::shared_ptr<InstructionBranch> instruction_branch)
{
    this->left = instruction_branch->getLeftBranch();
    this->right = instruction_branch->getRightBranch();

    if (left->isOnlyRegister())
    {
        left_reg = left->getRegisterBranch();
        if (right->isOnlyRegister())
        {
            // Register to register assignment "mov reg, reg", 8 bit or 16 bit assignment?
            if (is_reg_16_bit(left_reg->getValue()))
            {
                // 16 bit assignment here
                return MOV_REG_TO_REG_W1;
            }
            else
            {
                // Must be an 8 bit assignment.
                return MOV_REG_TO_REG_W0;
            }
        }
        else if (right->isOnlyImmediate())
        {
            // Register to register assignment, 8 bit or 16 bit assignment?
            if (is_reg_16_bit(left_reg->getValue()))
            {
                // 16 bit assignment here
                return MOV_IMM_TO_REG_W1;
            }
            else
            {
                // Must be an 8 bit assignment.
                return MOV_IMM_TO_REG_W0;
            }
        }
        else if (right->isAccessingMemory())
        {
            // Is this the accumulator we are going to store this into?
            if (is_accumulator_and_not_ah(left_reg->getValue()))
            {
                if (is_reg_16_bit(left_reg->getValue()))
                {
                    return MOV_MEMOFFS_TO_ACC_W1;
                }
                else
                {
                    return MOV_MEMOFFS_TO_ACC_W0;
                }
            }
        }
    }
    else if (left->isAccessingMemory())
    {
        // Memory assignment.
        if (right->isOnlyRegister())
        {
            right_reg = right->getRegisterBranch();
            // mov mem, reg
            // Is this the accumulator we are referring to?
            if (is_accumulator_and_not_ah(right_reg->getValue()))
            {
                if (is_reg_16_bit(right_reg->getValue()))
                {
                    return MOV_ACC_TO_MEMOFFS_W1;
                }
                else
                {
                    return MOV_ACC_TO_MEMOFFS_W0;
                }
            }
        }
        else if (right->isOnlyImmediate())
        {
            // mov mem, imm
            if (left->getDataSize() == OPERAND_DATA_SIZE_WORD)
            {
                return MOV_IMM_TO_MEM_W1;
            }
            else
            {
                return MOV_IMM_TO_MEM_W0;
            }
        }
    }
}

bool Assembler8086::is_accumulator_and_not_ah(std::string _register)
{
    return _register == "al"
            || _register == "ax";
}

bool Assembler8086::is_reg(std::string _register)
{
    return (_register == "al"
            || _register == "ah"
            || _register == "ax"
            || _register == "bh"
            || _register == "bl"
            || _register == "bx"
            || _register == "ch"
            || _register == "cl"
            || _register == "cx"
            || _register == "dh"
            || _register == "dl"
            || _register == "dx");
}

char Assembler8086::get_reg(std::string _register)
{
    if (_register == "al" || _register == "ax")
    {
        return 0;
    }
    else if (_register == "cl" || _register == "cx")
    {
        return 1;
    }
    else if (_register == "dl" || _register == "dx")
    {
        return 2;
    }
    else if (_register == "bl" || _register == "bx")
    {
        return 3;
    }
    else if (_register == "ah" || _register == "sp")
    {
        return 4;
    }
    else if (_register == "ch" || _register == "bp")
    {
        return 5;
    }
    else if (_register == "dh" || _register == "si")
    {
        return 6;
    }
    else if (_register == "bh" || _register == "di")
    {
        return 7;
    }
}

bool Assembler8086::is_reg_16_bit(std::string _register)
{
    return (_register == "ax" ||
            _register == "bx" ||
            _register == "cx" ||
            _register == "dx" ||
            _register == "sp" ||
            _register == "bp" ||
            _register == "ss" ||
            _register == "ds" ||
            _register == "es" ||
            _register == "es");
}