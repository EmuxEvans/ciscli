/****************************************************************************
 * CLI parser common definitions
 ****************************************************************************
 * libcli makes it easy to generate Cisco router style CLIs
 * Copyright (C) 2013 Nirenjan Krishnan <nirenjan@nirenjan.org>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 ***************************************************************************/
/** @file */
#ifndef HDR_PARSER_COMMON_H
#define HDR_PARSER_COMMON_H

#include <stdint.h>

/** @brief Parser node privilege information
 *
 * The privilege level of the node is stored in the lower 4 bits of the
 * flags field of the header. The macros here allow the parser to determine
 * whether to display the node or not, based on the privilege setting.
 */
#define PARSER_NODE_FLAG_PRIVILEGE_MASK     0x0000000F
#define PARSER_NODE_FLAG_PRIVILEGE_SHIFT    0

#define PARSER_NODE_PRIVILEGE(node) (((parser_node_header_t *)(node)->flags \
    & PARSER_NODE_FLAG_PRIVILEGE_MASK) >> PARSER_NODE_FLAG_PRIVILEGE_SHIFT)

#define PARSER_NODE_FLAG_HIDDEN             0x00000010
#define PARSER_NODE_FLAG_INVISIBLE          0x00000020
#define PARSER_NODE_FLAG_SET_VALUE          0x00000040
#define PARSER_NODE_FLAG_SET_BIT            0x00000080
#define PARSER_NODE_FLAG_NEGATABLE          0x00000100
#define PARSER_NODE_FLAG_SET_NEGATE         0x00000200
#define PARSER_NODE_FLAG_KEYWORD_MIN_MATCH  0x00000400


enum parser_node_type_e {
    PARSER_NODE_TYPE_ROOT = 0,
    PARSER_NODE_TYPE_KEYWORD,
    PARSER_NODE_TYPE_INTEGER,
    PARSER_NODE_TYPE_STRING,
    PARSER_NODE_TYPE_CONSTANT,
    PARSER_NODE_TYPE_CONDITIONAL,
    PARSER_NODE_TYPE_EOL,
    PARSER_NODE_TYPE_MAX
}

#define HELP_TEXT_LENGTH        128

/** @brief Common header for parser nodes
 *
 * This is a common header field for all parser nodes.
 */
typedef struct parser_node_header_s {
    /** @brief Pointer to child node if parser accepts the current node
     * @private
     */
    struct parser_node_header_s *child;

    /** @brief Pointer to sibling node if parser rejects the current node
     * @private
     */
    struct parser_node_header_s *sibling;

    /** @brief Flags controlling the parser behaviour
     *
     * This field stores bits to alter the parser behaviour based on the
     * node. Flags such as hidden command, negatable command, etc. are stored
     * in this field, as is the minimum privilege level, among others.
     */
    uint32_t flags;

    /** @brief Type of the node
     *
     * This field is a discriminator to distinguish the type of the node
     * managed by this header.
     */
    uint32_t type;

    /** @brief Help text
     *
     * This field contains the help text to display when the complete or
     * help functions are called.
     */
    char help_text[HELP_TEXT_LENGTH];
} parser_node_header_t;

#define KEYWORD_LENGTH_MAX      32
#define STRING_LENGTH_MAX       32
/** @brief Layout for keyword nodes
 *
 * This node accepts a specific keyword from the user.
 */
typedef struct parser_node_keyword_s {
    parser_node_header_t    header;

    /** @brief Keyword to be matched against
     *
     * This field holds a keyword upto KEYWORD_LENGTH_MAX - 1 letters long.
     * Keywords may consist of the letters A-Z, a-z, 0-9 and hyphen.
     */
    char keyword[KEYWORD_LENGTH_MAX];

    /** @brief Minimum characters to match
     *
     * This field controls the matching by the parser. Unless this many
     * characters are received from the user, this node will not be matched.
     */
    uint32_t minimum_match;

    /** @brief Index of value to set
     *
     * If the header flag contains PARSER_NODE_FLAG_SET_VALUE, then this
     * field indicates which entry in the control structure should be updated.
     */
    uint32_t index;

    /** @brief Value to set
     *
     * If the header flag contains PARSER_NODE_FLAG_SET_VALUE, then this field
     * indicates the value to be programmed into the specified index. If the
     * header flag also contains the flag PARSER_NODE_FLAG_SET_BIT, then this
     * field indicates which bit is to be set in the index above.
     */
    int64_t value;

    /** @brief String to set
     *
     * If the header flag contains PARSER_NODE_FLAG_SET_VALUE, and the index
     * indicates a string value, then this field contains the string to be
     * copied to the control structure at the specified index.
     */
    char string[STRING_LENGTH_MAX];
} parser_node_keyword_t;

#define INTEGER_FORMAT_DEC  (1 << 0)
#define INTEGER_FORMAT_HEX  (1 << 1)
#define INTEGER_FORMAT_OCT  (1 << 2)
#define INTEGER_FORMAT_BIN  (1 << 3)
#define INTEGER_FORMAT_ALL  (INTEGER_FORMAT_DEC | INTEGER_FORMAT_HEX | \
                             INTEGER_FORMAT_OCT | INTEGER_FORMAT_BIN)

#define INTEGER_OFFSET(index, size) ((size == 32) ? index : (index + INT_64_OFFSET))

/** @brief Layout for integer nodes
 *
 * This node accepts an integer from the user.
 */
typedef struct parser_node_integer_s {
    parser_node_header_t    header;

    /** @brief Minimum accepted value
     *
     * This field holds the minimum integer value that can be accepted
     * by this node.
     */
    int64_t min_accepted;

    /** @brief Maximum accepted value
     *
     * This field holds the maximum integer value that can be accepted
     * by this node.
     */
    int64_t max_accepted;

    /** @brief Index of value to set
     *
     * This field indicates which entry in the control structure should be
     * updated with the integer from the user.
     */
    uint32_t index;

    /** @brief Formats accepted
     *
     * This is a bitfield which tells the parser which of the following
     * numeric formats can be accepted:
     * * Decimal - `[1-9][0-9]+`
     * * Binary - `0[bB][01]+`
     * * Hexadecimal - `0[xX][0-9a-fA-F]+`
     * * Octal - `0[0-7]+`
     */
    uint32_t formats;
} parser_node_integer_t;

#define AF_NONE     0
#define AF_IPV4     1
#define AF_IPV6     2
#define AF_MAC      3

/** @brief Structure to store IPv4/IPv6/MAC addresses */
typedef struct parser_address_s {
    /** @brief Address family
     *
     * This field serves as a discriminator to determine the address type
     * stored in the array.
     * */
    uint8_t af;

    /** @brief IP address mask
     *
     * This field saves the netmask for IP addresses.
     */
    uint8_t mask;

    /** @brief Address field
     *
     * This is an array which stores the actual address in network byte order.
     */
    uint8_t addr[16];
} parser_address_t;

/** @brief Layout for address nodes
 *
 * This node accepts an address (IPv4/IPv6/MAC) from the user.
 */
typedef struct parser_node_address_s {
    parser_node_header_t    header;

    /** @brief Index of value to set
     *
     * This field indicates which entry in the control structure should be
     * updated with the address from the user.
     */
    uint32_t index;

    /** @brief Format accepted
     *
     * This is a field which tells the parser which address format
     * can be accepted.
     */
    uint32_t format;
} parser_node_address_t;


#endif /* !defined HDR_PARSER_COMMON_H */
