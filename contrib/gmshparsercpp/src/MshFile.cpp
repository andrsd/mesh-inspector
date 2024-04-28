// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gmshparsercpp/MshFile.h"
#include "fmt/printf.h"
#include <system_error>

namespace gmshparsercpp {

MshFile::MshFile(const std::string & file_name) :
    file_name(file_name),
    file(this->file_name),
    lexer(&this->file),
    version(0.),
    binary(false),
    endianness(0)
{
    if (!this->file.is_open())
        throw Exception("Unable to open file '{}'.", this->file_name);
}

MshFile::~MshFile()
{
    close();
}

double
MshFile::get_version() const
{
    return this->version;
}

bool
MshFile::is_ascii() const
{
    return !this->binary;
}

const std::vector<MshFile::PhysicalName> &
MshFile::get_physical_names() const
{
    return this->physical_names;
}

const std::vector<MshFile::PointEntity> &
MshFile::get_point_entities() const
{
    return this->point_entities;
}

const std::vector<MshFile::MultiDEntity> &
MshFile::get_curve_entities() const
{
    return this->curve_entities;
}

const std::vector<MshFile::MultiDEntity> &
MshFile::get_surface_entities() const
{
    return this->surface_entities;
}

const std::vector<MshFile::MultiDEntity> &
MshFile::get_volume_entities() const
{
    return this->volume_entities;
}

const std::vector<MshFile::Node> &
MshFile::get_nodes() const
{
    return this->nodes;
}

const std::vector<MshFile::ElementBlock> &
MshFile::get_element_blocks() const
{
    return this->element_blocks;
}

void
MshFile::parse()
{
    MshLexer::Token token = this->lexer.peek();
    do {
        if (token.type == MshLexer::Token::Section) {
            token = this->lexer.read();
            process_section(token);
        }
        else
            throw Exception("Expected start of section marker not found.");
        token = this->lexer.peek();
    } while (token.type != MshLexer::Token::EndOfFile);
}

void
MshFile::process_section(const MshLexer::Token & token)
{
    if (token.str == "$MeshFormat")
        process_mesh_format_section();
    else if (token.str == "$PhysicalNames")
        process_physical_names_section();
    else if (token.str == "$Entities")
        process_entities_section();
    else if (token.str == "$PartitionedEntities")
        skip_section();
    else if (token.str == "$Nodes")
        process_nodes_section();
    else if (token.str == "$Elements")
        process_elements_section();
    else if (token.str == "$Periodic")
        skip_section();
    else if (token.str == "$GhostElements")
        skip_section();
    else if (token.str == "$Parametrizations")
        skip_section();
    else if (token.str == "$NodeData")
        skip_section();
    else if (token.str == "$ElementData")
        skip_section();
    else if (token.str == "$ElementNodeData")
        skip_section();
    else if (token.str == "$InterpolationScheme")
        skip_section();
    else
        skip_section();
}

void
MshFile::read_end_section_marker(const std::string & section_name)
{
    auto sct_end = this->lexer.read();
    if (sct_end.type != MshLexer::Token::Section || sct_end.str != section_name)
        throw Exception("{} tag not found.", section_name);
}

void
MshFile::process_mesh_format_section()
{
    this->version = this->lexer.read().as<double>();
    this->binary = this->lexer.read().as<int>() == 1;
    auto data_size = this->lexer.read().as<int>();
    int maj_ver = this->version;
    if (maj_ver == 2 || maj_ver == 4) {
        if ((maj_ver == 2) && (data_size != sizeof(double)))
            throw Exception("Unexpected data size found: {}", data_size);
        else if ((maj_ver == 4) && (data_size != sizeof(size_t)))
            throw Exception("Unexpected data size found: {}", data_size);
        if (this->binary)
            this->endianness = this->lexer.read_blob<int>();
    }
    else
        throw Exception("Unsupported version {}", this->version);

    read_end_section_marker("$EndMeshFormat");

    this->lexer.set_binary(this->binary);
}

void
MshFile::process_physical_names_section()
{
    auto num_entities = this->lexer.read().as<int>();
    for (int i = 0; i < num_entities; i++) {
        auto dimension = this->lexer.read().as<int>();
        auto tag = this->lexer.read().as<int>();
        auto name = this->lexer.read().as<std::string>();
        PhysicalName pn = { dimension, tag, name };
        this->physical_names.push_back(pn);
    }

    read_end_section_marker("$EndPhysicalNames");
}

void
MshFile::process_entities_section()
{
    auto num_points = this->lexer.get<size_t>();
    auto num_curves = this->lexer.get<size_t>();
    auto num_surfaces = this->lexer.get<size_t>();
    auto num_volumes = this->lexer.get<size_t>();

    for (size_t i = 0; i < num_points; i++) {
        PointEntity pe;
        pe.tag = this->lexer.get<int>();
        pe.x = this->lexer.get<double>();
        pe.y = this->lexer.get<double>();
        pe.z = this->lexer.get<double>();
        pe.physical_tags = process_array_of_ints();
        this->point_entities.push_back(pe);
    }

    for (size_t i = 0; i < num_curves; i++) {
        MultiDEntity ent;
        ent.tag = this->lexer.get<int>();
        ent.min_x = this->lexer.get<double>();
        ent.min_y = this->lexer.get<double>();
        ent.min_z = this->lexer.get<double>();
        ent.max_x = this->lexer.get<double>();
        ent.max_y = this->lexer.get<double>();
        ent.max_z = this->lexer.get<double>();
        ent.physical_tags = process_array_of_ints();
        ent.bounding_tags = process_array_of_ints();
        this->curve_entities.push_back(ent);
    }

    for (size_t i = 0; i < num_surfaces; i++) {
        MultiDEntity ent;
        ent.tag = this->lexer.get<int>();
        ent.min_x = this->lexer.get<double>();
        ent.min_y = this->lexer.get<double>();
        ent.min_z = this->lexer.get<double>();
        ent.max_x = this->lexer.get<double>();
        ent.max_y = this->lexer.get<double>();
        ent.max_z = this->lexer.get<double>();
        ent.physical_tags = process_array_of_ints();
        ent.bounding_tags = process_array_of_ints();
        this->surface_entities.push_back(ent);
    }

    for (size_t i = 0; i < num_volumes; i++) {
        MultiDEntity ent;
        ent.tag = this->lexer.get<int>();
        ent.min_x = this->lexer.get<double>();
        ent.min_y = this->lexer.get<double>();
        ent.min_z = this->lexer.get<double>();
        ent.max_x = this->lexer.get<double>();
        ent.max_y = this->lexer.get<double>();
        ent.max_z = this->lexer.get<double>();
        ent.physical_tags = process_array_of_ints();
        ent.bounding_tags = process_array_of_ints();
        this->volume_entities.push_back(ent);
    }
    read_end_section_marker("$EndEntities");
}

void
MshFile::process_nodes_section()
{
    int maj_ver = this->version;
    if (maj_ver == 2)
        process_nodes_section_v2();
    else if (maj_ver == 4)
        process_nodes_section_v4();
    read_end_section_marker("$EndNodes");
}

void
MshFile::process_nodes_section_v2()
{
    auto num_nodes = this->lexer.read().as<size_t>();
    for (auto i = 0; i < num_nodes; i++) {
        Node node;
        node.dimension = 0;
        node.entity_tag = this->lexer.get<int>();

        Point pt;
        pt.x = this->lexer.get<double>();
        pt.y = this->lexer.get<double>();
        pt.z = this->lexer.get<double>();
        node.coordinates.push_back(pt);
        node.tags.push_back(node.entity_tag);
        this->nodes.push_back(node);
    }
}

void
MshFile::process_nodes_section_v4()
{
    auto num_entity_blocks = this->lexer.get<size_t>();
    auto num_nodes = this->lexer.get<size_t>();
    auto min_node_tag = this->lexer.get<size_t>();
    auto max_node_tag = this->lexer.get<size_t>();

    for (int i = 0; i < num_entity_blocks; i++) {
        Node node;
        node.dimension = this->lexer.get<int>();
        node.entity_tag = this->lexer.get<int>();
        node.parametric = this->lexer.get<int>() == 1;
        auto num_nodes_in_block = this->lexer.get<size_t>();
        for (std::size_t i = 0; i < num_nodes_in_block; i++) {
            auto tag = this->lexer.get<size_t>();
            node.tags.push_back(tag);
        }
        for (std::size_t i = 0; i < num_nodes_in_block; i++) {
            Point pt;
            pt.x = this->lexer.get<double>();
            pt.y = this->lexer.get<double>();
            pt.z = this->lexer.get<double>();
            node.coordinates.push_back(pt);
            if (node.parametric) {
                Point par_pr;
                if (node.dimension >= 1)
                    par_pr.x = this->lexer.get<double>();
                if (node.dimension >= 2)
                    par_pr.y = this->lexer.get<double>();
                if (node.dimension == 3)
                    par_pr.z = this->lexer.get<double>();
                node.par_coords.push_back(par_pr);
            }
        }
        this->nodes.push_back(node);
    }
}

void
MshFile::process_elements_section()
{
    int maj_ver = this->version;
    if (maj_ver == 2)
        process_elements_section_v2();
    else if (maj_ver == 4)
        process_elements_section_v4();
    read_end_section_marker("$EndElements");
}

void
MshFile::process_elements_section_v2()
{
    auto num_elements = this->lexer.read().as<size_t>();
    if (this->binary) {
        for (auto i = 0; i < num_elements; i++) {
            auto el_type = static_cast<ElementType>(this->lexer.get<int>());
            auto dim = get_element_dimension(el_type);
            auto n_els = this->lexer.get<int>();
            auto two = this->lexer.get<int>();
            for (auto k = 0; k < n_els; k++) {
                Element el;

                el.tag = this->lexer.get<int>();
                auto phys = this->lexer.get<int>();
                auto ent = this->lexer.get<int>();
                auto n_elem_nodes = get_nodes_per_element(el_type);
                for (auto j = 0; j < n_elem_nodes; j++) {
                    auto nid = this->lexer.get<int>();
                    el.node_tags.push_back(nid);
                }
                auto & blk = get_element_block_by_tag_create(phys);
                blk.tag = phys;
                blk.dimension = dim;
                blk.element_type = el_type;
                blk.elements.push_back(el);
            }
        }
    }
    else {
        for (auto i = 0; i < num_elements; i++) {
            Element el;
            el.tag = this->lexer.get<int>();
            auto el_type = static_cast<ElementType>(this->lexer.get<int>());
            auto two = this->lexer.get<int>();
            auto phys = this->lexer.get<int>();
            auto ent = this->lexer.get<int>();
            auto dim = get_element_dimension(el_type);
            auto n_elem_nodes = get_nodes_per_element(el_type);
            for (auto j = 0; j < n_elem_nodes; j++) {
                auto nid = this->lexer.get<size_t>();
                el.node_tags.push_back(nid);
            }

            auto & blk = get_element_block_by_tag_create(phys);
            blk.tag = phys;
            blk.dimension = dim;
            blk.element_type = el_type;
            blk.elements.push_back(el);
        }
    }
}

void
MshFile::process_elements_section_v4()
{
    auto num_entity_blocks = this->lexer.get<size_t>();
    auto num_elements = this->lexer.get<size_t>();
    auto min_node_tag = this->lexer.get<size_t>();
    auto max_node_tag = this->lexer.get<size_t>();

    for (int i = 0; i < num_entity_blocks; i++) {
        ElementBlock blk;
        blk.dimension = this->lexer.get<int>();
        blk.tag = this->lexer.get<int>();
        blk.element_type = static_cast<ElementType>(this->lexer.get<int>());
        auto num_nodes_per_element = get_nodes_per_element(blk.element_type);
        auto num_elements_in_block = this->lexer.get<size_t>();
        for (size_t j = 0; j < num_elements_in_block; j++) {
            Element el;
            el.tag = this->lexer.get<size_t>();
            for (int k = 0; k < num_nodes_per_element; k++) {
                auto tag = this->lexer.get<size_t>();
                el.node_tags.push_back(tag);
            }
            blk.elements.push_back(el);
        }
        this->element_blocks.push_back(blk);
    }
}

std::vector<int>
MshFile::process_array_of_ints()
{
    std::vector<int> array;
    auto n = this->lexer.get<size_t>();
    for (size_t i = 0; i < n; i++) {
        auto num = this->lexer.get<int>();
        array.push_back(num);
    }
    return array;
}

void
MshFile::skip_section()
{
    MshLexer::Token token;
    do {
        token = this->lexer.read();
    } while (token.type != MshLexer::Token::Section);
}

int
MshFile::get_nodes_per_element(ElementType element_type)
{
    // clang-format off
    switch (element_type) {
        case LINE2: return 2;
        case TRI3: return 3;
        case QUAD4: return 4;
        case TET4: return 4;
        case HEX8: return 8;
        case PRISM6: return 6;
        case PYRAMID5: return 5;
        case LINE3: return 3;
        case TRI6: return 6;
        case QUAD9: return 9;
        case TET10: return 10;
        case HEX27: return 27;
        case PRISM18: return 18;
        case PYRAMID14: return 14;
        case POINT: return 1;
        case QUAD8: return 8;
        case HEX20: return 20;
        case PRISM15: return 15;
        case PYRAMID13: return 13;
        case ITRI9: return 9;
        case TRI10: return 10;
        case ITRI12: return 12;
        case TRI15: return 15;
        case ITRI15: return 15;
        case TRI21: return 21;
        case LINE4: return 4;
        case LINE5: return 5;
        case LINE6: return 6;
        case TET20: return 20;
        case TET35: return 35;
        case TET56: return 56;
        case HEX64: return 64;
        case HEX125: return 125;
        default:
            throw Exception("Unknown element type '{}'", element_type);
    }
    // clang-format on
}

int
MshFile::get_element_dimension(ElementType element_type)
{
    switch (element_type) {
    case POINT:
        return 0;

    case LINE2:
    case LINE3:
    case LINE4:
    case LINE5:
    case LINE6:
        return 1;

    case TRI3:
    case QUAD4:
    case TRI6:
    case QUAD9:
    case QUAD8:
    case ITRI9:
    case TRI10:
    case ITRI12:
    case TRI15:
    case ITRI15:
    case TRI21:
        return 2;

    case TET4:
    case HEX8:
    case PRISM6:
    case PYRAMID5:
    case TET10:
    case HEX27:
    case PRISM18:
    case PYRAMID14:
    case HEX20:
    case PRISM15:
    case PYRAMID13:
    case TET20:
    case TET35:
    case TET56:
    case HEX64:
    case HEX125:
        return 3;

    default:
        throw Exception("Unknown element type '{}'", element_type);
    }
}

MshFile::ElementBlock &
MshFile::get_element_block_by_tag_create(int tag)
{
    for (auto & eblk : this->element_blocks) {
        if (eblk.tag == tag)
            return eblk;
    }
    ElementBlock blk;
    this->element_blocks.push_back(blk);
    return this->element_blocks.back();
}

void
MshFile::close()
{
    if (this->file.is_open())
        this->file.close();
}

} // namespace gmshparsercpp
