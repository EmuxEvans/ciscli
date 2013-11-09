Parser Graph Structure
======================

The parser graph is a set of nodes with a common header. The header contains
pointers to what we call the child node, and the sibling node. The child node
is the next node in the graph which will be visited if the parser accepts this
node in the parse chain. On the other hand, if the parser rejects this node,
then the parser will check the sibling node.

Consider the following set of commands:

    show interface
    show statistics
    interface up
    interface down

The parser will have an initial node which sets up the CLI control structure.
This initial node will have the exact same header as all other nodes. We will
also have at least one terminal node called of type End of Line (EOL). This
allows the applicationd developer to have multiple EOL functions to handle
different parse chains. Let us say that the nodes are as follows:

    node_init
    node_show
    node_show_interface
    node_show_statistics
    node_interface
    node_interface_up
    node_interface_down
    node_eol

The graph would look like this (lines indicating child nodes and arrows
indicating sibling nodes):

    node_init
        show            ->      interface
            interface               up
                node_eol                node_eol
         -> statistics           -> down
                node_eol                node_eol

The idea is once the parser matches the show node, it will check if the command
matches with `show interface`. If that fails, it will then check `show
statistics`, and when that matches, it will proceed to the EOL node and call
the function specified by that node.

It is possible for the parser not to match any node. If the string is empty,
or consists of only whitespace or comments, the parser will simply ignore the
string. However, if it has some text, then the parser will abort with the
error "Unrecognized command". If the parser has matched at least one node, but
is not at a point where it can hit an EOL node, then it will throw the error
"Incomplete command". If the parser matches more than one node, then it will
throw the error "Ambiguous command"
