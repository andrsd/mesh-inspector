// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <fstream>
#include <vector>
#include "gmshparsercpp/Enums.h"
#include "gmshparsercpp/Exception.h"
#include "gmshparsercpp/MshLexer.h"

namespace gmshparsercpp {

class MshLexer;

/// Class for parsing MSH files
///
class MshFile {
public:
    struct PhysicalName {
        /// Physical entity dimension
        int dimension;
        /// Entity tag
        int tag;
        /// Entity name
        std::string name;

        PhysicalName() : dimension(-1), tag(-1) {}
        PhysicalName(int dim, int tag, const std::string & name) :
            dimension(dim),
            tag(tag),
            name(name)
        {
        }
    };

    struct PointEntity {
        /// Entity tag
        int tag;
        /// physical location
        double x, y, z;
        /// physical tags
        std::vector<int> physical_tags;

        PointEntity() : tag(-1), x(0.), y(0.), z(0.) {}
        PointEntity(int tag, double x, double y, double z, const std::vector<int> & phys_tags) :
            tag(tag),
            x(x),
            y(y),
            z(z),
            physical_tags(phys_tags)
        {
        }
    };

    struct MultiDEntity {
        /// Entity tag
        int tag;
        double min_x, min_y, min_z;
        double max_x, max_y, max_z;
        /// Physical tags
        std::vector<int> physical_tags;
        /// bounding tags
        std::vector<int> bounding_tags;

        MultiDEntity() : tag(-1), min_x(0.), min_y(0.), min_z(0.), max_x(0.), max_y(0.), max_z(0.)
        {
        }
        MultiDEntity(int tag,
                     double min_x,
                     double min_y,
                     double min_z,
                     double max_x,
                     double max_y,
                     double max_z,
                     const std::vector<int> & phys_tags,
                     const std::vector<int> & bnd_tags) :
            tag(tag),
            min_x(min_x),
            min_y(min_y),
            min_z(min_z),
            max_x(max_x),
            max_y(max_y),
            max_z(max_z),
            physical_tags(phys_tags),
            bounding_tags(bnd_tags)
        {
        }
    };

    struct Point {
        double x, y, z;

        Point() : x(0.), y(0.), z(0.) {}
        Point(double x, double y, double z) : x(x), y(y), z(z) {}
    };

    struct Node {
        /// Physical entity dimension
        int dimension;
        /// Entity tag
        int entity_tag;
        /// Is parametric
        bool parametric;
        /// Node tags
        std::vector<int> tags;
        /// Coordinates
        std::vector<Point> coordinates;
        /// Parametric coordinates
        std::vector<Point> par_coords;

        Node() : dimension(-1), entity_tag(-1), parametric(false) {}
    };

    struct Element {
        /// Element tag
        int tag;
        /// Node tags
        std::vector<int> node_tags;

        Element() : tag(-1) {}
    };

    struct ElementBlock {
        /// Block dimension
        int dimension;
        /// Block tag
        int tag;
        /// Element type
        ElementType element_type;
        /// Elements
        std::vector<Element> elements;

        ElementBlock() : dimension(-1), tag(-1), element_type(NONE) {}
    };

    /// Construct MSH file
    ///
    /// @param file_name The MSH file name
    explicit MshFile(const std::string & file_name);

    virtual ~MshFile();

    /// Get file format version
    ///
    /// @return File format version
    double get_version() const;

    /// Query if the file is ASCII
    ///
    /// @return True if the file format is in ASCII, False if it is binary
    bool is_ascii() const;

    /// Get physical names
    ///
    /// @return List of physical names
    const std::vector<PhysicalName> & get_physical_names() const;

    /// Get point entities
    ///
    /// @return List of point entities
    const std::vector<PointEntity> & get_point_entities() const;

    /// Get curve entities
    ///
    /// @return List of curve entities
    const std::vector<MultiDEntity> & get_curve_entities() const;

    /// Get surface entities
    ///
    /// @return List of surface entities
    const std::vector<MultiDEntity> & get_surface_entities() const;

    /// Get volume entities
    ///
    /// @return List of volume entities
    const std::vector<MultiDEntity> & get_volume_entities() const;

    /// Get nodes
    ///
    /// @return List of nodes
    const std::vector<Node> & get_nodes() const;

    /// Get element blocks
    ///
    /// @return List of element blocks
    const std::vector<ElementBlock> & get_element_blocks() const;

    /// Parse the file
    void parse();

    /// Close the file
    void close();

    /// Get the number of nodes per element type
    ///
    /// @param element_type GMSH element type
    /// @return Number of nodes per element
    static int get_nodes_per_element(ElementType element_type);

    /// Get element dimension
    ///
    /// @param element_type GMSH element type
    /// @return Dimension
    static int get_element_dimension(ElementType element_type);

protected:
    void process_section(const MshLexer::Token & token);
    void process_mesh_format_section();
    void process_physical_names_section();
    void process_entities_section();
    void process_nodes_section();
    void process_nodes_section_v2();
    void process_nodes_section_v4();
    void process_elements_section();
    void process_elements_section_v2();
    void process_elements_section_v4();
    std::vector<int> process_array_of_ints();
    void skip_section();
    void read_end_section_marker(const std::string & section_name);
    ElementBlock & get_element_block_by_tag_create(int tag);

    /// File name
    std::string file_name;
    /// Input stream
    std::ifstream file;
    /// Lexer for lexicographic analysis
    MshLexer lexer;
    /// File format version
    double version;
    /// ASCII/binary flag
    bool binary;
    /// Endianness for binary files
    int endianness;
    /// Physical names
    std::vector<PhysicalName> physical_names;
    /// Point entities
    std::vector<PointEntity> point_entities;
    /// Curve entities
    std::vector<MultiDEntity> curve_entities;
    /// Surface entities
    std::vector<MultiDEntity> surface_entities;
    /// Volume entities
    std::vector<MultiDEntity> volume_entities;
    /// Nodes
    std::vector<Node> nodes;
    /// Element blocks
    std::vector<ElementBlock> element_blocks;
};

} // namespace gmshparsercpp
