#ifndef OCTREE_HPP
#define OCTREE_HPP

struct VoxelNode {
    VoxelNode() {
        static int idCounter = 0;
        for (int i = 0; i < 8; i++) {
            children[i] = nullptr;
        }
        isSolid = false;
        id = idCounter++;
    }

    bool isLeaf() {
        for (int i = 0; i < 8; i++) {
            if (children[i] != nullptr) {
                return false;
            }
        }
        return true;
    }

    VoxelNode* children[8];
    bool isSolid;
    int id;
};


#endif // OCTREE_HPP