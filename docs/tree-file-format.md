Binary Parse Tree File Format
===============================
**Warning: This document is a work in progress, and should not be treated as a
final spec.**

This document describes the BPT format for CisCLI. The BPT is compiled from the
text representation and is used by the CisCLI shell to build the parse tree at
runtime.

# BPT Layout

    +--------------------------------+
    | BPT File Header                |
    +--------------------------------+
    | Mode 1 Header                  |
    +--------------------------------+
    | Command 1                      |
    +--------------------------------+
    | Command 2                      |
    +--------------------------------+
    | ...                            |
    +--------------------------------+
    | Command n                      |
    +--------------------------------+
    | Mode 2 Header                  |
    +--------------------------------+
    | Command 1                      |
    +--------------------------------+
    | Command 2                      |
    +--------------------------------+
    | ...                            |
    +--------------------------------+
    | Command n                      |
    +--------------------------------+
    | ......                         |
    +--------------------------------+
    | Mode n Header                  |
    +--------------------------------+
    | Command 1                      |
    +--------------------------------+
    | Command 2                      |
    +--------------------------------+
    | ...                            |
    +--------------------------------+
    | Command n                      |
    +--------------------------------+


# BPT File Header

The file header has the following layout. All values are in big-endian byte
order, unless otherwise specified.

       3                   2                   1                   0
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                           MagicID                             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |   Version    |      00h       |            MaxModes           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                           MaxNodes                            |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                        PromptCommand                          |
    |                                                               |
    |                                                               |
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

* The magic identifier is the 4-byte string `BPT\0` (\0 is the null terminator).
* Version is a 1 byte version identifier, which is split into a 4-bit major
  identifier and a 4-bit minor identifier. This should be set to 10h. BPT
  decoders will identify themselves with a supported major version and minor
  version. Compliant decoders should handle any file with a major version
  identifier less than or equal to the decoder version. The minor version is
  used to identify addenda to the specification which do not break backward
  compatibility, but such addenda may be ignored by earlier versions of the
  decoder.
* The MaxModes field is 2 bytes long and is used by the parser to allocate the
  necessary memory for modes. A maximum of 65535 modes are supported.
* The MaxNodes field is 4 bytes long and an indicator to the application for
  allocating the necessary memory for parser nodes. A maximum of 4294967295
  nodes can be created, although the system may not allow allocation of that
  many nodes.
* The PromptCommand field is 64 bytes long and is used to call a system command
  to generate the prompt string. The prompt command must accept one input, which
  is the format string corresponding to each mode. This field is null
  terminated.

# Mode Header

The mode header is used to identify a set of commands that belong to a
particular mode. Each mode can have a different command prompt and this is
handled by a format string stored in the mode header.

       3                   2                   1                   0
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |            ModeID             |         ParentModeID          |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                          RootNodeID                           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                         FormatString                          |
    |                                                               |
    |                                                               |
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

* ModeID is a 2-byte identifier for the mode.
* ParentModeID is a 2-byte identifier for the parent mode. This may be set to
  zero to indicate that there is no parent for this mode.
* RootNodeID is a 4-byte identifier that identifies the root command node for
  this mode.
* FormatString is a 64-byte null-terminated string that is passed to the prompt
  command to generate the prompt string.

# Command Nodes

Each command node has a common node header, followed by a node-type specific
data section. The format for the header follows.

## Command Node Header

       3                   2                   1                   0
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                            NodeID                             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                          ChildNodeID                          |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                         SiblingNodeID                         |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |     Type      |   NodeFlags   |              Flags            |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                           HelpString                          |
    |                                                               |
    |                                                               |
    |                                                               |
    |                                                               |
    |                                                               |
    |                                                               |
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

* NodeID is a 4-byte identifier for the current command node.
* ChildNodeID is a 4-byte pointer to the child node of the current node. This
  may be set to 0 to indicate that there is no child for this node.
* SiblingNodeID is a 4-byte pointer to the sibling node of the current node.
  This may be set to 0 to indicate that there is no sibling for this node.
* Type is a 1-byte node-type identifier. Node types are listed below.
* NodeFlags is a 1-byte field that holds node-specific flags.
* Flags is a 2-byte field that holds the parser flags for this node.
* HelpString is a 128-byte null terminated string that holds the help text for
  this node.

### Parser Flags

The parser flags is a 16-bit field laid out as follows:

                 1                   0
     6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |    Reserved     | | | | |  Priv |
    +-----------------+-+-+-+-+-------+
                       A A A A
    Negatable ---------+ | | |
    Set Negate ----------+ | |
    Invisible -------------+ |
    Hidden ------------------+

## Node Types

Rather than having a single giant structure to hold every node type, CisCLI uses
the type discriminator in the header to identify each node. All node types begin
with the common Command Node Header as described above. Node types are currently
defined as follows

* KEYWORD - Identifier is 1. Used to identify constant keywords.
* INTEGER - Identifier is 2. Used to identify integers in hexadecimal, decimal
  or octal formats. May be constricted by a range.
* DOUBLE - Identifier is 3. Used to identify floating point numbers in decimal
  or hexadecimal formats. May be constricted by a range.
* STRING - Identifier is 4. Used to identify variadic strings. Constrained to
  128 characters or less.
* ADDRESS - Identifier is 5. Used to identify IPv4, IPv6 or MAC addresses.
* CONSTANT - Identifier is 6. Used to set a value in the parser control
  structure.
* CONDITIONAL - Identifier is 7. Used to create branch nodes in the parser tree
  dependent on elements in the control structure.
* TERMINAL/EOL - Identifier is 8. Used to identify the end of the command chain.

## Keyword Nodes

Keyword nodes are used to generate nodes which accept constant keywords.
Keywords can have any byte sequence that is null terminated, however, it is
recommended to use only the printable character set.

       3                   2                   1                   0
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                            Keyword                            |
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                            MinMatch                           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                             Index                             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                             Value                             |
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                            String                             |
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


* Keyword is a 32 byte null terminated string that is used to match against the
  input from the user. A partial match will also succeed, as long as MinMatch
  characters are matched.
* MinMatch is a 4-byte value, although the practical maximum value is the length
  of the keyword, and it indicates the minimum number of characters to match to
  accept this node. A setting of 0 indicates no minimum match requirement.
* Index is a 4-byte value that indicates the index to set a value, if the flag
  PARSER_NODE_FLAG_KW_SET_VALUE is set.
* Value is a 8-byte (int64_t) that is used to set the corresponding value.
  However, if PARSER_NODE_FLAG_KW_SET_BIT is set, then this field indicates the
  bit to be set.
* If the flag PARSER_NODE_FLAG_KW_SET_STRING is set, then String is copied to
  the corresponding index in the control structure.

Flags are set in the NodeFlags attribute of the parent control structure.

## Integer Nodes

Integer nodes are used to accept a range of integers from the user. Input can be
in decimal, hexadecimal or octal format, and the node can be configured to
accept any one of these.

## Double Nodes

Double nodes are used to accept floating point input from the user.

## String Nodes

String nodes are used to accept a variable string from the user. This can be
used for things like description text, or file paths.

## Address Nodes

Address nodes are used to accept IPv4, IPv6 or MAC addresses from the user.

## Constant Nodes

Constant nodes are used to set fields in the parser control structure.

## Conditional Nodes

Conditional nodes are used to switch branches in the parser tree, depending on
the programmed condition. Refer to the conditional node design document for more
information.

## Terminal Nodes (EOL)

Terminal nodes are used to identify the end of the parser node chain and are
used to perform individual actions.
