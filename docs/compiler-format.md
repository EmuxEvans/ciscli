BPT Compiler Input File Syntax
==============================

The syntax compiler allows the user to describe the CLI command chain(s) in a
simple structured format, which the compiler will then compile down to a BPT
file.

# Comments

Comments begin with the `#` sign and terminate at the end of the line

# Preprocessor Commands

The BPT preprocessor allows the user to create multiple files, and selectively
compile portions of the CLI chain.

## Import command

The `import` command is used to parse the contents of the specified file before
resuming parsing of the current file. Import directories can be specified on
the command line with the -I switch. Import directories are parsed in the
following order:

* Relative to the directory of the current file.
* Relative to the directories specified on the command line, in the order
  specified.

**Syntax:**

    import "path/to/file" # Relative path
    import "file"

## Define command

The `define` command is used to create a definition table. It takes two
arguments, a symbol argument for the name of the definition, and a second
argument of any type, though usually a string or a number.

**Syntax:**

    define :STRING_NAME, "string"
    define :NUMBER_NAME, 2


# Parse Tree commands

This is the actual syntax of the parse tree definition. Each command usually
accepts a block either enclosed in `do...end` or curly brackets. If the curly
bracket syntax is used, then the command arguments must be enclosed within
parentheses. See each command for specific details.

## Tree command

The `tree` command is used to specify the parse tree to use. Multiple parse
trees can be used and action points can be set to switch between trees. All
nodes must be enclosed within a `tree` block.

In addition to the individual parser tree, the user can specify which subtree to
use. This has the added benefit of chaining multiple trees in a parent-child
relationship. The parser will attempt to evaluate the entered command in the
current tree. If it fails, it will attempt the evaluation in the parent tree, up
until the point where there is no parent tree.

**Syntax:**

    tree "treename" do
        ...
    end

    tree("treename") {
        ...
    }

## Keyword command

This node is used to specify a keyword node, which can accept a single keyword
string.

**Syntax:**

    keyword "keywordstring" do
        ...
    end

    keyword("keywordstring") {
        ...
    }

### Set subcommand

The `set` subcommand is used within a keyword block to mark the parent keyword
node to set a value into the parser control structure. The value set can be an
integer, floating point number, IPv4/IPv6 address, MAC address, string, or a
single bit within an integer. It takes in 3 parameters, the type of value to
set, the position in the parser control structure in which to set it, and the
value to set.

The type is one of the following `BIT`, `INT`, `NUM`, `STR`, `MAC`, `IP4` or
`IP6`. If the type is `BIT`, then the value must be between `0` and `MAX_BIT`

**Syntax:**

    set <type>, <position>, <value>

## Integer command

This node is used to specify an integer node, which can accept a 64-bit signed
integer.  It takes one argument, the position at which to save the parsed
integer.

**Syntax:**

    integer <position> do
        ...
    end

    integer(<position>) {
        ...
    }

### Range subcommand

The `range` subcommand is used within an integer block to restrict the range of
inputs that can be accepted by the parent node. It takes in two parameters, the
minimum and the maximum that can be accepted, both inclusive. If the range
subcommand is not specified, then the node defaults to using the default MIN and
MAX for the integer type.

The range can be used to set an explicit minimum or an explicit maximum, or
both. If the application programmer wishes to use the default minimum or
maximum, then he should just use MIN or MAX as the parameter. If the supplied
minimum exceeds the maximum, an error is raised.

**Syntax:**

    range <min>, <max>
    range MIN, <max>
    range <min>, MAX
    range MIN, MAX

## Number command

This node is used to specify a number node, which can accept a double precision
floating point number. It takes one argument, the position at which to save the
parsed number.

**Syntax:**

    number <position> do
        ...
    end

    number(<position>) {
        ...
    }

### Range subcommand

The `range` subcommand acts very similar to the command for integer nodes,
however, the minimum and maximum are now the maximum positive and negative
values that can be stored in a double precision floating point number.

## String command

This node is used to specify a string node, which can accept a null terminated
string up to a predefined maximum length. It takes in one argument, the position
at which to save the parsed string.

**Syntax:**

    string <position> do
        ...
    end

    string(<position>) {
        ...
    }

## IP command

This node is used to specify an IP address node, which can parse an IPv4 or an
IPv6 address. It takes in two arguments, the address family and the position at
which to save the parsed address.

**Syntax:**

    ip <af>, <position> do
        ...
    end

    ip(<af>, <position>) {
        ...
    }

## MAC command

This node is used to specify an Ethernet MAC address. It uses the same storage
as the integer objects and takes in a single positional parameter.

**Syntax:**

    mac <position> do
        ...
    end

    mac(<position>) {
        ...
    }

## Constant command

This node is used to set a value into the parser control structure. 
## Action command

This node is used as the terminal node to specify an action to take upon hitting
the return key. Actions can be one of the following types:

* `SYSCALL` - Calls a system command
* `LIBCALL` - Calls a library function. This can be either local to the
  binary including the ciscli library, or an external library.
* `INTERNAL` - Calls an internal ciscli command.
