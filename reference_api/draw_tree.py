from ete3 import Tree, TreeStyle, NodeStyle, faces, AttrFace, CircleFace, TextFace
import pandas as pd

import sys

if len(sys.argv) < 2:
    print("usage: <file> <root_symbol> <deep_level>")
    exit(-1)

root = sys.argv[1]
level = sys.argv[2]

# TREE_FILE = "references_commit_creds_2_deep.csv"
# TREE_FILE = "references_namespace_2_deep.csv"
# TREE_FILE = "references_commit_creds_3_deep.csv"
TREE_FILE = f"references_{root}_{level}_deep.csv"


if root != "namespace":
    ROOT_SYMBOL = "commit_creds"
else:
    ROOT_SYMBOL = "switch_task_namespaces"

MAX_SPHERE_SIZE= 100

max_weight = 0
def build_tree():
    global max_weight
    t = Tree()
    df = pd.read_csv(TREE_FILE)
    df = df.drop_duplicates(subset=["Kernel Version","Subsystem","File","Callee","Caller","Caller Type"])
    df = df.reset_index()

    root = t.add_child(name=ROOT_SYMBOL)
    root.add_features(weight=0)
    already_in_tree = {ROOT_SYMBOL: root}
    # root_name = TextFace(ROOT_SYMBOL, fsize=16)
    # root_name.margin_left = 100
    # t.add_face(root_name, column=0)ls -la

    for index, row in df.iterrows():
        name = row["Caller"]
        caller = already_in_tree.get(name)
        if caller == None:
            
            callee = already_in_tree.get(row["Callee"]) 
            if callee == None:
                print("FATAL ERORR, missing callee")
                exit(-1)
            
            tree_node = callee.add_child(name=name)
            tree_node.add_features(weight=1, type=row["Caller Type"])
            already_in_tree[name] = tree_node
        else:
            if caller.weight < MAX_SPHERE_SIZE:
                caller.add_features(weight=caller.weight+1, type=row["Caller Type"])
                if(caller.weight+1 > max_weight):
                    max_weight = caller.weight
    return t 

    
def layout(node):
    if "weight" in node.features:

        if node.name == ROOT_SYMBOL:
            print(node.name)
            # Add node name to all non-root nodes
            N = AttrFace("name", fsize=20, fgcolor="float")
            faces.add_face_to_node(N, node, 0)
        else:
            # Add node name to all non-root nodes
            N = AttrFace("name", fsize=16, fgcolor="black")
            faces.add_face_to_node(N, node, 0)

        # Creates a sphere face whose size is proportional to node's
        # feature "weight"
        if node.name != ROOT_SYMBOL:
            # normal_weight = (node.weight//max_weight)*MAX_SPHERE_SIZE
            normal_weight = node.weight
            if node.type == "syscall":
                C = CircleFace(radius=normal_weight, color="Aqua", style="sphere")
            if node.type == "struct":
                C = CircleFace(radius=normal_weight, color="SpringGreen", style="sphere")
            if node.type == "function":
                C = CircleFace(radius=normal_weight, color="SteelBlue", style="sphere")
            if node.type == "macro":
                C = CircleFace(radius=normal_weight, color="Gold", style="sphere")
            if node.type == "error":
                C = CircleFace(radius=normal_weight, color="Red", style="sphere")
            # Let's make the sphere transparent
            C.opacity = 0.3
            # And place as a float face over the tree
            faces.add_face_to_node(C, node, 0, position="float")

def get_tree_style():
    # Create an empty TreeStyle
    ts = TreeStyle()

    # Set our custom layout function
    ts.layout_fn = layout

    # Draw a tree
    ts.mode = "c"

    # ts.aligned_header.add_face(TextFace(ROOT_SYMBOL, fsize=100), column=0)
    ts.legend.add_face(TextFace("Lightblue: syscall", fsize=25), column=0)
    ts.legend.add_face(TextFace("Green: struct", fsize=25), column=0)
    ts.legend.add_face(TextFace("Blue: function", fsize=25), column=0)
    ts.legend.add_face(TextFace("Yellow: macro", fsize=25), column=0)
    ts.legend.add_face(TextFace("Red: error", fsize=25), column=0)

    # We will add node names manually
    ts.show_leaf_name = False

    return ts

if __name__ == "__main__":
    # t, ts = get_example_tree()

    #t.render("bubble_map.png", w=600, dpi=300, tree_style=ts)
    # t.show(tree_style=ts)
    tree = build_tree()
    style = get_tree_style()
    tree.show(tree_style=style)