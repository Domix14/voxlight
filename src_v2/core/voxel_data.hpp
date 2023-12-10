#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

template <typename T>
class VoxelData {
   public:
    VoxelData();
    ~VoxelData();

    bool setVoxel(int x, int y, int z, T value);
    T getVoxel(int x, int y, int z) const;

    T const* getData() const;
    std::vector<T> getDataAsVector() const;
    std::size_t getWidth() const;
    std::size_t getHeight() const;
    std::size_t getDepth() const;

    void shrinkToFit();
    void resize(std::size_t w, std::size_t h, std::size_t d);
    size_t getByteSize() const;

    void loadFromVoxFile(std::filesystem::path path);

   private:
    size_t getIndex(int x, int y, int z) const;

    std::vector<T> data;
    std::size_t width;
    std::size_t height;
    std::size_t depth;
};

template <typename T>
VoxelData<T>::VoxelData() : width(0), height(0), depth(0) {}

template <typename T>
VoxelData<T>::~VoxelData() {}

template <typename T>
bool VoxelData<T>::setVoxel(int x, int y, int z, T value) {
    auto index = getIndex(x, y, z);
    std::cout << "Index: " << index << "\n";
    if (index < data.size()) {
        data.at(getIndex(x, y, z)) = value;
        return true;
    };
    return false;
}

template <typename T>
T VoxelData<T>::getVoxel(int x, int y, int z) const {
    auto index = getIndex(x, y, z);
    if (index < data.size()) {
        return data.at(getIndex(x, y, z));
    };
    return T();
}

template <typename T>
T const* VoxelData<T>::getData() const {
    return data.data();
}

template <typename T>
std::vector<T> VoxelData<T>::getDataAsVector() const {
    return data;
}

template <typename T>
std::size_t VoxelData<T>::getWidth() const {
    return width;
}

template <typename T>
std::size_t VoxelData<T>::getHeight() const {
    return height;
}

template <typename T>
std::size_t VoxelData<T>::getDepth() const {
    return depth;
}

template <typename T>
size_t VoxelData<T>::getIndex(int x, int y, int z) const {
    return x + y * width + z * width * height;
}

template <typename T>
void VoxelData<T>::shrinkToFit() {
    bool voxelFlag = false;
    size_t minDepth = depth;
    size_t maxDepth = 0;
    for (size_t z = 0; z < depth; ++z) {
        voxelFlag = false;
        for (size_t x = 0; x < width && !voxelFlag; ++x) {
            for (size_t y = 0; y < height && !voxelFlag; ++y) {
                voxelFlag = getVoxel(x, y, z) != T(0);
                if (voxelFlag && z < minDepth) {
                    minDepth = z;
                }
                if (voxelFlag && z > maxDepth) {
                    maxDepth = z;
                }
            }
        }
    }

    size_t minWidth = width;
    size_t maxWidth = 0;
    for (size_t x = 0; x < width; ++x) {
        voxelFlag = false;
        for (size_t z = minDepth; z <= maxDepth && !voxelFlag; ++z) {
            for (size_t y = 0; y < height && !voxelFlag; ++y) {
                voxelFlag = getVoxel(x, y, z) != T(0);
                if (voxelFlag && x < minWidth) {
                    minWidth = x;
                }
                if (voxelFlag != T(0) && x > maxWidth) {
                    maxWidth = x;
                }
            }
        }
    }

    size_t minHeight = height;
    size_t maxHeight = 0;
    for (size_t y = 0; y < height; ++y) {
        voxelFlag = false;
        for (size_t x = minWidth; x <= maxWidth && !voxelFlag; ++x) {
            for (size_t z = minDepth; z <= maxDepth && !voxelFlag; ++z) {
                voxelFlag = getVoxel(x, y, z) != T(0);
                if (voxelFlag && y < minHeight) {
                    minHeight = y;
                }
                if (voxelFlag && y > maxHeight) {
                    maxHeight = y;
                }
            }
        }
    }

    std::vector<T> newData((maxWidth - minWidth + 1) * (maxHeight - minHeight + 1) * (maxDepth - minDepth + 1));
    for (size_t z = minDepth; z <= maxDepth; ++z) {
        for (size_t x = minWidth; x <= maxWidth; ++x) {
            for (size_t y = minHeight; y <= maxHeight; ++y) {
                newData[(x - minWidth) + (y - minHeight) * (maxWidth - minWidth + 1) +
                        (z - minDepth) * (maxWidth - minWidth + 1) * (maxHeight - minHeight + 1)] = getVoxel(x, y, z);
            }
        }
    }
    width = maxWidth - minWidth + 1;
    height = maxHeight - minHeight + 1;
    depth = maxDepth - minDepth + 1;
    data = std::move(newData);
}

template <typename T>
void VoxelData<T>::resize(std::size_t w, std::size_t h, std::size_t d) {
    width = w;
    height = h;
    depth = d;
    data.resize(width * height * depth);
}

template <typename T>
size_t VoxelData<T>::getByteSize() const {
    return data.size() * sizeof(T);
}

template <typename T>
void VoxelData<T>::loadFromVoxFile(std::filesystem::path path) {
    auto readTag = [](std::istreambuf_iterator<char>& iterator) {
        std::string tag;
        for (std::size_t i = 0; i < 4; i++) {
            tag += *iterator;
            std::advance(iterator, 1);
        }
        return tag;
    };

    auto readInt = [](std::istreambuf_iterator<char>& iterator) {
        std::array<std::uint8_t, 4> data;
        for (std::size_t i = 0; i < 4; i++) {
            data[i] = *iterator;
            std::advance(iterator, 1);
        }
        return static_cast<std::int32_t>(data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24);
    };

    if (std::filesystem::exists(path) && path.extension() == ".vox") {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        auto iterator = std::istreambuf_iterator<char>(file);

        readTag(iterator);  // VOX tag
        readInt(iterator);  // version

        while (iterator != std::istreambuf_iterator<char>()) {
            auto tag = readTag(iterator);
            auto contentSize = readInt(iterator);
            readInt(iterator);  // Child chunks

            if (tag == "SIZE") {
                auto w = readInt(iterator);
                auto d = readInt(iterator);
                auto h = readInt(iterator);
                resize(w, h, d);
            } else if (tag == "XYZI") {
                // assert(data.size() == contentSize / 4);
                auto voxelCount = readInt(iterator);
                for (auto i = 0; i < voxelCount; i++) {
                    auto x = static_cast<std::uint32_t>(*iterator);
                    std::advance(iterator, 1);
                    auto z = static_cast<std::uint32_t>(*iterator);
                    std::advance(iterator, 1);
                    auto y = static_cast<std::uint32_t>(*iterator);
                    std::advance(iterator, 1);
                    T colorIndex = static_cast<T>(*iterator);
                    std::advance(iterator, 1);
                    setVoxel(x, y, z, colorIndex);
                }
            } else {
                std::advance(iterator, contentSize);
            }
        }
    }
}